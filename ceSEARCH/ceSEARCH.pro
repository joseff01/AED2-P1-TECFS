QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../lib/Huffman.cpp \
    ../lib/UtilityText.cpp \
    ../lib/tinyxml2.cpp \
    main.cpp \
    cesearch_gui.cpp

HEADERS += \
    ../lib/Huffman.h \
    ../lib/List.h \
    ../lib/Node.h \
    ../lib/RequestConstants.h \
    ../lib/UtilityText.h \
    ../lib/json.hpp \
    ../lib/tinyxml2.h \
    cesearch_gui.h

FORMS += \
    cesearch_gui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
