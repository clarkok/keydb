# basic
UTIL_TARGET	= key-util
LIB_TARGET	= libkeydb.a
TEST_TARGET = gtest
TARGETS = $(UTIL_TARGET) $(LIB_TARGET) $(TEST_TARGET)

TESTS			= test/slice_test.cpp test/buffer_test.cpp test/exception_test.cpp \
						test/disk_test.cpp test/utils_test.cpp test/main.cpp \
						test/db_test.cpp test/hashtable_test.cpp
LIB_SRC 	= drive/naive_disk.cpp utils.cpp db_impl.cpp drive/bitmap.cpp
TARGET_MAIN	= main.cpp

ALL_SRC		= $(TESTS) $(LIB_SRC) $(TARGET_MAIN)

UTIL_SRCS	= $(LIB_SRC) $(TARGET_MAIN)
TEST_SRCS = $(TESTS) $(LIB_SRC)
LIB_SRCS 	= $(LIB_SRC)

# directories
SOURCE_DIR	= src/
INCLUDE_DIR	= src/
OBJECT_DIR	= objs/
DEPENDENCY_DIR	= dep/

# compiler & flags
COMPILER	= clang++
LINKER		= clang++

COMMON_FLAGS	= -O2 -std=c++11 -Wall -g -I$(INCLUDE_DIR) -Iinclude/keydb/
COMPILE_FLAGS	= $(COMMON_FLAGS) -c
LINK_FLAGS	= $(COMMON_FLAGS) -g -lpthread -lgtest -lgtest_main -lcrypto -L/usr/lib
DEP_FLAGS	= $(COMMON_FLAGS) -MM -MF $(DEPENDENCY_DIR)$$(addsuffix .d,$$(basename $$(subst /,-,$$<)))

# Don't change lines below
COMPILE_CMD	= $(COMPILER) $(COMPILE_FLAGS) -o $$@ $$<
LINK_CMD	= $(LINKER) $(LINK_FLAGS) -o $@ $^
DEP_CMD		= $(COMPILER) $(DEP_FLAGS) $$<

UTIL_BSN	= $(basename $(UTIL_SRCS))
TEST_BSN	= $(basename $(TEST_SRCS))
LIB_BSN		= $(basename $(LIB_SRCS))

UTIL_OBJS		= $(addsuffix .o, $(addprefix $(OBJECT_DIR),$(subst /,-,$(UTIL_BSN))))
TEST_OBJS		= $(addsuffix .o, $(addprefix $(OBJECT_DIR),$(subst /,-,$(TEST_BSN))))
LIB_OBJS		= $(addsuffix .o, $(addprefix $(OBJECT_DIR),$(subst /,-,$(LIB_BSN))))

.PHONY	: all clean prepare test lib util

all : $(TARGETS)

clean 	:
	rm -rf $(TARGETS) $(OBJECT_DIR)*.o $(DEPENDENCY_DIR)*.d $(DEPENDENCY_DIR)*.P

prepare:
	mkdir -p $(SOURCE_DIR) $(INCLUDE_DIR) $(OBJECT_DIR) $(DEPENDENCY_DIR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

lib: $(LIB_TARGET)

util: $(UTIL_TARGET)

$(UTIL_TARGET) 	: $(UTIL_OBJS)
	$(LINK_CMD)

$(TEST_TARGET) : $(TEST_OBJS)
	$(LINK_CMD)

$(LIB_TARGET) : $(LIB_OBJS)
	ar rcs $@ $^

define COMPILE_MACRO
$(OBJECT_DIR)$(subst /,-,$(basename $(1)).o) 	: $(SOURCE_DIR)$(1)
	$(COMPILE_CMD)
	@$(DEP_CMD)
	@sed -e 's#\s*[-_a-zA-Z/]*\.o#$$@#g'\
		< $(DEPENDENCY_DIR)$$(addsuffix .d,$$(basename $$(subst /,-,$$<))) \
	   	> $(DEPENDENCY_DIR)$$(addsuffix .P,$$(basename $$(subst /,-,$$<)))
endef

$(foreach i,$(ALL_SRC),$(eval $(call COMPILE_MACRO,$(i))))

-include $(DEPENDENCY_DIR)*.P
