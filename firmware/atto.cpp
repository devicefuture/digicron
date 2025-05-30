#include "atto.h"
#include "input.h"
#include "ui.h"

atto::AttoBindings atto::bindings;

void atto::AttoBindings::bindToContext(catto_Context* context) {
    catto_addCommand(context, "print", &_print);
    catto_addCommand(context, "cls", &_cls);
    catto_addCommand(context, "pos", &_pos);
    catto_addCommand(context, "resetscroll", &_resetScroll);
    catto_addCommand(context, "scroll", &_scroll);
    catto_addCommand(context, "blit", &_blit);

    catto_addCommand(context, "menu", &_menu);

    catto_addFunction(context, "key", &_key);

    catto_setVariable(context, "joyback", catto_asTypedString((catto_Char*)"Escape"));
    catto_setVariable(context, "joyhome", catto_asTypedString((catto_Char*)"Home"));
    catto_setVariable(context, "joyup", catto_asTypedString((catto_Char*)"ArrowUp"));
    catto_setVariable(context, "joydown", catto_asTypedString((catto_Char*)"ArrowDown"));
    catto_setVariable(context, "joyleft", catto_asTypedString((catto_Char*)"ArrowLeft"));
    catto_setVariable(context, "joyright", catto_asTypedString((catto_Char*)"ArrowRight"));
    catto_setVariable(context, "joysel", catto_asTypedString((catto_Char*)"Enter"));
}

attoProc::AttoProcess* atto::AttoBindings::_getProcess(catto_Context* context) {
    return (attoProc::AttoProcess*)context->userData;
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

void atto::AttoBindings::_pos(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    mainScreen->setPosition(
        catto_asNumber(catto_evalNextArg(context)),
        catto_asNumber(catto_evalNextArg(context))
    );
}

void atto::AttoBindings::_resetScroll(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();
    
    mainScreen->resetScroll();
}

void atto::AttoBindings::_scroll(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();
    
    String string = catto_asString(catto_evalNextArg(context));
    unsigned int maxLength = catto_hasNextArg(context) ? catto_asNumber(catto_evalNextArg(context)) : display::COLUMNS;

    mainScreen->scroll(string, maxLength);
}

void atto::AttoBindings::_blit(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    mainScreen->blit();
    mainScreen->setBlitMode(true);
}

catto_TypedValue atto::AttoBindings::_key(catto_Context* context, catto_DataType returnType) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    if (ui::currentScreen != mainScreen) {
        return catto_asTypedString((catto_Char*)"");
    }

    const char* string = "";

    switch (input::getButtonStatus()) {
        case input::Button::BACK: string = "Escape"; break;
        case input::Button::HOME: string = "Home"; break;
        case input::Button::UP: string = "ArrowUp"; break;
        case input::Button::DOWN: string = "ArrowDown"; break;
        case input::Button::LEFT: string = "ArrowLeft"; break;
        case input::Button::RIGHT: string = "ArrowRight"; break;
        case input::Button::SELECT: string = "Enter"; break;
        default: break;
    }

    return catto_asTypedString((catto_Char*)string);
}

void atto::AttoBindings::_menu(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);
    bool isContextual = false;
    String title;

    catto_TypedValue titleArg = catto_evalNextArg(context);
    catto_TypedValue itemsArg;
    catto_List* items;

    if (titleArg.type == CATTO_DATA_TYPE_STRING) {
        isContextual = true;
        title = catto_asString(titleArg);
        itemsArg = catto_evalNextArg(context);
    } else {
        itemsArg = titleArg;
    }

    if (itemsArg.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

        return;
    }

    items = itemsArg.value.asList;

    process->_overlayScreen = new AttoContextualMenu(process, title, catto_getNextArg(context));

    for (unsigned int i = 0; i < items->length; i++) {
        ((ui::Menu*)process->_overlayScreen)->items.push(new String(catto_asString(items->values[i])));
    }

    process->_overlayScreen->open();
}

void atto::AttoErrorMessageScreen::update() {
    clear();

    print("APP ERR\n");
    scroll(_message);
}

void atto::AttoErrorMessageScreen::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::BUTTON_DOWN && event.data.button == input::Button::BACK) {
        close();
    }
}

atto::AttoContextualMenu::AttoContextualMenu(attoProc::AttoProcess* process, String title, catto_AstNode* resultVariable) : ui::ContextualMenu(process, title) {
    _context = process->_context;
    _resultVariable = resultVariable;
}

void atto::AttoContextualMenu::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        catto_assignValue(_context, _resultVariable, catto_asTypedNumber(event.data.index));
        catto_setVariable(_context, "cancel", catto_asTypedNumber(false));

        ((attoProc::AttoProcess*)ownerProcess)->destoryOverlayScreen();

        return;
    }

    if (event.type == ui::EventType::CANCEL) {
        catto_setVariable(_context, "cancel", catto_asTypedNumber(true));

        ((attoProc::AttoProcess*)ownerProcess)->destoryOverlayScreen();

        return;
    }
}