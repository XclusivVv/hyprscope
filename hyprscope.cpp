#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>

inline HANDLE PHANDLE = nullptr;

float g_magnification = 1.0f;

// Hook into render to apply magnification
void onRender(void*, SCallbackInfo&, std::any data) {
    const auto STAGE = std::any_cast<eRenderStage>(data);
    
    // Apply before rendering starts
    if (STAGE == RENDER_PRE) {
        g_pHyprOpenGL->m_renderData.mouseZoomFactor = g_magnification;
    }
}

void scopeIn(std::string) {
    g_magnification += 0.1f;
    if (g_magnification > 5.0f) 
        g_magnification = 5.0f;
    
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", g_magnification * 100);
    
    // Damage all monitors to trigger re-render
    for (auto& m : g_pCompositor->m_vMonitors) {
        g_pHyprRenderer->damageMonitor(m.get());
    }
}

void scopeOut(std::string) {
    g_magnification -= 0.1f;
    if (g_magnification < 0.1f) 
        g_magnification = 0.1f;
    
    // Auto-reset to 1.0 if close
    if (std::abs(g_magnification - 1.0f) < 0.01f) {
        g_magnification = 1.0f;
    }
    
    Debug::log(LOG, "[hyprscope] Magnification: {:.0f}%", g_magnification * 100);
    
    // Damage all monitors to trigger re-render
    for (auto& m : g_pCompositor->m_vMonitors) {
        g_pHyprRenderer->damageMonitor(m.get());
    }
}

void scopeReset(std::string) {
    g_magnification = 1.0f;
    Debug::log(LOG, "[hyprscope] Reset to 100%");
    
    // Damage all monitors to trigger re-render
    for (auto& m : g_pCompositor->m_vMonitors) {
        g_pHyprRenderer->damageMonitor(m.get());
    }
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;
    
    // Register render hook
    HyprlandAPI::registerCallbackDynamic(PHANDLE, "render", onRender);
    
    // Register dispatchers
    HyprlandAPI::addDispatcher(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcher(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcher(PHANDLE, "scopereset", scopeReset);
    
    Debug::log(LOG, "[hyprscope] Loaded v1.0 - Display magnification active");
    
    return {"hyprscope", "Display magnification", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    // Reset magnification on unload
    g_pHyprOpenGL->m_renderData.mouseZoomFactor = 1.0f;
    Debug::log(LOG, "[hyprscope] Unloaded");
}
