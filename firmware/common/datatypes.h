#ifndef DC_COMMON_DATATYPES_H_
#define DC_COMMON_DATATYPES_H_

#ifndef DIGICRON_H_
    #include <Arduino.h>
#endif

template<typename T> T* store(T value);
template<typename T> T discard(T* itemPtr);

namespace dataTypes {
    template<typename T> struct _ListItem {
        T* valuePtr;
        _ListItem<T>* nextItemPtr;
    };

    template<typename T> class StoredValue {
        public:
            T value;

            StoredValue<T>(T valueToStore);
            ~StoredValue<T>();
    };

    #ifdef DIGICRON_H_
        class String {
            public:
                String();
                String(const char* value);
                String(const String& other);
                String(char c);
                String(unsigned int value, unsigned char base = 10);
                String(int value, unsigned char base = 10);
                String(unsigned long value, unsigned char base = 10);
                String(long value, unsigned char base = 10);
                String(double value, unsigned char base = 10);
                ~String();

                String& operator=(const String& other);
                char operator[](int index) {return charAt(index);}
                const bool operator==(const String& other) {return equals(other);}
                const bool operator==(const char* other) {return equals(other);}
                const bool operator!=(const String& other) {return !equals(other);}
                const bool operator!=(const char* other) {return !equals(other);}

                char* c_str() const;
                unsigned int length() const;
                char charAt(int index);
                const bool equals(const String& other);
                const bool equals(const char* other);

                unsigned char concat(const String& other);
                unsigned char concat(const char* value, unsigned int length);
                unsigned char concat(const char* value);
                unsigned char concat(char c);

                long toInt();
                long toLong();
                float toFloat();
                double toDouble();

            private:
                char* _value = nullptr;
                unsigned int _length = 0;
        };
    #else
        typedef String String;
    #endif

    class Buffer {
        public:
            char* data = nullptr;

            Buffer() {}
            Buffer(unsigned int size);
            Buffer(dataTypes::String string);
            Buffer(const char* chars);
            ~Buffer();

            unsigned int getSize();

        private:
            unsigned int _size = 0;
    };

    template<typename T> class List {
        public:
            typedef _ListItem<T>* Iterator;

            typedef void (*IterationCallback)(T* itemPtr, unsigned int index);
            typedef T* (*MappingFunction)(T* itemPtr, unsigned int index);
            typedef bool (*FilteringFunction)(T* itemPtr, unsigned int index);

            List<T>();
            ~List<T>();

            T* operator[](int index);

            void start(Iterator* iterator = nullptr);
            T* next(Iterator* iterator = nullptr);
            unsigned int length();
            void empty();
            unsigned int push(T* valuePtr);
            T* pop();
            unsigned int unshift(T* valuePtr);
            T* shift();
            unsigned int insert(unsigned int index, T* valuePtr);
            T* remove(unsigned int index);
            void set(unsigned int index, T* valuePtr);
            int indexOf(T* valuePtr);
            void forEach(IterationCallback iterationCallback);
            List<T> map(MappingFunction mappingFunction);
            List<T> filter(FilteringFunction filteringFunction);
            List<T> concat(List<T> otherList);

        private:
            _ListItem<T>* _firstItemPtr;
            Iterator _defaultIterator;
            unsigned int _length;

            _ListItem<T>* getItemAtIndex(int index);
            _ListItem<T>* getLastItem();
    };
}

#endif