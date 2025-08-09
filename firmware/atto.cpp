#include "atto.h"
#include "fs.h"
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
    catto_addCommand(context, "intinput", &_intinput);
    catto_addCommand(context, "menu", &_menu);
    catto_addCommand(context, "yesno", &_yesno);
    catto_addCommand(context, "noyes", &_noyes);
    catto_addCommand(context, "blinkvalue", &_blinkvalue);
    catto_addCommand(context, "valuebase", &_valuebase);
    catto_addCommand(context, "valuerange", &_valuerange);
    catto_addCommand(context, "minvalue", &_minvalue);
    catto_addCommand(context, "maxvalue", &_maxvalue);

    catto_addCommand(context, "open", &_open);
    catto_addCommand(context, "close", &_close);
    catto_addCommand(context, "read", &_read);
    catto_addCommand(context, "write", &_write);

    catto_addFunction(context, "key", &_key);
    catto_addFunction(context, "exists", &_exists);

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

catto_TypedValue atto::AttoBindings::_exists(catto_Context* context, catto_DataType returnType) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_Char* path = catto_asString(catto_evalNextArg(context));

    bool result = fs::exists(path);

    free(path);

    return catto_asTypedNumber(result ? 1 : 0);
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

void atto::AttoBindings::_intinput(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_TypedValue titleValue = catto_evalNextArg(context);
    catto_Char* title = catto_asString(titleValue);
    catto_AstNode* valueArg = catto_getNextArg(context);

    catto_Int initialValue = catto_hasNextArg(context) ? catto_asNumber(catto_evalNextArg(context)) : 0;
    AttoIntInput* input = new AttoIntInput(process, valueArg);

    process->_overlayScreen = input;

    input->setTitle(title);
    input->setValue(initialValue);
    input->open();

    if (process->_blinkValue) {
        input->setValueBlinking(true);

        process->_blinkValue = false;
    }

    if (process->_valueBase) {
        input->setBase(process->_valueBase);

        process->_valueBase = 10;
    }

    input->setRange(process->_minValue, process->_maxValue);

    process->_minValue = ui::INT_INPUT_DEFAULT_MIN_VALUE;
    process->_maxValue = ui::INT_INPUT_DEFAULT_MAX_VALUE;

    free(title);
}

void atto::AttoBindings::_menu(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);
    bool isContextual = false;
    char* title;

    catto_TypedValue titleValue = catto_evalNextArg(context);
    catto_TypedValue itemsValue;
    catto_List* items;

    if (titleValue.type != CATTO_DATA_TYPE_LIST) {
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

void atto::AttoBindings::_valuebase(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_Int valueBase = catto_asNumber(catto_evalNextArg(context));

    if (valueBase != 2 && valueBase != 8 && valueBase != 10 && valueBase != 16) {
        return;
    }

    process->_valueBase = valueBase;
}

void atto::AttoBindings::_valuerange(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    process->_minValue = catto_asNumber(catto_evalNextArg(context));
    process->_maxValue = catto_asNumber(catto_evalNextArg(context));
}

void atto::AttoBindings::_minvalue(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    process->_minValue = catto_asNumber(catto_evalNextArg(context));
}

void atto::AttoBindings::_maxvalue(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    process->_maxValue = catto_asNumber(catto_evalNextArg(context));
}

void atto::AttoBindings::_open(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);
    fs::FileMode fileMode = fs::FileMode::READ;

    catto_Char* path = catto_asString(catto_evalNextArg(context));
    catto_AstNode* handleIdArg = catto_getNextArg(context);
    catto_Char* mode = catto_asString(catto_evalNextArg(context));

    bool shouldCreateParentDirectories = false;

    if (catto_stringsEqualCaseInsensitive(mode, "w")) {
        fileMode = fs::FileMode::WRITE;
        shouldCreateParentDirectories = true;
    } else if (catto_stringsEqualCaseInsensitive(mode, "a")) {
        fileMode = fs::FileMode::APPEND;
        shouldCreateParentDirectories = true;
    }

    if (shouldCreateParentDirectories) {
        fs::ensureParentDirectories(path);
    }

    fs::FileHandle* fileHandle = fs::open(process, path, fileMode);

    if (fileHandle) {
        unsigned int handleId = -1;

        for (unsigned int i = 0; i < process->_fileHandles.length(); i++) {
            if (!process->_fileHandles[i]) {
                handleId = i;

                break;
            }
        }

        if (handleId == -1) {
            process->_fileHandles.push(fileHandle);

            handleId = process->_fileHandles.length() - 1;
        } else {
            process->_fileHandles.set(handleId, fileHandle);
        }

        catto_assignValue(context, handleIdArg, catto_asTypedNumber(handleId));
    } else {
        // TODO: Throw a better error in atto
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
    }

    free(path);
    free(mode);
}

void atto::AttoBindings::_close(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_Count handleId = catto_asNumber(catto_evalNextArg(context));

    fs::FileHandle* fileHandle = process->_fileHandles[handleId];

    delete fileHandle;

    process->_fileHandles.set(handleId, nullptr);
}

void atto::AttoBindings::_read(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_Count handleId = catto_asNumber(catto_evalNextArg(context));
    catto_AstNode* valueArg = catto_getNextArg(context);
    catto_Count length = catto_asNumber(catto_evalNextArg(context));

    fs::FileHandle* fileHandle = process->_fileHandles[handleId];

    if (!fileHandle) {
        // TODO: Throw a better error in atto
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        return;
    }

    String value = "";

    for (unsigned int i = 0; i < length || length == 0; i++) {
        if (!fileHandle->isAvailable()) {
            break;
        }

        char c = fileHandle->read();

        if (!c) {
            break;
        }

        value.concat(c);
    }

    printf("Read %s\n", value.c_str());

    catto_assignValue(context, valueArg, catto_asTypedString(value.c_str()));
}

void atto::AttoBindings::_write(catto_Context* context) {
    attoProc::AttoProcess* process = _getProcess(context);

    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Count handleId = catto_asNumber(catto_evalNextArg(context));

    fs::FileHandle* fileHandle = process->_fileHandles[handleId];

    if (!fileHandle) {
        // TODO: Throw a better error in atto
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        free(value);
        return;
    }

    fileHandle->write(value, catto_stringLength(value));
    printf("Write %s\n", value);

    free(value);
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

atto::AttoIntInput::AttoIntInput(attoProc::AttoProcess* process, catto_AstNode* resultVariable) : ui::IntInput(process) {
    _context = process->_context;
    _resultVariable = resultVariable;
}

void atto::AttoIntInput::handleEvent(ui::Event event) {
    if (event.type == ui::EventType::CONFIRM_VALUE) {
        catto_assignValue(_context, _resultVariable, catto_asTypedNumber(getValue()));
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