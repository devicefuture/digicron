#ifndef COUNTER_H_
#define COUNTER_H_

#include "../../applib/digicron.h"

using namespace dc;
using namespace dc::dataTypes;

class Counter {
    public:
        Counter(String name, long value = 0);

        String getId();
        String getName();
        void setName(String name);
        long getValue();
        void setValue(long value);
        void incrementValue(long amount);
        unsigned int getBase();
        void setBase(unsigned int base);
        long getResetValue();
        void setResetValue(long resetValue);

        bool checkIfShouldSave();
        void triggerSave();

        bool loadFromFile(String id);
        bool saveToFile();

    private:
        String _id;
        String _name;
        long _value;
        unsigned int _base = 10;
        long _resetValue = 0;
        unsigned char _savePendingState = 0;
};

extern List<Counter> counters;
extern bool saveTriggered;

#endif