TARGET = harbour-lines
CONFIG += link_pkgconfig
CONFIG += sailfishapp
PKGCONFIG += glib-2.0 gio-2.0 mlite5
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-psabi
QT += svg

CONFIG(debug, debug|release) {
  DEFINES += LINES_DEBUG=1
  QMAKE_CXXFLAGS_DEBUG *= -O0
  QMAKE_CFLAGS_DEBUG *= -O0
}

SOURCES += \
    src/LinesBalls.cpp \
    src/LinesDir.cpp \
    src/LinesPrefs.cpp \
    src/LinesScores.cpp \
    src/LinesSet.cpp \
    src/LinesState.cpp \
    src/main.cpp \
    src/QuickLinesColors.cpp \
    src/QuickLinesGame.cpp \
    src/QuickLinesModel.cpp \
    src/QuickNextBallsModel.cpp \
    src/SystemState.cpp

HEADERS += \
    src/LinesBalls.h \
    src/LinesDebug.h \
    src/LinesScores.h \
    src/LinesSet.h \
    src/LinesState.h \
    src/LinesPrefs.h \
    src/LinesTypes.h \
    src/QuickLines.h \
    src/QuickLinesColors.h \
    src/QuickLinesGame.h \
    src/QuickLinesModel.h \
    src/QuickNextBallsModel.h \
    src/SystemState.h

OTHER_FILES += \
    harbour-lines.desktop \
    harbour-lines.png \
    qml/common/*.qml \
    qml/common/images/*.svg \
    qml/sailfish/*.qml \
    qml/sailfish/cover/*.qml \
    qml/sailfish/images/*.svg \
    rpm/harbour-lines.changes \
    rpm/harbour-lines.spec \
    translations/*.ts

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += \
    translations/harbour-lines.ts \
    translations/harbour-lines-ru.ts

# qtsvg

DEFINES += QT_STATIC QT_STATICPLUGIN

QT_SVG_PLUGIN_DIR = qtsvg

INCLUDEPATH += \
  $$QT_SVG_PLUGIN_DIR

SOURCES += \
  $$QT_SVG_PLUGIN_DIR/qsvgiohandler.cpp \
  $$QT_SVG_PLUGIN_DIR/qsvgplugin.cpp

HEADERS += \
  $$QT_SVG_PLUGIN_DIR/qsvgiohandler.h \
  $$QT_SVG_PLUGIN_DIR/qsvgplugin.h

OTHER_FILES += \
  $$QT_SVG_PLUGIN_DIR/svg.json
