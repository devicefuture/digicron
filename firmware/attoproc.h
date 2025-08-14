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
#include "fs.h"
#include "ui.h"
#include "atto.h"

namespace attoProc {
    class AttoProcess : public proc::Process {
        public:
            AttoProcess(String code);
            ~AttoProcess();

            proc::ProcessType getType() override {return proc::ProcessType::ATTO;}
            void step() override;
            void stop() override;
            ui::Screen* getMainScreen() {return _mainScreen;}
            void destroyOverlayScreen();

        protected:
            friend class atto::AttoBindings;
            friend class atto::AttoTextInput;
            friend class atto::AttoIntInput;
            friend class atto::AttoContextualMenu;
            friend class atto::AttoConfirmationMenu;

            catto_Context* _context;
            ui::Screen* _mainScreen;
            ui::Screen* _overlayScreen = nullptr;
            bool _willDestroyOverlayScreen = false;

            bool _blinkValue = false;
            unsigned int _valueBase = 10;
            long _minValue = ui::INT_INPUT_DEFAULT_MIN_VALUE;
            long _maxValue = ui::INT_INPUT_DEFAULT_MAX_VALUE;
            bool _delayActive = false;
            unsigned long _delayEndTime;

            dataTypes::List<fs::FileHandle> _fileHandles;
    };
}

#endif