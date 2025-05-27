#include "display.h"

#ifndef DC_SIMULATOR
    HCMS39xx display::driver(16, DATA_PIN, RS_PIN, CLOCK_PIN, ENABLE_PIN, BLANK_PIN);
#else
    #include <emscripten.h>
    #include <emscripten/bind.h>

    char simulatorData[display::DATA_SIZE];

    EM_JS(void, sendDisplayDataToSimulator, (uint8_t* dataPtr, uint32_t size), {
        renderDisplayData(new Uint8ClampedArray(HEAPU8.buffer.slice(dataPtr), 0, size));
    });
#endif

void display::init() {
    #ifndef DC_SIMULATOR
        driver.begin();
        driver.clear();
        driver.displayUnblank();
        driver.setBrightness(15);
    #endif
}

void display::render(char data[display::DATA_SIZE]) {
    #ifndef DC_SIMULATOR
        driver.printDirect((uint8_t*)data, display::DATA_SIZE);
    #else
        memcpy(simulatorData, data, display::DATA_SIZE);
    #endif
}

#ifdef DC_SIMULATOR
    void render() {
        sendDisplayDataToSimulator((uint8_t*)simulatorData, display::DATA_SIZE);
    }

    EMSCRIPTEN_BINDINGS(dc_display) {
        emscripten::function("display_render", render);
    }
#endif