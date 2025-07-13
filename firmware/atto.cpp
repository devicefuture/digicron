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

    catto_addCommand(context, "input", &_input);
    catto_addCommand(context, "menu", &_menu);
    catto_addCommand(context, "yesno", &_yesno);
    catto_addCommand(context, "noyes", &_noyes);
    catto_addCommand(context, "blinkvalue", &_blinkvalue);

    catto_addFunction(context, "key", &_key);

    catto_setVariable(context, "cols", catto_asTypedNumber(8));
    catto_setVariable(context, "rows", catto_asTypedNumber(2));

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

void atto::AttoBindings::_input(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_AstNode* valueArg = catto_getNextArg(context);

    if (valueArg->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF || !valueArg->value.asExpressionLeaf.subjectVariable) {
        // Retain compatibility for calls that use a fixed string prompt as first argument
        valueArg = catto_getNextArg(context);
    }

    catto_Char* initialValue = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString("");
    AttoTextInput* input = new AttoTextInput(process, valueArg);

    process->_overlayScreen = input;

    input->setValue(initialValue);
    input->open();
    input->selectAll();

    free(initialValue);
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
    catto_Int initialIndex = catto_asNumber(catto_evalNextArg(context));
    AttoContextualMenu* menu = new AttoContextualMenu(process, title, indexArg);

    free(title);

    process->_overlayScreen = menu;

    for (unsigned int i = 0; i < items->length; i += items->fieldCount > 0 ? items->fieldCount : 1) {
        char* item = catto_asString(items->values[i]);

        menu->items.push(new String(item));

        free(item);
    }

    menu->open();
    menu->setCurrentIndex(initialIndex >= 0 ? initialIndex : items->length + initialIndex);

    if (process->_blinkValue) {
        menu->setSelectionBlinking(true);

        process->_blinkValue = false;
    }
}

void atto::AttoBindings::_yesno_or_noyes(catto_Context* context, bool swapYesNo) {
    attoProc::AttoProcess* process = _getProcess(context);
    char* title;

    catto_TypedValue titleValue = catto_evalNextArg(context);

    title = catto_asString(titleValue);

    catto_AstNode* indexArg = catto_getNextArg(context);
    AttoConfirmationMenu* menu = new AttoConfirmationMenu(process, title, swapYesNo, indexArg);

    free(title);

    process->_overlayScreen = menu;

    catto_TypedValue itemsValue = catto_evalNextArg(context);

    if (itemsValue.type == CATTO_DATA_TYPE_LIST) {
        catto_List* items = itemsValue.value.asList;

        for (unsigned int i = 0; i < items->length; i += items->fieldCount > 0 ? items->fieldCount : 1) {
            char* item = catto_asString(items->values[i]);

            menu->items.push(new String(item));

            free(item);
        }
    }

    menu->open();
}

void atto::AttoBindings::_yesno(catto_Context* context) {
    _yesno_or_noyes(context, false);
}

void atto::AttoBindings::_noyes(catto_Context* context) {
    _yesno_or_noyes(context, true);
}

void atto::AttoBindings::_blinkvalue(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    process->_blinkValue = true;
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

atto::AttoTextInput::AttoTextInput(attoProc::AttoProcess* process, catto_AstNode* resultVariable) : ui::TextInput(process) {
    _context = process->_context;
    _resultVariable = resultVariable;
}

void atto::AttoTextInput::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::CONFIRM_VALUE) {
        catto_assignValue(_context, _resultVariable, catto_asTypedString(getValue().c_str()));
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

atto::AttoConfirmationMenu::AttoConfirmationMenu(attoProc::AttoProcess* process, String title, bool swapYesNo, catto_AstNode* resultVariable) : ui::ConfirmationMenu(process, title, swapYesNo) {
    _context = process->_context;
    _resultVariable = resultVariable;
}

void atto::AttoConfirmationMenu::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::ITEM_SELECT) {
        if (event.data.index < 2) {
            catto_assignValue(_context, _resultVariable, catto_asTypedNumber(yesSelected()));
        } else {
            catto_assignValue(_context, _resultVariable, catto_asTypedNumber(event.data.index));
        }

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