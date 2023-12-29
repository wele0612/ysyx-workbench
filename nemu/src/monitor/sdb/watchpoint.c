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

bool is_watch_point_inpool(int NO,WP *pool){
  if(pool==NULL){
    return false;
  }
  if(pool->NO==NO){
    return true;
  }else{
    return is_watch_point_inpool(NO,pool->next);
  }
}

//return linked list head, with item removed
void wp_pool_add(int NO,char* e){
  Suffix_expr expr;
  int i;
  bool success;

  if(!is_watch_point_inpool(NO,free_)){
    printf("NO%d is not avaliable.\n",NO);
    return;
  }

  expr=parse_expr(e);
  printf("Expression: %s\n",e);
  if(expr.length==-1){
    printf("Error: Invalid expression.\n");
    return;
  }

  for(i=0;i<expr.length;i++){
    printf("%s ",expr.tokens[i].str);
    //return;
  }
  printf("\n");

  word_t value=eval_expr(expr,&success);
  if(!success){
    printf("Error: Invalid expression.\n");
  }
  WP* new_pool;
  free_=wp_pool_remove(NO,free_,&new_pool);
  assert(new_pool!=NULL);
  new_pool->next=head;
  new_pool->NO=NO;
  strncpy(new_pool->description,e,DESCRIPTION_LENGTH-1);
  new_pool->prev_value=value;
  head=new_pool;  

  return;
}

WP* wp_pool_remove(int NO,WP *pool,WP **removed_item){
  WP *ans;
  if(pool==NULL){
    return NULL;
  }
  if(pool->NO==NO){
    ans=pool->next;
    //printf("%d -----\n",pool->NO);
    *removed_item=pool;
    return ans;
  }
  pool->next=wp_pool_remove(NO,pool->next,removed_item);
  return pool;
}


void wp_pool_display(WP *pool){
  if(pool==NULL){
    return;
  }
  printf("NO.%d \t%s\n",pool->NO,pool->description);
  wp_pool_display(pool->next);
}

void wp_pool_display_head(){
  printf("Watchpoints:\n");
  wp_pool_display(head);
}

/* TODO: Implement the functionality of watchpoint */

