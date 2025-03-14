#include "fs.h"
#include "datatypes.h"

auto openFileHandles = dataTypes::List<fs::FileHandle>();

fs::FileHandle::FileHandle(proc::Process* process) : fs::FileHandle::FileHandle() {
    ownerProcess = process;
}

fs::FileHandle::FileHandle(String path, FileMode mode) {
    bool isAlreadyOpen = isFileOpen(path);
    char pathCharArray[path.length() + 1];

    _path = path;
    _mode = mode;
    _isOpen = true;
    _errorOnOpen = false;

    path.toCharArray(pathCharArray, sizeof(pathCharArray));

    openFileHandles.push(this);

    if (isAlreadyOpen) {
        close();
        return;
    }

    if (path == "" || !_openFile(pathCharArray)) {
        _errorOnOpen = true;
        close();
        return;
    }
}

fs::FileHandle::FileHandle(proc::Process* process, String path, FileMode mode) : fs::FileHandle::FileHandle(path, mode) {
    ownerProcess = process;
}

fs::FileHandle::~FileHandle() {
    close();
}

String fs::FileHandle::readString() {
    String data = "";

    while (isAvailable()) {
        char c = read();

        if (c == '\0') {
            break;
        }

        data += c;
    }

    return data;
}

const char* fs::FileHandle::readChars() {
    String string = readString();
    
    return string.c_str();
}

void fs::FileHandle::write(char* charArray) {
    unsigned int i = 0;

    while (charArray[i] != '\0') {
        write(charArray[i++]);
    }
}

void fs::FileHandle::write(String string) {
    write(string.c_str());
}

void fs::FileHandle::seek(int position, fs::SeekOrigin origin) {
    if (!isAvailable()) {
        return;
    }

    switch (origin) {
        case SeekOrigin::START:
        default:
            break;

        case SeekOrigin::CURRENT:
            position += tell();
            break;

        case SeekOrigin::END:
            position += getSize();
            break;
    }

    _seekFile(position);
}

void fs::FileHandle::start() {
    seek(0);
}

void fs::FileHandle::close() {
    if (!_isOpen) {
        return;
    }

    _isOpen = false;

    int handleIndex = openFileHandles.indexOf(this);

    if (handleIndex >= 0) {
        openFileHandles.remove(handleIndex);
    }

    if (_errorOnOpen) {
        Serial.print("Unable to open file at path: ");
        Serial.println(_path);
        return;
    }

    _closeFile();
}

fs::FileHandle* fs::open(String path, fs::FileMode mode) {
    auto fileHandle = new FileHandle(path, mode);

    if (!fileHandle->isOpen()) {
        return nullptr;
    }

    return fileHandle;
}

const char* fs::getFileModeString(fs::FileMode mode) {
    switch (mode) {
        case FileMode::READ: default: return "r";
        case FileMode::WRITE: return "w";
        case FileMode::APPEND: return "a";
    }
}

bool fs::isFileOpen(String path) {
    openFileHandles.start();

    while (auto fileHandle = openFileHandles.next()) {
        if (fileHandle->getPath() == path) {
            return true;
        }
    }

    return false;
}