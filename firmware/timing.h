#ifndef TIME_H_
#define TIME_H_

namespace timing {
    struct LeapAdjustment {
        unsigned long occurrence;
        int adjustment;
    };

    const struct LeapAdjustment NO_LEAP_ADJUSTMENT = {0, 0};

    const int RTC_TICK_PERIOD = 1000;

    class Time {
        public:
            unsigned long nonLeapMillisecondsInDay = 24 * 60 * 60 * 1000;

            Time(int year, unsigned int month, unsigned int day, unsigned int hour = 0, unsigned int minute = 0, unsigned int second = 0);
            Time(int year, unsigned int dayOfYear, unsigned long millisecondOfDay = 0);
            Time();

            virtual unsigned int daysInYear() {return 1;}
            virtual unsigned int daysInMonth(unsigned int month) {return daysInYear();}
            virtual unsigned long millisecondsInDay() {return nonLeapMillisecondsInDay;}
            virtual struct LeapAdjustment leapAdjustmentToday() {return NO_LEAP_ADJUSTMENT;}

            bool inLeapMillisecond();
            unsigned long postLeapMillisecondOffset();

            void setDate(int year, unsigned int month, unsigned int day);
            void setTime(unsigned int hour, unsigned int minute, unsigned int second);
            void incrementTime(int milliseconds);
            void toLocalTime(int timeShift);
            void toGlobalTime();
            int timeShift();

            virtual int year();
            virtual unsigned int month();
            virtual unsigned int day();
            virtual unsigned int hour();
            virtual unsigned int minute();
            virtual unsigned int second();
            virtual unsigned int millisecond();

            unsigned int dayOfYear();
            virtual unsigned long millisecondOfDay();
            virtual unsigned long millisecondOfDayIgnoringLeap();

            virtual unsigned int weekday() {return 0;}

        protected:
            int _year;
            int _dayOfYear;
            long _millisecondOfDay;
            int _timeShift;

            void _accumulateDays();
            void _accumulateYears();

            struct LeapAdjustment _cachedLeapAdjustment = NO_LEAP_ADJUSTMENT;
            int _cachedLeapAdjustmentYear = 0;
            int _cachedLeapAdjustmentDayOfYear = -1;
            int _cachedLeapAdjustmentTimeShift = 0;
    };

    class EarthTimeNonLeaping : public Time {
        public:
            EarthTimeNonLeaping(int year, unsigned int month, unsigned int day, unsigned int hour = 0, unsigned int minute = 0, unsigned int second = 0) : Time(year, month, day, hour, minute, second) {
                setDate(year, month, day);
                setTime(hour, minute, second);
            };

            EarthTimeNonLeaping(int year, unsigned int dayOfYear, unsigned long millisecondOfDay = 0) : Time(year, dayOfYear, millisecondOfDay) {};
            EarthTimeNonLeaping() : Time() {}

            unsigned int daysInYear() override;
            unsigned int daysInMonth(unsigned int month) override;
            unsigned long millisecondsInDay() override;
            struct LeapAdjustment leapAdjustmentToday() override;

            unsigned int hour() override;
            unsigned int minute() override;
            unsigned int second() override;

            unsigned int weekday() override;
    };

    class EarthTime : public EarthTimeNonLeaping {
        public:
            using EarthTimeNonLeaping::EarthTimeNonLeaping;

            struct LeapAdjustment leapAdjustmentToday() override;
            void syncToSystemTime();
    };

    extern timing::EarthTime earthTime;

    unsigned long getCurrentTick();
    unsigned long getCurrentMicrosecond();

    void init();
}

#endif