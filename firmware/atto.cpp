#include "atto.h"
#include "ui.h"

atto::AttoBindings atto::bindings;

void atto::AttoBindings::bindToContext(catto_Context* context) {
    catto_addCommand(context, "print", &_print);
    catto_addCommand(context, "cls", &_cls);
    catto_addCommand(context, "blit", &_blit);
}

proc::AttoProcess* atto::AttoBindings::_getProcess(catto_Context* context) {
    return (proc::AttoProcess*)context->userData;
}

void atto::AttoBindings::_print(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();
    bool appendFlag = false;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);
        char* string = catto_asString(catto_evalExpression(context, arg));

        if (!catto_hasNextArg(context) && catto_hasAppendFlag(arg)) {
            appendFlag = true;
        }

        mainScreen->print(string);

        free(string);

        if (catto_hasNextArg(context)) {
            mainScreen->print(" ");
        }
    }

    if (!appendFlag) {
        mainScreen->print("\n");
    }
}

void atto::AttoBindings::_cls(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    mainScreen->clear();
}

void atto::AttoBindings::_blit(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    mainScreen->blit();
    mainScreen->setBlitMode(true);
}