#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE PHANDLE = nullptr;

SDispatchResult scopeIn(std::string arg) {
    Debug::log(LOG, "[hyprscope] scopeIn() CALLED with arg: '{}'", arg);
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.5");
    
    Debug::log(LOG, "[hyprscope] system() returned: {}", result);
    
    return {};
}

SDispatchResult scopeOut(std::string arg) {
    Debug::log(LOG, "[hyprscope] scopeOut() CALLED with arg: '{}'", arg);
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.0");
    
    Debug::log(LOG, "[hyprscope] system() returned: {}", result);
    
    return {};
}

SDispatchResult scopeReset(std::string arg) {
    Debug::log(LOG, "[hyprscope] scopeReset() CALLED with arg: '{}'", arg);
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.0");
    
    Debug::log(LOG, "[hyprscope] system() returned: {}", result);
    
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    Debug::log(LOG, "[hyprscope] PLUGIN_INIT called, registering dispatchers...");
    
    bool result1 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopein", scopeIn);
    bool result2 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopeout", scopeOut);
    bool result3 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopereset", scopeReset);
    
    Debug::log(LOG, "[hyprscope] Dispatcher registration results: {} {} {}", result1, result2, result3);
    Debug::log(LOG, "[hyprscope] Plugin fully loaded");
    
    return {"hyprscope", "Debug zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Debug::log(LOG, "[hyprscope] PLUGIN_EXIT called");
}
