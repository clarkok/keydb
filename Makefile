# basic
TARGET		= gtest
TESTS			= test/slice_test.cpp test/buffer_test.cpp test/exception_test.cpp \
						test/disk_test.cpp
SOURCES		= $(TESTS) main.cpp naive_disk.cpp

# directories
SOURCE_DIR	= src/
INCLUDE_DIR	= src/
OBJECT_DIR	= objs/
DEPENDENCY_DIR	= dep/

# compiler & flags
COMPILER	= clang++
LINKER		= clang++

COMMON_FLAGS	= -std=c++11 -Wall -g -I$(INCLUDE_DIR)
COMPILE_FLAGS	= $(COMMON_FLAGS) -c
LINK_FLAGS	= $(COMMON_FLAGS) -g -lpthread -lgtest -lgtest_main -lcrypto -L/usr/lib
DEP_FLAGS	= $(COMMON_FLAGS) -MM -MF $(DEPENDENCY_DIR)$$(addsuffix .d,$$(basename $$(subst /,-,$$<)))

# Don't change lines below
COMPILE_CMD	= $(COMPILER) $(COMPILE_FLAGS) -o $$@ $$<
LINK_CMD	= $(LINKER) $(LINK_FLAGS) -o $@ $^
DEP_CMD		= $(COMPILER) $(DEP_FLAGS) $$<

BASENAME	= $(basename $(SOURCES))
OBJECTS		= $(addsuffix .o, $(addprefix $(OBJECT_DIR),$(subst /,-,$(BASENAME))))

$(TARGET) 	: $(OBJECTS)
	$(LINK_CMD)

define COMPILE_MACRO
$(OBJECT_DIR)$(subst /,-,$(basename $(1)).o) 	: $(SOURCE_DIR)$(1)
	$(COMPILE_CMD)
	@$(DEP_CMD)
	@sed -e 's#\s*[-_a-zA-Z/]*\.o#$$@#g'\
		< $(DEPENDENCY_DIR)$$(addsuffix .d,$$(basename $$(subst /,-,$$<))) \
	   	> $(DEPENDENCY_DIR)$$(addsuffix .P,$$(basename $$(subst /,-,$$<)))
endef

$(foreach i,$(SOURCES),$(eval $(call COMPILE_MACRO,$(i))))

-include $(DEPENDENCY_DIR)*.P

.PHONY	: clean prepare test

clean 	:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCY_DIR)*.d $(DEPENDENCY_DIR)*.P

prepare:
	mkdir -p $(SOURCE_DIR) $(INCLUDE_DIR) $(OBJECT_DIR) $(DEPENDENCY_DIR)

test: $(TARGET)
	./$(TARGET)
