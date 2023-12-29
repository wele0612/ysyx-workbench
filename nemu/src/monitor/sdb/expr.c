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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>

#define TKPRIOR_OPRAND 255
enum {
  TK_NOTYPE, 
  TK_END,

  TK_EQ,

  TK_PLUS,
  TK_MINUS,
  TK_MULTIPLY,
  TK_DIVIDE,

  TK_LEFT_B,
  TK_RIGHT_B,
  TK_NUM_DEC,
  TK_NUM_HEX,

  TK_NEG,
  TK_PTR_DEREF

  /* TODO: Add more token types */

};
/*
enum {
  EXPR_TERMINATE,
  EXPR_CONST,
  EXPR_REG,
  EXPR_OPERATOR
};
*/

static struct rule {
  const char *regex;
  int token_type;
  int priority;//priority of operator, not rule!!
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE,0},    // spaces
  {"\\+", TK_PLUS,3},         // plus
  {"==", TK_EQ,6},        // equal
  //------------added in PA1--------
  
  {"\\(",TK_LEFT_B,100},
  {"\\)",TK_RIGHT_B,TKPRIOR_OPRAND},
  
  {"-",TK_MINUS,3},
  {"\\*",TK_MULTIPLY,2},//may need to fix precedence levels
  {"/",TK_DIVIDE,2},
  {"0x[0-9a-f]+",TK_NUM_HEX,TKPRIOR_OPRAND},
  {"[0-9]+",TK_NUM_DEC,TKPRIOR_OPRAND}

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  int priority;
} Token;

#define EXPR_MAX_TOKENS 32

static Token tokens[EXPR_MAX_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  bool is_binary_operator;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    if(nr_token>=EXPR_MAX_TOKENS-1){
      printf("Error: expression too long.\n");
      return false;
    }
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        is_binary_operator=false;
        if(nr_token>0){
          if(tokens[nr_token].priority==TKPRIOR_OPRAND){
            is_binary_operator=true;
          }
        }

        if(is_binary_operator){
          printf("%.*s may be special\n",substr_len, substr_start);
        }

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case(TK_NOTYPE): continue;
          default: 
            tokens[nr_token].priority=rules[i].priority;
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  tokens[nr_token+1].type=TK_END;
  tokens[nr_token+1].str[0]='\0';

  for(i=0;i<EXPR_MAX_TOKENS;i++){
    printf("%d -> %s\n",i,tokens[i].str);
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  *success=true;
  return 0;
}
