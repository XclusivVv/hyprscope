#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <cstdlib>
#include <string>

inline HANDLE PHANDLE = nullptr;
float g_zoomLevel = 1.0f;

SDispatchResult scopeIn(std::string) {
    g_zoomLevel += 0.25f;
    if (g_zoomLevel > 10.0f) 
        g_zoomLevel = 10.0f;
    
    std::string cmd = "hyprctl keyword cursor:zoom_factor " + std::to_string(g_zoomLevel);
    std::system(cmd.c_str());
    
    return {};
}

SDispatchResult scopeOut(std::string) {
    g_zoomLevel -= 0.25f;
    if (g_zoomLevel < 1.0f) 
        g_zoomLevel = 1.0f;
    
    std::string cmd = "hyprctl keyword cursor:zoom_factor " + std::to_string(g_zoomLevel);
    std::system(cmd.c_str());
    
    return {};
}

SDispatchResult scopeReset(std::string) {
    g_zoomLevel = 1.0f;
    std::system("hyprctl keyword cursor:zoom_factor 1.0");
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
    
    return {"hyprscope", "Incremental cursor zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    std::system("hyprctl keyword cursor:zoom_factor 1.0");
}
