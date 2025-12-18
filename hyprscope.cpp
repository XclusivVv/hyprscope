#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>

inline HANDLE PHANDLE = nullptr;
float g_zoomLevel = 1.0f;

SDispatchResult scopeIn(std::string) {
    g_zoomLevel += 0.1f;
    if (g_zoomLevel > 5.0f)
        g_zoomLevel = 5.0f;
    
    // Get the config value and update it directly
    auto* configValue = g_pConfigManager->getConfigValuePtr("cursor:zoom_factor");
    if (configValue) {
        configValue->setValue(g_zoomLevel);
    }
    
    Debug::log(LOG, "[hyprscope] Zoom: {:.1f}", g_zoomLevel);
    
    return {};
}

SDispatchResult scopeOut(std::string) {
    g_zoomLevel -= 0.1f;
    if (g_zoomLevel < 0.1f)
        g_zoomLevel = 0.1f;
    
    if (std::abs(g_zoomLevel - 1.0f) < 0.05f)
        g_zoomLevel = 1.0f;
    
    // Get the config value and update it directly
    auto* configValue = g_pConfigManager->getConfigValuePtr("cursor:zoom_factor");
    if (configValue) {
        configValue->setValue(g_zoomLevel);
    }
    
    Debug::log(LOG, "[hyprscope] Zoom: {:.1f}", g_zoomLevel);
    
    return {};
}

SDispatchResult scopeReset(std::string) {
    g_zoomLevel = 1.0f;
    
    auto* configValue = g_pConfigManager->getConfigValuePtr("cursor:zoom_factor");
    if (configValue) {
        configValue->setValue(1.0f);
    }
    
    Debug::log(LOG, "[hyprscope] Reset to 1.0");
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
    
    Debug::log(LOG, "[hyprscope] Loaded v1.0");
    
    return {"hyprscope", "Incremental cursor zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    auto* configValue = g_pConfigManager->getConfigValuePtr("cursor:zoom_factor");
    if (configValue) {
        configValue->setValue(1.0f);
    }
    Debug::log(LOG, "[hyprscope] Unloaded");
}
