CSRCDIR := $(CURDIR)/csrc
VSRCDIR := $(CURDIR)/vsrc
all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

pre_dualswitch:
	verilator --cc --top-module dual_switch.v	
	verilator --cc --exe --build -j --trace -Wall $(CSRCDIR)/pre_dual_switch/sim_main.cpp \
		$(VSRCDIR)/pre_dual_switch/dual_switch.v 
	$(call git_commit, "built pre_dual_switch") # DO NOT REMOVE THIS LINE!!!

clean:
	rm -r obj_dir

include ../Makefile
