CSRCDIR := $(CURDIR)/csrc
VSRCDIR := $(CURDIR)/vsrc
all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

pre_dualswitch:
	verilator --cc --exe --build -j 0 -Wall $(CSRCDIR)/pre_dual_switch/sim_main.cpp \
		$(VSRCDIR)/pre_dual_switch/dual_switch.v 

clean:
	rm -r obj_dir

include ../Makefile
