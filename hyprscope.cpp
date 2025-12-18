#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <fstream>

inline HANDLE PHANDLE = nullptr;

void logToFile(const std::string& msg) {
    std::ofstream f("/tmp/hyprscope.log", std::ios::app);
    f << msg << std::endl;
    f.close();
}

SDispatchResult scopeIn(std::string arg) {
    logToFile("scopeIn CALLED");
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.5");
    
    logToFile("system returned: " + std::to_string(result));
    
    return {};
}

SDispatchResult scopeOut(std::string arg) {
    logToFile("scopeOut CALLED");
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.0");
    
    logToFile("system returned: " + std::to_string(result));
    
    return {};
}

SDispatchResult scopeReset(std::string arg) {
    logToFile("scopeReset CALLED");
    
    int result = system("hyprctl keyword cursor:zoom_factor 1.0");
    
    logToFile("system returned: " + std::to_string(result));
    
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    logToFile("PLUGIN_INIT called");
    
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopereset", scopeReset);
    
    logToFile("Dispatchers registered");
    
    return {"hyprscope", "File log zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    logToFile("PLUGIN_EXIT called");
}
