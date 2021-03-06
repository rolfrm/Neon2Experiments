OPT = -g3 -Og
LIB_SOURCES1 = main.c gui/stb_truetype.c gl/gl_module.c gl/gl_utils.c gui/gui2.c gui/utf8.c gui/tables.c gui/persist_oop.c game/game.c
LIB_SOURCES = $(addprefix src/, $(LIB_SOURCES1))
CC = gcc
TARGET = nth.so
LIB_OBJECTS =$(LIB_SOURCES:src/%.c=obj/%.o)

LDFLAGS= -L. $(OPT) -Wextra #-lmcheck #-ftlo #setrlimit on linux 
LIBS= -ldl -lm  -lpthread -liron -lGL -lGLEW -lglfw -ldatastream_server -lxxhash
ALL= $(TARGET) 
CFLAGS = -Iinclude -Isrc -std=c11 -c $(OPT) -D_GNU_SOURCE -Wall -Wextra -Werror=implicit-function-declaration -Wformat=0  -fdiagnostics-color -Wextra -Werror -Wwrite-strings -fbounds-check -fPIC  #-DDEBUG

$(TARGET): directories $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) $(LIB_OBJECTS) $(LIBS) --shared -o $@

all: $(ALL)

obj/%.o : src/%.c
	$(CC) $(CFLAGS) -fPIC $< -o $@ -MMD -MF $@.depends

directories:
	mkdir -p obj

depend: h-depend
clean:
	rm -f $(LIB_OBJECTS) $(ALL) obj/*.o.depends obj/*.o.depends

-include $(LIB_OBJECTS:.o=.o.depends)
