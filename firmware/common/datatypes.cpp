#ifndef DC_COMMON_DATATYPES_CPP_
#define DC_COMMON_DATATYPES_CPP_

#ifndef DIGICRON_H_
    #include "datatypes.h"
#endif

template<typename T> T* store(T value) {
    auto storedValue = new dataTypes::StoredValue<T>(value);

    return &storedValue->value;
}

template<typename T> T discard(T* itemPtr) {
    auto itemValue = *itemPtr;

    free(itemPtr);

    return itemValue;
}

template<typename T> dataTypes::StoredValue<T>::StoredValue(T valueToStore) {
    value = valueToStore;
}

template<typename T> dataTypes::StoredValue<T>::~StoredValue() {}

#ifdef DIGICRON_H_
    inline dataTypes::String::String() {}

    inline dataTypes::String::String(const char* value) {
        while (value[_length] != '\0') {
            _length++;
        }

        _value = (char*)malloc(_length + 1);

        for (unsigned int i = 0; i <= _length; i++) {
            _value[i] = value[i];
        }
    }

    inline dataTypes::String::String(const dataTypes::String& other) : String(other.c_str()) {}

    inline dataTypes::String::String(char c) {
        _value = (char*)malloc(2);
        _value[0] = c;
        _value[1] = '\0';
        _length = 1;
    }

    inline dataTypes::String::String(unsigned int value, unsigned char base) : String(utils::numberToString(value, base)) {}
    inline dataTypes::String::String(int value, unsigned char base) : String(utils::numberToString(value, base)) {}
    inline dataTypes::String::String(unsigned long value, unsigned char base) : String(utils::numberToString(value, base)) {}
    inline dataTypes::String::String(long value, unsigned char base) : String(utils::numberToString(value, base)) {}
    inline dataTypes::String::String(double value, unsigned char base) : String(utils::numberToString(value, base)) {}

    inline dataTypes::String::~String() {
        if (_value) {
            free(_value);
        }
    }

    inline dataTypes::String& dataTypes::String::operator=(const dataTypes::String& other) {
        if (this == &other) {
            return *this;
        }

        char* otherCstr = other.c_str();

        _length = other.length();
        _value = (char*)realloc(_value, _length + 1);

        for (unsigned int i = 0; i <= _length; i++) {
            _value[i] = otherCstr[i];
        }

        return *this;
    }

    inline dataTypes::String& dataTypes::String::operator+(const dataTypes::String& other) {
        String& combined = *this;

        combined.concat(other);

        return combined;
    }

    inline char* dataTypes::String::c_str() const {
        return _value;
    }

    inline unsigned int dataTypes::String::length() const {
        return _length;
    }

    inline char dataTypes::String::charAt(int index) {
        if (!_value || index >= _length) {
            return '\0';
        }

        return _value[index];
    }

    inline const bool dataTypes::String::equals(const String& other) {
        if (this == &other) {
            return true;
        }

        if (length() != other.length()) {
            return false;
        }

        return equals(other.c_str());
    }

    inline const bool dataTypes::String::equals(const char* other) {
        if (other == _value) {
            return true;
        }

        for (unsigned int i = 0; i < _length; i++) {
            if (_value[i] != other[i]) {
                return false;
            }
        }

        if (other[_length]) {
            // The other string is longer than this one
            return false;
        }

        return true;
    }

    inline unsigned char dataTypes::String::concat(const dataTypes::String& other) {
        return concat(other.c_str(), other.length());
    }

    inline unsigned char dataTypes::String::concat(const char* value, unsigned int length) {
        unsigned int newLength = _length + length;

        if (!value) {
            return 0;
        }

        if (length == 0) {
            return 1;
        }

        char* newValue = (char*)realloc(_value, newLength + 1);

        if (!newValue) {
            return 0;
        }

        _value = newValue;

        for (unsigned int i = 0; i < length; i++) {
            _value[_length + i] = value[i];
        }

        _value[newLength] = '\0';
        _length = newLength;

        return 1;
    }

    inline unsigned char dataTypes::String::concat(const char* value) {
        return concat(String(value));
    }

    inline unsigned char dataTypes::String::concat(char c) {
        return concat(&c, 1);
    }

    inline dataTypes::String dataTypes::String::substring(unsigned int start, unsigned int end) {
        if (start > end) {
            unsigned int temp = start;

            start = end;
            end = temp;
        }

        if (end > _length) {
            end = _length;
        }

        char newValue[end - start + 1];

        memcpy(newValue, _value + start, end - start);

        newValue[end - start] = '\0';

        return String(newValue);
    }

    inline long dataTypes::String::toInt() {
        return utils::stringToLong(String(c_str()));
    }

    inline float dataTypes::String::toFloat() {
        return utils::stringToDouble(String(c_str()));
    }

    inline double dataTypes::String::toDouble() {
        return utils::stringToDouble(String(c_str()));
    }
