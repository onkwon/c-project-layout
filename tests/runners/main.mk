COMPONENT_NAME = main

SRC_FILES = \
	../src/main.c \

TEST_SRC_FILES = \
	src/main_test.cpp \
	src/test_all.cpp \

INCLUDE_DIRS = \
	../include \
	$(CPPUTEST_HOME)/include \

MOCKS_SRC_DIRS =
CPPUTEST_CPPFLAGS =

include MakefileRunner.mk
