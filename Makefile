NAME = libtarpe2d
MODULES = 

PWDRAWLIB_PATH = pwdrawlib
PWDRAWLIB_LIB_DIR = $(PWDRAWLIB_PATH)/lib
PWDRAWLIB_NAME = pwdraw
PWDRAWLIB_BUILD_GOAL = lib_static



_INCLUDE_DIR = include
INCLUDES = -I$(_INCLUDE_DIR)

_SRC_DIR = src
SRC_DIRS = $(_SRC_DIR) $(addprefix $(_SRC_DIR)/,$(MODULES))

BIN_DIR = bin

_MAIN_OBJ_DIR = obj
_OBJ_DIR = $(_MAIN_OBJ_DIR)/$(NAME)
OBJ_DIRS = $(_OBJ_DIR) $(addprefix $(_OBJ_DIR)/,$(MODULES))

LIB_DIR = lib


LIB_SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
LIB_OBJS = $(LIB_SRCS:$(_SRC_DIR)/%.c=$(_OBJ_DIR)/%.o)
LIB_TARGET_STATIC = $(LIB_DIR)/$(NAME).a


_MAIN_SRC_DIR = .
MAIN_SRCS = $(foreach dir,$(_MAIN_SRC_DIR),$(wildcard $(dir)/*.c))
MAIN_OBJS = $(MAIN_SRCS:$(_MAIN_SRC_DIR)/%.c=$(_MAIN_OBJ_DIR)/%.o)
MAIN_TARGET = $(BIN_DIR)/main


_DIRS_MAY_NOT_EXIST = $(BIN_DIR) $(_MAIN_OBJ_DIR) $(OBJ_DIRS) $(LIB_DIR)



OPT = -g
WARN_CONF = -Wall -Wextra -Wpedantic -Werror
STD = -std=c17
CFLAGS = $(INCLUDES) $(OPT) $(WARN_CONF) $(STD)

MAIN_LDFLAGS = -L$(PWDRAWLIB_LIB_DIR)
MAIN_LDLIBS = -l$(PWDRAWLIB_NAME)

CC = gcc



default: all

.PHONY: default all makedirs lib_static main
all: makedirs lib_static main

makedirs: $(_DIRS_MAY_NOT_EXIST)
lib_static: $(LIB_TARGET_STATIC)
main: $(MAIN_TARGET)



vpath %.c $(SRC_DIRS)
define make-lib-objs-goals
$1/%.o: %.c $1
	$(CC) $(CFLAGS) -c $$< -o $$@
endef
$(foreach dir,$(OBJ_DIRS),$(eval $(call make-lib-objs-goals,$(dir))))

$(LIB_TARGET_STATIC): $(LIB_DIR) $(LIB_OBJS)
	ar rcs $@ $(filter-out $<,$^)


$(_MAIN_OBJ_DIR)/%.o: $(_MAIN_SRC_DIR)/%.c $(_MAIN_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_TARGET): $(LIB_OBJS) $(MAIN_OBJS) $(BIN_DIR) pwdrawlib_build
	$(CC) $(MAIN_LDFLAGS) $(LIB_OBJS) $(MAIN_OBJS) $(MAIN_LDLIBS) -o $@

.PHONY: pwdrawlib_build
pwdrawlib_build: $(PWDRAWLIB_PATH)
	$(MAKE) -C $< $(PWDRAWLIB_BUILD_GOAL)


$(_DIRS_MAY_NOT_EXIST):
	mkdir -p $@


.PHONY: run clean
run: $(MAIN_TARGET)
	$(MAIN_TARGET)

clean:
	rm -f $(MAIN_TARGET) $(MAIN_OBJS) $(LIB_TARGET_STATIC) $(LIB_OBJS)
