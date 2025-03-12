#ifndef FS_H_
#define FS_H_

#include <Arduino.h>

#ifndef DC_SIMULATOR
    #include <Adafruit_LittleFS.h>
#else
    #include <stdio.h>
#endif

namespace fs {
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
            FileHandle(String path, FileMode mode = FileMode::READ);
            ~FileHandle();

            String getPath() {return _path;}
            FileMode getMode() {return _mode;}
            bool isOpen() {return _isOpen;}
            bool isAvailable();

            char read();
            String readString();

            void write(char c);
            void write(String string);
            void write(char* string);

            unsigned int getSize();
            unsigned int tell();
            void seek(int position, SeekOrigin origin = SeekOrigin::START);
            void start();
            void close();

        protected:
            String _path;
            FileMode _mode;
            bool _isOpen;
            bool _errorOnOpen;

            #ifndef DC_SIMULATOR
                lfs_file _file;
            #else
                FILE* _file;
            #endif

            bool _openFile(char* path);
            void _seekFile(unsigned int position);
            void _closeFile();
    };

    FileHandle* open(String path, FileMode mode);
    const char* getFileModeString(FileMode mode);
    bool isFileOpen(String path);
}

#endif