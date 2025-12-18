#define WLR_USE_UNSTABLE
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE PHANDLE = nullptr;
float g_zoom = 1.0f;

SDispatchResult scopeIn(std::string) {
    g_zoom += 0.1f;
    if (g_zoom > 5.0f) g_zoom = 5.0f;
    std::string cmd = "hyprctl keyword cursor:zoom_factor " + std::to_string(g_zoom) + " &";
    system(cmd.c_str());
    return {};
}

SDispatchResult scopeOut(std::string) {
    g_zoom -= 0.1f;
    if (g_zoom < 0.1f) g_zoom = 0.1f;
    if (std::abs(g_zoom - 1.0f) < 0.05f) g_zoom = 1.0f;
    std::string cmd = "hyprctl keyword cursor:zoom_factor " + std::to_string(g_zoom) + " &";
    system(cmd.c_str());
    return {};
}

SDispatchResult scopeReset(std::string) {
    g_zoom = 1.0f;
    system("hyprctl keyword cursor:zoom_factor 1.0 &");
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE h) {
    PHANDLE = h;
    HyprlandAPI::addDispatcherV2(h, "scopein", scopeIn);
    HyprlandAPI::addDispatcherV2(h, "scopeout", scopeOut);
    HyprlandAPI::addDispatcherV2(h, "scopereset", scopeReset);
    return {"hyprscope", "zoom", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
