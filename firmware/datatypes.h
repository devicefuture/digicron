#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>

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

    typedef String String;

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
            void emptyAndDelete();
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