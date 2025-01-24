#include "systemapps.h"
#include "apps.h"

#include "test/build/app.wasm.h"
#include "counter/build/app.wasm.h"

void systemApps::init() {
    apps::registry.push(new apps::SystemWasmApp("test", "Test", (char*)apps_test_build_app_wasm, apps_test_build_app_wasm_len));
    apps::registry.push(new apps::SystemWasmApp("counter", "Counter", (char*)apps_counter_build_app_wasm, apps_counter_build_app_wasm_len));
}