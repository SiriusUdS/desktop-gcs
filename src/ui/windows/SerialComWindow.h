#ifndef SERIALCOMWINDOW_H
#define SERIALCOMWINDOW_H

#include "BoardComStateMonitor.h"
#include "UIWindow.h"

#include <ini.h>

class SerialComWindow : public UIWindow {
public:
    SerialComWindow();

    void renderImpl() override;
    void loadState(const mINI::INIStructure& ini) override;
    void saveState(mINI::INIStructure& ini) const override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    enum RecvBufferDisplayMode { TEXT = 0, HEXA = 1 };

    static const char* INI_RECV_BUFFER_DISPLAY_MODE;

    void renderBoardComStateTableRow(const char* boardName, BoardComStateMonitor::State state) const;
    void renderPacketRateTableRow(const char* packetName, double rate) const;
    void recvBufferContentModal();
    void updateRecvBufferContentDisplay(bool syncToCurrentBuffer);

    int recvBufferDisplayMode{TEXT};
    bool recvBufferDisplayPause{};
    std::vector<char> recvBufferContentDisplay;
};

#endif // SERIALCOMWINDOW_H
