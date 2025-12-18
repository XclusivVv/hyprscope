#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE PHANDLE = nullptr;

SDispatchResult scopeIn(std::string) {
    // Just execute a simple command that we KNOW works
    system("hyprctl keyword cursor:zoom_factor 1.5 2>&1 | logger -t hyprscope");
    Debug::log(LOG, "[hyprscope] scopein executed");
    return {};
}

SDispatchResult scopeOut(std::string) {
    system("hyprctl keyword cursor:zoom_factor 1.0 2>&1 | logger -t hyprscope");
    Debug::log(LOG, "[hyprscope] scopeout executed");
    return {};
}

SDispatchResult scopeReset(std::string) {
    system("hyprctl keyword cursor:zoom_factor 1.0 2>&1 | logger -t hyprscope");
    Debug::log(LOG, "[hyprscope] scopereset executed");
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopereset", scopeReset);
    
    Debug::log(LOG, "[hyprscope] Loaded - test version");
    
    return {"hyprscope", "Test zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Debug::log(LOG, "[hyprscope] Unloaded");
}
