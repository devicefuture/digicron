#ifndef DC_SIMULATOR

#include <littlefs/lfs.h>

#include "fs.h"

lfs_t* fs::filesystem = nullptr;

bool fs::FileHandle::isAvailable() {
    return _isOpen;
}

char fs::FileHandle::read() {
    if (!isAvailable()) {
        return '\0';
    }

    static char readData[1];

    int result = lfs_file_read(filesystem, &_file, readData, 1);

    return result == 1 ? readData[0] : '\0';
}

void fs::FileHandle::write(char c) {
    if (!isAvailable()) {
        return;
    }

    if (!(_mode == FileMode::WRITE || _mode == FileMode::APPEND)) {
        return;
    }

    static char writeData[1];

    writeData[0] = c;

    lfs_file_write(filesystem, &_file, writeData, 1);
}

unsigned int fs::FileHandle::getSize() {
    if (!isAvailable()) {
        return 0;
    }

    int result = lfs_file_size(filesystem, &_file);

    return result > 0 ? result : 0;
}

unsigned int fs::FileHandle::tell() {
    if (!isAvailable()) {
        return 0;
    }

    return lfs_file_tell(filesystem, &_file);
}

bool fs::FileHandle::_openFile(char* path) {
    unsigned int flags;

    switch (_mode) {
        case FileMode::READ: flags = LFS_O_RDONLY; break;
        case FileMode::WRITE: flags = LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC; break;
        case FileMode::APPEND: flags = LFS_O_RDWR | LFS_O_CREAT | LFS_O_APPEND; break;
    }

    bool success = lfs_file_open(filesystem, &_file, path, flags) >= 0;

    return success;
}

void fs::FileHandle::_seekFile(unsigned int position) {
    lfs_file_seek(filesystem, &_file, position, LFS_SEEK_SET);
}

void fs::FileHandle::_closeFile() {
    lfs_file_close(filesystem, &_file);
}

bool fs::init() {
    // TODO: Implement filesystem init, including flash read/write calls
    return false;
}

#endif