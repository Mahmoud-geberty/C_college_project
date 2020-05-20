CFLAGS=-g -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

MAIN_SRC=$(wildcard src/*.c)
OBJ=$(patsubst %.c,%,$(MAIN_SRC))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libutil.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The target build
all: $(OBJ) $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAGS)
dev: all

#$(OBJ): LDLIBS += $(TARGET)
$(OBJ): $(TARGET)

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit tests
.PHONY: tests
tests: LDLIBS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

#the cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

#The install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The checker 
check:
	@echo Files with potentially dangerous functions.
	#some complex regex I don't wanna write down here.
