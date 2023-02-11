NAME = libtarpe2d
MODULES = bh86

LIBTARPE2D_DRAW_PATH = libtarpe2d_draw
LIBTARPE2D_DRAW_LIB_DIR = $(LIBTARPE2D_DRAW_PATH)/lib
LIBTARPE2D_DRAW_NAME = tarpe2d_draw
LIBTARPE2D_DRAW_BUILD_GOAL = lib_static

LIBTARPE2D_DRAW_INCULDE_DIRS = $(LIBTARPE2D_DRAW_PATH)/include
LIBTARPE2D_LDFLAGS = 
LIBTARPE2D_LDLIBS = -lSDL2 -lSDL2_gpu



_INCLUDE_DIRS = include $(LIBTARPE2D_DRAW_INCULDE_DIRS)
INCLUDES = $(addprefix -I,$(_INCLUDE_DIRS))
HEADERS = $(foreach dir,$(_INCLUDE_DIRS),$(wildcard $(dir)/**/*.h))

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

MAIN_LDFLAGS = -L$(LIBTARPE2D_DRAW_LIB_DIR) $(LIBTARPE2D_LDFLAGS)
MAIN_LDLIBS = -lm -l$(LIBTARPE2D_DRAW_NAME) $(LIBTARPE2D_LDLIBS)

CC = gcc



default: all

.PHONY: default all makedirs lib_static main
all: makedirs lib_static main

makedirs: $(_DIRS_MAY_NOT_EXIST)
lib_static: $(LIB_TARGET_STATIC)
main: $(MAIN_TARGET)



vpath %.c $(SRC_DIRS)
define make-lib-objs-goals
$1/%.o: %.c $1 $(HEADERS)
	$(CC) $(CFLAGS) -c $$< -o $$@
endef
$(foreach dir,$(OBJ_DIRS),$(eval $(call make-lib-objs-goals,$(dir))))

$(LIB_TARGET_STATIC): $(LIB_DIR) $(LIB_OBJS) 
	ar rcs $@ $(filter-out $<,$^)


$(_MAIN_OBJ_DIR)/%.o: $(_MAIN_SRC_DIR)/%.c $(_MAIN_OBJ_DIR) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN_TARGET): $(LIB_OBJS) $(MAIN_OBJS) $(BIN_DIR) libtarpe2d_draw_build
	$(CC) $(MAIN_LDFLAGS) $(LIB_OBJS) $(MAIN_OBJS) $(MAIN_LDLIBS) -o $@

.PHONY: libtarpe2d_draw_build
libtarpe2d_draw_build: $(LIBTARPE2D_DRAW_PATH)
	$(MAKE) -C $< $(LIBTARPE2D_DRAW_BUILD_GOAL)


$(_DIRS_MAY_NOT_EXIST):
	mkdir -p $@


.PHONY: run clean
run: $(MAIN_TARGET)
	$(MAIN_TARGET)

clean:
	rm -f $(MAIN_TARGET) $(MAIN_OBJS) $(LIB_TARGET_STATIC) $(LIB_OBJS)
