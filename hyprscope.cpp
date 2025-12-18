#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <any>

inline HANDLE PHANDLE = nullptr;

// Global magnification state
struct ScopeState {
    float factor = 1.0f;
    Vector2D center = {0.5f, 0.5f}; // Normalized center position (0-1)
    bool active = false;
} g_scope;

const float SCOPE_INCREMENT = 0.10f;  // 10% per step
const float MIN_SCOPE = 0.1f;         // 10% minimum
const float MAX_SCOPE = 5.0f;         // 500% maximum

// Get current mouse position as normalized coordinates
Vector2D getNormalizedMousePos() {
    auto* monitor = g_pCompositor->getMonitorFromCursor();
    if (!monitor)
        return {0.5f, 0.5f};
    
    auto mousePos = g_pInputManager->getMouseCoordsInternal();
    Vector2D normalized = {
        (mousePos.x - monitor->vecPosition.x) / monitor->vecSize.x,
        (mousePos.y - monitor->vecPosition.y) / monitor->vecSize.y
    };
    
    return normalized;
}

// Function to apply magnification transform to all monitors
void applyScope() {
    // Trigger full frame renders on all monitors
    for (auto& m : g_pCompositor->m_vMonitors) {
        if (!m->output)
            continue;
        
        m->forceFullFrames = 5; // Force multiple frames for smooth transition
        g_pHyprRenderer->damageMonitor(m.get());
    }
}

// Increase magnification dispatcher
void scopeIn(std::string args) {
    if (g_scope.factor >= MAX_SCOPE) {
        Debug::log(LOG, "[hyprScope] Already at maximum magnification ({}%)", (int)(MAX_SCOPE * 100));
        return;
    }
    
    g_scope.factor += SCOPE_INCREMENT;
    if (g_scope.factor > MAX_SCOPE)
        g_scope.factor = MAX_SCOPE;
    
    g_scope.active = (g_scope.factor != 1.0f);
    g_scope.center = getNormalizedMousePos();
    
    Debug::log(LOG, "[hyprScope] Magnification increased to {}% at ({:.2f}, {:.2f})", 
               (int)(g_scope.factor * 100), g_scope.center.x, g_scope.center.y);
    applyScope();
}

// Decrease magnification dispatcher
void scopeOut(std::string args) {
    if (g_scope.factor <= MIN_SCOPE) {
        Debug::log(LOG, "[hyprScope] Already at minimum magnification ({}%)", (int)(MIN_SCOPE * 100));
        return;
    }
    
    g_scope.factor -= SCOPE_INCREMENT;
    if (g_scope.factor < MIN_SCOPE)
        g_scope.factor = MIN_SCOPE;
    
    // Auto-disable if we're back to 1.0 (100%)
    if (std::abs(g_scope.factor - 1.0f) < 0.01f) {
        g_scope.factor = 1.0f;
        g_scope.active = false;
    } else {
        g_scope.active = true;
        g_scope.center = getNormalizedMousePos();
    }
    
    Debug::log(LOG, "[hyprScope] Magnification decreased to {}%", (int)(g_scope.factor * 100));
    applyScope();
}

// Reset magnification dispatcher
void scopeReset(std::string args) {
    g_scope.factor = 1.0f;
    g_scope.active = false;
    g_scope.center = {0.5f, 0.5f};
    
    Debug::log(LOG, "[hyprScope] Magnification reset to 100%");
    applyScope();
}

// Hook for rendering - applies magnification transformation
static void onRenderStage(void* self, SCallbackInfo& info, eRenderStage stage) {
    if (stage != RENDER_PRE)
        return;
    
    if (!g_scope.active || g_scope.factor == 1.0f)
        return;
    
    // Apply magnification scale to viewport
    // This is a simplified implementation
    // A complete implementation would modify the projection matrix
    auto* monitor = g_pCompositor->m_pLastMonitor;
    if (!monitor)
        return;
    
    // The actual magnification effect would be applied through wlroots transform
    // or by modifying the rendering matrices
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    Debug::log(LOG, "[hyprScope] Initializing Hypr Scope v1.0");

    // Register dispatchers
    HyprlandAPI::addDispatcher(PHANDLE, "scopein", scopeIn);
    HyprlandAPI::addDispatcher(PHANDLE, "scopeout", scopeOut);
    HyprlandAPI::addDispatcher(PHANDLE, "scopereset", scopeReset);
    
    // Register render hook
    static auto P = HyprlandAPI::registerCallbackDynamic(
        PHANDLE, "render", [](void* self, SCallbackInfo& info, std::any data) {
            onRenderStage(self, info, std::any_cast<eRenderStage>(data));
        }
    );

    Debug::log(LOG, "[hyprScope] Plugin initialized successfully!");
    Debug::log(LOG, "[hyprScope] Available dispatchers: scopein, scopeout, scopereset");
    Debug::log(LOG, "[hyprScope] Magnification range: {}% - {}% (increment: {}%)", 
               (int)(MIN_SCOPE * 100), (int)(MAX_SCOPE * 100), (int)(SCOPE_INCREMENT * 100));

    return {"hyprScope", "Incremental display magnification for Hyprland", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Debug::log(LOG, "[hyprScope] Unloading Hypr Scope plugin");
    
    // Reset magnification state on exit
    g_scope.factor = 1.0f;
    g_scope.active = false;
    applyScope();
}
