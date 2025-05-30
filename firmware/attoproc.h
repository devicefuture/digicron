#ifndef ATTOPROC_H_
#define ATTOPROC_H_

#include <Arduino.h>
#include <catto-config.h>
#include <catto.h>

namespace attoProc {
    class AttoProcess;
}

#include "datatypes.h"
#include "proc.h"
#include "ui.h"
#include "atto.h"

namespace attoProc {
    class AttoProcess : public proc::Process {
        public:
            AttoProcess(String code);

            proc::ProcessType getType() override {return proc::ProcessType::ATTO;}
            void step() override;
            void stop() override;
            ui::Screen* getMainScreen() {return _mainScreen;}
            void destoryOverlayScreen();

        protected:
            friend class atto::AttoBindings;
            friend class atto::AttoContextualMenu;

            catto_Context* _context;
            ui::Screen* _mainScreen;
            ui::Screen* _overlayScreen = nullptr;
            bool _willDestroyOverlayScreen = false;
            atto::AttoErrorMessageScreen* _errorMessageScreen;
    };
}

#endif