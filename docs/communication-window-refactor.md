# Communication Window Refactor — compact operational dashboard

> Living design doc. First iteration implemented from the whiteboard mock
> (`Documents/GitHub/mock.jpg`). Expect to edit the **calibration / channel-map** and layout
> details after the first on-hardware pass.

## Context
The Communication window (`src/ui/windows/CommunicationWindow.cpp`) was a vertical stack of
`CollapsingHeader` sections dumping **every** field of `SystemState` / `ExtendedSystemState` in big
`Field | ECU | FCU` tables, plus command buttons and UDP config. It read as a raw debug dump and was
slow to scan during a test.

The mock reorganizes it into a dense single-screen operational dashboard: board states + power rails +
ping at top; valves + a per-device status matrix; engineering telemetry (pressure/temp/thrust/mass in
real units) with now/max/tare readouts; a tank-fill gauge; heater/solenoid toggles; logging toggles.

Goal: same data inputs, recomposed for density and clarity.
- **Keep the existing full field tables**, moved verbatim into a separate **tab** (not deleted).
- **Wire the existing unit converters** to show engineering values, without touching the plot/vector
  storage (plots will be disabled). Maintain a **fixed 128-element** active window per displayed signal.
- now = current value, **max = peak-hold** (resettable), **offset = tare**.
- The bottom "State" grid is a **device status matrix** (ADC, storage, CAN, eth, power monitor,
  thermocouples, heater, solenoid, valves…), per board — not command buttons.
- CSV telemetry logging already dumps all decoded fields independently of the UI (`PacketCSVLogging`),
  so no logging changes are needed.

## ⚠️ Known unknowns to calibrate in iteration 2
These are wired through a single editable block at the top of the Dashboard code
(`kDashboardSignals` / channel-map). They compile and render but the **numbers are placeholders**:

1. **ADC channel → physical sensor mapping is undefined for the UDP path.** Firmware streams 8 ADS131M08
   channels (`AdcInfo::channels[8]`, signed 24-bit counts). Nothing in the GCS currently maps those 8
   channels to tank/chamber/fill pressure or Top/Throat/Tank thermistors — that mapping lived only in the
   retired serial path (`SerialConfig`, a 16-channel/12-bit layout that does not apply). The dashboard
   uses a documented placeholder map.
2. **ADC scale mismatch.** The converters (`PressureTransducer`, `TemperatureSensor`, `LoadCell`) assume a
   12-bit (0–4096) ADC; the ADS131M08 is 24-bit signed. A scale factor will likely be needed before the
   converters produce correct units. Currently passed straight through (scale = 1) and flagged in-code.

## Structure: tab bar inside the window
`renderImpl()` body wrapped in `ImGui::BeginTabBar("comm")` with three tabs:
1. **Dashboard** — the compact layout (the bulk of the work).
2. **Raw** — the original three full tables verbatim (`renderSystemStateTable`,
   `renderExtendedStateTable`, `renderGsSystemStateTable`, with their Hz lines).
3. **Setup** — UDP configuration + GsSystemState send/stream test tools.

## Dashboard layout (top → bottom)
1. **States bar.** Per board (ECU/FCU): colored state name, `SystemState` Hz + `ExtendedSystemState` Hz
   (`RateTracker` on the `*Count` atomics). Power rails 3.3 V / 5 V / 12 V from
   `power_monitor.channels[0..2]` (`bus_code*0.008` V, `shunt_code*0.4` mA), laid out **horizontally**
   (one group per rail, label / V / mA stacked). Ping button + **always-on** 1 Hz auto-ping
   (`ComTask::autoPing` is forced true each frame — no toggle) + pong count / last sender.
2. **Valves.** Valve rows NOS/IPA/Fill/Dump (`renderValveCommandRow`) each with a colored state dot;
   Solenoid + Heater toggles. (The device status now lives in the bit-level matrix, section 4.)
3. **Engineering telemetry (real units).** Read `motorAdcAverager.latestAverage()` /
   `fillingStationAdcAverager.latestAverage()` and run the existing converters
   (`src/data/data_conversion/*`): pressure (psi), thermistor temp (°C), load cell (lb), NOS tank mass
   (lb) + phase via `VaporPressure`. Thermocouples (°C) read from `fillingStationThermocouple_C[]`. Each
   signal shows now / max / tare via `SignalStat`. (No tank-fill gauge — see section 4.)
