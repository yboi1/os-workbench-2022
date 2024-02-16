#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define STACK_SIZE 4096
#define MAX_NUM 26

enum co_status{
  co_NEW = 1,
  co_RUNNING,
  co_WAITING,
  co_DEAD,
};

struct co {
  char name[10];
  void(*func)(void *);
  void *arg;

  // struct co* next;    // 维护一个循环列表
  enum co_status status;
  struct co*     waiter;
  jmp_buf        context;
  uint8_t        stack[STACK_SIZE];         // stack_switch_call (x86.h)  实现上下文的切换


};

// 维护一个链表表示目前存有的协程
typedef struct ListNode{
  struct co* val;
  struct ListNode* next;
  int num;
}List;

List* coList;


// 创建一个新的协程   只是创建，不会立即执行任务
struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  if(coList!=NULL && coList->num > MAX_NUM){
    printf("List size is too more\n");
    return NULL;
  }
  struct co* New = (struct co*)malloc(sizeof(struct co));
  strcpy(New->name, name);
  // New->name = name;
  New->func = func;
  New->arg = arg;
  New->status = co_NEW;

  List* tmp = coList;
  while(tmp->next != NULL){
    tmp=tmp->next;
  }  
  List *add_list = (List *)malloc(sizeof(List));
  add_list->val = New;
  tmp->next = add_list;
  coList->num++;

  return New;
}

// 等待--直到协程执行完毕   并将执行完任务的协程释放
void co_wait(struct co *co) {
  co->status = co_RUNNING;
  co->func(co->arg);
  co->status = co_DEAD;

  // List* del;
  if(!strcmp(coList->val->name,co->name)){
    coList=coList->next;
  }
  else{
    List* tmp = coList;
    while(!strcmp(tmp->next->val->name, co->name)){
      tmp=tmp->next;
    }  
    // del = tmp->next;
    tmp->next = tmp->next->next;

  }
  // free(del);
  free(co);
  coList->num--;
  
  //  将协程释放并移除列表
}


//  int setjmp(jmp_buf env);    返回值为 longjmp中的参数 val
//  void longjmp(jmp_buf env, int val);   无返回值

// 将资源转让给协程
void co_yield() {
/*
  首先先将当前执行的协程状态保存下来， 然后调用一个当前可以执行协程， 恢复其状态， 实现调换
*/

  // 找到当前执行的协程
  List* tmp = coList;
  while(tmp->next!=NULL && tmp->val->status!=co_RUNNING){
    tmp = tmp->next;
  }
  


  if(tmp->val->status==co_RUNNING){
    tmp->val->status = co_WAITING;

    List* find_p = coList;
    // 找到可以切换的协程
    while(find_p->next!=NULL && find_p->val->status!=co_DEAD && find_p->val->status!=co_RUNNING){
      find_p = find_p->next;
    }

    tmp->val->waiter = find_p->val;
    find_p->val->status=co_RUNNING;
    
    setjmp(tmp->val->context);

    longjmp(find_p->val->context,  1);
    
  }

}
