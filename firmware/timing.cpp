#include <Arduino.h>

#ifndef DC_SIMULATOR
    #include <nrf_clock.h>
    #include <nrf_timer.h>
#endif

#include "timing.h"

const int daysInEarthMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned long currentTick = 0;
unsigned int currentMillisecondOffset = 0;

timing::EarthTime timing::earthTime(2025, 1, 1, 12, 0, 0);

timing::Time::Time(int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second) {
    _year = 0;
    _dayOfYear = 0;
    _millisecondOfDay = 0;

    setDate(year, month, day);
    setTime(hour, minute, second);
}

timing::Time::Time(int year, unsigned int dayOfYear, unsigned long millisecondOfDay) {
    _year = year;
    _dayOfYear = dayOfYear;
    _millisecondOfDay = millisecondOfDay;
}

timing::Time::Time() {
    _year = 0;
    _dayOfYear = 0;
    _millisecondOfDay = 0;
}

bool timing::Time::inLeapMillisecond() {
    struct LeapAdjustment leapAdjustment = leapAdjustmentToday();

    if (leapAdjustment.adjustment == 0) {
        return false;
    }

    if (_millisecondOfDay < (long)leapAdjustment.occurrence) {
        return false;
    }

    if (_millisecondOfDay > (long)leapAdjustment.occurrence + leapAdjustment.adjustment) {
        return false;
    }

    return true;
}

unsigned long timing::Time::postLeapMillisecondOffset() {
    struct LeapAdjustment leapAdjustment = leapAdjustmentToday();

    if (leapAdjustment.adjustment == 0) {
        return 0;
    }

    if (_millisecondOfDay < (long)leapAdjustment.occurrence) {
        return 0;
    }

    return leapAdjustment.adjustment;
}

void timing::Time::setDate(int year, unsigned int month, unsigned int day) {
    if (year > 0) {
        year--;
    }

    _year = year;
    _dayOfYear = 0;

    day--;

    for (unsigned int currentMonth = 1; currentMonth < month; currentMonth++) {
        _dayOfYear += daysInMonth(currentMonth);
    }

    _dayOfYear += day;

}

void timing::Time::setTime(unsigned int hour, unsigned int minute, unsigned int second) {
    _millisecondOfDay = 0;
    _millisecondOfDay += hour * 60 * 60 * 1000;
    _millisecondOfDay += minute * 60 * 1000;
    _millisecondOfDay += second * 1000;

    _accumulateDays();
}

void timing::Time::incrementTime(int milliseconds) {
    _millisecondOfDay += milliseconds;

    _accumulateDays();
}

void timing::Time::toLocalTime(int timeShift) {
    incrementTime(timeShift);

    _timeShift = timeShift;
}

void timing::Time::toGlobalTime() {
    return toLocalTime(-_timeShift);
}

int timing::Time::timeShift() {
    return _timeShift;
}

int timing::Time::year() {
    return _year >= 0 ? _year + 1 : _year;
}

unsigned int timing::Time::month() {
    int currentDay = _dayOfYear;
    unsigned int currentMonth = 1;

    while (true) {
        int daysInThisMonth = daysInMonth(currentMonth);

        if (currentDay < daysInThisMonth) {
            break;
        }

        currentMonth++;
        currentDay -= daysInThisMonth;
    }

    return currentMonth;
}

unsigned int timing::Time::day() {
    unsigned int day = _dayOfYear;
    unsigned int dayMonth = month();

    for (unsigned int currentMonth = 1; currentMonth < dayMonth; currentMonth++) {
        day -= daysInMonth(currentMonth);
    }

    return day + 1;
}

unsigned int timing::Time::hour() {
    return (millisecondOfDayIgnoringLeap() / 1000 / 60 / 60) % 24;
}

unsigned int timing::Time::minute() {
    return (millisecondOfDayIgnoringLeap() / 1000 / 60) % 60;
}

unsigned int timing::Time::second() {
    if (inLeapMillisecond()) {
        return 60 + ((_millisecondOfDay - leapAdjustmentToday().occurrence) / 1000);
    }

    return (millisecondOfDayIgnoringLeap() / 1000) % 60;
}

unsigned int timing::Time::millisecond() {
    return millisecondOfDayIgnoringLeap() % 1000;
}

unsigned int timing::Time::dayOfYear() {
    return _dayOfYear + 1;
}

unsigned long timing::Time::millisecondOfDay() {
    return _millisecondOfDay;
}

