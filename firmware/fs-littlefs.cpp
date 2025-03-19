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

    if (_file->isDirectory()) {
        _closeFile();

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

bool fs::exists(String path) {
    return InternalFS.exists(path.c_str());
}

fs::EntryType fs::getEntryType(String path) {
    lfs_info info;

    if (lfs_stat(InternalFS._getFS(), path.c_str(), &info) < 0) {
        return EntryType::ERROR;
    }

    return info.type == LFS_TYPE_DIR ? EntryType::DIRECTORY : EntryType::FILE;
}

bool fs::remove(String path) {
    // `InternalFS` handles this for both files and directories recursively
    return InternalFS.remove(path.c_str());
}

bool fs::rename(String oldPath, String newPath) {
    return InternalFS.rename(oldPath.c_str(), newPath.c_str());
}

bool fs::createDirectory(String path) {
    // `InternalFS` also automatically creates parent directories where required
    return InternalFS.mkdir(path.c_str());
}

fs::DirectoryListing* fs::listDirectory(proc::Process* process, String path) {
    dataTypes::List<String> entries;

    if (getEntryType(path) != EntryType::DIRECTORY) {
        return nullptr;
    }

    lfs_t* fs = InternalFS._getFS();
    lfs_dir_t dir;

    if (lfs_dir_open(fs, &dir, path.c_str()) < 0) {
        return nullptr;
    }

    while (true) {
        lfs_info info;

        if (lfs_dir_read(fs, &dir, &info) != 1) {
            break;
        }

        String* name = new String(info.name);

        if (*name == "." || *name == "..") {
            continue;
        }

        entries.push(name);
    }

    if (lfs_dir_close(fs, &dir) < 0) {
        return nullptr;
    }

    return new DirectoryListing(process, entries);
}

fs::DirectoryListing* fs::listDirectory(String path) {
    return listDirectory(nullptr, path);
}

#endif