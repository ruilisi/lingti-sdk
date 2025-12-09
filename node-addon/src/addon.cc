#include <napi.h>
#include "lingti_sdk.h"
#include <cstring>

// Helper function to create JavaScript strings from C strings and free them
Napi::String CreateAndFreeString(Napi::Env env, char* cStr) {
    if (cStr == nullptr) {
        return Napi::String::New(env, "");
    }
    Napi::String result = Napi::String::New(env, cStr);
    FreeString(cStr);
    return result;
}

// Wrapper for StartTun2R
Napi::Value StartTun2R_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for configJSON").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string configJSON = info[0].As<Napi::String>().Utf8Value();
    int result = StartTun2R(configJSON.c_str());

    return Napi::Number::New(env, result);
}

// Wrapper for StartTun2RWithConfigFile
Napi::Value StartTun2RWithConfigFile_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const char* configPath = nullptr;
    if (info.Length() > 0 && info[0].IsString()) {
        std::string path = info[0].As<Napi::String>().Utf8Value();
        configPath = path.c_str();
    }

    int result = StartTun2RWithConfigFile(configPath);

    return Napi::Number::New(env, result);
}

// Wrapper for StopTun2R
Napi::Value StopTun2R_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    int result = StopTun2R();

    return Napi::Number::New(env, result);
}

// Wrapper for IsServiceRunning
Napi::Value IsServiceRunning_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    int result = IsServiceRunning();

    return Napi::Boolean::New(env, result == 1);
}

// Wrapper for GetSDKVersion
Napi::Value GetSDKVersion_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    char* version = GetSDKVersion();
    return CreateAndFreeString(env, version);
}

// Wrapper for GetLastErrorMessage
Napi::Value GetLastErrorMessage_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    char* errorMsg = GetLastErrorMessage();
    return CreateAndFreeString(env, errorMsg);
}

// Wrapper for GetTrafficStats
Napi::Value GetTrafficStats_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    unsigned long long txBytes = 0, rxBytes = 0, txPkts = 0, rxPkts = 0;
    GetTrafficStats(&txBytes, &rxBytes, &txPkts, &rxPkts);

    Napi::Object result = Napi::Object::New(env);
    result.Set("txBytes", Napi::Number::New(env, static_cast<double>(txBytes)));
    result.Set("rxBytes", Napi::Number::New(env, static_cast<double>(rxBytes)));
    result.Set("txPkts", Napi::Number::New(env, static_cast<double>(txPkts)));
    result.Set("rxPkts", Napi::Number::New(env, static_cast<double>(rxPkts)));

    return result;
}

// Wrapper for GetLastPingStats
Napi::Value GetLastPingStats_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    long long router = 0, takeoff = 0, landing = 0;
    GetLastPingStats(&router, &takeoff, &landing);

    Napi::Object result = Napi::Object::New(env);
    result.Set("router", Napi::Number::New(env, static_cast<double>(router)));
    result.Set("takeoff", Napi::Number::New(env, static_cast<double>(takeoff)));
    result.Set("landing", Napi::Number::New(env, static_cast<double>(landing)));

    return result;
}

// Wrapper for RunPing
Napi::Value RunPing_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    int result = RunPing();

    return Napi::Number::New(env, result);
}

// Wrapper for StopPing
Napi::Value StopPing_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    int result = StopPing();

    return Napi::Number::New(env, result);
}

// Wrapper for FlushDNSCache
Napi::Value FlushDNSCache_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    int result = FlushDNSCache();

    return Napi::Number::New(env, result);
}

// Wrapper for DeleteService - runs "sc delete lingtiwfp"
Napi::Value DeleteService_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    #ifdef _WIN32
    // Execute "sc delete lingtiwfp" command
    int result = system("sc delete lingtiwfp >nul 2>&1");
    return Napi::Number::New(env, result);
    #else
    // Not supported on non-Windows platforms
    return Napi::Number::New(env, -1);
    #endif
}

// Wrapper for GetConsoleConfig
Napi::Value GetConsoleConfig_Wrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    char *gateway = nullptr, *mask = nullptr, *ip = nullptr, *dns = nullptr;
    int state = GetConsoleConfig(&gateway, &mask, &ip, &dns);

    Napi::Object result = Napi::Object::New(env);
    result.Set("state", Napi::Number::New(env, state));
    result.Set("gateway", CreateAndFreeString(env, gateway));
    result.Set("mask", CreateAndFreeString(env, mask));
    result.Set("ip", CreateAndFreeString(env, ip));
    result.Set("dns", CreateAndFreeString(env, dns));

    // State enum mapping
    const char* stateStr = "unknown";
    switch(state) {
        case 0: stateStr = "completed"; break;
        case 1: stateStr = "failed"; break;
        case 2: stateStr = "idle"; break;
        case 3: stateStr = "in_progress"; break;
    }
    result.Set("stateStr", Napi::String::New(env, stateStr));

    return result;
}

// Export error codes as constants
Napi::Object GetErrorCodes(Napi::Env env) {
    Napi::Object codes = Napi::Object::New(env);
    codes.Set("SUCCESS", Napi::Number::New(env, LINGTI_SUCCESS));
    codes.Set("ERR_NULL_CONFIG", Napi::Number::New(env, LINGTI_ERR_NULL_CONFIG));
    codes.Set("ERR_JSON_PARSE", Napi::Number::New(env, LINGTI_ERR_JSON_PARSE));
    codes.Set("ERR_ALREADY_RUN", Napi::Number::New(env, LINGTI_ERR_ALREADY_RUN));
    codes.Set("ERR_LOAD_CONFIG", Napi::Number::New(env, LINGTI_ERR_LOAD_CONFIG));
    codes.Set("ERR_NOT_RUNNING", Napi::Number::New(env, LINGTI_ERR_NOT_RUNNING));
    return codes;
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // Export all functions
    exports.Set("startTun2R", Napi::Function::New(env, StartTun2R_Wrapper));
    exports.Set("startTun2RWithConfigFile", Napi::Function::New(env, StartTun2RWithConfigFile_Wrapper));
    exports.Set("stopTun2R", Napi::Function::New(env, StopTun2R_Wrapper));
    exports.Set("isServiceRunning", Napi::Function::New(env, IsServiceRunning_Wrapper));
    exports.Set("getSDKVersion", Napi::Function::New(env, GetSDKVersion_Wrapper));
    exports.Set("getLastErrorMessage", Napi::Function::New(env, GetLastErrorMessage_Wrapper));
    exports.Set("getTrafficStats", Napi::Function::New(env, GetTrafficStats_Wrapper));
    exports.Set("getLastPingStats", Napi::Function::New(env, GetLastPingStats_Wrapper));
    exports.Set("runPing", Napi::Function::New(env, RunPing_Wrapper));
    exports.Set("stopPing", Napi::Function::New(env, StopPing_Wrapper));
    exports.Set("flushDNSCache", Napi::Function::New(env, FlushDNSCache_Wrapper));
    exports.Set("deleteService", Napi::Function::New(env, DeleteService_Wrapper));
    exports.Set("getConsoleConfig", Napi::Function::New(env, GetConsoleConfig_Wrapper));

    // Export error codes
    exports.Set("ErrorCodes", GetErrorCodes(env));

    return exports;
}

NODE_API_MODULE(lingti_sdk, Init)
