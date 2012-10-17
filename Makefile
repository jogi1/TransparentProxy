CC=gcc
CFLAGS_NORMAL =-Wall -c -Wall -I/usr/include -I./src
CFLAGS_DEBUG  =-c -Wall -I/usr/include -I./include -D__CONSISTENCY_CHECK__
LDFLAGS= -lnetfilter_queue -lpcap -lncurses
SOURCES=src/transparentproxy.c src/packet_handler.c src/msg_functions.c src/decrypt_functions.c src/encrypt_functions.c src/check_functions.c src/crc.c src/debugging.c src/cmdline.c src/ncurses_view.c src/parse_commands.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tproxy
DATE = `date`
C_VERSION = `$(CC) -v 2>&1|grep version`
VERSION = 0.0.1


all: CFLAGS=$(CFLAGS_NORMAL)
all: clean  $(SOURCES) $(EXECUTABLE) 
	@echo "Building standard release"

debug: CFLAGS=$(CFLAGS_DEBUG)
debug: clean  $(SOURCES) $(EXECUTABLE)
	@echo "Building debug release"



version:
	echo "#define VERSION \"$(VERSION)\"\n#define BUILDTIME \"$(DATE)\"\n#define COMPILER_VERSION \"$(C_VERSION)\"" > include/version.h

$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o$@ $(LDFLAGS) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f src/*.o
	rm -f $(EXECUTABLE)

pack:
	tar cvfz ../mvd_parser.tar.gz Makefile $(SOURCES) ./include

