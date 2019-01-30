CC:= g++
LIBS:= -lpthread -lglog -lmbase
CPPFLAGS:= -std=c++11 -g -Wall -D_REENTRANT

BASE=Base/libmbase.so

SERVER = mergeServer
MONITOR = mergeMonitor

BINDIR = /lib

TARGET = $(MONITOR) $(SERVER)

MOBJECTS :=mergeMonitor.o
OBJECTS :=MergeRunable.o CThreadPool.o mongoose.o webserver.o

all : $(BASE) $(TARGET)

install:	$(TARGET)
	cp $(TARGET) $(BINDIR)
	@cd Base; $(MAKE) install
	
FORCE:

$(BASE): FORCE
	@cd Base; $(MAKE) all

$(SERVER): $(OBJECTS)
	$(CC) -o $(SERVER) $(OBJECTS) $(LIBS)
	
$(MONITOR): $(MOBJECTS)
	$(CC) -o $(MONITOR) $(MOBJECTS) $(LIBS)

%.o:%.cpp   
	$(CC) -g3 -c $(CPPFLAGS) $< -o $@
	
%.o:%.c   
	$(CC) -g3 -c $(CPPFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MOBJECTS) $(TARGET)
	@cd Base; $(MAKE) clean
