#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE PHANDLE = nullptr;

float g_magnification = 1.0f;

void scopeIn(std::string) {
    g_magnification += 0.1f;
    if (g_magnification > 5.0f) g_magnification = 5.0f;
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", g_magnification * 100);
}

void scopeOut(std::string) {
    g_magnification -= 0.1f;
    if (g_magnification < 0.1f) g_magnification = 0.1f;
    if (std::abs(g_magnification - 1.0f) < 0.01f) g_magnification = 1.0f;
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", g_magnification * 100);
}

void scopeReset(std::string) {
    g_magnification = 1.0f;
    Debug::log(LOG, "[hyprscope] Reset to 100%");
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    HyprlandAPI::addDispatcher(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcher(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcher(PHANDLE, "scopereset", scopeReset);
    
    Debug::log(LOG, "[hyprscope] Loaded v1.0");
    
    return {"hyprscope", "Display magnification", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Debug::log(LOG, "[hyprscope] Unloaded");
}
