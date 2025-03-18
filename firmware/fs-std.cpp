#ifdef DC_SIMULATOR

#include <unistd.h>

#include "fs.h"

bool fs::FileHandle::isAvailable() {
    return _isOpen && !feof(_file);
}

char fs::FileHandle::read() {
    if (!isAvailable()) {
        return '\0';
    }

    int c = fgetc(_file);

    if (c < 0) {
        return '\0';
    }

    return (char)c;
}

void fs::FileHandle::write(char c) {
    if (!_isOpen) {
        return;
    }

    if (!(_mode == FileMode::WRITE || _mode == FileMode::APPEND)) {
        return;
    }

    fputc(c, _file);
}

unsigned int fs::FileHandle::getSize() {
    if (!_isOpen) {
        return 0;
    }

    long currentPosition = ftell(_file);

    fseek(_file, 0, SEEK_END);

    unsigned int size = ftell(_file);

    fseek(_file, currentPosition, SEEK_SET);

    return size;
}

unsigned int fs::FileHandle::tell() {
    if (!_isOpen) {
        return 0;
    }

    return ftell(_file);
}

void fs::FileHandle::truncate(unsigned int size) {
    ftruncate(fileno(_file), size);
}

bool fs::FileHandle::_openFile(char* path) {
    _file = fopen(path, getFileModeString(_mode));

    return _file;
}

void fs::FileHandle::_seekFile(unsigned int position) {
    fseek(_file, position, SEEK_SET);
}

void fs::FileHandle::_closeFile() {
    fclose(_file);
}

bool fs::init() {
    return true;
}

#endif