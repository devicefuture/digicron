#ifndef COUNTER_H_
#define COUNTER_H_

#include "../../applib/digicron.h"

using namespace dc;
using namespace dc::dataTypes;

class Counter {
    public:
        Counter(String name, long value = 0);

        String getName();
        void setName(String name);
        long getValue();
        void setValue(long value);
        void incrementValue(long amount);
        unsigned int getBase();
        void setBase(unsigned int base);
        long getResetValue();
        void setResetValue(long resetValue);

    private:
        String _name;
        long _value;
        unsigned int _base = 10;
        long _resetValue = 0;
};

extern List<Counter> counters;

#endif