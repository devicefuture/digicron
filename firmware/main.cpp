#include <Arduino.h>

#ifndef DC_SIMULATOR
    #include <bluefruit.h>
    #include "HCMS39xx.h"
    #include "font5x7.h"
#else
    #include <emscripten.h>
    #include <stdio.h>
#endif

#include "datatypes.h"
#include "timing.h"
#include "power.h"
#include "display.h"
#include "ui.h"
#include "input.h"
#include "home.h"
#include "proc.h"
#include "fs.h"
#include "apps.h"
#include "_sysfs.h"

#define BACK_BTN_PIN 6
#define HOME_BTN_PIN 5
#define JOY_UP_PIN 10
#define JOY_DOWN_PIN 12
#define JOY_LEFT_PIN 9
#define JOY_RIGHT_PIN 11
#define JOY_SELECT_PIN 13

ui::Screen* testScreen = new ui::Screen();

ui::Icon* tmIcon = ui::constructIcon(
    "###  "
    " #   "
    "     "
    "## ##"
    "# # #"
);

unsigned int displayMode = 0;
long lastTick = 0;

#ifndef DC_SIMULATOR
    BLEDis bledis;
    BLEDfu bledfu;
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, world!");

    timing::init();
    display::init();
    input::init();

    ui::currentScreen = testScreen;

    testScreen->print(" device  future");
    testScreen->print(tmIcon);

    fs::init();

    sysfs::populate();
    apps::scan();

    #ifndef DC_SIMULATOR
        Bluefruit.begin();
        Bluefruit.setTxPower(0);
        Bluefruit.setName("DigiCron");

        bledis.setModel("DeviceFuture DigiCron");
        bledis.setManufacturer("LiveG Technologies");
        bledis.begin();

        bledfu.begin();

        Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
        Bluefruit.Advertising.addTxPower();
        Bluefruit.Advertising.addService(bledis);
        Bluefruit.Advertising.addService(bledfu);
        Bluefruit.ScanResponse.addName();
        Bluefruit.Advertising.setInterval(32, 244);
        Bluefruit.Advertising.setFastTimeout(30);
        Bluefruit.Advertising.start(10);

        while (true) {
            loop();
        }
    #endif
}

void loop() {
    long currentTick = timing::getCurrentTick();
    int difference = currentTick - lastTick;

    timing::earthTime.incrementTime(difference);

    lastTick = currentTick;

    if (ui::currentScreen == testScreen && millis() > 3000) {
        home::homeScreen.open(true);
    }

    // #ifndef DC_SIMULATOR
    //     Serial.printf("Free heap: %d\r\n", dbgHeapFree());
    // #endif

    proc::stepProcesses();
    ui::renderCurrentScreen();
}

#ifdef DC_SIMULATOR

int main(int argc, char** argv) {
    setup();

    emscripten_set_main_loop(loop, 0, true);

    return 0;
}

#endif