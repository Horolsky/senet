KERNEL_NAME = $(shell uname -s)
MACHINE_NAME = $(shell uname -m)
PROJ_V = 0.0.1
PROJ_N = senet_xtc
PROJ_SO = $(PROJ_N)-$(PROJ_V)-$(MACHINE_NAME)-$(KERNEL_NAME).so

.PHONY: all clean clean_test build test format tidy check check_dep

CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -pthread -fPIC -g -std=gnu++14 -Wall -Wextra -pedantic 
#  -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined

SRC_D := src
LIB_D := lib
TST_D := test
BLD_D := build

UNITS_H = $(SRC_D)/*.hpp
# UNITS_O := $(patsubst $(SRC_D)/%.cpp, $(BLD_D)/%.o, $(wildcard $(SRC_D)/*.cpp))
# not working because of f_rules
UNITS_O = $(BLD_D)/s_state.o $(BLD_D)/s_moves.o $(BLD_D)/s_event.o $(BLD_D)/s_ply.o $(BLD_D)/f_eval.o 
source = $(patsubst $(BLD_D)/%.o, $(SRC_D)/%.cpp, $(1))

#build object file
$(UNITS_O): $(UNITS_H)
	$(CXX) $(CPPFLAGS) -I$(SRC_D) $(CXXFLAGS) -c \
            $(call source, $@) -o $@

print:
	@echo $(PROJ_SO)

build: $(UNITS_O)
	$(CXX) $(CPPFLAGS) $(UNITS_O) -shared -o $(BLD_D)/$(PROJ_SO)

all : test

format: .clang-format
	clang-format -i -style=file src/* test/*

check:
	cppcheck -q --enable=all --verbose src/* test/* 
check_dep:
	cppcheck -q --enable=all --verbose --error-exitcode=1 src/* test/* --xml 2>cppcheck.out.xml

tidy: .clang-tidy
	clang-tidy --extra-arg-before=-xc++ --format-style=file src/* test/* 

clean_test:
	rm -fr test/*.o test/*.a gtest_entry

clean: clean_test
	rm -fr build/*

# GOOGLE_TESTS: PREPARATION

ifndef GTEST_DIR
GTEST_DIR = ../../../../googletest/googletest
endif
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
GTEST_MAIN = $(TST_D)/gtest_main.a
GTEST_O = $(addprefix $(TST_D)/, gtest-all.o gtest_main.o)
gtsrc = $(patsubst $(TST_D)/%.o, $(GTEST_DIR)/src/%.cc, $(1))

$(GTEST_O): $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(call gtsrc, $@) -o $@

$(GTEST_MAIN) : $(GTEST_O)
	$(AR) $(ARFLAGS) $@ $^

# GOOGLE_TEST: PROJECT SPECIFIC TASKS
TEST_ENTRY = $(TST_D)/gtest_entry.cpp

$(TST_D)/gtest_entry.o : $(TEST_ENTRY) $(TST_D)/*.hpp $(SRC_D)/*.hpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I$(SRC_D) $(CXXFLAGS) -c $(TEST_ENTRY) -o $@

$(TST_D)/gtest_entry : $(TST_D)/gtest_entry.o $(GTEST_MAIN) $(UNITS_O)
	@echo "Building $@ for $(KERNEL_NAME) $(MACHINE_NAME)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@ 

test: $(TST_D)/gtest_entry
	$(TST_D)/gtest_entry