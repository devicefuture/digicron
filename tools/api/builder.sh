#!/bin/bash

echo "Running API builder..."

export NAMESPACE=
export ENUM=
export CLASS=
export LAST_NAMESPACE_MEMBER=
export HAD_CONSTRUCTOR=false
export TAG_TYPES="EMPTY, Buffer, "
export TAG_TYPE_DELETES=""

function _closeNamespace {
    if [ "$NAMESPACE" != "" ]; then
        _closeClass

        echo } >> applib/digicron.h
        echo >> applib/digicron.h

        export NAMESPACE=
        export LAST_NAMESPACE_MEMBER=
    fi
}

function namespace {
    _closeNamespace

    export NAMESPACE=$1

    echo namespace $NAMESPACE { >> applib/digicron.h
    echo "#include \"${NAMESPACE:-IMPORT_PATH}.h\"" >> tools/api/_api-includes.h
    echo Including namespace: dc::$NAMESPACE
}

function enum {
    if [ "$LAST_NAMESPACE_MEMBER" != "" ]; then
        echo "" >> applib/digicron.h
    fi

    export LAST_NAMESPACE_MEMBER=$1

    echo "    enum $1 {" >> applib/digicron.h
    echo -n "Including enum: dc::$NAMESPACE::$1 {"

    shift

    while (($#)); do
        echo -n "        $1" >> applib/digicron.h
        echo -n $1

        shift

        if (($#)); then
            echo "," >> applib/digicron.h
            echo -n ", "
        fi
    done

    echo >> applib/digicron.h
    echo "    };" >> applib/digicron.h
    echo "}"
}

function _closeClass {
    if [ "$CLASS" != "" ]; then
        echo "    };" >> applib/digicron.h

        export CLASS=
    fi
}

function class {
    _closeClass

    if [ "$LAST_NAMESPACE_MEMBER" != "" ]; then
        echo "" >> applib/digicron.h
    fi

    export CLASS=$1
    export LAST_NAMESPACE_MEMBER=$1
    export HAD_CONSTRUCTOR=false
    export TAG_TYPES="$TAG_TYPES${NAMESPACE}_$CLASS, "
    export CLASS_EXTENDS=

    echo "        case api::Type::${NAMESPACE}_$CLASS: delete ($NAMESPACE::$CLASS*)storedInstance->instance; break;" >> tools/api/_api-deletes.h

    if [ "$2" = "extends" ]; then
        export CLASS_EXTENDS=$3

        (
            echo "    class $CLASS : public $CLASS_EXTENDS {"
            echo "        protected:"
            echo "            $CLASS(_Dummy dummy) : $CLASS_EXTENDS(dummy) {}"
            echo
            echo "        public:"
            echo "            using $CLASS_EXTENDS::$CLASS_EXTENDS;"
            echo
        ) >> applib/digicron.h

        echo "            (type == Type::${NAMESPACE}_$CLASS_EXTENDS && storedInstance->type == Type::${NAMESPACE}_$CLASS) ||" >> tools/api/_api-typeinheritance.h
        echo "            (type == _Type::${NAMESPACE}_$CLASS_EXTENDS && storedInstance->type == _Type::${NAMESPACE}_$CLASS) ||" >> tools/api/_digicron-typeinheritance.h

        shift
        shift
        shift

        while (($#)); do
            echo "            (type == Type::${NAMESPACE}_$1 && storedInstance->type == Type::${NAMESPACE}_$CLASS) ||" >> tools/api/_api-typeinheritance.h
            echo "            (type == _Type::${NAMESPACE}_$1 && storedInstance->type == _Type::${NAMESPACE}_$CLASS) ||" >> tools/api/_digicron-typeinheritance.h
            shift
        done

        echo "Including class: dc::$NAMESPACE::$CLASS (extends $CLASS_EXTENDS)"
    else
        (
            echo "    class $CLASS {"
            echo "        protected:"
            echo "            dc::_Sid _sid;"
            echo
            echo "            $CLASS(_Dummy dummy) {}"
            echo
            echo "        public:"
            echo "            virtual dc::_Sid _getSid() {return _sid;}"
            echo
            echo "            ~$CLASS() {dc_deleteBySid(_sid); _removeStoredInstance(this);}"
            echo
        ) >> applib/digicron.h

        echo Including class: dc::$NAMESPACE::$CLASS
    fi

    # echo "template $NAMESPACE::$CLASS* api::getBySid<$NAMESPACE::$CLASS>(api::Type type, api::Sid sid)" >> tools/api/_api-templates.h
    # echo "template api::Sid api::store<$NAMESPACE::$CLASS>(api::Type type, $NAMESPACE::$CLASS* instance)" >> tools/api/_api-templates.h
}

function method {
    if [ "$INTERNAL_NAME" = "" ]; then
        if [ "$OUT_OF_CLASS" = true ]; then
            local INTERNAL_NAME=dc_${NAMESPACE}_$2
        else
            local INTERNAL_NAME=dc_${NAMESPACE}_${CLASS}_$2
        fi
    fi

    name="$2"
    returnType="$1"
    virtualKeyword=
    overrideKeyword=
    superConstructorCall=

    internalReturnType=$returnType
    firmwareReturnType=$returnType

    if [ "$returnType" = "bool" ]; then
        firmwareReturnType="unsigned int"
    fi

    if [ "$returnType" = "String" ]; then
        internalReturnType=dc::_Sid
        firmwareReturnType=Sid
        nameAndType="dataTypes::String $name"
    else
        nameAndType="$returnType $name"
    fi

    if [ "$IN_CONSTRUCTOR" = true ]; then
        nameAndType=$1
    elif [ "$HAD_CONSTRUCTOR" = true ]; then
        echo >> applib/digicron.h

        export HAD_CONSTRUCTOR=false
    fi

    if [ "$VIRTUAL" = true ]; then
        virtualKeyword="virtual "
    fi

    if [ "$OVERRIDE" = true ]; then
        overrideKeyword=" override"
        superConstructorCall=" : $CLASS_EXTENDS((_Dummy) {})"
    fi

    if [ "$OUT_OF_CLASS" = true ]; then
        echo -n "    inline $nameAndType(" >> applib/digicron.h
    else
        echo -n "            $virtualKeyword$nameAndType(" >> applib/digicron.h
    fi

    echo "    m3ApiRawFunction($INTERNAL_NAME);" >> firmware/_api.h

    echo "m3ApiRawFunction(api::$INTERNAL_NAME) {" >> firmware/_api.cpp

    firmwareArgs=

    if [ "$IN_CONSTRUCTOR" = true ]; then
        passArgs=
        shortReturnType=i

        echo -n "WASM_IMPORT(\"digicron\", \"$INTERNAL_NAME\") dc::_Sid $INTERNAL_NAME(" >> tools/api/_digicron-imports.h
        echo -n "    m3_LinkRawFunction(runtime->modules, MODULE_NAME, \"$INTERNAL_NAME\", \"i(" >> tools/api/_api-linker.h
        echo "    m3ApiReturnType(Sid)" >> firmware/_api.cpp

        shift
    else
        shortReturnType=i

        case "$1" in
            "void")
                shortReturnType=v ;;

            "String")
                shortReturnType=i ;;
        esac

        passArgs=
        prependArg=
        prependShortArg=

        if [ "$internalReturnType" != "void" ]; then
            echo "    m3ApiReturnType($firmwareReturnType)" >> firmware/_api.cpp
        fi
    
        if [ "$OUT_OF_CLASS" != true ]; then
            passArgs="_sid"
            prependArg="dc::_Sid sid"
            prependShortArg=i

            echo "    m3ApiGetArg(Sid, _sid)" >> firmware/_api.cpp
        fi

        echo -n "WASM_IMPORT(\"digicron\", \"$INTERNAL_NAME\") $internalReturnType $INTERNAL_NAME($prependArg" >> tools/api/_digicron-imports.h

        echo -n "    m3_LinkRawFunction(runtime->modules, MODULE_NAME, \"$INTERNAL_NAME\", \"$shortReturnType($prependShortArg" >> tools/api/_api-linker.h

        shift
        shift

        if [ "$OUT_OF_CLASS" != true ] && (($#)); then
            echo -n ", " >> tools/api/_digicron-imports.h
            passArgs="$passArgs, "
        fi
    fi

    if [ "$OUT_OF_CLASS" = true ]; then
        echo -n "Including function: $returnType dc::$NAMESPACE::$name("
    elif [ "$IN_CONSTRUCTOR" = true ]; then
        echo -n "Including class constructor: $returnType dc::$NAMESPACE::$CLASS::$CLASS("
    else
        echo -n "Including class method: $returnType dc::$NAMESPACE::$CLASS::$name("
    fi

    while (($#)); do
        argType=$1
        internalArgType=$argType
        shortArgType=i
        argName=$2
        internalArgCall=$argName
        firmwareArgType=$internalArgType
        firmwareArgCall=$internalArgCall

        case "$argType" in
            "char*"|"STRING"|"void*")
                shortArgType="*" ;;

            "float")
                shortArgType="f" ;;

            "double")
                shortArgType="F" ;;
        esac

        if [[ "$argType" =~ ^ENUM\  ]]; then
            argType=${argType##ENUM }
            internalArgType=dc::_Enum
            firmwareArgType="unsigned int"
            firmwareArgCall="($argType)$argName"
        fi

        if [[ "$argType" =~ ^CLASS\  ]]; then
            argType=${argType##CLASS }
            internalArgType=dc::_Sid
            internalArgCall="$argName._getSid()"
            firmwareArgType=Sid
            firmwareArgCall="*api::getBySid<$argType>(Type::${argType/::/_}, $argName)"
        fi

        if [[ "$argType" =~ ^CLASSPTR\  ]]; then
            argType=${argType##CLASSPTR }
            internalArgType=dc::_Sid
            internalArgCall="$argName->_getSid()"
            firmwareArgType=Sid
            firmwareArgCall="api::getBySid<$argType>(Type::${argType/::/_}, $argName)"
            argType="$argType*"
        fi

        if [ "$argType" = "String" ]; then
            argType=dataTypes::String
            internalArgType="char*"
            internalArgCall="$argName.c_str()"
            firmwareArgType="char*"
            firmwareArgCall="String($argName)"
        fi

        echo -n "$argType $argName" >> applib/digicron.h
        echo -n "$internalArgType $argName" >> tools/api/_digicron-imports.h
        echo -n "$shortArgType" >> tools/api/_api-linker.h
        passArgs="$passArgs$internalArgCall"
        firmwareArgs="$firmwareArgs$firmwareArgCall"
        echo -n "$argType $argName"

        if [ "$firmwareArgType" = "char*" ]; then
            echo "    m3ApiGetArgMem(char*, $argName)" >> firmware/_api.cpp
        else
            echo "    m3ApiGetArg($firmwareArgType, $argName)" >> firmware/_api.cpp
        fi

        shift
        shift

        if (($#)); then
            echo -n ", " >> applib/digicron.h
            echo -n ", " >> tools/api/_digicron-imports.h
            passArgs="$passArgs, "
            firmwareArgs="$firmwareArgs, "
            echo -n ", "
        fi
    done

    if [ "$IN_CONSTRUCTOR" = true ]; then
        echo ")$superConstructorCall {_sid = $INTERNAL_NAME($passArgs); _addStoredInstance(_Type::${NAMESPACE}_$CLASS, this);}" >> applib/digicron.h

        (
            echo
            echo -n "    auto instance = new $NAMESPACE::$CLASS("
        ) >> firmware/_api.cpp
    elif [ "$returnType" = "String" ]; then
        echo ")$overrideKeyword {dc::_Sid sid = $INTERNAL_NAME($passArgs); char array[dc_getBufferSize(sid)]; dc_copyBufferInto(sid, array); dataTypes::String str(array); dc_deleteBySid(sid); return str;}" >> applib/digicron.h

        if [ "$OUT_OF_CLASS" = true ]; then
            echo -n "    Sid result = api::store<dataTypes::Buffer>(Type::Buffer, (proc::WasmProcess*)runtime->userdata, new dataTypes::Buffer($NAMESPACE::$name(" >> firmware/_api.cpp
        else
            echo -n "    Sid result = api::store<dataTypes::Buffer>(Type::Buffer, (proc::WasmProcess*)runtime->userdata, new dataTypes::Buffer(api::getBySid<$NAMESPACE::$CLASS>(Type::${NAMESPACE}_$CLASS, _sid)->$name(" >> firmware/_api.cpp
        fi
    else
        echo ")$overrideKeyword {return $INTERNAL_NAME($passArgs);}" >> applib/digicron.h

        if [ "$passArgs" != "" ]; then
            echo >> firmware/_api.cpp
        fi

        if [ "$returnType" != "void" ]; then
            if [ "$OUT_OF_CLASS" = true ]; then
                echo -n "    $firmwareReturnType result = $NAMESPACE::$name(" >> firmware/_api.cpp
            else
                echo -n "    $firmwareReturnType result = api::getBySid<$NAMESPACE::$CLASS>(Type::${NAMESPACE}_$CLASS, _sid)->$name(" >> firmware/_api.cpp
            fi
        else
            if [ "$OUT_OF_CLASS" = true ]; then
                echo -n "    $NAMESPACE::$name(" >> firmware/_api.cpp
            else
                echo -n "    api::getBySid<$NAMESPACE::$CLASS>(Type::${NAMESPACE}_$CLASS, _sid)->$name(" >> firmware/_api.cpp
            fi
        fi
    fi

    if [ "$PASS_PROCESS" = true ]; then
        echo -n "(proc::WasmProcess*)runtime->userdata" >> firmware/_api.cpp

        if [ "$firmwareArgs" != "" ]; then
            echo -n ", " >> firmware/_api.cpp
        fi
    fi

    echo ");" >> tools/api/_digicron-imports.h
    echo ")\", &$INTERNAL_NAME);" >> tools/api/_api-linker.h

    if [ "$returnType" = "String" ]; then
        echo "$firmwareArgs)));" >> firmware/_api.cpp
    else
        echo "$firmwareArgs);" >> firmware/_api.cpp
    fi

    echo "" >> firmware/_api.cpp
    echo ")"

    if [ "$IN_CONSTRUCTOR" = true ]; then
        echo "    Sid result = api::store<$NAMESPACE::$CLASS>(Type::${NAMESPACE}_$CLASS, (proc::WasmProcess*)runtime->userdata, instance);" >> firmware/_api.cpp
        echo >> firmware/_api.cpp
    fi

    if [ "$returnType" = "void" ]; then
        echo "    m3ApiSuccess();" >> firmware/_api.cpp
    else
        echo "    m3ApiReturn(result);" >> firmware/_api.cpp
    fi

    echo "}" >> firmware/_api.cpp
    echo >> firmware/_api.cpp

    echo $INTERNAL_NAME >> applib/digicron.syms
}

function constructor {
    if [ "$INTERNAL_NAME" = "" ]; then
        local INTERNAL_NAME=dc_${NAMESPACE}_${CLASS}_new
    fi

    IN_CONSTRUCTOR=true INTERNAL_NAME=$INTERNAL_NAME method $CLASS "$@"

    export HAD_CONSTRUCTOR=true
}

function fn {
    _closeClass

    if [ "$LAST_NAMESPACE_MEMBER" != "" ]; then
        echo "" >> applib/digicron.h

        LAST_NAMESPACE_MEMBER=
    fi

    OUT_OF_CLASS=true INTERNAL_NAME=$INTERNAL_NAME method "$@"
}

function callable {
    echo -n "            virtual $1 $2(" >> applib/digicron.h

    shift
    shift

    while (($#)); do
        echo -n "$1 $2" >> applib/digicron.h

        shift
        shift

        if (($#)); then
            echo -n ", " >> applib/digicron.h
        fi
    done

    echo ") {}" >> applib/digicron.h
}

function list {
    itemType=$1
    name=$2
    updaterName=${3:-updateItems}
    clearerName=${4:-clearItems}
    adderName=${5:-addItem}

    if [ "$itemType" = "String" ]; then
        itemType=dataTypes::String
    fi

    if [ "$HAD_CONSTRUCTOR" = true ]; then
        echo >> applib/digicron.h

        export HAD_CONSTRUCTOR=false
    fi

    echo "            dataTypes::List<$itemType> $name;" >> applib/digicron.h
    echo "            void $updaterName() {$clearerName(); $name.start(); while (auto item = $name.next()) {$adderName(*item);}}" >> applib/digicron.h

    echo "Including list: List<$itemType> $NAMESPACE::$CLASS::$name (updater: $updaterName, clearer: $clearerName, adder: $adderName)"
}

function struct {
    echo >> applib/digicron.h
    echo "    struct $1 {" >> applib/digicron.h

    shift

    while (($#)); do
        echo "        $1 $2;" >> applib/digicron.h

        shift
        shift
    done

    echo "    };" >> applib/digicron.h
}

> tools/api/_api-linker.h
> tools/api/_api-includes.h
> tools/api/_api-typeinheritance.h
> tools/api/_api-templates.h
> tools/api/_api-deletes.h
> firmware/_api.cpp
> firmware/_api.h

tee -a tools/api/_api-linker.h > /dev/null << EOF
void api::linkFunctions(IM3Runtime runtime) {
    const char* MODULE_NAME = "digicron";

    m3_LinkRawFunction(runtime->modules, MODULE_NAME, "dc_getGlobalI32", "i(*)", &dc_getGlobalI32);
    m3_LinkRawFunction(runtime->modules, MODULE_NAME, "dc_deleteBySid", "v(i)", &dc_deleteBySid);
    m3_LinkRawFunction(runtime->modules, MODULE_NAME, "dc_getBufferSize", "i(i)", &dc_getBufferSize);
    m3_LinkRawFunction(runtime->modules, MODULE_NAME, "dc_copyBufferInto", "v(i*)", &dc_copyBufferInto);

EOF

tee -a firmware/_api.cpp > /dev/null << EOF
// Autogenerated by \`tools/api/builder.sh\` using declarations from \`tools/api/api.sh\`

#include <Arduino.h>
#include <m3_env.h>

#include "_api.h"
#include "proc.h"
#include "datatypes.h"
// {{ includes }}

dataTypes::List<api::StoredInstance> api::storedInstances;

template<typename T> T* api::getBySid(api::Type type, api::Sid sid) {
    StoredInstance* storedInstance = storedInstances[sid];

    if (!storedInstance || (storedInstance->type != type && !(
        // {{ typeInheritance }}
        false
    ))) {
        Serial.println("Inheritance check failed");

        return new T(); // To ensure an object is always referenced
    }

    return (T*)storedInstance->instance;
}

api::Sid api::findOwnSid(void* instance) {
    storedInstances.start();

    unsigned int index = 0;

    while (auto storedInstance = storedInstances.next()) {
        if (storedInstance->instance == instance) {
            return index;
        }

        index++;
    }

    return -1;
}

template<typename T> api::Sid api::store(api::Type type, proc::Process* ownerProcess, T* instance) {
    StoredInstance* storedInstance = nullptr;
    bool foundStoredInstance = false;
    unsigned int index = 0;

    storedInstances.start();

    while ((storedInstance = storedInstances.next())) {
        if (storedInstance->type == Type::EMPTY) {
            foundStoredInstance = true;
            break;
        }

        index++;
    }

    if (!storedInstance) {
        storedInstance = new StoredInstance();
    }

    storedInstance->type = type;
    storedInstance->ownerProcess = ownerProcess;
    storedInstance->instance = instance;

    if (foundStoredInstance) {
        return index;
    } else {
        return storedInstances.push(storedInstance) - 1;
    }
}

void deleteStoredInstance(api::StoredInstance* storedInstance) {
    if (!storedInstance || storedInstance->type == api::Type::EMPTY) {
        return;
    }

    switch (storedInstance->type) {
        case api::Type::Buffer: delete (dataTypes::Buffer*)storedInstance->instance; break;
// {{ deletes }}
        default: delete storedInstance->instance; break;
    }

    storedInstance->type = api::Type::EMPTY;
    storedInstance->ownerProcess = nullptr;
    storedInstance->instance = nullptr;
}

void api::deleteBySid(api::Sid sid) {
    deleteStoredInstance(storedInstances[sid]);
}

void api::deleteAllByOwnerProcess(proc::Process* ownerProcess) {
    storedInstances.start();

    while (auto storedInstance = storedInstances.next()) {
        if (storedInstance->type == Type::EMPTY || storedInstance->ownerProcess != ownerProcess) {
            continue;
        }

        deleteStoredInstance(storedInstance);
    }
}

m3ApiRawFunction(api::dc_getGlobalI32) {
    m3ApiReturnType(uint32_t)
    m3ApiGetArgMem(char*, id)

    IM3Global global = m3_FindGlobal(runtime->modules, id);

    if (global) {
        M3TaggedValue globalValue;

        m3_GetGlobal(global, &globalValue);

        m3ApiReturn(globalValue.value.i32);
    } else {
        m3ApiReturn(0);
    }
}

m3ApiRawFunction(api::dc_deleteBySid) {
    m3ApiGetArg(Sid, _sid)

    api::deleteBySid(_sid);

    m3ApiSuccess();
}

m3ApiRawFunction(api::dc_getBufferSize) {
    m3ApiReturnType(unsigned int)
    m3ApiGetArg(Sid, _sid)

    unsigned int result = api::getBySid<dataTypes::Buffer>(Type::Buffer, _sid)->getSize();

    m3ApiReturn(result);
}

m3ApiRawFunction(api::dc_copyBufferInto) {
    m3ApiGetArg(Sid, _sid)
    m3ApiGetArgMem(char*, destination)

    dataTypes::Buffer* buffer = api::getBySid<dataTypes::Buffer>(Type::Buffer, _sid);

    for (unsigned int i = 0; i < buffer->getSize(); i++) {
        destination[i] = buffer->data[i];
    }

    m3ApiSuccess();
}

EOF

tee -a firmware/_api.h > /dev/null << EOF
// Autogenerated by \`tools/api/builder.sh\` using declarations from \`tools/api/api.sh\`

#ifndef API_H_
#define API_H_

#include <wasm3.h>

// {{ includes }}

namespace api {
    typedef int Sid;

    enum Type {/* {{ tagTypes }} */};

    struct StoredInstance {
        Type type;
        proc::Process* ownerProcess;
        void* instance;
    };

    extern dataTypes::List<StoredInstance> storedInstances;

    template<typename T> T* getBySid(Type type, Sid sid);
    Sid findOwnSid(void* instance);
    template<typename T> Sid store(Type type, proc::Process* ownerProcess, T* instance);
    void deleteBySid(Sid sid);
    void deleteAllByOwnerProcess(proc::Process* ownerProcess);

    m3ApiRawFunction(dc_getGlobalI32);
    m3ApiRawFunction(dc_deleteBySid);
    m3ApiRawFunction(dc_getBufferSize);
    m3ApiRawFunction(dc_copyBufferInto);

EOF

mkdir -p applib

> tools/api/_digicron-imports.h
> tools/api/_digicron-typeinheritance.h
> applib/digicron.h
> applib/digicron.syms

tee -a applib/digicron.h > /dev/null << EOF
// Autogenerated by \`tools/api/builder.sh\` using declarations from \`tools/api/api.sh\`

#ifndef DIGICRON_H_
#define DIGICRON_H_

#include <stdint.h>

#define WASM_EXPORT extern "C" __attribute__((used)) __attribute__((visibility ("default")))
#define WASM_EXPORT_AS(name) WASM_EXPORT __attribute__((export_name(name)))
#define WASM_IMPORT(module, name) __attribute__((import_module(module))) __attribute__((import_name(name)))
#define WASM_CONSTRUCTOR __attribute__((constructor))

#define ONCE inline

namespace dc {
    typedef unsigned int _Enum;
    typedef int _Sid;
    typedef struct {} _Dummy;
}

void setup();
void loop();

extern "C" {

WASM_IMPORT("digicron", "dc_getGlobalI32") uint32_t dc_getGlobalI32(const char* id);
WASM_IMPORT("digicron", "dc_deleteBySid") void dc_deleteBySid(dc::_Sid sid);
WASM_IMPORT("digicron", "dc_getBufferSize") unsigned int dc_getBufferSize(dc::_Sid sid);
WASM_IMPORT("digicron", "dc_copyBufferInto") void dc_copyBufferInto(dc::_Sid sid, void* destination);

// {{ imports }}

}

// {{ stdlib }}


namespace dc {

EOF

cat firmware/common/datatypes.h >> applib/digicron.h
echo >> applib/digicron.h
echo >> applib/digicron.h

echo dc_getGlobalI32 >> applib/digicron.syms
echo dc_deleteBySid >> applib/digicron.syms
echo dc_getBufferSize >> applib/digicron.syms
echo dc_copyBufferInto >> applib/digicron.syms

export -f _closeNamespace namespace _closeClass class method constructor

tee -a applib/digicron.h > /dev/null << EOF
enum _Type {/* {{ tagTypes }} */};

struct _StoredInstance {
    _Type type;
    void* instance;
};

inline dataTypes::List<_StoredInstance> _storedInstances;

template<typename T> T* _getBySid(_Type type, _Sid sid) {
    _storedInstances.start();

    while (_StoredInstance* storedInstance = _storedInstances.next()) {
        if (storedInstance->type != type && !(
            // {{ typeInheritance }}
            false
        )) {
            continue;
        }

        T* castedInstance = (T*)(storedInstance->instance);

        if (castedInstance->_getSid() == sid) {
            return castedInstance;
        }
    }

    return nullptr;
}

inline void _addStoredInstance(_Type type, void* instance) {
    auto storedInstance = new _StoredInstance {
        .type = type,
        .instance = instance
    };

    _storedInstances.push(storedInstance);
}

inline void _removeStoredInstance(void* instance) {
    _storedInstances.start();

    unsigned int index = 0;

    while (_StoredInstance* storedInstance = _storedInstances.next()) {
        if (storedInstance->instance == instance) {
            delete _storedInstances.remove(index);

            return;
        }

        index++;
    }
}

EOF

. tools/api/api.sh

_closeNamespace

echo "}" >> tools/api/_api-linker.h

cat tools/api/_api-linker.h >> firmware/_api.cpp

tee -a firmware/_api.h > /dev/null << EOF

    void linkFunctions(IM3Runtime runtime);
}

#endif
EOF

for header in firmware/common/*.h; do
    if [ "$header" = "firmware/common/datatypes.h" ]; then
        continue
    fi

    cat $header >> applib/digicron.h
    echo >> applib/digicron.h
    echo >> applib/digicron.h
done

for header in firmware/common/*.cpp; do
    cat $header >> applib/digicron.h
    echo >> applib/digicron.h
    echo >> applib/digicron.h
done

tee -a applib/digicron.h > /dev/null << EOF
}

WASM_EXPORT_AS("_setup") inline void _setup() {
    setup();
}

WASM_EXPORT_AS("_loop") inline void _loop() {
    loop();
}

// {{ callables }}

#endif
EOF

TAG_TYPES=$(echo $TAG_TYPES | sed "s/\(.*\),/\1/")

sed -i -e "\|// {{ includes }}|{r tools/api/_api-includes.h" -e "d}" firmware/_api.cpp
sed -i -e "\|// {{ typeInheritance }}|{r tools/api/_api-typeinheritance.h" -e "d}" firmware/_api.cpp
sed -i -e "\|// {{ templates }}|{r tools/api/_api-templates.h" -e "d}" firmware/_api.cpp
sed -i -e "\|// {{ deletes }}|{r tools/api/_api-deletes.h" -e "d}" firmware/_api.cpp
sed -i -e "\|// {{ includes }}|{r tools/api/_api-includes.h" -e "d}" firmware/_api.h
sed -i "s|/\* {{ tagTypes }} \*/|$TAG_TYPES|" firmware/_api.h
sed -i "s|/\* {{ tagTypes }} \*/|$TAG_TYPES|" applib/digicron.h
sed -i -e "\|// {{ stdlib }}|{r tools/api/digicron-stdlib.h" -e "d}" applib/digicron.h
sed -i -e "\|// {{ imports }}|{r tools/api/_digicron-imports.h" -e "d}" applib/digicron.h
sed -i -e "\|// {{ typeInheritance }}|{r tools/api/_digicron-typeinheritance.h" -e "d}" applib/digicron.h
sed -i -e "\|// {{ callables }}|{r tools/api/digicron-callables.h" -e "d}" applib/digicron.h

cp tools/api/digicron-stdlib.cpp applib/digicron-stdlib.cpp