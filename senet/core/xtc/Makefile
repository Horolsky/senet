KERNEL_NAME = $(shell uname -s)
MACHINE_NAME = $(shell uname -m)
PROJ_V = 0.0.1
PROJ_N = senet_xtc
PROJ_SO = $(PROJ_N)-$(PROJ_V)-$(MACHINE_NAME)-$(KERNEL_NAME).so

.PHONY: all clean clean_test build test format tidy check check_dep update_libs

CPPFLAGS += -isystem $(GTEST_DIR)/include
# TODO: flag for clang -Wgnu-anonymous-struct to avoid -std=gnu++2a
TESTFLAGS += -pthread -fPIC -g -std=gnu++14 -Wall -Wextra -pedantic 
BUILDFLAGS += -pthread -fPIC -std=gnu++14 -O3
#  -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined

SRC_D := src
LIB_D := lib
TST_D := test
BLD_D := build

#external dependencies
DEPS = $(LIB_D)/bitf

HEADERS = $(SRC_D)/*.hpp
BLD_O := $(patsubst $(SRC_D)/%.cpp, $(BLD_D)/%.o, $(wildcard $(SRC_D)/*.cpp))
TST_O := $(patsubst $(SRC_D)/%.cpp, $(TST_D)/%.o, $(wildcard $(SRC_D)/*.cpp))
source = $(patsubst $(BLD_D)/%.o, $(SRC_D)/%.cpp, $(1))
testsource = $(patsubst $(TST_D)/%.o, $(SRC_D)/%.cpp, $(1))

#build object file
$(BLD_O): $(HEADERS) $(DEPS)
	$(CXX) $(CPPFLAGS) -I$(SRC_D) $(BUILDFLAGS) -c \
            $(call source, $@) -o $@

$(TST_O): $(HEADERS) $(DEPS)
	$(CXX) $(CPPFLAGS) -I$(SRC_D) $(TESTFLAGS) -c \
            $(call testsource, $@) -o $@

print:
	@echo $(PROJ_SO)

# $(DEPS): update_libs

update_libs:
	make -C $(LIB_D) update

$(BLD_D):
	mkdir -p $(BLD_D)

setup: $(BLD_D) clean $(BLD_O)
	$(CXX) $(BUILDFLAGS) $(BLD_O) -shared -o $(BLD_D)/$(PROJ_SO)

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
	rm -fr test/*.o test/*.a test/gtest_entry

clean: clean_test
	rm -fr $(BLD_D)/*

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
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(TESTFLAGS) -c \
            $(call gtsrc, $@) -o $@

$(GTEST_MAIN) : $(GTEST_O)
	$(AR) $(ARFLAGS) $@ $^

# GOOGLE_TEST: PROJECT SPECIFIC TASKS
TEST_ENTRY = $(TST_D)/gtest_entry.cpp

$(TST_D)/gtest_entry.o : $(TEST_ENTRY) $(TST_D)/*.hpp $(SRC_D)/*.hpp $(GTEST_HEADERS) $(DEPS)
	$(CXX) $(CPPFLAGS) -I$(SRC_D) $(TESTFLAGS) -c $(TEST_ENTRY) -o $@

$(TST_D)/gtest_entry : $(TST_D)/gtest_entry.o $(GTEST_MAIN) $(TST_O)
	@echo "Building $@ for $(KERNEL_NAME) $(MACHINE_NAME)"
	$(CXX) $(CPPFLAGS) $(TESTFLAGS) -lpthread $^ -o $@ 

test: $(TST_D)/gtest_entry
	$(TST_D)/gtest_entry