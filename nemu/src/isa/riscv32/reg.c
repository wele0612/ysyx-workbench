/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define ISA_REG_DISPLAY_COLUMNS 2

void isa_reg_display() {
  int i;
  int64_t value;
  bool success;
  printf("PC = \33[1;36m0x%lx\33[0m",(uint64_t)cpu.pc);
  for(i=0;i<sizeof(regs)/sizeof(regs[0]);i++){
    if(i%ISA_REG_DISPLAY_COLUMNS==0){
      printf("\n");
    }
    value=isa_reg_str2val(regs[i],&success);
    printf("\33[1;36m%s\33[0m %ld 0x%lx  \t",regs[i],value,(uint64_t)value);
  }
  printf("\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int i;
  printf("Accessing %s.\n",s);
  for(i=0;i<sizeof(regs)/sizeof(regs[0]);i++){
    if(strcmp(s,regs[i])==0){
      *success=true;
      return cpu.gpr[i];
    }
  }
  *success=false;
  return 0;
}
