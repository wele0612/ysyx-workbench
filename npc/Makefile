BUILD_DIR = ./build
NVBOARD_HOME = /home/tianyi/projects/ysyx-workbench/nvboard

TOP_NAME = Top_Module
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O2 --x-assign fast --x-initial fast --noassert
OBJ_DIR = $(BUILD_DIR)/obj_dir

BIN = $(BUILD_DIR)/$(TOP_NAME)


export PATH := $(PATH):$(abspath ./utils)

VSRCS = $(shell find -name "*.v")
NVBOARD_SRCS = $(shell find $(NVBOARD_HOME)/SRC -name "*.cpp")
NVBOARD_INC = $(NVBOARD_HOME)/include

nvboard: verilog $(VSRCS) $(CSRCS)
	verilator $(VERILATOR_CFLAGS) \
		--top-module $(TOP_NAME) $(VSRCS) $(CSRCS) \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

testmake:
	echo $(VSRC)

test:
	mill -i __.test

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i __.test.runMain Elaborate -td $(BUILD_DIR)

help:
	mill -i __.test.runMain Elaborate --help

compile:
	mill -i __.compile

bsp:
	mill -i mill.bsp.BSP/install

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: nvboard test verilog help compile bsp reformat checkformat clean

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Done."

-include ../Makefile
