COMPONENT_NAME = main

SRC_FILES = \
	$(PRJ_BASE)/src/main.c \

TEST_SRC_FILES = \
	src/main_test.cpp \
	src/test_all.cpp \

INCLUDE_DIRS = \
	$(PRJ_BASE)/include \
	$(CPPUTEST_HOME)/include \

MOCKS_SRC_DIRS =
CPPUTEST_CPPFLAGS =

include MakefileRunner.mk
