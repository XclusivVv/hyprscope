#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>

inline HANDLE PHANDLE = nullptr;

SDispatchResult scopeIn(std::string) {
    // Get current zoom factor
    static auto PCURSORZOOMFACTOR = CConfigValue<Hyprlang::FLOAT>("cursor:zoom_factor");
    float currentZoom = *PCURSORZOOMFACTOR;
    
    // Increase by 10%
    currentZoom += 0.1f;
    if (currentZoom > 5.0f)
        currentZoom = 5.0f;
    
    // Set the config value
    g_pConfigManager->setFloat("cursor:zoom_factor", currentZoom);
    
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", currentZoom * 100);
    
    return {};
}

SDispatchResult scopeOut(std::string) {
    // Get current zoom factor
    static auto PCURSORZOOMFACTOR = CConfigValue<Hyprlang::FLOAT>("cursor:zoom_factor");
    float currentZoom = *PCURSORZOOMFACTOR;
    
    // Decrease by 10%
    currentZoom -= 0.1f;
    if (currentZoom < 0.1f)
        currentZoom = 0.1f;
    
    // Auto-reset to 1.0 if close
    if (std::abs(currentZoom - 1.0f) < 0.01f) {
        currentZoom = 1.0f;
    }
    
    // Set the config value
    g_pConfigManager->setFloat("cursor:zoom_factor", currentZoom);
    
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", currentZoom * 100);
    
    return {};
}

SDispatchResult scopeReset(std::string) {
    // Reset to 1.0
    g_pConfigManager->setFloat("cursor:zoom_factor", 1.0f);
    
    Debug::log(LOG, "[hyprscope] Reset to 100%");
    
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    // Register dispatchers
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcherV2(PHANDLE, "scopereset", scopeReset);
    
    Debug::log(LOG, "[hyprscope] Loaded v1.0 - Cursor zoom control");
    
    return {"hyprscope", "Incremental cursor zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // Reset zoom on exit
    g_pConfigManager->setFloat("cursor:zoom_factor", 1.0f);
    Debug::log(LOG, "[hyprscope] Unloaded");
}
