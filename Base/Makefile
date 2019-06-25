VERSION=v1.0
LIBDIR=../libs
CC = g++

CPPFLAGS := -g -Wall -fPIC -Wno-error=overflow -Wno-deprecated -std=c++11
CFLAGS  := -Wall -std=c++11 -g

OPEN_LIB = -lpthread -lcurl -lglog

OBJS := common.o CommonList.o LibcurClient.o  MessageQueue.o ConfigFileReader.o Lock.o Condition.o BaseList.o
TATIC_NAME = libbase.a

all : $(TATIC_NAME)

FORCE:
all:$(OBJS)
$(TATIC_NAME):$(OBJS)
	ar -cr $(TATIC_NAME) $^
	 cp $(TATIC_NAME) $(LIBDIR)
$(OBJS):
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^ $(OPEN_LIB)

clean:
	rm -f $(OBJS) $(LIB)

common.o:common.cpp common.h
LibcurClient.o: LibcurClient.cpp LibcurClient.h
CommonList.o: CommonList.cpp  CommonList.h
MessageQueue.o: MessageQueue.c MessageQueue.h
ConfigFileReader.o: ConfigFileReader.cpp ConfigFileReader.h
Lock.o: Lock.cpp Lock.h
Condition.o: Condition.cpp Condition.h
BaseList.o: BaseList.cpp BaseList.h
