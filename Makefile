# BUILD NECE RADIT BEZ libwinpthread-1.dll
name=main.exe

CC=gcc

# REMOVE -g FLAG FOR DEBUGGING WHEN BUILDING A RELEASE
CFLAGS=-IC:/mingw_link/mingw_include -Iengine
CFLAGS+=-g

LDLIBS=-LC:/mingw_link/mingw_lib -lglfw3 -lopengl32 -lgdi32

LDFLAGS_debug="-Wl,-subsystem,console"
LDFLAGS_release="-Wl,-subsystem,windows"
LDFLAGS=$(LDFLAGS_debug) 

SRCS=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c) $(wildcard */*/*/*.c)
OBJS=$(SRCS:.c=.o)
#DEPS=$(OBJS:.o=.d)

.PHONY: all clean
all: $(name)

$(name): $(OBJS)
	$(CC) -o $(name) $(OBJS) $(LDLIBS) $(LDFLAGS)

#-include $(DEPS)

#rm -f $(name)
#rm -f $(DEPS) $(OBJS)
#Del "%variable%"
#Del $(name)
clean:
	del /S *.o
	del $(name)