ifeq ($(BUILD_TYPE), RELEASE)
  BUILD_DIR := objects/release
  TARGET_DIR := release
  CXXFLAGS := -O3 -march=native
else
  BUILD_DIR := objects/debug
  TARGET_DIR := debug
  CXXFLAGS := -O0 -g3
endif

CXX := c++
CXXFLAGS += -std=c++11 -pedantic -Wall -Wshadow
INCDIRS := src
DEFS :=
LDFLAGS := -L$(TARGET_DIR)

SUBMAKEFILES := unit_tests/sub.mk demo/sub.mk
