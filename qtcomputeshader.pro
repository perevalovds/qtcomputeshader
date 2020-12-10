INCLUDEPATH += $$PWD
SOURCES += $$PWD/computesurface.cpp
HEADERS += $$PWD/computesurface.h

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

RESOURCES += \
    shaders.qrc
