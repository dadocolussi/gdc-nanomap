TARGET := tests/gdc-nanomap_unit_tests
TGT_INCDIRS := ../include
SOURCES := unit_tests.cpp

define COPY_HEADERS_AND_RUN_TESTS
	$(TARGET_DIR)/$(TARGET)
	mkdir -p $(TARGET_DIR)/include/gdc
	cp $(DIR)/../include/gdc/*.hpp $(TARGET_DIR)/include/gdc/
endef

TGT_POSTMAKE := $(COPY_HEADERS_AND_RUN_TESTS)
