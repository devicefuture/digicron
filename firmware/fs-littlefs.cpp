#ifndef DC_SIMULATOR

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

#include "fs.h"

bool fs::FileHandle::isAvailable() {
    return _isOpen && _file->available();
}

char fs::FileHandle::read() {
    if (!isAvailable()) {
        return '\0';
    }

    static char c;

    int result = _file->read(&c, 1);

    return result == 1 ? c : '\0';
}

void fs::FileHandle::write(char c) {
    if (!_isOpen) {
        return;
    }

    if (!(_mode == FileMode::WRITE || _mode == FileMode::APPEND)) {
        return;
    }

    _file->write(&c, 1);
}

unsigned int fs::FileHandle::getSize() {
    if (!_isOpen) {
        return 0;
    }

    return _file->size();
}

unsigned int fs::FileHandle::tell() {
    if (!isAvailable()) {
        return 0;
    }

    return _file->position();
}

void fs::FileHandle::truncate(unsigned int size) {
    _file->truncate(size);
}

bool fs::FileHandle::_openFile(char* path) {
    unsigned int fileMode = _mode == FileMode::READ ? Adafruit_LittleFS_Namespace::FILE_O_READ : Adafruit_LittleFS_Namespace::FILE_O_WRITE;

    _file = new Adafruit_LittleFS_Namespace::File(InternalFS);

    if (!_file->open(path, fileMode)) {
        return false;
    }

    if (_mode == FileMode::WRITE) {
        _file->truncate(0);
        _file->seek(0);
    }

    return true;
}

void fs::FileHandle::_seekFile(unsigned int position) {
    _file->seek(position);
}

void fs::FileHandle::_closeFile() {
    _file->close();
}

bool fs::init() {
    return InternalFS.begin();
}

#endif