#endif

inline dataTypes::Buffer::Buffer(unsigned int size) {
    _size = size;
    data = (char*)malloc(size);
}

inline dataTypes::Buffer::Buffer(dataTypes::String string) {
    _size = string.length() + 1;

    data = (char*)malloc(_size);

    const char* cstr = string.c_str();

    for (unsigned int i = 0; i < _size; i++) {
        data[i] = cstr[i];
    }
}

inline dataTypes::Buffer::Buffer(const char* chars) : dataTypes::Buffer::Buffer(String(chars)) {}

inline dataTypes::Buffer::~Buffer() {
    if (data) {
        free(data);
    }
}

inline unsigned int dataTypes::Buffer::getSize() {
    return _size;
}

template<typename T> dataTypes::List<T>::List() {
    _firstItemPtr = nullptr;
    _length = 0;
}

template<typename T> dataTypes::List<T>::~List() {
    empty();
}

template<typename T> dataTypes::_ListItem<T>* dataTypes::List<T>::getItemAtIndex(int index) {
    if (!_firstItemPtr) {
        return nullptr;
    }

    if (index < 0) {
        int resolvedIndex = _length + index;

        if (resolvedIndex < 0) {
            return nullptr;
        }

        return getItemAtIndex(resolvedIndex);
    }

    auto currentItemPtr = _firstItemPtr;

    while (index > 0) {
        if (!currentItemPtr) {
            return nullptr;
        }

        currentItemPtr = currentItemPtr->nextItemPtr;
        index--;
    }

    return currentItemPtr;
}

template<typename T> T* dataTypes::List<T>::operator[](int index) {
    auto item = getItemAtIndex(index);

    if (!item) {
        return nullptr;
    }

    return item->valuePtr;
}

template<typename T> dataTypes::_ListItem<T>* dataTypes::List<T>::getLastItem() {
    if (!_firstItemPtr) {
        return nullptr;
    }

    auto currentItemPtr = _firstItemPtr;
    _ListItem<T>* previousItemPtr;

    do {
        previousItemPtr = currentItemPtr;
        currentItemPtr = currentItemPtr->nextItemPtr;
    } while (currentItemPtr);

    return previousItemPtr;
}

template<typename T> void dataTypes::List<T>::start(Iterator* iterator) {
    if (iterator == nullptr) {
        iterator = &_defaultIterator;
    }

    *iterator = _firstItemPtr;
}

template<typename T> T* dataTypes::List<T>::next(Iterator* iterator) {
    if (iterator == nullptr) {
        iterator = &_defaultIterator;
    }

    auto itemPtr = *iterator;

    if (!*iterator) {
        return nullptr;
    }

    *iterator = (*iterator)->nextItemPtr;

    return itemPtr->valuePtr;
}

template<typename T> unsigned int dataTypes::List<T>::length() {
    return _length;
}

template<typename T> void dataTypes::List<T>::empty() {
    if (!_firstItemPtr) {
        return;
    }

    auto currentItemPtr = _firstItemPtr;

    while (currentItemPtr) {
        auto nextItemPtr = currentItemPtr->nextItemPtr;

        delete currentItemPtr;

        currentItemPtr = nextItemPtr;
    }

    _firstItemPtr = nullptr;
    _length = 0;
}

template<typename T> unsigned int dataTypes::List<T>::push(T* valuePtr) {
    auto itemPtr = new _ListItem<T> {
        .valuePtr = valuePtr,
        .nextItemPtr = nullptr
    };

    _length++;

    if (!_firstItemPtr) {
        _firstItemPtr = itemPtr;

        return 1;
    }

    getLastItem()->nextItemPtr = itemPtr;

    return _length;
}

template<typename T> T* dataTypes::List<T>::pop() {
    if (!_firstItemPtr) {
        return nullptr;
    }

    if (!_firstItemPtr->nextItemPtr) {
        auto item = _firstItemPtr;

        _firstItemPtr = nullptr;
        _length--;

        return item->valuePtr;
    }

    auto lastItemPtr = getLastItem();
    auto lastItemValuePtr = lastItemPtr->valuePtr;

    getItemAtIndex(-2)->nextItemPtr = nullptr;
    _length--;

    delete lastItemPtr;

    return lastItemValuePtr;
}

