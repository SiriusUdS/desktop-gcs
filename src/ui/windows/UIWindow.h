#pragma once

#include <ini.h>

class UIWindow {
public:
    virtual ~UIWindow() = default;

    void render() {
        if (!hasLazyInit) {
            lazyInit();
            hasLazyInit = true;
        }

        renderImpl();
    }

    // TODO: Document difference between init and lazyInit
    virtual void init() {
    }
    virtual void loadState(const mINI::INIStructure& ini) {
    }
    virtual void saveState(mINI::INIStructure& ini) const {
    }
    virtual const char* name() const = 0;
    virtual const char* dockspace() const = 0;

protected:
    virtual void lazyInit() {
    }
    virtual void renderImpl() = 0;

    bool hasLazyInit{};
};
