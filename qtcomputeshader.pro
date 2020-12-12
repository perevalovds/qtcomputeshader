INCLUDEPATH += $$PWD
SOURCES += $$PWD/glwrappers.cpp
HEADERS += $$PWD/glwrappers.h

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

RESOURCES += \
    shaders.qrc
