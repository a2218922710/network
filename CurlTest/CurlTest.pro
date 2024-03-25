QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#just qt5 win32-2019
DEFINES += CURL_STATICLIB

INCLUDEPATH += $$PWD/win32-msvc2019/debug/include
DEPENDPATH += $$PWD/win32-msvc2019/debug/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/win32-msvc2019/release/lib/ -lcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/win32-msvc2019/debug/lib/ -lcurl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/win32-msvc2019/release/lib/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/win32-msvc2019/debug/lib/ -lssl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/win32-msvc2019/release/lib/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/win32-msvc2019/debug/lib/ -lcrypto

include($$PWD/network/network.pri)

INCLUDEPATH += $$PWD
HEADERS += $$files($$PWD/*.h, false)
SOURCES += $$files($$PWD/*.cpp, false)


