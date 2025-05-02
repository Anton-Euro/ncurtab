CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lncurses
SOURCES = src/main.c src/filesystem.c src/ui.c src/tabs.c
OBJECTS = $(SOURCES:src/%.c=build/%.o)
EXECUTABLE = build/NCurTab

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p build
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo build CC = $(CC)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build