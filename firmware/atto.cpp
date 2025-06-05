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

    _addStringConstant(context, "joyback", "Escape");
    _addStringConstant(context, "joyhome", "Home");
    _addStringConstant(context, "joyup", "ArrowUp");
    _addStringConstant(context, "joydown", "ArrowDown");
    _addStringConstant(context, "joyleft", "ArrowLeft");
    _addStringConstant(context, "joyright", "ArrowRight");
    _addStringConstant(context, "joysel", "Enter");
}

attoProc::AttoProcess* atto::AttoBindings::_getProcess(catto_Context* context) {
    return (attoProc::AttoProcess*)context->userData;
}

void atto::AttoBindings::_addStringConstant(catto_Context* context, const char* name, const char* value) {
    catto_TypedValue string = catto_asTypedString((catto_Char*)value);

    catto_setVariable(context, name, string);

    catto_addTypedValueToGc(context, string);
    catto_gc(context);
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
    
    char* string = catto_asString(catto_evalNextArg(context));
    unsigned int maxLength = catto_hasNextArg(context) ? catto_asNumber(catto_evalNextArg(context)) : display::COLUMNS;

    mainScreen->scroll(string, maxLength);

    free(string);
}

void atto::AttoBindings::_blit(catto_Context* context) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    mainScreen->blit();
    mainScreen->setBlitMode(true);
}

catto_TypedValue atto::AttoBindings::_key(catto_Context* context, catto_DataType returnType) {
    ui::Screen* mainScreen = _getProcess(context)->getMainScreen();

    if (ui::buttonDownScreen != mainScreen) {
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
    char* title;

    catto_TypedValue titleValue = catto_evalNextArg(context);
    catto_TypedValue itemsValue;
    catto_List* items;

    if (titleValue.type == CATTO_DATA_TYPE_STRING) {
        isContextual = true;
        title = catto_asString(titleValue);
        itemsValue = catto_evalNextArg(context);
    } else {
        itemsValue = titleValue;
    }

    if (itemsValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

        free(title);

        return;
    }

    items = itemsValue.value.asList;

    catto_AstNode* indexArg = catto_getNextArg(context);
    AttoContextualMenu* menu = new AttoContextualMenu(process, title, indexArg);

    free(title);

    process->_overlayScreen = menu;

    for (unsigned int i = 0; i < items->length; i++) {
        char* item = catto_asString(items->values[i]);

        menu->items.push(new String(item));

        free(item);
    }

    menu->open();
    menu->setCurrentIndex(catto_asNumber(catto_evalExpression(context, indexArg)));
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

        ((attoProc::AttoProcess*)ownerProcess)->destroyOverlayScreen();

        return;
    }

    if (event.type == ui::EventType::CANCEL) {
        catto_setVariable(_context, "cancel", catto_asTypedNumber(true));

        ((attoProc::AttoProcess*)ownerProcess)->destroyOverlayScreen();

        return;
    }
}