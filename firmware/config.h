#ifndef CONFIG_H_
#define CONFIG_H_

#include "datatypes.h"
#include "utils.h"

namespace config {
    struct Property {
        dataTypes::String section = "";
        dataTypes::String key = "";
        dataTypes::String value = "";
    };

    class Config {
        public:
            Config();
            ~Config();

            dataTypes::String getStringOrDefault(dataTypes::String section, dataTypes::String key, dataTypes::String defaultValue);
            dataTypes::String getString(dataTypes::String section, dataTypes::String key);
            long getLongOrDefault(dataTypes::String section, dataTypes::String key, long defaultValue);
            long getLong(dataTypes::String section, dataTypes::String key);
            double getDoubleOrDefault(dataTypes::String section, dataTypes::String key, double defaultValue);
            double getDouble(dataTypes::String section, dataTypes::String key);
            bool getBoolOrDefault(dataTypes::String section, dataTypes::String key, bool defaultValue);
            bool getBool(dataTypes::String section, dataTypes::String key);

            void setString(dataTypes::String section, dataTypes::String key, dataTypes::String value);
            void setLong(dataTypes::String section, dataTypes::String key, long value);
            void setDouble(dataTypes::String section, dataTypes::String key, double value);
            void setBool(dataTypes::String section, dataTypes::String key, bool value);

            void fromIni(dataTypes::String ini);
            dataTypes::String toIni();

            bool loadFromFile(dataTypes::String path);
            bool saveToFile(dataTypes::String path);

        private:
            dataTypes::List<Property> _properties;
    };
}

#endif