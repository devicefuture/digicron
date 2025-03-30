#ifdef DC_SIMULATOR

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <ftw.h>

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

const char* getFileModeString(fs::FileMode mode) {
    switch (mode) {
        case fs::FileMode::READ: default: return "r";
        case fs::FileMode::WRITE: return "w";
        case fs::FileMode::APPEND: return "a";
    }
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

bool fs::exists(String path) {
    return getEntryType(path) != EntryType::ERROR;
}

fs::EntryType fs::getEntryType(String path) {
    struct stat s;

    if (stat(path.c_str(), &s) != 0) {
        return EntryType::ERROR;
    }

    return S_ISDIR(s.st_mode) ? EntryType::DIRECTORY : EntryType::FILE;
}

// @source reference https://stackoverflow.com/a/5467788
int removerCallback(const char* fpath, const struct stat *sb, int typeflag, FTW* ftwbuf) {
    return remove(fpath);
}

bool fs::remove(String path) {
    if (isFileOpen(path)) {
        return false;
    }

    EntryType type = getEntryType(path);

    if (type == EntryType::DIRECTORY) {
        return nftw(path.c_str(), removerCallback, 64, FTW_DEPTH | FTW_PHYS) == 0;
    }

    return ::remove(path.c_str()) == 0;
}

bool fs::rename(String oldPath, String newPath) {
    return ::rename(oldPath.c_str(), newPath.c_str()) == 0;
}

// @source reference https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
bool createDirectoryPart(String path) {
    if (mkdir(path.c_str(), 0777) == 0) {
        return true;
    }

    // If error is unrelated to file existence, then fail
    if (errno != EEXIST) {
        return false;
    }

    // Fail if the existing entry is not a directory
    if (fs::getEntryType(path) != fs::EntryType::DIRECTORY) {
        return false;
    }

    return true;
}

bool createDirectoryIncludingParents(String path, bool parentsOnly) {
    unsigned int lastIndex = 0;

    while (true) {
        int currentIndex = path.indexOf('/', lastIndex + 1);

        if (currentIndex < 0) {
            if (parentsOnly) {
                break;
            }

            if (!createDirectoryPart(path)) {
                return false;
            }

            break;
        }

        lastIndex = currentIndex;

        if (currentIndex == 0) {
            // Currently looking at root part of path; skip it
            continue;
        }

        if (!createDirectoryPart(path.substring(0, currentIndex))) {
            return false;
        }
    }

    return true;
}

bool fs::createDirectory(String path) {
    return createDirectoryIncludingParents(path, false);
}

bool fs::ensureParentDirectories(String path) {
    return createDirectoryIncludingParents(path, true);
}

// @source reference https://stackoverflow.com/a/12506
fs::DirectoryListing* fs::listDirectory(proc::Process* process, String path) {
    if (getEntryType(path) != EntryType::DIRECTORY) {
        return nullptr;
    }

    DIR* dir = opendir(path.c_str());

    if (!dir) {
        return nullptr;
    }

    struct dirent *entry;

    auto listing = new DirectoryListing(process);

    while ((entry = readdir(dir)) != nullptr) {
        String name = String(entry->d_name);

        if (name == "." || name == "..") {
            continue;
        }

        listing->_addEntry(name);
    }

    if (closedir(dir) != 0) {
        delete listing;

        return nullptr;
    }

    return listing;
}

fs::DirectoryListing* fs::listDirectory(String path) {
    return listDirectory(nullptr, path);
}

#endif