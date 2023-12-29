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

#ifndef __SDB_H__
#define __SDB_H__

#define TOKEN_STR_LENGTH 32
#define DESCRIPTION_LENGTH 32


#include <common.h>

typedef struct token {
  int type;
  char str[TOKEN_STR_LENGTH];
  int16_t priority;
  int64_t num_value;
} Token;

typedef struct suffix_expression{
  int length;
  Token* tokens;
} Suffix_expr;

typedef struct watchpoint {
  int NO;
  word_t prev_value;
  char description[DESCRIPTION_LENGTH];
  Suffix_expr expr;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

word_t expr(char *e, bool *success);
word_t eval_expr(Suffix_expr expr,bool *success);
Suffix_expr parse_expr(char *e);

WP* wp_pool_remove(int NO,WP *pool,WP **removed_item);
void wp_pool_display_head();
void wp_pool_add(int NO,char* e);

#endif
