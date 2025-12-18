#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>

inline HANDLE PHANDLE = nullptr;

SDispatchResult scopeIn(std::string) {
    // Get current zoom value
    static auto* const PCURSORZOOM = (Hyprlang::CConfigValue*)HyprlandAPI::getConfigValue(PHANDLE, "cursor:zoom_factor");
    float currentZoom = PCURSORZOOM->floatValue;
    
    // Increase by 0.1
    currentZoom += 0.1f;
    if (currentZoom > 5.0f)
        currentZoom = 5.0f;
    
    // Set via hyprctl
    HyprlandAPI::invokeHyprctlCommand("keyword", "cursor:zoom_factor " + std::to_string(currentZoom), "");
    
    Debug::log(LOG, "[hyprscope] Zoom: {:.1f}", currentZoom);
    
    return {};
}

SDispatchResult scopeOut(std::string) {
    // Get current zoom value
    static auto* const PCURSORZOOM = (Hyprlang::CConfigValue*)HyprlandAPI::getConfigValue(PHANDLE, "cursor:zoom_factor");
    float currentZoom = PCURSORZOOM->floatValue;
    
    // Decrease by 0.1
    currentZoom -= 0.1f;
    if (currentZoom < 0.1f)
        currentZoom = 0.1f;
    
    // Auto-reset near 1.0
    if (std::abs(currentZoom - 1.0f) < 0.05f)
        currentZoom = 1.0f;
    
    // Set via hyprctl
    HyprlandAPI::invokeHyprctlCommand("keyword", "cursor:zoom_factor " + std::to_string(currentZoom), "");
    
    Debug::log(LOG, "[hyprscope] Zoom: {:.1f}", currentZoom);
    
    return {};
}

SDispatchResult scopeReset(std::string) {
    HyprlandAPI::invokeHyprctlCommand("keyword", "cursor:zoom_factor 1.0", "");
    Debug::log(LOG, "[hyprscope] Reset to 1.0");
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
    
    Debug::log(LOG, "[hyprscope] Loaded v1.0");
    
    return {"hyprscope", "Incremental cursor zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    HyprlandAPI::invokeHyprctlCommand("keyword", "cursor:zoom_factor 1.0", "");
    Debug::log(LOG, "[hyprscope] Unloaded");
}
