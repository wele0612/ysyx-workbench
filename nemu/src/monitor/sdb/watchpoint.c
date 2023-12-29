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

#include "sdb.h"

#define NR_WP 32



static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

//return linked list head, with item removed
void wp_pool_add(int NO,char* e){
  Suffix_expr expr;
  int i;
  bool success;
  expr=parse_expr(e);
  printf("Expression: %s",e);
  if(expr.length==-1){
    printf("Error: Invalid expression.\n");
    return;
  }

  for(i=0;i<expr.length;i++){
    printf("%s ",expr.tokens[i].str);
    return;
  }
  printf("\n");

  word_t value=eval_expr(expr,&success);
  if(!success){
    printf("Error: Invalid expression.\n");
  }

  WP* new_pool=malloc(sizeof(WP));
  assert(new_pool!=NULL);
  new_pool->next=head;
  new_pool->NO=NO;
  strncpy(new_pool->description,e,DESCRIPTION_LENGTH-1);
  new_pool->prev_value=value;
  head=new_pool;

  return;
}

WP* wp_pool_remove(int NO,WP *pool){
  if(pool==NULL){
    return NULL;
  }
  if(pool->NO==NO){
    free(pool);
    return pool->next;
  }
  pool->next=wp_pool_remove(NO,pool->next);
  return pool;
}


void wp_pool_display(WP *pool){
  if(pool==NULL){
    return;
  }
  printf("NO.%d %s\n",pool->NO,pool->description);
  wp_pool_display(pool->next);
}

void wp_pool_display_head(){
  wp_pool_display(head);
}

/* TODO: Implement the functionality of watchpoint */

