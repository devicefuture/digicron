#ifndef COUNTERSCREEN_H_
#define COUNTERSCREEN_H_

#include "../../applib/digicron.h"

#include "counters.h"

using namespace dc;
using namespace dc::dataTypes;

class CounterScreen : public ui::Screen {
    public:
        Counter* getCounter();

        void update() override;
        void handleEvent(ui::Event event) override;

    protected:
        int _counterIndex = 0;
};

extern CounterScreen* counterScreen;

#endif