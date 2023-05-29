

CONFIG += c++17
CXXFLAGS += -std=c++17

SOURCES += main.cpp \
    UserInterface.cpp \
    BluetoothManager.cpp \
    DeviceManager.cpp \
    AudioIOManager.cpp

HEADERS += UserInterface.h \
    BluetoothManager.h \
    DeviceManager.h \
    AudioIOManager.h \
    Device.h

RESOURCES += qml.qrc

# Additional libraries if needed
# LIBS += ...

# If using additional includes or libraries, uncomment and specify them
# INCLUDEPATH += ...
# LIBS += ...
