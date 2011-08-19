#===========================================================

DEFINES += APP_NAME=\\\"VstBoard\\\"

PORTAUDIO_PATH 	= $$PWD/libs/portaudio
PORTMIDI_PATH 	= $$PWD/libs/portmidi
VSTSDK_PATH	= $$PWD/libs/vstsdk2.4

CONFIG += vstsdk
CONFIG += scriptengine

#use ring buffers to sync multiple audio inputs
#DEFINES += CIRCULAR_BUFFER

#limit buffers range from -1.0 to 1.0
#DEFINES += BUFFER_ZERODB_CLIPPING

#use lines instead of curves
#DEFINES += SIMPLE_CABLES

#===========================================================

CONFIG(debug, debug|release) {
    POST =
    build_postfix=debug
    DEFINES += DEBUG
} else {
    POST =
    build_postfix=release
}

win32-g++ {
    DEFINES += WINVER=0x0501
    DEFINES += _WIN32_WINNT=0x0501
 #   LIBS += -L$$quote($$MINGW_PATH/lib)
 #   INCLUDEPATH += $$quote($$MINGW_PATH/include)
}

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    INCLUDEPATH += $$quote($$(INCLUDE))
    LIBS += -L$$quote($$(LIB))
 #   QMAKE_CFLAGS += -Fd$$top_destdir/$$TARGET

#to add symbols :
#    QMAKE_CXXFLAGS_RELEASE +=  -Zi
#    QMAKE_LFLAGS_RELEASE += /DEBUG

#ltcg
    QMAKE_CXXFLAGS_RELEASE +=  -GL
    QMAKE_LFLAGS_RELEASE += /LTCG
}
