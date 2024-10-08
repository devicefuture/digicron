#ifndef DC_SIMULATOR

#include <Arduino.h>
#include <nrfx.h>

#include "power.h"

#define VBAT_PIN A6

bool power::isCharging() {
    return NRF_POWER->USBREGSTATUS > 0;
}

float power::getBatteryVoltage() {
    float vbat = analogRead(VBAT_PIN);

    vbat *= 2; // Multiply due to measured value being halved by resistor divider
    vbat *= 3.6; // Multiply by reference voltage
    vbat /= 1024; // Convert data value to voltage

    return vbat;
}

float power::getBatteryLevel() {
    /*
        Logistical curve to convert battery voltage to level:
            y = L / (100 + e^(-k * (x - x_0)))
        Where:
            L = 100
            k = 12
            x = VBAT reading
            x_0 = 3.66
    */
    return 100 / (1 + exp(-12 * (getBatteryVoltage() - 3.66)));
}

#else

#include "power.h"

bool power::isCharging() {
    return true;
}

float power::getBatteryVoltage() {
    return 4.2;
}

float power::getBatteryLevel() {
    return 100;
}

#endif