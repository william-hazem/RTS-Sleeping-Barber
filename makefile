# source code
SRC = main.cpp

# library flags
CFLAG = -lpthread -Iinclude

LIB_DIR = lib
OBJ_DIR = obj

all: logger | main

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

main: $(SRC) | logger
	g++ $(SRC) $(CFLAG) -L$(LIB_DIR) -llogger -o main

logger: $(LIB_DIR) | $(OBJ_DIR)
	gcc -c include/logger.c -o $(OBJ_DIR)/logger.o
	ar rcs $(LIB_DIR)/liblogger.a $(OBJ_DIR)/logger.o