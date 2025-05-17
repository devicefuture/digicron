#include "config.h"
#include "fs.h"

config::Config::Config() {}

config::Config::~Config() {
    _properties.emptyAndDelete();
}

dataTypes::String config::Config::getStringOrDefault(dataTypes::String section, dataTypes::String key, dataTypes::String defaultValue) {
    _properties.start();

    while (Property* property = _properties.next()) {
        if (property->section == section && property->key == key) {
            return property->value;
        }
    }

    return defaultValue;
}

dataTypes::String config::Config::getString(dataTypes::String section, dataTypes::String key) {
    return getStringOrDefault(section, key, "");
}

long config::Config::getLongOrDefault(dataTypes::String section, dataTypes::String key, long defaultValue) {
    dataTypes::String stringValue = getStringOrDefault(section, key, utils::longToString(defaultValue, 10));

    return utils::stringToLong(stringValue, 10);
}

long config::Config::getLong(dataTypes::String section, dataTypes::String key) {
    return getLongOrDefault(section, key, 0);
}

double config::Config::getDoubleOrDefault(dataTypes::String section, dataTypes::String key, double defaultValue) {
    dataTypes::String stringValue = getStringOrDefault(section, key, utils::doubleToString(defaultValue, 15));

    return utils::stringToDouble(stringValue);
}

double config::Config::getDouble(dataTypes::String section, dataTypes::String key) {
    return getDoubleOrDefault(section, key, 0);
}

bool config::Config::getBoolOrDefault(dataTypes::String section, dataTypes::String key, bool defaultValue) {
    return getStringOrDefault(section, key, defaultValue ? "true" : "false") == "true";
}

bool config::Config::getBool(dataTypes::String section, dataTypes::String key) {
    return getBoolOrDefault(section, key, false);
}

void config::Config::setString(dataTypes::String section, dataTypes::String key, dataTypes::String value) {
    _properties.start();

    while (Property* property = _properties.next()) {
        if (property->section == section && property->key == key) {
            property->value = value;

            return;
        }
    }

    Property* newProperty = new Property();

    newProperty->section = section;
    newProperty->key = key;
    newProperty->value = value;

    _properties.push(newProperty);
}

void config::Config::setLong(dataTypes::String section, dataTypes::String key, long value) {
    setString(section, key, utils::longToString(value, 10));
}

void config::Config::setDouble(dataTypes::String section, dataTypes::String key, double value) {
    setString(section, key, utils::doubleToString(value, 15));
}

void config::Config::setBool(dataTypes::String section, dataTypes::String key, bool value) {
    setString(section, key, value ? "true" : "false");
}

void config::Config::fromIni(dataTypes::String ini) {
    dataTypes::String section = "";
    dataTypes::String propertyKey = "";
    dataTypes::String propertyValue = "";

    bool inComment = false;
    bool inSection = false;
    bool inPropertyValue = false;
    bool enteredPropertyBody = false;
    bool hadAssignmentOperator = false;

    for (unsigned int i = 0; i < ini.length(); i++) {
        char c = ini[i];

        if (inComment && c != '\n') {
            continue;
        }

        if (!inPropertyValue && !inSection && c == '[') {
            inSection = true;
            section = "";
            continue;
        }

        if (inSection && c == ']' && ini[i + 1] == '\n') {
            inSection = false;
            continue;
        }

        if (c == '\n') {
            if (propertyKey != "" && inPropertyValue) {
                setString(section, propertyKey, propertyValue);
            }

            inComment = false;
            inPropertyValue = false;
            hadAssignmentOperator = false;
            propertyKey = "";

            continue;
        }

        if (inSection) {
            section.concat(c);
            continue;
        }

        if (c == ';') {
            inComment = true;
            continue;
        }

        if (!inPropertyValue && c == ' ') {
            continue;
        }

        if (!inPropertyValue && c == '=') {
            inPropertyValue = true;
            hadAssignmentOperator = true;

            propertyValue = "";

            continue;
        }

        if (inPropertyValue && hadAssignmentOperator && c == ' ') {
            continue;
        }

        if (!inPropertyValue) {
            propertyKey.concat(c);
            continue;
        }

        if (inPropertyValue) {
            propertyValue.concat(c);

            hadAssignmentOperator = false;

            continue;
        }
    }

    if (propertyKey != "" && inPropertyValue) {
        setString(section, propertyKey, propertyValue);
    }
}

dataTypes::String config::Config::toIni() {
    dataTypes::String ini = "";
    dataTypes::List<dataTypes::String> sections;
    bool firstSection = true;

    _properties.start();

    while (Property* property = _properties.next()) {
        if (property->section == "") {
            firstSection = false;

            ini.concat(property->key);
            ini.concat('=');
            ini.concat(property->value);
            ini.concat('\n');

            continue;
        }

        bool shouldAddSection = true;

        sections.start();

        while (dataTypes::String* section = sections.next()) {
            if (*section == property->section) {
                shouldAddSection = false;

                break;
            }
        }

        if (shouldAddSection) {
            sections.push(new dataTypes::String(property->section));
        }
    }

    sections.start();

    while (dataTypes::String* section = sections.next()) {
        if (firstSection) {
            firstSection = false;
        } else {
            ini.concat('\n');
        }

        ini.concat('[');
        ini.concat(*section);
        ini.concat("]\n");

        _properties.start();

        while (Property* property = _properties.next()) {
            if (property->section != *section) {
                continue;
            }

            ini.concat(property->key);
            ini.concat('=');
            ini.concat(property->value);
            ini.concat('\n');
        }

        delete section;
    }

    return ini;
}

bool config::Config::loadFromFile(dataTypes::String path) {
    fs::FileHandle* file = fs::open(path, fs::FileMode::READ);

    if (!file) {
        return false;
    }

    fromIni(file->readString());

    delete file;

    return true;
}

bool config::Config::saveToFile(dataTypes::String path) {
    if (!fs::ensureParentDirectories(path)) {
        return false;
    }

    fs::FileHandle* file = fs::open(path, fs::FileMode::WRITE);

    if (!file) {
        return false;
    }

    file->write(toIni());
    file->close();

    delete file;

    return true;
}