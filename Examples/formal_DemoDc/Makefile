#generally makefile for EtherCAT for linux
#questions: Motrotech, info@motrotech.com

TARGET= demo3.0
INCFLAGS=-I$(PROJPATH)/../../SDK/INC -I$(PROJPATH)/../../SDK/INC/Linux -I$(PROJPATH)/

# add -D_CONSOLE to enable the default trace implemenation (fprintf to stderr)
CFLAGS=-Wall $(INCFLAGS) -g -D__LINUX_GCC__ 
CROSS_COMPILE ?=arm-a9-linux-gnueabihf-#arm-linux-gnueabihf-
#CROSS_COMPILE ?=
CPP=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc

PROJPATH = .
#LIB=-L$(PROJPATH)/../../SDK/LIB/Linux/armv6-vfp-eabihf -lAtemRasSrv -lEcMaster -lkyapi -lpthread -lm -ldl -lrt
LIB=-L$(PROJPATH)/../../SDK/LIB/Linux/armv6-vfp-eabihf -lAtemRasSrv -lEcMaster -lkyapi -lpthread -lm -ldl -lrt
SRC=$(PROJPATH)/

# OBJECTS
OBJECTS = DCDemo.o ecatDemoCommon.o ecatNotification.o Logging.o selectLinkLayer.o EcTimer.o motrotech.o

OEMOBJECTS = DCDemoMain.o

EXTLINKS=


all:	$(OBJECTS) $(OEMOBJECTS) 
		$(CPP) -L. -o $(TARGET) $(OEMOBJECTS) $(OBJECTS) $(CFLAGS) $(LIB) 

DCDemoMain.o:		DCDemoMain.cpp
		$(CPP) $(CFLAGS) -c $<

#standard rule to make objects of lzs
%.o:		$(SRC)/%.cpp
		$(CPP) $(CFLAGS) -c $<

strip:
		$(STRIP) --strip-all $(TARGET)

clean:
		rm *.o $(TARGET) 

