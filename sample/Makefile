MK_CMD	= clang++ --std=c++11 -o $@ $< -L ../ -lkeydb

ALL		= sample1 sample2

.PHONY : clean ALL_TARGET

ALL_TARGET : $(ALL)

clean:
	rm -rf $(ALL)

sample1 : sample1.cpp
	$(MK_CMD)

sample2 : sample2.cpp
	$(MK_CMD)
