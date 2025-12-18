#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <fstream>
#include <ctime>

inline HANDLE PHANDLE = nullptr;

void logToFile(const std::string& msg) {
    std::ofstream f("/tmp/hyprscope.log", std::ios::app);
    time_t now = time(0);
    f << ctime(&now) << " - " << msg << std::endl;
    f.close();
}

SDispatchResult scopeIn(std::string arg) {
    logToFile("scopeIn CALLED!");
    system("hyprctl keyword cursor:zoom_factor 2.0");
    return {};
}

SDispatchResult scopeOut(std::string arg) {
    logToFile("scopeOut CALLED!");
    system("hyprctl keyword cursor:zoom_factor 1.0");
    return {};
}

SDispatchResult scopeReset(std::string arg) {
    logToFile("scopeReset CALLED!");
    system("hyprctl keyword cursor:zoom_factor 1.0");
    return {};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    logToFile("PLUGIN_API_VERSION called");
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    logToFile("=== PLUGIN_INIT STARTED ===");
    
    PHANDLE = handle;
    
    logToFile("About to register scopein...");
    bool r1 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopein", scopeIn);
    logToFile(r1 ? "scopein registered SUCCESS" : "scopein registered FAIL");
    
    logToFile("About to register scopeout...");
    bool r2 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopeout", scopeOut);
    logToFile(r2 ? "scopeout registered SUCCESS" : "scopeout registered FAIL");
    
    logToFile("About to register scopereset...");
    bool r3 = HyprlandAPI::addDispatcherV2(PHANDLE, "scopereset", scopeReset);
    logToFile(r3 ? "scopereset registered SUCCESS" : "scopereset registered FAIL");
    
    logToFile("=== PLUGIN_INIT COMPLETED ===");
    
    return {"hyprscope", "Init test", "xclusivvv", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    logToFile("=== PLUGIN_EXIT CALLED ===");
}
