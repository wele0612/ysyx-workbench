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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <utils.h>
//#include <stdio.h>
#include "sdb.h"
#include <memory/vaddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  //To fix the bug mentioned in PA1
  set_nemu_state(NEMU_QUIT, 0, 0);
  return -1;
}

static int cmd_si(char *args){
  uint64_t steps;
  int n=0;
  if(args!=NULL){
    n=sscanf(args,"%lu",&steps);
  }
  if(n!=1){
    steps=1;
    printf("Invalid steps \"%s\". Using default steps=1\n", args);
  }
  printf("Executing %lu steps.\n",steps);
  cpu_exec(steps);
  return 0;
}

static int cmd_info(char *args){
  if(args!=NULL){
    if(strcmp(args,"r")==0){
      isa_reg_display();
      return 0;
    }
    if(strcmp(args,"w")==0){
      return 0;
    }
  }
  printf("Invalid parameters. Use \'info r\' or \'info w\'.\n");
  return 0;
}

static int cmd_x(char *args){
  int length,n=0,i,j;
  bool success;
  word_t valuew;
  uint8_t value8;

  //read n words each time(n=4 for 32bit ISA)
  int isa_wordlength=sizeof(word_t);
  uint64_t addr;
  if(1){
    n=sscanf(args,"%d",&length);

    j=length;
    for(i=0;j>0;i++){
      j/=10;
    }
    addr=expr(args+i,&success);
    if(!success){
      printf("Invalid address expression.\n");
      return 0;
    }

    if(n==1){
      printf("Memory (\33[1;36m0x%lx\33[0m)+%d\n",addr,length);
      printf(ANSI_FG_YELLOW);
      printf("           +0 +1 +2 +3\n");
      printf(ANSI_NONE);

      for(i=0;i<length;i++){
        printf("\33[1;36m0x%lx\33[0m ",addr);
        for(j=0;j<isa_wordlength;j++){
          value8=(uint8_t)vaddr_read(addr+j,1);
          printf("%2x ",value8);
        }
        valuew=vaddr_read(addr,isa_wordlength);
        printf(" 0x%8x \t %d",*((uint32_t*)&valuew),*((int32_t*)&valuew));
        printf("\n");
        addr+=isa_wordlength;
      }
      return 0;
    }
  }
  printf("Invalid arguments for command \'x\'.\n");
  return 0;
}

static int cmd_p(char *args){
  word_t value;
  bool success;
  if(args!=NULL){
    value=expr(args,&success);    

    if(success){
      printf("\33[1;36mPC 0x%lx\33[0m => %ld\n",(uint64_t)cpu.pc,(int64_t)value);
    }else{
      Log("Invalid expression.");
    }
  }
  return 0;
}

static int cmd_w(char *args){
  return 0;
}

static int cmd_d(char *args){
  return 0;
}



static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "qnemu", "Exit NEMU", cmd_q },//changed to avoid conflict with gdb
  { "si", "Singal-step execute", cmd_si},
  { "info", "info r -> print regsisters, info w -> print watch point infomation", cmd_info},
  { "x", "Scan memory. Format: x N EXPR", cmd_x},
  { "p", "Print values. Expressions supported. Format: p EXPR", cmd_p},
  { "w", "Set watchpoint", cmd_w},
  { "d", "Delete watchpoint", cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
