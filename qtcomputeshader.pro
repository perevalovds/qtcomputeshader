INCLUDEPATH += $$PWD
SOURCES += $$PWD/glsurface.cpp
HEADERS += $$PWD/glsurface.h

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

RESOURCES += \
    shaders.qrc
