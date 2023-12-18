#include <nvboard.h>
#include <Vtop.h>
#include <stdio.h>
static TOP_NAME dut;
static int i=0;
void nvboard_bind_all_pins(Vtop* top);

void single_cycle(){
	dut.clk=0;
	dut.eval();
	dut.clk=1;
	dut.eval();
}
int main(){
	nvboard_bind_all_pins(&dut);
	nvboard_init();
	
	dut.rst=1;
	single_cycle();
	single_cycle();
	dut.rst=0;

	while(1){
		printf("cycle %d!\n",++i);
		nvboard_update();
		single_cycle();
	}
}
