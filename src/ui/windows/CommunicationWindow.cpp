#include "CommunicationWindow.h"
#include "ComTask.h"
#include "CommandControl.h"
#include "CRC.h"
#include "GSDataCenter.h"
#include "ICom.h"
#include "ITileLoader.h"
#include "Logging.h"
#include "UdpConfig.h"

#include "LoadCell.h"
#include "PressureTransducer.h"
#include "TemperatureSensor.h"
#include "VaporPressure.h"

#include "ThemedColors.h"

#include "command/set_control_flag.hpp"
#include "framing/ethernet_header.hpp"
#include "framing/payload_type.hpp"
#include "system/board_id.hpp"
#include "telemetry/gs_system_state.hpp"
#include "telemetry/telemetry_type.hpp"
#include "devices/valve/valve_state.hpp"
#include "devices/ematch/ematch_info.hpp"
#include "devices/solenoid/solenoid_info.hpp"
#include "peripherals/adc/adc_state.hpp"
#include "peripherals/can/can_state.hpp"
#include "peripherals/ethernet/ethernet_state.hpp"
#include "peripherals/power_monitor/power_monitor_state.hpp"
#include "peripherals/storage/sd_write_engine_info.hpp"
#include "peripherals/storage/storage_error.hpp"
#include "peripherals/storage/storage_state.hpp"
#include "peripherals/thermocouple/thermocouple_state.hpp"
#include "system/state.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <map>
#include <span>
#include <string>
#include <utility>

// ============================================================================
// Member helpers shared across frames (declared in CommunicationWindow.h)
// ============================================================================
double RateTracker::update(uint64_t currentCount) {
    const auto now = std::chrono::steady_clock::now();
    const double dt = std::chrono::duration<double>(now - lastTime).count();
    if (dt >= 0.5) {
        rateHz = static_cast<double>(currentCount - lastCount) / dt;
        lastCount = currentCount;
        lastTime = now;
    }
    return rateHz;
}

void SignalStat::push(float raw) {
    latest = raw;
    ring[head] = raw;
    head = (head + 1) % WINDOW;
    if (count < WINDOW) {
        count++;
    }
    const float tared = raw - offset;
    if (!hasPeak || tared > peak) {
        peak = tared;
        hasPeak = true;
    }
}

float SignalStat::avg() const {
    const std::size_t n = std::min(count, AVG_SAMPLES);
    if (n == 0) {
        return latest - offset;
    }
    double sum = 0.0;
    for (std::size_t k = 0; k < n; k++) {              // walk back from the most recent sample
        const std::size_t idx = (head + WINDOW - 1 - k) % WINDOW;
        sum += ring[idx];
    }
    return static_cast<float>(sum / static_cast<double>(n)) - offset;
}

