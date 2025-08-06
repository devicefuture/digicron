#include "attoproc.h"

atto::AttoErrorMessageScreen errorMessageScreen;

attoProc::AttoProcess::AttoProcess(String code) : proc::Process::Process() {
    _context = catto_newContext();

    _context->userData = this;

    catto_addContextStandardCommands(_context);

    cattox_test_init(_context);
    cattox_csv_init(_context);

    atto::bindings.bindToContext(_context);

    catto_load(_context, code.c_str());

    _mainScreen = new ui::Screen(this);

    _mainScreen->open(true);
}

void attoProc::AttoProcess::step() {
    if (!_running) {
        return;
    }

    if (_overlayScreen) {
        if (_willDestroyOverlayScreen) {
            delete _overlayScreen;

            _overlayScreen = nullptr;
            _willDestroyOverlayScreen = false;
        } else {
            return;
        }
    }

    if (!catto_step(_context)) {
        stop();

        return;
    }
}

void attoProc::AttoProcess::stop() {
    if (!_running) {
        return;
    }

    if (_context->errorState != CATTO_ERROR_STATE_NONE) {
        String message = "Unknown error";

        switch (_context->errorState) {
            case CATTO_ERROR_STATE_UNEXPECTED_TOKEN: message = "Unexpected token"; break;
            case CATTO_ERROR_STATE_NO_RETURN: message = "Nothing to return to"; break;
            case CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK: message = "Mismatched statement opening mark"; break;
            case CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK: message = "Mismatched statement closing mark"; break;
            case CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP: message = "Loop control command was used outside of loop"; break;
            case CATTO_ERROR_STATE_UNKNOWN_PROCEDURE: message = "Procedure is not defined"; break;
            case CATTO_ERROR_STATE_NOT_A_FUNCTION: message = "Attempt to call variable that is not a function"; break;
            case CATTO_ERROR_STATE_NOT_A_LIST: message = "Cannot perform list operation on non-list variable"; break;
            case CATTO_ERROR_STATE_INVALID_LIST_VALUE: message = "Invalid list value"; break;
            case CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE: message = "Expected variable name"; break;
            case CATTO_ERROR_STATE_UNKNOWN_FIELD: message = "Field is not present in list declaration"; break;

            default: break;
        }

        errorMessageScreen.resetScroll();
        errorMessageScreen.setMessage(message);

        errorMessageScreen.open();
    }

    catto_freeContext(_context);

    delete _mainScreen;

    if (_overlayScreen) {
        delete _overlayScreen;
    }

    Process::stop();
}

void attoProc::AttoProcess::destroyOverlayScreen() {
    _willDestroyOverlayScreen = true;
}