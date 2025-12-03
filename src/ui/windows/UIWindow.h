#ifndef UIWINDOW_H
#define UIWINDOW_H

#include <ini.h>

class UIWindow {
public:
    virtual ~UIWindow() = default;

    // TODO: Use constructors instead of init?
    virtual void init() {
    }
    virtual void render() = 0;
    virtual void loadState(const mINI::INIStructure& ini) {
    }
    virtual void saveState(mINI::INIStructure& ini) const {
    }
    virtual const char* name() const = 0;
    virtual const char* dockspace() const = 0;
};

#endif // UIWINDOW_H
