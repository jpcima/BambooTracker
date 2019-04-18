SOURCES += \
    $$PWD/RtAudio.cpp

HEADERS += \
    $$PWD/RtAudio.h

linux {
    DEFINES += __LINUX_ALSA__
    LIBS += -lasound
    DEFINES += __LINUX_PULSE__
    LIBS += -lpulse-simple -lpulse
    DEFINES += __UNIX_JACK__
    LIBS += -ljack
}
win32 {
    DEFINES += __WINDOWS_DS__
    LIBS += -ldsound -lole32
    DEFINES += __WINDOWS_WASAPI__
    LIBS += -lksguid
}
macx {
    DEFINES += __MACOSX_CORE__
    LIBS += -framework CoreAudio -framework CoreFoundation
}
