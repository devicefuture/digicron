#ifndef NUMERIC_INPUTS_H_
#define NUMERIC_INPUTS_H_

#include "../ui.h"
#include "../timing.h"

namespace ui {
    const long INT_INPUT_DEFAULT_MIN_VALUE = -INT32_MAX;
    const long INT_INPUT_DEFAULT_MAX_VALUE = INT32_MAX;

    class IntInput : public Screen {
        public:
            using Screen::Screen;

            IntInput(String title, long value) : Screen() {
                setTitle(title);
                setValue(value);
                _updateDisplayValueRange();
            }

            IntInput(proc::Process* process, String title, long value) : Screen(process) {
                setTitle(title);
                setValue(value);
                _updateDisplayValueRange();
            }

            String getTitle() {return _title;}
            void setTitle(String title) {_title = title;}
            long getValue() {return _value;}
            void setValue(long value) {_value = value;}
            bool getValueBlinking() {return _blinkValue;}

            void setValueBlinking(bool blinkValue) {
                _blinkValue = blinkValue;
                _blinkStartTime = timing::getCurrentTick();
            }

            unsigned int getBase() {return _base;}

            void setBase(unsigned int base) {
                _base = base;
                _updateDisplayValueRange();
            }

            void setRange(long minValue, long maxValue) {
                _minValue = minValue;
                _maxValue = maxValue;
            }

            void open(bool urgent = false) override;

            void update() override;
            void _handleEvent(Event event) override;

        protected:
            String _title = "";
            long _value = 0;
            bool _blinkValue = false;
            unsigned int _blinkStartTime = 0;
            unsigned int _base = 10;
            long _minValue = INT_INPUT_DEFAULT_MIN_VALUE;
            long _maxValue = INT_INPUT_DEFAULT_MAX_VALUE;
            long _minDisplayValue = 0;
            long _maxDisplayValue = 0;

            void _updateDisplayValueRange();
    };

    extern Icon* numericValueChangeableIcon;
}

#endif