4. **Status (bit-level) matrix.** The mock's bottom grid. 3 columns: **Device | ECU (state + bits) |
   FCU (state + bits)**. For each peripheral the cell shows the textual `*State` plus one chip per named
   bit of that peripheral's `Info.status` struct; chip is **green when the bit = 1, red when 0** (literal
   bit value, not a good/bad judgement). Rows: ADC, Storage, CAN, Ethernet (FCU), Power monitor, the four
   valves (NOS/IPA on ECU, Fill/Dump on FCU), the four thermocouples TC1–TC4 (FCU), Heater (FCU). Boards
   that lack a device show `-`.
5. **Logging.** Fast-recording + Disable-logging toggles (`renderBaseFlagToggle`,
   `ControlFlagBase::FastRecording` / `DisableLogging`).

### Iteration 7 — feedback applied
- **Fixed the 2-column overlap.** The outer wrapping table (nested tables inside it laid out on top of
  each other) was replaced with two `BeginGroup`/`EndGroup` blocks separated by `SameLine` — each group
  sizes to its own content, so the stacked tables in a column flow normally. Section titles switched from
  `ImGui::SeparatorText` (spans full width, would force a group to full window width) to a content-width
  `sectionHeader()` helper (a colored text label).
  - Follow-up: also added `ImGuiTableFlags_NoHostExtendX` to every dashboard table. Without it a table
    stretches to the full available width (SizingFixedFit only sizes columns, not the table box), so the
    left group filled the window and pushed the right group off-screen. NoHostExtendX makes each table
    only as wide as its columns, so both groups are content-width and sit side by side.

### Iteration 6 — feedback applied
- **Two big columns.** The whole dashboard is wrapped in a `dash` 2-column table (content-sized,
  `SizingFixedFit`) to avoid scroll bars: LEFT = States, ping, Valves, Status; RIGHT = Telemetry
  (Pressure, Load, Temps, Phase), Logging.
- **Valve buttons aligned.** The Valves table now has a dedicated `Valve` name column, so the command
  buttons line up across rows. `renderValveCommandRow` gained `showName` (false in the dashboard, name
  comes from the table column).
- **10% increments.** Valve command buttons are now Cl, 10, 20 … 90, Op (+ Force), as compact
  `SmallButton`s (applies in the Raw tab too).
- **PT tare removed** — the pressure table dropped its tare column (cur / avg 2s / max / raw only).
- **CoolProp removed from this window.** The NOS tank-mass calc (`TankMass::getNOSTankMass_lb`, which
  pulls CoolProp `PropsSI`) and its readout/`SignalStat` are gone. The Derived block is now just the
  NOS **phase** (Liquid/Gas via `VaporPressure`, a pure correlation — no CoolProp).
- **NOTE / open:** CoolProp is still referenced elsewhere in the codebase
  (`TankMassWindow`, `TankMassCalculatorWindow`, `src/plot/processors/TankMassPlotDataProcessor`). If the
  goal is to drop CoolProp project-wide (it was "throwing a lot of errors"), those + `vcpkg.json` need a
  separate pass — say the word.

