BINDIR=bin
OBJDIR=obj
SRCDIR=src
EXEC=$(BINDIR)/bfunc

SRC := $(shell find $(SRCDIR) -name \*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

CC=g++
DEBUG=-DDEBUG -g -w -pedantic -Wall
RELEASE=-DNDEBUG -flto -O2
CXXFLAGS=-std=c++11

# Примеры библиотечных переменных
# INC и LIB - обязательно абсолютные пути c -I и -L соответственно
# LDFLAGS - это LIB + подключаемые библиотеки
#INC=-I/home/users/s35-7/0000/gear/NSL/include
#LIB=-L/home/users/s35-7/0000/gear/NSL/lib
#LDFLAGS=$(LIB) -lBinaryFileStream
INC=
LIB=
LDFLAGS=$(LIB)

all: debug

clean:
	-rm -r $(OBJDIR) $(BINDIR)

debug: CXXFLAGS := $(DEBUG) $(CXXFLAGS)
debug: $(EXEC)

release: CXXFLAGS := $(RELEASE) $(CXXFLAGS)
release: $(EXEC)

$(EXEC): $(OBJ) | $(BINDIR)
	$(CC) $(CXXFLAGS) -o $(EXEC) $(OBJ) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p "$(dir $@)"
	$(CC) -c $(CXXFLAGS) $(INC) $< -o $@

$(OBJ): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

.PHONY: all clean debug release
