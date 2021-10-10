QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 qscintilla2

RC_ICONS = images/logo.ico

VERSION = 1.0.0
QMAKE_TARGET_COMPANY = MineLines
QMAKE_TARGET_PRODUCT = Constellation Code
QMAKE_TARGET_DESCRIPTION = Constellation Code is a lightweight code editor for Unity

HEADERS      = mainwindow.h \
    DiscordSDK/include/discord_register.h \
    DiscordSDK/include/discord_rpc.h \
    DiscordSDK/src/backoff.h \
    DiscordSDK/src/connection.h \
    DiscordSDK/src/msg_queue.h \
    DiscordSDK/src/rapidjson/allocators.h \
    DiscordSDK/src/rapidjson/cursorstreamwrapper.h \
    DiscordSDK/src/rapidjson/document.h \
    DiscordSDK/src/rapidjson/encodedstream.h \
    DiscordSDK/src/rapidjson/encodings.h \
    DiscordSDK/src/rapidjson/error/en.h \
    DiscordSDK/src/rapidjson/error/error.h \
    DiscordSDK/src/rapidjson/filereadstream.h \
    DiscordSDK/src/rapidjson/filewritestream.h \
    DiscordSDK/src/rapidjson/fwd.h \
    DiscordSDK/src/rapidjson/internal/biginteger.h \
    DiscordSDK/src/rapidjson/internal/diyfp.h \
    DiscordSDK/src/rapidjson/internal/dtoa.h \
    DiscordSDK/src/rapidjson/internal/ieee754.h \
    DiscordSDK/src/rapidjson/internal/itoa.h \
    DiscordSDK/src/rapidjson/internal/meta.h \
    DiscordSDK/src/rapidjson/internal/pow10.h \
    DiscordSDK/src/rapidjson/internal/regex.h \
    DiscordSDK/src/rapidjson/internal/stack.h \
    DiscordSDK/src/rapidjson/internal/strfunc.h \
    DiscordSDK/src/rapidjson/internal/strtod.h \
    DiscordSDK/src/rapidjson/internal/swap.h \
    DiscordSDK/src/rapidjson/istreamwrapper.h \
    DiscordSDK/src/rapidjson/memorybuffer.h \
    DiscordSDK/src/rapidjson/memorystream.h \
    DiscordSDK/src/rapidjson/msinttypes/inttypes.h \
    DiscordSDK/src/rapidjson/msinttypes/stdint.h \
    DiscordSDK/src/rapidjson/ostreamwrapper.h \
    DiscordSDK/src/rapidjson/pointer.h \
    DiscordSDK/src/rapidjson/prettywriter.h \
    DiscordSDK/src/rapidjson/rapidjson.h \
    DiscordSDK/src/rapidjson/reader.h \
    DiscordSDK/src/rapidjson/schema.h \
    DiscordSDK/src/rapidjson/stream.h \
    DiscordSDK/src/rapidjson/stringbuffer.h \
    DiscordSDK/src/rapidjson/writer.h \
    DiscordSDK/src/rpc_connection.h \
    DiscordSDK/src/serialization.h \
    csharpapi.h \
    discord.h \
    settingswindow.h \
    templatewindow.h
SOURCES      = main.cpp mainwindow.cpp \
    DiscordSDK/src/connection_win.cpp \
    DiscordSDK/src/discord_register_linux.cpp \
    DiscordSDK/src/discord_register_win.cpp \
    DiscordSDK/src/discord_rpc.cpp \
    DiscordSDK/src/dllmain.cpp \
    DiscordSDK/src/rpc_connection.cpp \
    DiscordSDK/src/serialization.cpp \
    csharpapi.cpp \
    discord.cpp \
    settingswindow.cpp \
    templatewindow.cpp


RESOURCES    = application.qrc

FORMS += \
    settingswindow.ui

DISTFILES += \
    Discord SDK/src/CMakeLists.txt \
    Discord SDK/src/discord_register_osx.m \
    DiscordSDK/src/CMakeLists.txt \
    DiscordSDK/src/CMakeLists.txt \
    DiscordSDK/src/discord_register_osx.m \
    DiscordSDK/src/discord_register_osx.m
