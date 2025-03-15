#!/bin/bash

namespace utils
    INTERNAL_NAME=dc_utils_numberToStringUInt fn "String" numberToString "unsigned int" number "unsigned int" base
    INTERNAL_NAME=dc_utils_numberToStringInt fn "String" numberToString "int" number "unsigned int" base
    INTERNAL_NAME=dc_utils_numberToStringULong fn "String" numberToString "unsigned long" number "unsigned int" base
    INTERNAL_NAME=dc_utils_numberToStringLong fn "String" numberToString "long" number "unsigned int" base
    INTERNAL_NAME=dc_utils_numberToStringDouble fn "String" numberToString "double" number "unsigned int" base
    fn "long" stringToLong "String" string
    fn "double" stringToDouble "String" string

namespace proc
    PASS_PROCESS=true fn "void" stop

namespace console
    PASS_PROCESS=true fn "void" logPart "String" value
    INTERNAL_NAME=dc_console_logPartChars PASS_PROCESS=true fn "void" logPart "char*" value
    INTERNAL_NAME=dc_console_logPartUInt PASS_PROCESS=true fn "void" logPart "unsigned int" value
    INTERNAL_NAME=dc_console_logPartInt PASS_PROCESS=true fn "void" logPart "int" value
    INTERNAL_NAME=dc_console_logPartULong PASS_PROCESS=true fn "void" logPart "unsigned long" value
    INTERNAL_NAME=dc_console_logPartLong PASS_PROCESS=true fn "void" logPart "long" value
    INTERNAL_NAME=dc_console_logPartDouble PASS_PROCESS=true fn "void" logPart "double" value
    INTERNAL_NAME=dc_console_logPartPtr PASS_PROCESS=true fn "void" logPart "void*" value
    PASS_PROCESS=true fn "void" logNewline

namespace timing
    class Time
        constructor
        INTERNAL_NAME=dc_timing_Time_newUsingDate constructor "int" year "unsigned int" month "unsigned int" day "unsigned int" hour "unsigned int" minute "unsigned int" second
        INTERNAL_NAME=dc_timing_Time_newUsingMilliseconds constructor "int" year "unsigned int" month "unsigned int" day "unsigned long" millisecondOfDay

        method "unsigned int" daysInYear
        method "unsigned int" daysInMonth "unsigned int" month
        method "unsigned long" millisecondsInDay

        method "bool" inLeapMillisecond
        method "unsigned long" postLeapMillisecondOffset

        method "void" setDate "int" year "unsigned int" month "unsigned int" day
        method "void" setTime "unsigned int" hour "unsigned int" minute "unsigned int" second
        method "void" incrementTime "int" millseconds
        method "void" toLocalTime "int" timeShift
        method "void" toGlobalTime
        method "int" timeShift

        method "int" year
        method "unsigned int" month
        method "unsigned int" day
        method "unsigned int" hour
        method "unsigned int" minute
        method "unsigned int" second
        method "unsigned int" millisecond

        method "unsigned int" dayOfYear
        method "unsigned long" millisecondOfDay
        method "unsigned long" millisecondOfDayIgnoringLeap

        method "unsigned int" weekday

    class EarthTime extends Time
        OVERRIDE=true constructor
        OVERRIDE=true INTERNAL_NAME=dc_timing_EarthTime_newUsingDate constructor "int" year "unsigned int" month "unsigned int" day "unsigned int" hour "unsigned int" minute "unsigned int" second
        OVERRIDE=true INTERNAL_NAME=dc_timing_EarthTime_newUsingMilliseconds constructor "int" year "unsigned int" month "unsigned int" day "unsigned long" millisecondOfDay

        method "void" syncToSystemTime

    fn "unsigned long" getCurrentTick

namespace input
    enum Button \
        "NONE = 0" \
        "BACK = 1" \
        "HOME = 2" \
        "UP = 3" \
        "DOWN = 4" \
        "LEFT = 5" \
        "RIGHT = 6" \
        "SELECT = 7"

