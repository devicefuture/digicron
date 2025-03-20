#ifndef FS_H_
#define FS_H_

#include <Arduino.h>

#ifndef DC_SIMULATOR
    #include <Adafruit_LittleFS.h>
#else
    #include <stdio.h>
#endif

#include "datatypes.h"
#include "proc.h"

namespace fs {
    enum EntryType {
        ERROR = 0,
        FILE,
        DIRECTORY
    };

    enum FileMode {
        READ,
        WRITE,
        APPEND
    };

    enum SeekOrigin {
        START,
        CURRENT,
        END
    };

    class FileHandle {
        public:
            proc::Process* ownerProcess = nullptr;

            FileHandle() : FileHandle("", FileMode::READ) {}
            FileHandle(proc::Process* process);
            FileHandle(String path, FileMode mode = FileMode::READ);
            FileHandle(proc::Process* process, String path, FileMode mode = FileMode::READ);
            ~FileHandle();

            String getPath() {return _path;}
            FileMode getMode() {return _mode;}
            bool isOpen() {return _isOpen;}
            bool isAvailable();

            char read();
            String readString();

            void write(char c);
            void write(String string);

            unsigned int getSize();
            unsigned int tell();
            void seek(int position, SeekOrigin origin = SeekOrigin::START);
            void truncate(unsigned int size);
            void start();
            void close();

        protected:
            String _path;
            FileMode _mode;
            bool _isOpen = false;
            bool _errorOnOpen = false;
            bool _alreadyOpen = false;

            #ifndef DC_SIMULATOR
                Adafruit_LittleFS_Namespace::File* _file;
            #else
                ::FILE* _file;
            #endif

            bool _openFile(char* path);
            void _seekFile(unsigned int position);
            void _closeFile();
    };

    class DirectoryListing {
        public:
            proc::Process* ownerProcess = nullptr;

            DirectoryListing() : DirectoryListing(dataTypes::List<String>()) {}
            DirectoryListing(proc::Process* process);
            DirectoryListing(dataTypes::List<String> entries);
            DirectoryListing(proc::Process* process, dataTypes::List<String> entries);
            ~DirectoryListing();

            void start() {_entries.start();}
            String next() {return *_entries.next();}
            unsigned int length() {return _entries.length();}

        protected:
            dataTypes::List<String> _entries;
    };

    bool init();
    FileHandle* open(proc::Process* process, String path, FileMode mode);
    FileHandle* open(String path, FileMode mode);
    bool isFileOpen(String path);
    bool exists(String path);
    EntryType getEntryType(String path);
    bool remove(String path);
    bool rename(String oldPath, String newPath);
    bool createDirectory(String path);
    DirectoryListing* listDirectory(proc::Process* process, String path);
    DirectoryListing* listDirectory(String path);
}

#endif