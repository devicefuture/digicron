#ifndef COUNTERSCREEN_H_
#define COUNTERSCREEN_H_

#include "../../applib/digicron.h"

using namespace dc;
using namespace dc::dataTypes;

class CounterScreen : public dc::ui::Screen {
    public:
        void update() override;
        void handleEvent(dc::ui::Event event) override;

    protected:
        int _value = 0;
};

#endif