template<typename T> unsigned int dataTypes::List<T>::unshift(T* valuePtr) {
    auto itemPtr = new _ListItem<T> {
        .valuePtr = valuePtr,
        .nextItemPtr = nullptr
    };

    _length++;

    if (!_firstItemPtr) {
        _firstItemPtr = itemPtr;

        return 1;
    }

    auto oldFirstItemPtr = _firstItemPtr;

    _firstItemPtr = itemPtr;

    itemPtr->nextItemPtr = oldFirstItemPtr;

    return _length;
}

template<typename T> T* dataTypes::List<T>::shift() {
    if (!_firstItemPtr) {
        return nullptr;
    }

    _length--;

    auto itemPtr = _firstItemPtr;
    auto itemValuePtr = itemPtr->valuePtr;

    _firstItemPtr = _firstItemPtr->nextItemPtr;

    delete itemPtr;

    return itemValuePtr;
}

template<typename T> unsigned int dataTypes::List<T>::insert(unsigned int index, T* valuePtr) {
    if (index == 0) {
        return unshift(valuePtr);
    }

    if (index >= _length) {
        return push(valuePtr);
    }

    auto previousItemPtr = getItemAtIndex(index - 1);

    if (!previousItemPtr) {
        return _length;
    }

    _length++;

    auto itemPtr = new _ListItem<T> {
        .valuePtr = valuePtr,
        .nextItemPtr = previousItemPtr->nextItemPtr
    };

    previousItemPtr->nextItemPtr = itemPtr;

    return _length;
}

template<typename T> T* dataTypes::List<T>::remove(unsigned int index) {
    if (index == 0) {
        return shift();
    }

    if (index == _length - 1) {
        return pop();
    }

    auto previousItemPtr = getItemAtIndex(index - 1);

    if (!previousItemPtr) {
        return nullptr;
    }

    auto itemPtr = previousItemPtr->nextItemPtr;

    if (!itemPtr) {
        return nullptr;
    }

    _length--;

    auto itemValuePtr = itemPtr->valuePtr;
    auto nextItemPtr = itemPtr->nextItemPtr;

    previousItemPtr->nextItemPtr = nextItemPtr;

    delete itemPtr;

    return itemValuePtr;
}

template<typename T> void dataTypes::List<T>::set(unsigned int index, T* valuePtr) {
    auto item = getItemAtIndex(index);

    if (!item) {
        return;
    }

    item->valuePtr = valuePtr;
}

template<typename T> int dataTypes::List<T>::indexOf(T* valuePtr) {
    auto currentItemPtr = _firstItemPtr;
    int index = 0;

    while (currentItemPtr) {
        if (currentItemPtr->valuePtr == valuePtr) {
            return index;
        }

        currentItemPtr = currentItemPtr->nextItemPtr;
        index++;
    }

    return -1;
}

template<typename T> void dataTypes::List<T>::forEach(IterationCallback iterationCallback) {
    auto currentItemPtr = _firstItemPtr;
    unsigned int index = 0;

    while (currentItemPtr) {
        iterationCallback(currentItemPtr->valuePtr, index);

        currentItemPtr = currentItemPtr->nextItemPtr;
        index++;
    }
}

template<typename T> dataTypes::List<T> dataTypes::List<T>::map(MappingFunction mappingFunction) {
    List<T> newList;
    auto currentItemPtr = _firstItemPtr;
    unsigned int index = 0;

    while (currentItemPtr) {
        newList.push(mappingFunction(currentItemPtr->valuePtr, index));

        currentItemPtr = currentItemPtr->nextItemPtr;
        index++;
    }

    return newList;
}

template<typename T> dataTypes::List<T> dataTypes::List<T>::filter(FilteringFunction filteringFunction) {
    List<T> newList;
    auto currentItemPtr = _firstItemPtr;
    unsigned int index = 0;

    while (currentItemPtr) {
        if (filteringFunction(currentItemPtr->valuePtr, index)) {
            newList.push(currentItemPtr->valuePtr);
        }

        currentItemPtr = currentItemPtr->nextItemPtr;
        index++;
    }

    return newList;
}

template<typename T> dataTypes::List<T> dataTypes::List<T>::concat(dataTypes::List<T> otherList) {
    List<T> newList;

    start();

    while (auto itemPtr = next()) {
        newList.push(itemPtr);
    }

    otherList.start();

    while (auto itemPtr = otherList.next()) {
        newList.push(itemPtr);
    }

    return newList;
}

#endif