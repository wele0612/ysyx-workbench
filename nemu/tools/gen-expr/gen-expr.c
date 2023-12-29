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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

const char oprands[4]={'+','-','*','/'};

int random_expr(int max_depth,char* buf){
  int use_struct=rand()%2;
  int length,i;
  length=0;

  if(use_struct){
    buf[0]='(';
    buf++;
    length++;
  }

  i=2;//rand()%3;
  if(max_depth==0){
    i=0;
  }

  switch(i){
    case(0):
      buf[0]='1'+rand()%8;
      length++;
      buf++;
      break;
    case(1):
      length+=random_expr(max_depth-1,buf);
      buf+=length;
      break;
    case(2):
      i=random_expr(max_depth-1,buf);
      buf+=i;
      length+=i;

      buf[0]=oprands[rand()%4];
      buf+=1;
      length++;

      i=random_expr(max_depth-1,buf);
      buf+=i;
      length+=i;

      break;
  }

  if(use_struct){
    buf[0]=')';
    length+=1;
  }
  return length;
}

static void gen_rand_expr() {
  int length=random_expr(4,buf);
  buf[length]='\0';
  return;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