namespace ui
    enum EventType BUTTON_DOWN BUTTON_UP ITEM_SELECT CANCEL CONFIRM_VALUE

    enum PopupTransitionState NONE OPENING CLOSING

    enum PenMode OFF ON INVERT

    struct Event "EventType" type "union {input::Button button; unsigned int index;}" data

    class Icon
        constructor

        method "void" setPixel "unsigned int" x "unsigned int" y "ENUM ui::PenMode" value

    class Screen
        PASS_PROCESS=true constructor

        method "void" clear
        method "void" setPosition "unsigned int" column "unsigned int" row
        method "void" setPixel "unsigned int" x "unsigned int" y "ENUM ui::PenMode" value
        INTERNAL_NAME=dc_ui_Screen_printChar method "void" print "char" c
        method "void" print "String" string
        INTERNAL_NAME=dc_ui_Screen_printChars method "void" print "char*" chars
        INTERNAL_NAME=dc_ui_Screen_printUInt method "void" print "unsigned int" value
        INTERNAL_NAME=dc_ui_Screen_printInt method "void" print "int" value
        INTERNAL_NAME=dc_ui_Screen_printULong method "void" print "unsigned long" value
        INTERNAL_NAME=dc_ui_Screen_printLong method "void" print "long" value
        INTERNAL_NAME=dc_ui_Screen_printDouble method "void" print "double" value
        INTERNAL_NAME=dc_ui_Screen_printIcon method "void" print "CLASSPTR ui::Icon" icon
        method "void" printRepeated "String" string "unsigned int" times
        method "void" scroll "String" string "unsigned int" maxLength
        method "void" resetScroll
        method "void" pad "unsigned int" size "char" c
        method "void" rect "unsigned int" x1 "unsigned int" y1 "unsigned int" x2 "unsigned int" y2 "ENUM ui::PenMode" value
        method "void" filledRect "unsigned int" x1 "unsigned int" y1 "unsigned int" x2 "unsigned int" y2 "ENUM ui::PenMode" value

        callable "void" update
        callable "void" handleEvent "ui::Event" event

        VIRTUAL=true method "void" open "bool" urgent
        VIRTUAL=true method "void" close
        method "void" swapWith "CLASSPTR ui::Screen" currentScreen

    class Menu extends Screen
        OVERRIDE=true PASS_PROCESS=true constructor

        method "void" clearItems
        method "void" addItem "String" item
        method "unsigned int" getCurrentIndex
        method "void" setCurrentIndex "unsigned int" index

        list "String" items

    class ContextualMenu extends Menu Screen
        OVERRIDE=true PASS_PROCESS=true constructor
        INTERNAL_NAME=dc_ui_ContextualMenu_newWithTitle PASS_PROCESS=true constructor "String" title

        method "String" getTitle
        method "void" setTitle "String" title
        method "bool" getSelectionBlinking
        method "void" setSelectionBlinking "bool" blinkSelection

    class ConfirmationMenu extends ContextualMenu Menu Screen
        OVERRIDE=true PASS_PROCESS=true constructor
        INTERNAL_NAME=dc_ui_ConfirmationMenu_newWithTitle PASS_PROCESS=true constructor "String" title "bool" swapYesNo

        method "bool" yesSelected

    class Popup extends Screen
        OVERRIDE=true PASS_PROCESS=true constructor

    class TextInput extends ContextualMenu Menu Screen
        OVERRIDE=true PASS_PROCESS=true constructor
        INTERNAL_NAME=dc_ui_TextInput_newWithValue PASS_PROCESS=true constructor "String" value

        method "String" getValue
        method "void" setValue "String" value
        INTERNAL_NAME=dc_ui_TextInput_typeTextChar method "void" typeText "char" text
        method "void" typeText "String" text
        method "void" selectAll

    class IntInput extends Screen
        OVERRIDE=true PASS_PROCESS=true constructor
        INTERNAL_NAME=dc_ui_TextInput_newWithTitleAndValue PASS_PROCESS=true constructor "String" title "long" value

        method "String" getTitle
        method "void" setTitle "String" title
        method "long" getValue
        method "void" setValue "long" value
        method "bool" getValueBlinking
        method "void" setValueBlinking "bool" blinkValue
        method "unsigned int" getBase
        method "void" setBase "unsigned int" base
        method "void" setRange "long" minValue "long" maxValue

# namespace fs
#     enum FileMode READ WRITE APPEND
#     enum SeekOrigin START CURRENT END

#     class FileHandle
#         PASS_PROCESS=true constructor
#         INTERNAL_NAME=dc_fs_FileHandle_newWithPathAndMode PASS_PROCESS=true constructor "String" path "ENUM fs::FileMode" mode

#         method "String" getPath
#         method "ENUM fs::FileMode" getMode
#         method "bool" isOpen
#         method "bool" isAvailable
#         method "char" read
#         method "String" readString
#         method "char*" readChars
#         method "void" write "char" c
#         INTERNAL_NAME=dc_fs_FileHandle_writeString method "void" write "String" string
#         INTERNAL_NAME=dc_fs_FileHandle_writeChars method "void" write "char*" chars
#         method "unsigned int" getSize
#         method "unsigned int" tell
#         method "void" seek "int" position "ENUM fs::SeekOrigin" origins
#         method "void" start
#         method "void" close

#     fn "CLASSPTR fs::FileHandle" open "String" path "ENUM fs::FileMode" mode
#     fn "char*" getFileModeString "ENUM fs::FileMode" mode
#     fn "bool" isFileOpen "String" path

namespace test
    class TestClass
        constructor "unsigned int" seed

        VIRTUAL=true method "void" identify
        method "unsigned int" add "unsigned int" value "unsigned int" value2
        method "bool" bools "bool" a "bool" b "bool" c
        method "unsigned int" nextRandomNumber
        method "String" getString
        method "char*" getChars

    class TestSubclass extends TestClass
        OVERRIDE=true constructor "unsigned int" seed

        OVERRIDE=true method "void" identify
        method "void" subclass

    fn "void" sayHello
    fn "int" add "int" a "int" b
    fn "CLASSPTR test::TestClass" getMostRecentTest