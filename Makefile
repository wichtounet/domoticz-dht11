CXX=g++
LD=g++

default: release

include make-utils/flags-pi.mk
include make-utils/cpp-utils.mk

CXX_FLAGS += -pedantic -pthread

$(eval $(call auto_folder_compile,src))
$(eval $(call auto_add_executable,dht11_driver))

release: release_dht11_driver
release_debug: release_debug_dht11_driver
debug: debug_dht11_driver

all: release release_debug debug

clean: base_clean

include make-utils/cpp-utils-finalize.mk

.PHONY: default release_debug release debug all clean conf
