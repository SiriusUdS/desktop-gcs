#include "CommunicationWindow.h"
#include "ComTask.h"
#include "CommandControl.h"
#include "CRC.h"
#include "GSDataCenter.h"
#include "ICom.h"
#include "ITileLoader.h"
#include "Logging.h"
#include "UdpConfig.h"
#include "units.h"

#include "command/set_control_flag.hpp"
#include "framing/ethernet_header.hpp"
#include "framing/payload_type.hpp"
#include "system/board_id.hpp"
#include "telemetry/gs_system_state.hpp"
#include "telemetry/telemetry_type.hpp"
#include "devices/valve/valve_state.hpp"
#include "peripherals/adc/adc_state.hpp"
#include "peripherals/can/can_state.hpp"
#include "peripherals/ethernet/ethernet_state.hpp"
#include "peripherals/power_monitor/power_monitor_state.hpp"
#include "peripherals/storage/sd_write_engine_info.hpp"
#include "peripherals/storage/storage_error.hpp"
#include "peripherals/storage/storage_state.hpp"
#include "peripherals/thermocouple/thermocouple_state.hpp"
#include "system/state.hpp"

#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <map>
#include <span>
#include <string>

namespace {
// Derives a Hz rate from a monotonically increasing counter, refreshed every ~0.5 s.
struct RateTracker {
    uint64_t lastCount = 0;
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    double rateHz = 0.0;
    double update(uint64_t currentCount) {
        const auto now = std::chrono::steady_clock::now();
        const double dt = std::chrono::duration<double>(now - lastTime).count();
        if (dt >= 0.5) {
            rateHz = static_cast<double>(currentCount - lastCount) / dt;
            lastCount = currentCount;
            lastTime = now;
        }
        return rateHz;
    }
};

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
    constexpr float kShuntCodeToMilliAmp = 0.04f /* mV per code */ / kPowerMonitorRshuntOhms;
    const PowerMonitorInfo& epm = ecu.power_monitor;
    const PowerMonitorInfo& fpm = fcu.power_monitor;
    t.row("power_monitor.state", powerMonitorStateName(epm.state), powerMonitorStateName(fpm.state));
    t.row("power_monitor.status", powerMonitorStatusBits(epm.status), powerMonitorStatusBits(fpm.status));
    for (unsigned i = 0; i < POWER_MONITOR_CHANNEL_COUNT; i++) {
        const PowerMonitorChannel& ech = epm.channels[i];
        const PowerMonitorChannel& fch = fpm.channels[i];
        t.row(fmt("power_monitor.ch[%u].shunt_code", i).c_str(), fmt("%d (%.3f mA)", ech.shunt_code, ech.shunt_code * kShuntCodeToMilliAmp),
                                                                 fmt("%d (%.3f mA)", fch.shunt_code, fch.shunt_code * kShuntCodeToMilliAmp));
        t.row(fmt("power_monitor.ch[%u].bus_code", i).c_str(), fmt("%d (%.3f V)", ech.bus_code, ech.bus_code * 0.008f),
                                                               fmt("%d (%.3f V)", fch.bus_code, fch.bus_code * 0.008f));
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
void renderValveCommandRow(const char* name, CommandType valveCmd, bool binaryOnly) {
    static std::map<CommandType, bool> forced; // per-valve Force checkbox state, keyed by command
    bool& force = forced[valveCmd];
    const auto valveValue = [&force](uint32_t pct) { return force ? (pct | VALVE_FORCE_FLAG) : pct; };

    ImGui::PushID(name);
    ImGui::Text("%-5s", name);
    ImGui::SameLine();
    if (ImGui::Button("Closed")) { CommandControl::sendCommand(valveCmd, valveValue(0)); }
    if (!binaryOnly) {
        ImGui::SameLine();
        if (ImGui::Button("25%")) { CommandControl::sendCommand(valveCmd, valveValue(25)); }
        ImGui::SameLine();
        if (ImGui::Button("50%")) { CommandControl::sendCommand(valveCmd, valveValue(50)); }
        ImGui::SameLine();
        if (ImGui::Button("75%")) { CommandControl::sendCommand(valveCmd, valveValue(75)); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Open")) { CommandControl::sendCommand(valveCmd, valveValue(100)); }
    ImGui::SameLine();
    ImGui::Checkbox("Force", &force);
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
} // namespace

const char* const CommunicationWindow::name = "Communication";


const char* CommunicationWindow::getName() const {
    return "Communication";
}

void CommunicationWindow::renderImpl() {
    static bool loggingEnabled = false;
    static uint64_t lostPacketCount = 0;

    static RateTracker ecuSsRate, ecuExtRate, fcuSsRate, fcuExtRate, gsSsRate;
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

        ImGui::Separator();
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
        static char ipBuf[64];
        static int listenerPort = UdpConfig::defaultReceivePort;
        static int destinationPort = UdpConfig::defaultDestPort;
        static bool ipBufInitialized = false;
        
        if (!ipBufInitialized) {
            strcpy_s(ipBuf, UdpConfig::defaultDestIp);
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