unsigned long timing::Time::millisecondOfDayIgnoringLeap() {
    return _millisecondOfDay - postLeapMillisecondOffset();
}

void timing::Time::_accumulateDays() {
    while (_millisecondOfDay < 0) {
        _dayOfYear--;
        _millisecondOfDay += millisecondsInDay();

        _accumulateYears();
    }

    while (_millisecondOfDay >= (long)millisecondsInDay()) {
        _millisecondOfDay -= millisecondsInDay();
        _dayOfYear++;

        _accumulateYears();
    }
}

void timing::Time::_accumulateYears() {
    while (_dayOfYear < 0) {
        _dayOfYear += daysInYear();
        _year--;
    }

    while (_dayOfYear >= (int)daysInYear()) {
        _dayOfYear -= daysInYear();
        _year++;
    }
}

struct timing::LeapAdjustment timing::EarthTimeNonLeaping::leapAdjustmentToday() {
    return NO_LEAP_ADJUSTMENT;
}

unsigned long timing::EarthTimeNonLeaping::millisecondsInDay() {
    return timing::Time::millisecondsInDay() + leapAdjustmentToday().adjustment;
}

unsigned int timing::EarthTimeNonLeaping::daysInYear() {
    unsigned int y = _year + 1;

    if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) {
        return 366;
    }

    return 365;
}

unsigned int timing::EarthTimeNonLeaping::daysInMonth(unsigned int month) {
    if (daysInYear() == 366 && month == 2) {
        return 29;
    }

    return daysInEarthMonth[(month - 1) % 12];
}

unsigned int timing::EarthTimeNonLeaping::hour() {
    return timing::Time::hour();
}

unsigned int timing::EarthTimeNonLeaping::minute() {
    return timing::Time::minute();
}

unsigned int timing::EarthTimeNonLeaping::second() {
    return timing::Time::second();
}

// @source reference https://stackoverflow.com/a/53155217
unsigned int timing::EarthTimeNonLeaping::weekday() {
    int y = year();
    int m = month();
    int d = day();
    static int offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    if (m < 3) {
        y--;
    }

    return (y + (y / 4) - (y / 100) + (y / 400) + offsets[m - 1] + d) % 7;
}

struct timing::LeapAdjustment timing::EarthTime::leapAdjustmentToday() {
    if (
        year() == _cachedLeapAdjustmentYear &&
        dayOfYear() == _cachedLeapAdjustmentDayOfYear &&
        _timeShift == _cachedLeapAdjustmentTimeShift
    ) {
        return _cachedLeapAdjustment;
    }

    _cachedLeapAdjustment = NO_LEAP_ADJUSTMENT;

    EarthTimeNonLeaping adjust_2025_06_30(2025, 6, 30, 23, 59, 59); // TODO: Remove this since no leap second has been announced for 2025

    adjust_2025_06_30.toLocalTime(_timeShift);

    if (year() == adjust_2025_06_30.year() && dayOfYear() == adjust_2025_06_30.dayOfYear()) {
        _cachedLeapAdjustment = (timing::LeapAdjustment) {adjust_2025_06_30.millisecondOfDay() + 1000, 1000};
    }

    _cachedLeapAdjustmentYear = year();
    _cachedLeapAdjustmentDayOfYear = dayOfYear();
    _cachedLeapAdjustmentTimeShift = _timeShift;

    return _cachedLeapAdjustment;
}

void timing::EarthTime::syncToSystemTime() {
    _year = earthTime.year();
    _dayOfYear = earthTime.dayOfYear();
    _millisecondOfDay = earthTime.millisecondOfDay();
    _timeShift = earthTime.timeShift();
}

unsigned long timing::getCurrentTick() {
    #ifndef DC_SIMULATOR
        return currentTick + ((1000 + millis() - currentMillisecondOffset) % 1000);
    #else
        return millis();
    #endif
}

#ifndef DC_SIMULATOR

TimerHandle_t tickHandle;

void tick(TimerHandle_t xTimer) {
    currentTick += timing::RTC_TICK_PERIOD;
    currentMillisecondOffset = millis() % 1000;
}

void timing::init() {
    tickHandle = xTimerCreate(nullptr, pdMS_TO_TICKS(timing::RTC_TICK_PERIOD), pdTRUE, nullptr, tick);

    xTimerStart(tickHandle, 0);
}

#else

void timing::init() {}

#endif