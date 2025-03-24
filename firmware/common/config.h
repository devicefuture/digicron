#ifndef DC_COMMON_CONFIG_H_
#define DC_COMMON_CONFIG_H_

#ifndef DIGICRON_H_
    #include "../datatypes.h"
#endif

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

            void setString(dataTypes::String section, dataTypes::String key, dataTypes::String value);

            void fromIni(dataTypes::String ini);

        private:
            dataTypes::List<Property> _properties;
    };
}

#endif