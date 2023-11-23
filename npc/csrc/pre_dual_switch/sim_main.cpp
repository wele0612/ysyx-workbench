#include "Vdual_switch.h"
#include "verilated_vcd_c.h"
#include "verilated.h"
#include <cstdlib>
#include <cassert>
int main(int argc, char** argv) {
      VerilatedContext* contextp = new VerilatedContext;
      Verilated::traceEverOn(true);
      VerilatedVcdC* tfp= new VerilatedVcdC;


      contextp->commandArgs(argc, argv);
      Vdual_switch* top = new Vdual_switch{contextp};
      int a=0,b=0;
      
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
      
      delete top;
      delete contextp;
      return 0;
 }
