CC:= g++
LIBS:= -lpthread -lglog -lmbase
CPPFLAGS:= -std=c++11 -g -Wno-deprecated -Wall -D_REENTRANT -Wcpp

SERVER = mergeServer
MONITOR = mergeMonitor

BASE_ROOT = ./libs
BATHPATH = -L $(BASE_ROOT)
LIBVAR = -lbase

OPEN_LIB =  -lcurl -lglog -lpthread

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
	$(CC) -o $(SERVER) $(OBJECTS) $(BATHPATH) $(LIBVAR) $(OPEN_LIB)
	
$(MONITOR): $(MOBJECTS)
	$(CC) -o $(MONITOR) $(MOBJECTS) $(BATHPATH) $(LIBVAR) $(OPEN_LIB)

%.o:%.cpp   
	$(CC) -g3 -c $(CPPFLAGS) $< -o $@
	
%.o:%.c   
	$(CC) -g3 -c $(CPPFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(MOBJECTS) $(TARGET)
	@cd Base; $(MAKE) clean
