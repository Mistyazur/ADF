
QT      += axcontainer

############################################
## Free dm
############################################
#DEFINES += DM_FREE

#HEADERS += $$PWD/dmfree/dm.h \
#           $$PWD/dmprivate.h \

#SOURCES += $$PWD/dmfree/dm.cpp \
#           $$PWD/dmprivate.cpp \

###########################################
# Reg dm
###########################################
DEFINES += DM_REG

HEADERS += $$PWD/dmreg/dm.h \
           $$PWD/dmprivate.h \

SOURCES += $$PWD/dmreg/dm.cpp \
           $$PWD/dmprivate.cpp \
