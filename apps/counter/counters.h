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

    private:
        String _name;
        long _value;
};

extern List<Counter> counters;

#endif