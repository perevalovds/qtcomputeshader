INCLUDEPATH += $$PWD
SOURCES += $$PWD/openglwindow.cpp
HEADERS += $$PWD/openglwindow.h

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

RESOURCES += \
    shaders.qrc