namespace {
const char* boardStateName(uint8_t rawState) {
    using logic::control::State;
    switch (static_cast<State>(rawState)) {
    case State::Init:   return "INIT";
    case State::Safe:   return "SAFE";
    case State::Unsafe: return "UNSAFE";
    case State::Abort:  return "ABORT";
    case State::Error:  return "ERROR";
    case State::Ignite: return "IGNITE";
    case State::Launch: return "LAUNCH";
    case State::Test:   return "TEST";
    }
    return "Unknown";
}

const char* valveStateName(uint8_t rawState) {
    switch (static_cast<ValveState>(rawState)) {
    case ValveState::Unknown:  return "Unknown";
    case ValveState::Opened:   return "Opened";
    case ValveState::Closed:   return "Closed";
    case ValveState::Opening:  return "Opening";
    case ValveState::Closing:  return "Closing";
    case ValveState::Faulted:  return "Faulted";
    case ValveState::Floating: return "Floating";
    }
    return "Unknown";
}

const char* thermocoupleStateName(uint8_t rawState) {
    switch (static_cast<ThermocoupleState>(rawState)) {
    case ThermocoupleState::Unknown: return "Unknown";
    case ThermocoupleState::Active:  return "Active";
    case ThermocoupleState::Faulted: return "Faulted";
    }
    return "Unknown";
}

const char* adcStateName(AdcState s) {
    switch (s) {
    case AdcState::Unknown:   return "Unknown";
    case AdcState::Streaming: return "Streaming";
    case AdcState::Faulted:   return "Faulted";
    }
    return "Unknown";
}

const char* storageStateName(StorageState s) {
    switch (s) {
    case StorageState::Init:   return "Init";
    case StorageState::Active: return "Active";
    case StorageState::Error:  return "Error";
    }
    return "Unknown";
}

const char* storageErrorName(StorageError e) {
    switch (e) {
    case StorageError::None:          return "None";
    case StorageError::MountFail:     return "MountFail";
    case StorageError::FileOpenFail:  return "FileOpenFail";
    case StorageError::FileWriteFail: return "FileWriteFail";
    }
    return "Unknown";
}

const char* canStateName(CanState s) {
    switch (s) {
    case CanState::Unknown: return "Unknown";
    case CanState::Active:  return "Active";
    case CanState::Error:   return "Error";
    }
    return "Unknown";
}

const char* ethStateName(EthernetState s) {
    switch (s) {
    case EthernetState::Unknown: return "Unknown";
    case EthernetState::Up:      return "Up";
    case EthernetState::Down:    return "Down";
    }
    return "Unknown";
}

// INA3221 shunt resistor value, used to convert the shunt-voltage code into a current.
constexpr float kPowerMonitorRshuntOhms = 0.1f;
// shunt code LSB = 40 uV; current = (40 uV / Rshunt) per code = 0.4 mA at Rshunt = 0.1 ohm.
constexpr float kShuntCodeToMilliAmp = 0.04f /* mV per code */ / kPowerMonitorRshuntOhms;
constexpr float kBusCodeToVolt = 0.008f; // bus code LSB = 8 mV

const char* powerMonitorStateName(PowerMonitorState s) {
    switch (s) {
    case PowerMonitorState::Unknown: return "Unknown";
    case PowerMonitorState::Active:  return "Active";
    case PowerMonitorState::Faulted: return "Faulted";
    }
    return "Unknown";
}

// printf into a std::string — lets each table row format its value inline in one expression.
std::string fmt(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return std::string(buf);
}

// Status registers are rendered as a single "bitmap by name": each named bit/field
// shown as name=value, so the whole register reads at a glance on one row.
std::string adcStatusBits(const AdcStatus& s) {
    return fmt("initialized=%u data_valid=%u", s.initialized, s.data_valid);
}
std::string valveStatusBits(const ValveStatus& s) {
    return fmt("initialized=%u open_limit_high=%u closed_limit_high=%u in_transition=%u fault_both_switches=%u",
               s.initialized, s.open_limit_high, s.closed_limit_high, s.in_transition, s.fault_both_switches);
}
std::string storageStatusBits(const StorageStatus& s) {
    return fmt("initialized=%u plugged_in=%u error=%s",
               s.initialized, s.plugged_in, storageErrorName(s.error));
}
std::string canStatusBits(const CanStatus& s) {
    return fmt("initialized=%u tx_error=%u", s.initialized, s.tx_error);
}
std::string ethStatusBits(const EthernetStatus& s) {
    return fmt("initialized=%u tx_busy=%u tx_error=%u", s.initialized, s.tx_busy, s.tx_error);
}
std::string thermocoupleStatusBits(const ThermocoupleStatus& s) {
    return fmt("open_circuit=%u over_under_v=%u tc_out_range=%u cj_out_range=%u data_valid=%u comms_ok=%u",
               s.open_circuit, s.over_under_v, s.tc_out_range, s.cj_out_range, s.data_valid, s.comms_ok);
}
std::string powerMonitorStatusBits(const PowerMonitorStatus& s) {
    return fmt("data_valid=%u read_error=%u", s.data_valid, s.read_error);
}

constexpr const char* kAbsent = "-"; // field not present on this board (e.g. ECU has no Ethernet)

// A "Field | ECU | FCU" telemetry table. The body is a flat list of fieldRow() calls
// (captured via the FieldTable helper), so adding a field is a single new line — see
// renderSystemStateTable / renderExtendedStateTable below.
struct FieldTable {
    bool open = false;
    explicit FieldTable(const char* id) {
        constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
                                        | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable;
        open = ImGui::BeginTable(id, 3, flags);
        if (open) {
            ImGui::TableSetupColumn("Field");
            ImGui::TableSetupColumn("ECU");
            ImGui::TableSetupColumn("FCU");
            ImGui::TableHeadersRow();
        }
    }
    ~FieldTable() {
        if (open) {
            ImGui::EndTable();
        }
    }
    // A group separator: a labelled divider spanning the table, so dense records
    // (the ExtendedSystemState) read as grouped sections instead of one long list.
    void section(const char* label) {
        if (!open) {
            return;
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::SeparatorText(label);
    }
    // One field: its name and the per-board values (already formatted). Pass kAbsent
    // for a board that does not carry the field.
    void row(const char* field, const std::string& ecu, const std::string& fcu) {
        if (!open) {
            return;
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::TextUnformatted(field);
        ImGui::TableNextColumn(); ImGui::TextUnformatted(ecu.c_str());
        ImGui::TableNextColumn(); ImGui::TextUnformatted(fcu.c_str());
    }
};

// Render every field of EcuSystemState (left) and FcuSystemState (right) side by side.
// boardState is the header sender_state (not part of the record). To add a field, add
// one t.row(...) line.
void renderSystemStateTable(const EcuSystemState& ecu, uint8_t ecuBoardState,
                            const FcuSystemState& fcu, uint8_t fcuBoardState) {
    const SystemStateBase& e = ecu.base;
    const SystemStateBase& f = fcu.base;
    FieldTable t("systemstate_fields");

    t.row("board_state (header)", boardStateName(ecuBoardState), boardStateName(fcuBoardState));
    t.row("creation_timestamp_ms", fmt("%u", e.creation_timestamp_ms), fmt("%u", f.creation_timestamp_ms));

    t.row("adc.state", adcStateName(e.adc_info.state), adcStateName(f.adc_info.state));
    t.row("adc.status", adcStatusBits(e.adc_info.status), adcStatusBits(f.adc_info.status));
    for (unsigned i = 0; i < ADC_CHANNEL_COUNT; i++) {
        t.row(fmt("adc.channels[%u]", i).c_str(), fmt("%d", e.adc_info.channels[i]), fmt("%d", f.adc_info.channels[i]));
    }

    // valve_info[0] = NOS (ECU) / Fill (FCU); valve_info[1] = IPA (ECU) / Dump (FCU).
    for (int v = 0; v < 2; v++) {
        const ValveInfo& ev = e.valve_info[v];
        const ValveInfo& fv = f.valve_info[v];
        const char* tag = (v == 0) ? "valve[0] NOS/Fill" : "valve[1] IPA/Dump";
        t.row(fmt("%s.state", tag).c_str(), valveStateName(static_cast<uint8_t>(ev.state)), valveStateName(static_cast<uint8_t>(fv.state)));
        t.row(fmt("%s.status", tag).c_str(), valveStatusBits(ev.status), valveStatusBits(fv.status));
        t.row(fmt("%s.current_set_value", tag).c_str(), fmt("%u", ev.current_set_value), fmt("%u", fv.current_set_value));
    }

    t.row("storage.state", storageStateName(e.storage_info.state), storageStateName(f.storage_info.state));
    t.row("storage.status", storageStatusBits(e.storage_info.status), storageStatusBits(f.storage_info.status));
    t.row("storage.overrun_count", fmt("%u", e.storage_info.overrun_count), fmt("%u", f.storage_info.overrun_count));

    t.row("can.state", canStateName(e.can_info.state), canStateName(f.can_info.state));
    t.row("can.status", canStatusBits(e.can_info.status), canStatusBits(f.can_info.status));
    t.row("can.rx_dropped", fmt("%u", e.can_info.rx_dropped), fmt("%u", f.can_info.rx_dropped));

    // Ethernet is FCU-only (the ECU record has no eth_info).
    t.row("eth.state", kAbsent, ethStateName(fcu.eth_info.state));
    t.row("eth.status", kAbsent, ethStatusBits(fcu.eth_info.status));
    t.row("eth.rx_dropped", kAbsent, fmt("%u", fcu.eth_info.rx_dropped));
}

// Render every field of EcuExtendedSystemState (left) and FcuExtendedSystemState (right).
// Thermocouples are FCU-only. To add a field, add one t.row(...) line.
void renderExtendedStateTable(const EcuExtendedSystemState& ecu, const FcuExtendedSystemState& fcu) {
    FieldTable t("extendedstate_fields");

    const ExtendedSystemStateBase& eb = ecu.base;
    const ExtendedSystemStateBase& fb = fcu.base;

    t.section("Base");
    t.row("creation_timestamp_ms", fmt("%u", eb.creation_timestamp_ms), fmt("%u", fb.creation_timestamp_ms));
    t.row("control_flags_base", fmt("0x%02X", eb.control_flags_base), fmt("0x%02X", fb.control_flags_base));
    t.row("  bit FastRecording", fmt("%u", (eb.control_flags_base >> static_cast<unsigned>(ControlFlagBase::FastRecording)) & 1u),
                                 fmt("%u", (fb.control_flags_base >> static_cast<unsigned>(ControlFlagBase::FastRecording)) & 1u));
    t.row("  bit DisableLogging", fmt("%u", (eb.control_flags_base >> static_cast<unsigned>(ControlFlagBase::DisableLogging)) & 1u),
                                  fmt("%u", (fb.control_flags_base >> static_cast<unsigned>(ControlFlagBase::DisableLogging)) & 1u));
    t.row("control_flags_board", fmt("0x%02X", eb.control_flags_board), fmt("0x%02X", fb.control_flags_board));
    t.row("  bit SolenoidValve", kAbsent, // FCU per-board flag; the ECU has no per-board flags
          fmt("%u", (fb.control_flags_board >> static_cast<unsigned>(FcuControlFlag::SolenoidValve)) & 1u));
    t.row("  bit Heater", kAbsent, // FCU per-board flag; the ECU has no per-board flags
          fmt("%u", (fb.control_flags_board >> static_cast<unsigned>(FcuControlFlag::Heater)) & 1u));
    // Backup-domain retention health probed at boot (logic::control::BackupStatus); raw byte.
    t.row("backup_status", fmt("0x%02X", eb.backup_status), fmt("0x%02X", fb.backup_status));
    // Whole seconds since this board last heard a Ping (the GS heartbeat); saturates at 255.
    t.row("seconds_since_last_ping", fmt("%u", eb.seconds_since_last_ping), fmt("%u", fb.seconds_since_last_ping));

    // Board-wide async SD write-engine health (one card / one SDMMC / one engine per board).
    // Distinct from the high-rate storage.overrun_count: this counts whole blocks the engine
    // dropped because its write ring was full, plus a sticky DMA-fault flag.
    t.section("SD write engine");
    const SdWriteEngineInfo& es = eb.sd_write_engine_info;
    const SdWriteEngineInfo& fs = fb.sd_write_engine_info;
    t.row("sd_engine.overrun_count", fmt("%u", es.overrun_count), fmt("%u", fs.overrun_count));
    t.row("sd_engine.errored", fmt("%u", es.errored), fmt("%u", fs.errored));
    t.row("sd_engine.card_detected", fmt("%u", es.card_detected), fmt("%u", fs.card_detected));

    t.section("Refused commands");
    const RefusedCommandInfo& er = eb.refused_command_info;
    const RefusedCommandInfo& fr = fb.refused_command_info;
    t.row("refused.set_state_from", boardStateName(er.set_state_from), boardStateName(fr.set_state_from));
    t.row("refused.set_state_to", boardStateName(er.set_state_to), boardStateName(fr.set_state_to));
    t.row("refused.set_state_refused_count", fmt("%u", er.set_state_refused_count), fmt("%u", fr.set_state_refused_count));
    t.row("refused.set_flag_id", fmt("0x%04X", er.set_flag_id), fmt("0x%04X", fr.set_flag_id));
    t.row("refused.set_flag_value", fmt("%u", er.set_flag_value), fmt("%u", fr.set_flag_value));
    t.row("refused.set_flag_state", boardStateName(er.set_flag_state), boardStateName(fr.set_flag_state));
    t.row("refused.set_flag_refused_count", fmt("%u", er.set_flag_refused_count), fmt("%u", fr.set_flag_refused_count));
    t.row("refused.set_valve_id", fmt("0x%02X", er.set_valve_id), fmt("0x%02X", fr.set_valve_id));
    t.row("refused.set_valve_action", fmt("%u", er.set_valve_action), fmt("%u", fr.set_valve_action));
    t.row("refused.set_valve_value", fmt("%u", er.set_valve_value), fmt("%u", fr.set_valve_value));
    t.row("refused.set_valve_state", boardStateName(er.set_valve_state), boardStateName(fr.set_valve_state));
    t.row("refused.set_valve_refused_count", fmt("%u", er.set_valve_refused_count), fmt("%u", fr.set_valve_refused_count));

    // FCU heater — a bare on/off output (FCU-only). status.on echoes the commanded Heater
    // control flag; the timestamps are the ticks of the last on/off edges.
    t.section("Heater (FCU)");
    const HeaterInfo& fh = fcu.heater_info;
    t.row("heater.on", kAbsent, fmt("%u", fh.status.on));
    t.row("heater.last_on_ms", kAbsent, fmt("%u", fh.last_on_ms));
    t.row("heater.last_off_ms", kAbsent, fmt("%u", fh.last_off_ms));

    t.section("Thermocouples (FCU)");
    for (unsigned i = 0; i < THERMOCOUPLE_COUNT; i++) {
        const ThermocoupleInfo& tc = fcu.thermocouple_info[i];
        t.row(fmt("tc[%u].state", i).c_str(), kAbsent, thermocoupleStateName(static_cast<uint8_t>(tc.state)));
        t.row(fmt("tc[%u].status", i).c_str(), kAbsent, thermocoupleStatusBits(tc.status));
        t.row(fmt("tc[%u].thermocouple_code", i).c_str(), kAbsent, fmt("%d (%.3f C)", tc.thermocouple_code, tc.thermocouple_code / 128.0f));
        t.row(fmt("tc[%u].cold_junction_code", i).c_str(), kAbsent, fmt("%d (%.3f C)", tc.cold_junction_code, tc.cold_junction_code / 64.0f));
    }

    // INA3221 power monitor — now on both boards. shunt_code LSB = 40 uV, bus_code LSB = 8 mV.
    // Current = shunt voltage / Rshunt; with Rshunt = 0.1 ohm, 1 code = 40 uV / 0.1 ohm = 0.4 mA.
    t.section("Power monitor (INA3221)");
    const PowerMonitorInfo& epm = ecu.power_monitor;
    const PowerMonitorInfo& fpm = fcu.power_monitor;
    t.row("power_monitor.state", powerMonitorStateName(epm.state), powerMonitorStateName(fpm.state));
    t.row("power_monitor.status", powerMonitorStatusBits(epm.status), powerMonitorStatusBits(fpm.status));
    for (unsigned i = 0; i < POWER_MONITOR_CHANNEL_COUNT; i++) {
        const PowerMonitorChannel& ech = epm.channels[i];
        const PowerMonitorChannel& fch = fpm.channels[i];
        t.row(fmt("power_monitor.ch[%u].shunt_code", i).c_str(), fmt("%d (%.3f mA)", ech.shunt_code, ech.shunt_code * kShuntCodeToMilliAmp),
                                                                 fmt("%d (%.3f mA)", fch.shunt_code, fch.shunt_code * kShuntCodeToMilliAmp));
        t.row(fmt("power_monitor.ch[%u].bus_code", i).c_str(), fmt("%d (%.3f V)", ech.bus_code, ech.bus_code * kBusCodeToVolt),
                                                               fmt("%d (%.3f V)", fch.bus_code, fch.bus_code * kBusCodeToVolt));
    }
}

// Render the received GSSystemState (the ground-station panel state). This is a single source
// (not split ECU/FCU), so it is a plain Field | Value table rather than the per-board layout.
void renderGsSystemStateTable(const GSSystemState& gs) {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
                                    | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable;
    if (!ImGui::BeginTable("gssystemstate_fields", 2, flags)) {
        return;
    }
    ImGui::TableSetupColumn("Field");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();

    auto row = [](const char* field, const std::string& value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::TextUnformatted(field);
        ImGui::TableNextColumn(); ImGui::TextUnformatted(value.c_str());
    };

    row("isValveStartButtonPressed", fmt("%u", static_cast<unsigned>(gs.isValveStartButtonPressed)));
    row("isAllowFillSwitchOn", fmt("%u", static_cast<unsigned>(gs.isAllowFillSwitchOn)));
    row("isArmServoSwitchOn", fmt("%u", static_cast<unsigned>(gs.isArmServoSwitchOn)));
    row("isArmIgniterSwitchOn", fmt("%u", static_cast<unsigned>(gs.isArmIgniterSwitchOn)));
    row("isAllowDumpSwitchOn", fmt("%u", static_cast<unsigned>(gs.isAllowDumpSwitchOn)));
    row("isEmergencyStopButtonPressed", fmt("%u", static_cast<unsigned>(gs.isEmergencyStopButtonPressed)));
    row("isFireIgniterButtonPressed", fmt("%u", static_cast<unsigned>(gs.isFireIgniterButtonPressed)));
    row("isUnsafeKeySwitchPressed", fmt("%u", static_cast<unsigned>(gs.isUnsafeKeySwitchPressed)));
    row("fcuState", fmt("%u", static_cast<unsigned>(gs.fcuState)));
    row("ecuState", fmt("%u", static_cast<unsigned>(gs.ecuState)));

    ImGui::EndTable();
}

// Test action: fire `count` GSSystemState telemetry frames straight to an explicit IP (bypassing
// the configured command destination), with only the last 2 bytes (fcuState/ecuState) populated.
// Each is a self-contained EthernetHeader + GSSystemState + CRC-32 frame, same wire format as the
// boards' telemetry, so the receiver decodes it through processGsSystemStatePacket.
void sendGsSystemStateBurst(const char* ip, uint16_t port, uint8_t count) {
    if (!ComTask::com) {
        GCS_APP_LOG_WARN("CommunicationWindow: cannot send GsSystemState — no com interface.");
        return;
    }
    unsigned sent = 0;
    for (uint8_t i = 0; i < count; i++) {
        GSSystemState gs{};   // zero-init leaves the first 2 bytes (the switch/button bitfields) clear
        gs.fcuState = i;      // only the last 2 bytes are populated
        gs.ecuState = i;

        uint8_t frame[sizeof(EthernetHeader) + sizeof(GSSystemState) + sizeof(uint32_t)] = {0};
        EthernetHeader header{};
        header.sender_id = static_cast<uint32_t>(BoardId::GsControl);
        header.target_id = static_cast<uint32_t>(BoardId::GsControl); // addressed to board id 3 (GsControl)
        header.payload_type = static_cast<uint32_t>(PayloadType::Telemetry);
        header.payload_id = static_cast<uint32_t>(TelemetryType::SystemState);
        header.payload_size_bytes = sizeof(GSSystemState);
        header.sender_state = 0;
        header.seq = 0;                 // telemetry carries no command seq
        header.sender_timestamp_ms = 0; // empty for GS

        std::memcpy(frame, &header, sizeof(header));
        std::memcpy(frame + sizeof(header), &gs, sizeof(gs));
        const size_t crcRange = sizeof(header) + sizeof(gs);
        const uint32_t crc = CRC::computeCrc32(frame, crcRange);
        std::memcpy(frame + crcRange, &crc, sizeof(crc));

        if (ComTask::com->writeTo(std::span<const uint8_t>(frame, sizeof(frame)), ip, port)) {
            sent++;
        }
    }
    if (sent == count) {
        GCS_APP_LOG_INFO("CommunicationWindow: Sent {}/{} GsSystemState to {}:{}", sent, count, ip, port);
    } else {
        GCS_APP_LOG_ERROR("CommunicationWindow: Only sent {}/{} GsSystemState to {}:{} — writeTo failed (socket not open / send error).",
                          sent, count, ip, port);
    }
}

// One valve's command row that enqueues a SetValvePosition. ECU valves are binary
// (the board only accepts Open/Close), so they show just Closed/Open; FCU valves
// also expose the intermediate percentage positions. The per-valve "Force" checkbox
// ORs VALVE_FORCE_FLAG into the command value, requesting a switch-bypassed actuation
// (the board honours it for Open/Close and ignores it for the percentage positions).
void renderValveCommandRow(const char* name, CommandType valveCmd, bool binaryOnly, bool showName = true) {
    static std::map<CommandType, bool> forced; // per-valve Force checkbox state, keyed by command
    bool& force = forced[valveCmd];
    const auto valveValue = [&force](uint32_t pct) { return force ? (pct | VALVE_FORCE_FLAG) : pct; };

    ImGui::PushID(name);
    if (showName) {
        ImGui::Text("%-5s", name);
        ImGui::SameLine();
    }
    // Closed (0%), then 10% increments for non-binary valves, then Open (100%), then Force.
    if (ImGui::SmallButton("Cl")) { CommandControl::sendCommand(valveCmd, valveValue(0)); }
    if (!binaryOnly) {
        for (uint32_t pct = 10; pct <= 90; pct += 10) {
            ImGui::SameLine();
            char lbl[4];
            std::snprintf(lbl, sizeof(lbl), "%u", pct);
            if (ImGui::SmallButton(lbl)) { CommandControl::sendCommand(valveCmd, valveValue(pct)); }
        }
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Op")) { CommandControl::sendCommand(valveCmd, valveValue(100)); }
    ImGui::SameLine();
    ImGui::Checkbox("F", &force);
    ImGui::PopID();
}

// Enqueues a generic SetControlFlag command for a base (network-wide) flag. The flag id
// and on/off bit are packed into the command value as (flagId << 1) | on; CommandControl
// unpacks them and broadcasts the frame.
void sendBaseControlFlag(ControlFlagBase flag, bool on) {
    const uint32_t packed = (static_cast<uint32_t>(flag) << 1) | (on ? 1u : 0u);
    CommandControl::sendCommand(CommandType::SetControlFlag, packed);
}

// Off/On toggle for a single base flag. Each button sends exactly ONE SetControlFlag so the
// flag is set reliably (a multi-command sequence would have its first command preempted by
// the second). Telemetry is always persisted to the SD card; FastRecording only picks the
// SystemState log rate:
//   FastRecording off -> slow (100 Hz -> data_slow.bin)
//   FastRecording on  -> fast (2 kHz  -> data_fast.bin)
// ExtendedSystemState (data_ext.bin) is logged regardless. Confirm what the boards applied
// via the live control_flags_base bits in the table above.
void renderBaseFlagToggle(const char* label, ControlFlagBase flag) {
    ImGui::PushID(label);
    ImGui::Text("%-12s", label);
    ImGui::SameLine();
    if (ImGui::Button("Off")) { sendBaseControlFlag(flag, false); }
    ImGui::SameLine();
    if (ImGui::Button("On")) { sendBaseControlFlag(flag, true); }
    ImGui::PopID();
}

// Enqueues a SetState command requesting the given network-wide state.
void renderSetStateButton(const char* label, logic::control::State state) {
    if (ImGui::Button(label)) {
        CommandControl::sendCommand(CommandType::SetState, static_cast<uint32_t>(state));
    }
}

// ============================================================================
// Dashboard helpers (the compact operational view)
// ============================================================================

// The channel->sensor mapping + calibration live in SensorChannelMap.h (edit that to recalibrate).
using namespace SensorChannelMap;

// A small chip whose BACKGROUND is green (on) or red (off), with the label drawn on top.
// This is the one colouring primitive for every status indicator (rail-style): bits, valve
// states, on/off toggles. Drawn directly (no widget id) so it never collides in a table.
void colorChip(const char* label, bool on) {
    const ImU32 bg = on ? static_cast<ImU32>(ThemedColors::Button::green.resolve())
                        : static_cast<ImU32>(ThemedColors::Button::red.resolve());
    const ImVec2 pad = ImGui::GetStyle().FramePadding;
    const ImVec2 textSize = ImGui::CalcTextSize(label);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 size(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), bg, 3.0f);
    dl->AddText(ImVec2(p.x + pad.x, p.y + pad.y), ImGui::GetColorU32(ImGuiCol_Text), label);
    ImGui::Dummy(size);
}

// A content-width section title (unlike ImGui::SeparatorText, which spans the full content
// region and would force a side-by-side group to the full window width).
void sectionHeader(const char* label) {
    ImGui::Spacing();
    ImGui::TextColored(ThemedColors::Text::blue.resolve(), "%s", label);
}

} // namespace

const char* const CommunicationWindow::name = "Dashboard";


const char* CommunicationWindow::getName() const {
    return "Dashboard";
}

void CommunicationWindow::renderImpl() {
    if (ImGui::BeginTabBar("comm_tabs")) {
        if (ImGui::BeginTabItem("Dashboard")) {
            renderDashboardTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Raw")) {
            renderRawTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Setup")) {
            renderSetupTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void CommunicationWindow::renderDashboardTab() {
    const double ecuSs = ecuSsRate.update(GSDataCenter::ecuSystemStateCount.load());
    const double ecuExt = ecuExtRate.update(GSDataCenter::ecuExtendedSystemStateCount.load());
    const double fcuSs = fcuSsRate.update(GSDataCenter::fcuSystemStateCount.load());
    const double fcuExt = fcuExtRate.update(GSDataCenter::fcuExtendedSystemStateCount.load());

    const EcuExtendedSystemState ecuExtRec = GSDataCenter::latestEcuExtendedSystemState.load();
    const FcuExtendedSystemState fcuExtRec = GSDataCenter::latestFcuExtendedSystemState.load();
    const EcuSystemState ecuSsRec = GSDataCenter::latestEcuSystemState.load();
    const FcuSystemState fcuSsRec = GSDataCenter::latestFcuSystemState.load();

    const auto ecuAdc = GSDataCenter::motorAdcAverager.latestAverage();
    const auto fcuAdc = GSDataCenter::fillingStationAdcAverager.latestAverage();
    const auto adcOf = [&](bool fcu, int ch) -> float {
        const auto& a = fcu ? fcuAdc : ecuAdc;
        return (ch >= 0 && ch < static_cast<int>(a.size())) ? a[ch] * kAdcScale : 0.0f;
    };

    // --- Convert this frame's raw averages to engineering units, then sample them into the
    // active windows at a fixed 100 Hz (decoupled from the UI frame rate). Each elapsed 10 ms
    // tick records one sample (zero-order hold), so "current" updates 100x/s and the 2 s
    // average / since-start max are well-defined regardless of how fast we render. ---
    std::array<float, kPressureCount> vPressure{};
    for (std::size_t i = 0; i < pressurePsi.size(); i++) {
        vPressure[i] = PressureTransducer::adcToPressure_psi(adcOf(kPressure[i].fcu, kPressure[i].adcChannel), kPressure[i].sensorIndex);
    }
    std::array<float, kThermistorCount> vTemp{};
    for (std::size_t i = 0; i < tempC.size(); i++) {
        vTemp[i] = TemperatureSensor::adcToTemperature_C(adcOf(kThermistor[i].fcu, kThermistor[i].adcChannel));
    }
    std::array<float, kLoadCount> vLoad{};
    for (std::size_t i = 0; i < loadLb.size(); i++) {
        vLoad[i] = LoadCell::adcToWeight_lb(adcOf(kLoad[i].fcu, kLoad[i].adcChannel), kLoad[i].loadCellIndex);
    }
    std::array<float, kThermocoupleCount> vTc{};
    for (std::size_t i = 0; i < thermocoupleC.size(); i++) {
        vTc[i] = GSDataCenter::fillingStationThermocouple_C[kThermocouple[i].index].load();
    }
    const float tankTemp_C = vTemp[kTankThermistor];
    const float tankPress_psi = vPressure[kTankPressure];

    const auto nowTp = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(nowTp - lastSampleTime).count();
    std::size_t ticks = (elapsedMs > 0) ? static_cast<std::size_t>(elapsedMs / 10) : 0;
    if (ticks > SignalStat::WINDOW) {
        ticks = SignalStat::WINDOW; // cap catch-up after a long stall
    }
    lastSampleTime += std::chrono::milliseconds(static_cast<long long>(ticks) * 10);
    for (std::size_t t = 0; t < ticks; t++) {
        for (std::size_t i = 0; i < pressurePsi.size(); i++) {
            pressurePsi[i].push(vPressure[i]);
        }
        for (std::size_t i = 0; i < tempC.size(); i++) {
            tempC[i].push(vTemp[i]);
        }
        for (std::size_t i = 0; i < loadLb.size(); i++) {
            loadLb[i].push(vLoad[i]);
        }
        for (std::size_t i = 0; i < thermocoupleC.size(); i++) {
            thermocoupleC[i].push(vTc[i]);
        }
    }

    // The whole dashboard is two side-by-side groups (control/status left, telemetry right);
    // each group sizes to its own content so the tables stack normally instead of overlapping.
    ImGui::BeginGroup(); // ===== LEFT: states, ping, valves, status =====

    // --- States: one row per board; rails as columns (3.3V/5V/12V), each cell V over mA. ---
    sectionHeader("States");
    if (ImGui::BeginTable("states_bar", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("Board");
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("SS Hz");
        ImGui::TableSetupColumn("Ext Hz");
        ImGui::TableSetupColumn("3.3V");
        ImGui::TableSetupColumn("5V");
        ImGui::TableSetupColumn("12V");
        ImGui::TableHeadersRow();

        const auto boardRow = [](const char* boardName, uint8_t state, double ss, double ext, const PowerMonitorInfo& pm) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(boardName);
            ImGui::TableNextColumn(); ImGui::TextUnformatted(boardStateName(state));
            ImGui::TableNextColumn(); ImGui::Text("%.0f", ss);
            ImGui::TableNextColumn(); ImGui::Text("%.1f", ext);
            for (unsigned i = 0; i < POWER_MONITOR_CHANNEL_COUNT; i++) {
                ImGui::TableNextColumn();
                ImGui::Text("%.2f V\n%.1f mA", pm.channels[i].bus_code * kBusCodeToVolt,
                            pm.channels[i].shunt_code * kShuntCodeToMilliAmp);
            }
        };
        boardRow("ECU", GSDataCenter::motorBoardState.load(), ecuSs, ecuExt, ecuExtRec.power_monitor);
        boardRow("FCU", GSDataCenter::fillingStationBoardState.load(), fcuSs, fcuExt, fcuExtRec.power_monitor);

        ImGui::EndTable();
    }

    // The 1 Hz auto-ping heartbeat is always enabled (no button); we only surface how long
    // since the last successful ping (a pong received), tracked GS-side.
    ComTask::autoPing.store(true);
    const uint32_t pongCount = GSDataCenter::pongReceivedCount.load();
    if (pongCount != lastPongCountSeen) {
        lastPongCountSeen = pongCount;
        lastPongTime = std::chrono::steady_clock::now();
    }
    if (pongCount == 0) {
        ImGui::TextUnformatted("Time since last successful Ping (s): never");
    } else {
        const double since = std::chrono::duration<double>(std::chrono::steady_clock::now() - lastPongTime).count();
        ImGui::Text("Time since last successful Ping (s): %.1f", since);
    }

    // --- Valves: name in its own column (so the buttons align) | command buttons (Closed,
    // 10% increments, Open, Force) | live bit-field-like state (current state green). ---
    sectionHeader("Valves");
    if (ImGui::BeginTable("valves", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("Valve");
        ImGui::TableSetupColumn("Command");
        ImGui::TableSetupColumn("State");
        ImGui::TableHeadersRow();

        const auto valveStateCell = [](const ValveInfo& v) {
            ImGui::TableNextColumn();
            const auto vs = static_cast<ValveState>(static_cast<uint8_t>(v.state));
            const bool fault = (vs == ValveState::Faulted) || v.status.fault_both_switches;
            colorChip(fault ? "FAULT" : "OK", !fault);
            static constexpr std::pair<const char*, ValveState> kValveStates[] = {
                {"Closed", ValveState::Closed}, {"Closing", ValveState::Closing}, {"Opening", ValveState::Opening},
                {"Opened", ValveState::Opened}, {"Floating", ValveState::Floating}};
            for (const auto& [nm, st] : kValveStates) {
                ImGui::SameLine();
                colorChip(nm, vs == st);
            }
        };
        const auto valveRow = [&](const char* name, CommandType cmd, bool binaryOnly, const ValveInfo& v) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(name);
            ImGui::TableNextColumn(); renderValveCommandRow(name, cmd, binaryOnly, /*showName=*/false);
            valveStateCell(v);
        };
        valveRow("NOS", CommandType::NosValve, /*binaryOnly=*/true, ecuSsRec.base.valve_info[0]);
        valveRow("IPA", CommandType::IpaValve, /*binaryOnly=*/true, ecuSsRec.base.valve_info[1]);
        valveRow("Fill", CommandType::FillValve, /*binaryOnly=*/false, fcuSsRec.base.valve_info[0]);
        valveRow("Dump", CommandType::DumpValve, /*binaryOnly=*/false, fcuSsRec.base.valve_info[1]);

        // Solenoid (FCU) — toggle + open/closed state from the SolenoidValve control flag.
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::TextUnformatted("Sol");
        ImGui::TableNextColumn();
        ImGui::PushID("Solenoid");
        if (ImGui::SmallButton("Cl")) { CommandControl::sendCommand(CommandType::SolenoidValve, 0); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Op")) { CommandControl::sendCommand(CommandType::SolenoidValve, 1); }
        ImGui::PopID();
        ImGui::TableNextColumn();
        {
            const bool open = (fcuExtRec.base.control_flags_board >> static_cast<unsigned>(FcuControlFlag::SolenoidValve)) & 1u;
            colorChip(open ? "Open" : "Closed", open);
        }

        // Heater (FCU) — toggle + on/off state from heater_info.status.on.
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::TextUnformatted("Heat");
        ImGui::TableNextColumn();
        ImGui::PushID("Heater");
        if (ImGui::SmallButton("Off")) { CommandControl::sendCommand(CommandType::Heater, 0); }
        ImGui::SameLine();
        if (ImGui::SmallButton("On")) { CommandControl::sendCommand(CommandType::Heater, 1); }
        ImGui::PopID();
        ImGui::TableNextColumn();
        colorChip(fcuExtRec.heater_info.status.on ? "On" : "Off", fcuExtRec.heater_info.status.on);

        ImGui::EndTable();
    }

    // --- Status matrix: Device | ECU state | ECU bits | FCU state | FCU bits ---
    sectionHeader("Status");
    if (ImGui::BeginTable("status_matrix", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("Device");
        ImGui::TableSetupColumn("ECU state");
        ImGui::TableSetupColumn("ECU bits");
        ImGui::TableSetupColumn("FCU state");
        ImGui::TableSetupColumn("FCU bits");
        ImGui::TableHeadersRow();

        const auto boardCells = [](const char* state, std::initializer_list<std::pair<const char*, unsigned>> bitList) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(state);
            ImGui::TableNextColumn();
            bool first = true;
            for (const auto& [bitName, value] : bitList) {
                if (!first) {
                    ImGui::SameLine();
                }
                first = false;
                colorChip(bitName, value != 0);
            }
        };
        const auto absentBoard = []() {
            ImGui::TableNextColumn(); ImGui::TextDisabled("-");
            ImGui::TableNextColumn(); ImGui::TextDisabled("-");
        };
        const auto deviceName = [](const char* n) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(n);
        };

        const auto& eAdc = ecuSsRec.base.adc_info;
        const auto& fAdc = fcuSsRec.base.adc_info;
        deviceName("ADC");
        boardCells(adcStateName(eAdc.state), {{"init", eAdc.status.initialized}, {"valid", eAdc.status.data_valid}});
        boardCells(adcStateName(fAdc.state), {{"init", fAdc.status.initialized}, {"valid", fAdc.status.data_valid}});

        const auto& eSto = ecuSsRec.base.storage_info;
        const auto& fSto = fcuSsRec.base.storage_info;
        deviceName("Storage");
        boardCells(storageStateName(eSto.state), {{"init", eSto.status.initialized}, {"plugged", eSto.status.plugged_in}});
        boardCells(storageStateName(fSto.state), {{"init", fSto.status.initialized}, {"plugged", fSto.status.plugged_in}});

        const auto& eCan = ecuSsRec.base.can_info;
        const auto& fCan = fcuSsRec.base.can_info;
        deviceName("CAN");
        boardCells(canStateName(eCan.state), {{"init", eCan.status.initialized}, {"tx_err", eCan.status.tx_error}});
        boardCells(canStateName(fCan.state), {{"init", fCan.status.initialized}, {"tx_err", fCan.status.tx_error}});

        deviceName("Ethernet");
        absentBoard(); // ECU has no Ethernet
        {
            const auto& fEth = fcuSsRec.eth_info;
            boardCells(ethStateName(fEth.state),
                       {{"init", fEth.status.initialized}, {"tx_busy", fEth.status.tx_busy}, {"tx_err", fEth.status.tx_error}});
        }

        const auto& ePm = ecuExtRec.power_monitor;
        const auto& fPm = fcuExtRec.power_monitor;
        deviceName("Power mon.");
        boardCells(powerMonitorStateName(ePm.state), {{"valid", ePm.status.data_valid}, {"rd_err", ePm.status.read_error}});
        boardCells(powerMonitorStateName(fPm.state), {{"valid", fPm.status.data_valid}, {"rd_err", fPm.status.read_error}});

        for (std::size_t i = 0; i < kThermocoupleCount; i++) { // thermocouples (FCU): Top, Throat
            const ThermocoupleInfo& tc = fcuExtRec.thermocouple_info[kThermocouple[i].index];
            deviceName(kThermocouple[i].label);
            absentBoard();
            boardCells(thermocoupleStateName(static_cast<uint8_t>(tc.state)),
                       {{"open", tc.status.open_circuit},
                        {"ouv", tc.status.over_under_v},
                        {"tc_rng", tc.status.tc_out_range},
                        {"cj_rng", tc.status.cj_out_range},
                        {"valid", tc.status.data_valid},
                        {"comms", tc.status.comms_ok}});
        }

        deviceName("Heater");
        absentBoard();
        boardCells(fcuExtRec.heater_info.status.on ? "On" : "Off", {{"on", fcuExtRec.heater_info.status.on}});

        // Ematch / Solenoid (FCU only) — no state enum; derive a short label from the status bits.
        const EmatchStatus& es = fcuExtRec.ematch_info.status;
        deviceName("Ematch");
        absentBoard();
        boardCells(es.energised ? "Energised" : (es.detected ? "Idle" : "Absent"),
                   {{"det", es.detected}, {"energ", es.energised}});

        const SolenoidStatus& ss = fcuExtRec.solenoid_info.status;
        deviceName("Solenoid");
        absentBoard();
        boardCells(ss.open ? "Open" : (ss.detected ? "Closed" : "Absent"),
                   {{"det", ss.detected}, {"open", ss.open}});

        ImGui::EndTable();
    }

    ImGui::EndGroup();   // end LEFT
    ImGui::SameLine(0.0f, 20.0f);
    ImGui::BeginGroup(); // ===== RIGHT: telemetry, logging =====
    sectionHeader("Telemetry");

    // Pressure transducers — current (100 Hz) / 2 s average / since-start max, plus the raw
    // averaged ADC.
    if (ImGui::BeginTable("pt", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("PT");
        ImGui::TableSetupColumn("cur (psi)");
        ImGui::TableSetupColumn("avg 2s");
        ImGui::TableSetupColumn("max");
        ImGui::TableSetupColumn("raw");
        ImGui::TableHeadersRow();
        for (std::size_t i = 0; i < pressurePsi.size(); i++) {
            const float raw = adcOf(kPressure[i].fcu, kPressure[i].adcChannel);
            ImGui::PushID(kPressure[i].label);
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(kPressure[i].label);
            ImGui::TableNextColumn(); ImGui::Text("%.2f", pressurePsi[i].now());
            ImGui::TableNextColumn(); ImGui::Text("%.2f", pressurePsi[i].avg());
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", pressurePsi[i].max());
            ImGui::SameLine();
            if (ImGui::SmallButton("rst")) {
                pressurePsi[i].resetPeak();
            }
            ImGui::TableNextColumn(); ImGui::Text("%.0f", raw);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    // Load cells — current (100 Hz) / 2 s average / since-start max, raw ADC. The tank load
    // cell also takes three calibration mass entries (Empty / IPA / NOS); the thrust LC has none.
    if (ImGui::BeginTable("lc", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("LC");
        ImGui::TableSetupColumn("cur");
        ImGui::TableSetupColumn("avg 2s");
        ImGui::TableSetupColumn("max");
        ImGui::TableSetupColumn("raw");
        ImGui::TableSetupColumn("cal masses (kg)");
        ImGui::TableHeadersRow();
        for (std::size_t i = 0; i < loadLb.size(); i++) {
            const float raw = adcOf(kLoad[i].fcu, kLoad[i].adcChannel);
            const float scale = kLoad[i].displayScale; // converter outputs lb; show in N / kg
            const bool isTank = (static_cast<int>(i) == kTankLoad);
            ImGui::PushID(kLoad[i].label);
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(kLoad[i].label);
            ImGui::TableNextColumn(); ImGui::Text("%.2f", loadLb[i].now() * scale);
            ImGui::TableNextColumn(); ImGui::Text("%.2f", loadLb[i].avg() * scale);
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", loadLb[i].max() * scale);
            ImGui::SameLine();
            if (ImGui::SmallButton("rst")) {
                loadLb[i].resetPeak();
            }
            ImGui::TableNextColumn(); ImGui::Text("%.0f", raw);
            ImGui::TableNextColumn();
            if (isTank) {
                ImGui::SetNextItemWidth(64); ImGui::InputFloat("Empty", &tankLcMass[0]);
                ImGui::SetNextItemWidth(64); ImGui::InputFloat("IPA", &tankLcMass[1]);
                ImGui::SetNextItemWidth(64); ImGui::InputFloat("NOS", &tankLcMass[2]);
            } else {
                ImGui::TextDisabled("-");
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    // Temperatures — thermistors (Top/Throat/Tank) then thermocouples TC1/TC2, with peak-hold.
    if (ImGui::BeginTable("temps", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("Temp");
        ImGui::TableSetupColumn("C");
        ImGui::TableSetupColumn("max");
        ImGui::TableHeadersRow();
        const auto tempRow = [](const char* label, SignalStat& s) {
            ImGui::PushID(label);
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(label);
            ImGui::TableNextColumn(); ImGui::Text("%.2f", s.now());
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", s.max());
            ImGui::SameLine();
            if (ImGui::SmallButton("rst")) {
                s.resetPeak();
            }
            ImGui::PopID();
        };
        for (std::size_t i = 0; i < thermocoupleC.size(); i++) { // thermocouples: Top, Throat
            tempRow(kThermocouple[i].label, thermocoupleC[i]);
        }
        for (std::size_t i = 0; i < tempC.size(); i++) { // thermistor(s): Tank
            tempRow(kThermistor[i].label, tempC[i]);
        }
        ImGui::EndTable();
    }

    // Phase — NOS liquid/gas from the tank pressure vs the saturation pressure at tank temp
    // (VaporPressure is a pure correlation; no CoolProp / tank-mass calc).
    if (ImGui::BeginTable("phase", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
        ImGui::TableSetupColumn("Derived");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();
        const double vapor_psi = VaporPressure::vaporPressureNOS_psi(tankTemp_C);
        const char* phase = (tankPress_psi > vapor_psi) ? "Liquid" : "Gas";
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::TextUnformatted("NOS phase");
        ImGui::TableNextColumn(); ImGui::TextUnformatted(phase);
        ImGui::EndTable();
    }

    // --- Logging ---
    sectionHeader("Logging");
    renderBaseFlagToggle("Fast rec", ControlFlagBase::FastRecording);
    renderBaseFlagToggle("Disable log", ControlFlagBase::DisableLogging);

    ImGui::EndGroup(); // end RIGHT
}

void CommunicationWindow::renderRawTab() {
    const double ecuSs = ecuSsRate.update(GSDataCenter::ecuSystemStateCount.load());
    const double ecuExt = ecuExtRate.update(GSDataCenter::ecuExtendedSystemStateCount.load());
    const double fcuSs = fcuSsRate.update(GSDataCenter::fcuSystemStateCount.load());
    const double fcuExt = fcuExtRate.update(GSDataCenter::fcuExtendedSystemStateCount.load());
    const double gsSs = gsSsRate.update(GSDataCenter::gsSystemStateCount.load());

    if (ImGui::CollapsingHeader("SystemState (all fields)", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Rate  -  ECU: %.0f Hz   |   FCU: %.0f Hz", ecuSs, fcuSs);
        renderSystemStateTable(GSDataCenter::latestEcuSystemState.load(), GSDataCenter::motorBoardState.load(),
                               GSDataCenter::latestFcuSystemState.load(), GSDataCenter::fillingStationBoardState.load());
    }

    if (ImGui::CollapsingHeader("ExtendedSystemState (all fields)", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Rate  -  ECU: %.1f Hz   |   FCU: %.1f Hz", ecuExt, fcuExt);
        renderExtendedStateTable(GSDataCenter::latestEcuExtendedSystemState.load(),
                                 GSDataCenter::latestFcuExtendedSystemState.load());
    }

    if (ImGui::CollapsingHeader("GsSystemState (received)", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Rate: %.1f Hz   |   Records received: %llu", gsSs,
                    static_cast<unsigned long long>(GSDataCenter::gsSystemStateCount.load()));
        renderGsSystemStateTable(GSDataCenter::latestGsSystemState.load());
    }

    if (ImGui::CollapsingHeader("Commands")) {
        ImGui::TextUnformatted("Valve position");
        renderValveCommandRow("NOS", CommandType::NosValve, /*binaryOnly=*/true);  // ECU: open/close only
        renderValveCommandRow("IPA", CommandType::IpaValve, /*binaryOnly=*/true);  // ECU: open/close only
        renderValveCommandRow("Fill", CommandType::FillValve, /*binaryOnly=*/false);
        renderValveCommandRow("Dump", CommandType::DumpValve, /*binaryOnly=*/false);

        // FCU solenoid valve — a SetControlFlag toggle (set = open, clear = close), not a
        // SetValvePosition. Actuation is gated to Unsafe on the board.
        ImGui::PushID("Solenoid");
        ImGui::Text("%-5s", "Sol");
        ImGui::SameLine();
        if (ImGui::Button("Close")) { CommandControl::sendCommand(CommandType::SolenoidValve, 0); }
        ImGui::SameLine();
        if (ImGui::Button("Open")) { CommandControl::sendCommand(CommandType::SolenoidValve, 1); }
        ImGui::PopID();

        // FCU heater — a SetControlFlag toggle (set = on, clear = off). Not state-gated:
        // the FCU follows the flag in any state. Confirm via the heater.on row above.
        ImGui::PushID("Heater");
        ImGui::Text("%-5s", "Heat");
        ImGui::SameLine();
        if (ImGui::Button("Off")) { CommandControl::sendCommand(CommandType::Heater, 0); }
        ImGui::SameLine();
        if (ImGui::Button("On")) { CommandControl::sendCommand(CommandType::Heater, 1); }
        ImGui::PopID();

        ImGui::Separator();
        ImGui::TextUnformatted("State transitions");
        using logic::control::State;
        renderSetStateButton("Init", State::Init);     ImGui::SameLine();
        renderSetStateButton("Safe", State::Safe);     ImGui::SameLine();
        renderSetStateButton("Unsafe", State::Unsafe); ImGui::SameLine();
        renderSetStateButton("Abort", State::Abort);   ImGui::SameLine();
        renderSetStateButton("Error", State::Error);
        renderSetStateButton("Ignite", State::Ignite); ImGui::SameLine();
        renderSetStateButton("Launch", State::Launch); ImGui::SameLine();
        renderSetStateButton("Test", State::Test);

        ImGui::Separator();
        ImGui::TextUnformatted("SystemState log rate");
        renderBaseFlagToggle("Fast rec", ControlFlagBase::FastRecording);
    }
}

void CommunicationWindow::renderSetupTab() {
    if (ImGui::CollapsingHeader("Ping / GsSystemState test", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Ping")) {
            CommandControl::sendCommand(CommandType::Ping, 0);
        }
        ImGui::SameLine();
        bool autoPing = ComTask::autoPing.load();
        if (ImGui::Checkbox("Auto-ping (1 Hz)", &autoPing)) {
            ComTask::autoPing.store(autoPing);
        }
        ImGui::SameLine();
        ImGui::Text("Pongs received: %u  (last from board %u)",
                    static_cast<unsigned>(GSDataCenter::pongReceivedCount.load()),
                    static_cast<unsigned>(GSDataCenter::lastPongSenderId.load()));

        ImGui::Separator();
        if (ImGui::Button("Send 10 GsSystemState -> 192.168.0.101")) {
            sendGsSystemStateBurst("192.168.0.101", UdpConfig::defaultDestPort, 10);
        }
        bool streaming = ComTask::streamGsSystemState.load();
        if (ImGui::Checkbox("Stream GsSystemState to board (10 Hz, continuous)", &streaming)) {
            ComTask::streamGsSystemState.store(streaming);
        }
    }

    if (ImGui::CollapsingHeader("UDP Configuration", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (!ipBufInitialized) {
            strcpy_s(ipBuf, UdpConfig::defaultDestIp);
            listenerPort = UdpConfig::defaultReceivePort;
            destinationPort = UdpConfig::defaultDestPort;
            ipBufInitialized = true;
        }

        ImGui::InputText("Remote IP", ipBuf, IM_ARRAYSIZE(ipBuf));
        ImGui::InputInt("Listener Port", &listenerPort);
        ImGui::InputInt("Destination Port", &destinationPort);

        if (ImGui::Button("Update Connection", ImVec2(-FLT_MIN, 0))) {
            if (ComTask::updateConnection(ipBuf, static_cast<uint16_t>(destinationPort), static_cast<uint16_t>(listenerPort))) {
                //GCS_APP_LOG_INFO("Updated connection successfully to: {}:{}", ipBuf, destinationPort);
            } else {
                GCS_APP_LOG_ERROR("Failed to update connection");
            }
        }

        if (ComTask::com->getComType() == ComType::UDP) {
            uint64_t totalReceived = ComTask::getTotalReceivedPackets();
            uint64_t lostPackets = ComTask::getLostPacketCount();

            ImGui::Text("Amount of lost packets: %llu", lostPackets);
            ImGui::Text("Total received packets: %llu", totalReceived);

            float percent = 100.0f;
            if (totalReceived > 0) {
                percent = 1.0f - static_cast<float>(lostPackets)/static_cast<float>(totalReceived);
                percent *= 100.0f;
                ImGui::Text("Percent of packets received: %.2f%%", percent);
            }else {
                ImGui::Text("Percent of packets received: %.2f%%", percent);
            }

            ImGui::Checkbox("Enable logging everytime a packet is lost", &loggingEnabled);
            if (loggingEnabled) {
                if (lostPacketCount != ComTask::getLostPacketCount()) {
                    GCS_APP_LOG_INFO("Lost packet count: {}", lostPacketCount);
                    lostPacketCount = ComTask::getLostPacketCount();
                }
            }
        }
    }
}
