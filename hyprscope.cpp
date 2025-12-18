#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <any>

inline HANDLE PHANDLE = nullptr;

// Global scope state
struct ScopeState {
    float factor = 1.0f;
    Vector2D center = {0.5f, 0.5f}; // Normalized center position (0-1)
    bool enabled = false;
} g_scope;

const float SCOPE_INCREMENT = 0.10f; // 10% increment
const float MIN_SCOPE = 0.5f;
const float MAX_SCOPE = 3.0f;


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

// Function to apply scope transform to all monitors
void applyScope() {
    // Trigger full frame renders on all monitors
    for (auto& m : g_pCompositor->m_vMonitors) {
        if (!m->output)
            continue;
        
        m->forceFullFrames = 5; // Force multiple frames for smooth transition
        g_pHyprRenderer->damageMonitor(m.get());
    }
}

// Scope in dispatcher - increases magnification by 10%
void scopeIn(std::string args) {
    if (g_scope.factor >= MAX_SCOPE) {
        Debug::log(LOG, "[HyprScope] Already at maximum magnification");
        return;
    }
    
    // Increase by 10% of current value
    float newFactor = g_scope.factor * 1.10f;
    if (newFactor > MAX_SCOPE)
        newFactor = MAX_SCOPE;
    
    g_scope.factor = newFactor;
    g_scope.enabled = (g_scope.factor > 1.01f); // Enable if significantly above 1.0
    g_scope.center = getNormalizedMousePos();
    
    Debug::log(LOG, "[HyprScope] Scope in: {:.0f}% at ({:.2f}, {:.2f})", 
               g_scope.factor * 100, g_scope.center.x, g_scope.center.y);
    applyScope();
}

// Scope out dispatcher - decreases magnification by 10%
void scopeOut(std::string args) {
    if (g_scope.factor <= MIN_SCOPE) {
        Debug::log(LOG, "[HyprScope] Already at minimum magnification");
        return;
    }
    
    // Decrease by 10% of current value
    float newFactor = g_scope.factor / 1.10f;
    if (newFactor < MIN_SCOPE)
        newFactor = MIN_SCOPE;
    
    g_scope.factor = newFactor;
    
    // Auto-disable if we're back to approximately 1.0
    if (std::abs(g_scope.factor - 1.0f) < 0.02f) {
        g_scope.factor = 1.0f;
        g_scope.enabled = false;
    } else {
        g_scope.enabled = true;
        g_scope.center = getNormalizedMousePos();
    }
    
    Debug::log(LOG, "[HyprScope] Scope out: {:.0f}%", g_scope.factor * 100);
    applyScope();
}

// Reset scope dispatcher
void scopeReset(std::string args) {
    g_scope.factor = 1.0f;
    g_scope.enabled = false;
    g_scope.center = {0.5f, 0.5f};
    
    Debug::log(LOG, "[HyprScope] Scope reset to 100%");
    applyScope();
}

// Hook for rendering - applies scope transformation
static void onRenderStage(void* self, SCallbackInfo& info, eRenderStage stage) {
    if (stage != RENDER_PRE)
        return;
    
    if (!g_scope.enabled || g_scope.factor == 1.0f)
        return;
    
    // Apply scope scale to viewport
    // This is a simplified implementation
    // A complete implementation would modify the projection matrix
    auto* monitor = g_pCompositor->m_pLastMonitor;
    if (!monitor)
        return;
    
    // The actual scope effect would be applied through wlroots transform
    // or by modifying the rendering matrices
}



APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    Debug::log(LOG, "[HyprScope] Initializing HyprScope plugin v1.0");

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

    Debug::log(LOG, "[HyprScope] Plugin initialized successfully!");
    Debug::log(LOG, "[HyprScope] Available dispatchers: scopein, scopeout, scopereset");
    Debug::log(LOG, "[HyprScope] Magnification range: {:.0f}% - {:.0f}% (increment: 10%)", 
               MIN_SCOPE * 100, MAX_SCOPE * 100);

    return {"HyprScope", "Incremental display magnification for Hyprland", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Debug::log(LOG, "[HyprScope] Unloading HyprScope plugin");
    
    // Reset scope state on exit
    g_scope.factor = 1.0f;
    g_scope.enabled = false;
    applyScope();
}
