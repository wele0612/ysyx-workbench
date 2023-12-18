CSRCDIR := $(CURDIR)/csrc
VSRCDIR := $(CURDIR)/vsrc
all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
pre_dualswitch_nosim:
	verilator --cc --trace -Wall --top-module dual_switch $(VSRCDIR)/pre_dual_switch/dual_switch.v
	make -C ./obj_dir -f Vdual_switch.mk	
	gcc -I/usr/local/share/verilator/include -I/usr/local/share/verilator/include/vltstd -I./obj_dir -I$(NVBOARD_HOME)/include -c ./csrc/pre_dual_switch/sim_main.cpp -o sim_main.o
	gcc ./obj_dir/*.o sim_main.o $(NVBOARD_HOME)/build/*.o -o  sim_main -L/usr/lib -lSDL2
pre_dualswitch:
	verilator --cc --exe --build -j --trace -Wall -I$(NVBOARD_HOME)/include $(CSRCDIR)/pre_dual_switch/sim_main.cpp \
		$(VSRCDIR)/pre_dual_switch/dual_switch.v 
	$(call git_commit, "built pre_dual_switch") # DO NOT REMOVE THIS LINE!!!

clean:
	rm -r obj_dir

include ../Makefile
