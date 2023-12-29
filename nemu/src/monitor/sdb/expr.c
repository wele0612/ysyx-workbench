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
#include <stdlib.h>
#include <stdio.h>

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
  int16_t priority;
  int64_t num_value;
} Token;

typedef struct suffix_expression{
  int length;
  Token* tokens;
} Suffix_expr;

#define EXPR_MAX_TOKENS 32

static Token tokens[EXPR_MAX_TOKENS] __attribute__((used)) = {};
static int stackbuffer[EXPR_MAX_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
static int buffer_sp __attribute__((used))  = 0;

static bool make_token(char *e,int *length) {
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
        //Non-binary version of '-' and '*' are different, etc.
        if(nr_token>0){
          if(tokens[nr_token-1].priority==TKPRIOR_OPRAND){
            is_binary_operator=true;
          }
        }

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case(TK_NOTYPE): break;

          case(TK_MINUS):
            if (is_binary_operator){
              tokens[nr_token].priority=rules[i].priority;
              tokens[nr_token].type=rules[i].token_type;
            }else{
              tokens[nr_token].priority=1;
              tokens[nr_token].type=TK_NEG;
            }
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
            break;

          case(TK_MULTIPLY):
            if (is_binary_operator){
              tokens[nr_token].priority=rules[i].priority;
              tokens[nr_token].type=rules[i].token_type;
            }else{
              tokens[nr_token].priority=1;
              tokens[nr_token].type=TK_PTR_DEREF;
            }
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
            break;

          default: 
            tokens[nr_token].priority=rules[i].priority;
            tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            nr_token++;
        }

        break;
      }
    };

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  tokens[nr_token].type=TK_END;
  tokens[nr_token].str[0]='\0';
/*
  for(i=0;i<nr_token;i++){
    printf("%d -> (%d)%s\n",i,tokens[i].priority,tokens[i].str);
  }
  */

  *length=nr_token+1;
  return true;
}

Suffix_expr parse_expr(char *e){
  int length;
  int i,j;
  Token* suffix_expr;
  Suffix_expr ans;
  ans.length=-1;

  if (!make_token(e,&length)) {
    return ans;
  }
  
  suffix_expr=(Token*)malloc(sizeof(Token)*length);
  if(suffix_expr==NULL){
    return ans;
  }

  buffer_sp=0;
  i=0;
  j=0;
  while(i<length){
    switch(tokens[i].type){

      case(TK_NUM_DEC):
        sscanf(tokens[i].str,"%ld",&tokens[i].num_value);
        suffix_expr[j]=tokens[i];
        j++;
        break;
        /*stackbuffer[buffer_sp]=i;
        buffer_sp++;*/
      
      case(TK_NUM_HEX):
        sscanf(tokens[i].str+2,"%lx",&tokens[i].num_value);
        suffix_expr[j]=tokens[i];
        j++;
        break;

      case(TK_LEFT_B):
        stackbuffer[buffer_sp]=i;
        buffer_sp++;
        break;
      
      case(TK_RIGHT_B):
        while(1){
          if(buffer_sp<=0){
            printf("Error: extra \')\' in expression.\n");
            return ans;
          }

          buffer_sp--;
          if(tokens[stackbuffer[buffer_sp]].type==TK_LEFT_B){
            break;
          }else{
            suffix_expr[j]=tokens[stackbuffer[buffer_sp]];
            j++;
          }
        }
        break;

      case(TK_END):
        if(buffer_sp!=0){
          while(1){
            if(buffer_sp<=0){
              break;
            }
            buffer_sp--;
            if(tokens[stackbuffer[buffer_sp]].type==TK_LEFT_B){
              printf("Error: extra \'(\' in expression.\n");
              return ans;
            }else{
              suffix_expr[j]=tokens[stackbuffer[buffer_sp]];
              j++;
            }
          }
        }
        break;

      default:
        while(1){
          if(buffer_sp<=0){
            break;
          }
          if(tokens[stackbuffer[buffer_sp-1]].type==TK_LEFT_B\
            ||tokens[stackbuffer[buffer_sp-1]].priority>tokens[i].priority){
            break;
          }
          buffer_sp--;
          suffix_expr[j]=tokens[stackbuffer[buffer_sp]];
          j++;
        }
        stackbuffer[buffer_sp]=i;
        buffer_sp++;
        break;
    }
    i++;
  }

  ans.tokens=suffix_expr;
  ans.length=j;
  return ans;
}

word_t eval_expr(Token *suffix_expr,bool *success){
  //Token oprand1,oprand2;

  return 0;
}

word_t expr(char *e, bool *success) {
  Suffix_expr expr=parse_expr(e);
  int i;
  if(expr.length==-1){
    *success=false;
    return 0;
  }

  for(i=0;i<expr.length;i++){
    printf("%s",expr.tokens[i].str);
  }
  printf("\n");


  *success=true;
  return 0;
}
