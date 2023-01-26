NAME = libpwdraw


INCLUDES = -Iinclude
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib

LIB_SRCS = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJS = $(LIB_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIB_TARGET_STATIC = $(LIB_DIR)/$(NAME).a

_DIRS_MAY_NOT_EXIST = $(OBJ_DIR) $(LIB_DIR)


OPT = -g
WARN_CONF = -Wall -Wextra -Wpedantic -Werror
STD = -std=c17
CFLAGS = $(INCLUDES) $(OPT) $(WARN_CONF) $(STD)

CC = gcc


default: all

.PHONY: default all makedirs lib_static
all: makedirs lib_static

makedirs: $(_DIRS_MAY_NOT_EXIST)
lib_static: $(LIB_TARGET_STATIC)


$(LIB_TARGET_STATIC): $(LIB_DIR) $(LIB_OBJS)
	ar rcs $@ $(filter-out $<,$^)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(_DIRS_MAY_NOT_EXIST):
	mkdir -p $@


.PHONY: clean
clean:
	rm -f $(LIB_TARGET_STATIC) $(LIB_OBJS)