### Iteration 5 — feedback applied
- **Tank LC mass fields named** Empty / IPA / NOS (`tankLcMass[3]`, labelled `InputFloat`s in the
  load-cell table's cal-masses cell).
- **Status table aligned** — now 5 columns: Device | ECU state | ECU bits | FCU state | FCU bits, so
  state sits in its own sub-column and the bit chips in another (per board).
- **Current / 2 s average / max** for tank mass, thrust load cell, and both pressure transducers
  (Chamber, Tank). Implemented via a fixed **100 Hz sampler**: `SignalStat` is now a 256-deep ring
  sampled on each elapsed 10 ms tick (zero-order hold, decoupled from frame rate); `now()` = current,
  `avg()` = mean of the last 200 samples (~2 s), `max()` = peak since start. The Pressure, Load and
  Derived tables show cur / avg 2s / max columns (all signals are sampled at 100 Hz, so the load/temp
  rows carry valid averages too).
- **Open for next iteration:** the 3 tank masses are still capture-only (load-cell calibration math not
  wired); decide units/precision and whether temps also want avg.

### Iteration 4 — feedback applied
- **Everything is now the table pattern** (the rails-style bordered table the user liked): States,
  Valves, Pressure, Load cells, Temps, Derived, and the Status matrix are each a `BeginTable` with
  values, buttons and input fields living in cells.
- **Valve state beside the commands.** The Valves section is a 2-column table: column 1 = command
  buttons (`renderValveCommandRow` / Solenoid / Heater toggles), column 2 = live state. Valves moved out
  of the Status matrix. Solenoid and Heater rows show their on/off state chip in column 2.
- **Background-colored chips everywhere** — a single `colorChip(label, on)` primitive draws a filled
  green/red **background** with the label on top (drawn via the window draw list, no widget id, so it
  never collides inside a table). Replaces all green/red *text*. Used for status bits, valve states, and
  the solenoid/heater state. (The board-state cell is now plain text — it carried no real status color.)
- **Tank load-cell mass inputs.** The Load-cell table's tank row has three `InputFloat` mass-entry
  fields (`tankLcMass[3]` member); the thrust row shows `-` (no tare). Pressure rows keep a per-row
  **Tare** button; load/temp rows have a peak-hold **rst** button.
- **Open for next iteration:** what the 3 tank masses calibrate (the conversion math is not wired yet —
  they're captured only); column widths/labels; whether Solenoid/Heater also belong in the Status matrix
  (currently Heater is in both the Valves table and the Status matrix).

### Iteration 3 — feedback applied
- **Ping button removed.** The dashboard shows only "Time since last successful Ping (s)" — tracked
  GS-side as the elapsed time since `GSDataCenter::pongReceivedCount` last incremented (shows "never"
  until the first pong). Auto-ping stays forced on.
- **Power rails are now a table** (`states_bar`, 7 columns): one row per board, columns
  Board | State | SS Hz | Ext Hz | 3.3V | 5V | 12V, each rail cell V over mA. (Matches the mock's grid
  better than the previous horizontal groups.)
- **Thermocouples reduced to 2** (TC1, TC2) in both the telemetry readouts and the status matrix.
  `thermocoupleC` is now `std::array<SignalStat,2>`.
- **PT IPA and NOS removed** — `kPressure` is now just Chamber + Tank (ECU). `pressurePsi` is
  `std::array<SignalStat,2>`.
- **Raw beside converted** — pressure and load-cell readouts now show the raw averaged ADC value next to
  the converted value (`renderReadout` gained an optional `const float* raw`).
- **Valves: bit-field-like state** — in the status matrix, each valve renders the five possible states
  (Closed / Closing / Opening / Opened / Floating) as chips with the **current state green** and the rest
  red; a leading **dot flags a fault** (Faulted state or the both-switches fault bit). Replaces the raw
  ValveStatus bit chips for valve rows.
- **Open for next iteration:** rail labels (3.3/5/12 V) and order still assumed; the valve "ok/fault"
  dot label wording; whether non-valve devices should also move to a state-chip style.

### Iteration 2 — feedback applied
- Ping/auto-ping is now **always enabled** (forced on, toggle removed from the dashboard).
- Power rails rendered **horizontally** (was a vertical list).
- The bottom grid is now the **bit-level status matrix** described in section 4 (was a simple
  one-dot-per-device state grid). State is textual; each status bit is its own green/red chip.
- Removed the NOS-mass "Stats" tank gauge; NOS tank mass is now a plain readout in the Telemetry
  section. (The mock's bottom-right area is the status/state matrix, not a mass gauge.)
- **Open for next iteration:** Solenoid has no `Info.status` struct (it's a control flag), so it's not
  in the bit matrix yet — decide how to represent it. Bit chip labels are abbreviated; confirm wording.
  Per-bit good/bad coloring (vs literal 1/0) may be wanted for fault bits later.

## SignalStat — the 128-element active window
Local struct used as a window member per engineering signal: fixed `std::array<float,128>` ring, write
head, count, tare `offset`, peak-hold `peak`. `push(raw)` once per frame; `now()` = latest − offset;
`max()` = peak; `tare()`; `resetPeak()`.

## Class / state changes
Function-local `static`s become **members** of `CommunicationWindow` (like `TankMassWindow`): the
`RateTracker`s, UDP `ipBuf`/ports/`loggingEnabled`/`lostPacketCount`, and the `SignalStat`s. A tiny
status-dot helper (ImDrawList circle + `ThemedColors`); reuse `src/ui/widgets/OnOff` for toggles.

## Files
- `src/ui/windows/CommunicationWindow.cpp` — main refactor.
- `src/ui/windows/CommunicationWindow.h` — member variables.
- New includes: `data_conversion/{PressureTransducer,TemperatureSensor,LoadCell,TankMass,VaporPressure}.h`,
  `ThemedColors.h`, `widgets/OnOff.h`.
- No changes to data flow, `PacketProcessing`, or CSV logging.

## Verification
- Build (compile-bound; PCH path is the fast one).
- Run, feed telemetry. Check Dashboard states/Hz/power/ping match the Raw tab; tare zeroes a reading;
  peak-hold tracks + resets; device matrix colors track faults; Raw tab still shows all fields; Setup
  tab UDP update + GsSystemState send/stream still work.
- Engineering-unit values are placeholders until the channel map + ADC scale (above) are calibrated.
