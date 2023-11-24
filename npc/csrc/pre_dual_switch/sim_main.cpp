#include "Vdual_switch.h"
#include "verilated_vcd_c.h"
#include "verilated.h"
#include <nvboard.h>
#include <cstdlib>
#include <cassert>
int main(int argc, char** argv) {
      VerilatedContext* contextp = new VerilatedContext;
      Verilated::traceEverOn(true);
      VerilatedVcdC* tfp= new VerilatedVcdC;
	
      int a=0,b=0;
      nvboard_bind_pin(&a,false,true,1,SW1);
      nvboard_bind_pin(&b,false,true,1,SW2);
      nvboard_bind_pin(&(top->f),false,false,1,LD0);

      nvboard_init(); 

      contextp->commandArgs(argc, argv);
      Vdual_switch* top = new Vdual_switch{contextp};
      
      top->trace(tfp,10);

      tfp->open("./my_sim.vcd");

      srand(1);
      for(int i=0;i<20;i++){
	      a=rand()%2;
	      b=rand()%2;
	      top->a=a;
	      top->b=b;
	      top->eval();
	      printf("a = %d, b = %d, f = %d\n",a,b,top->f);
	      assert(top->f==(a^b));

	      tfp->dump(contextp->time());
	      contextp->timeInc(1);
      }

      tfp->close();
     
      while(1){
	      top->eval();
	      nvboard_update();
      }

      delete top;
      delete contextp;
      return 0;
 }
