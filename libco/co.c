#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// #define STACK_SIZE 4096
#define MAX_NUM 26

#define KB *1024
#define STACK_SIZE (64 KB)

//  协程栈的切换 汇编代码
static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
  asm volatile (
#if __x86_64__
    "movq %0, %%rsp; movq %2, %%rdi; jmp *%1" : : "b"((uintptr_t)sp),     "d"(entry), "a"(arg)
#else
    "movl %0, %%esp; movl %2, 4(%0); jmp *%1" : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg)
#endif
  );
}

enum co_status{
  CO_NEW = 1,
  CO_RUNNING,
  CO_WAITING,
  CO_DEAD,
};

struct co {
  const char* name;
  void(*func)(void *);
  void *arg;

  // struct co* next;    // 维护一个循环列表
  enum co_status status;
  struct co*     waiter;
  jmp_buf        context;
  // uint8_t        stack[STACK_SIZE];         // stack_switch_call (x86.h)  实现上下文的切换


  void* stackptr;
  struct co* next;
  struct co* prev;
  uint8_t stack[STACK_SIZE] __attribute__ ((aligned(16)));    // 每个协程的栈帧

};

// 维护一个链表表示目前存有的协程
// typedef struct ListNode{
//   struct co* val;
//   struct ListNode* next;
//   int num;
// }List;

// List* coList;

// 用于记录当前协程
struct co *co_main,*co_current;
int co_num=0;

// 用于对主协程的初始化
__attribute__((constructor)) void co_init() {
  co_main=malloc(sizeof(struct co));
  co_main->name="main";
  co_main->stackptr=co_main->stack+sizeof(co_main->stack);
  co_main->status=CO_RUNNING;
  memset(co_main->stack,0,sizeof(co_main->stack));

  // 链表的初始化
  co_main->next=co_main;
  co_main->prev=co_main;
  co_current=co_main;
  co_num+=1;
}

// 创建一个新的协程   只是创建，不会立即执行任务
struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  struct co* New = (struct co*)malloc(sizeof(struct co));
  // strcpy(New->name, name);
  New->name = name;
  New->func = func;
  New->arg = arg;
  New->status = CO_NEW;
  // 栈帧的初始化
  New->stackptr = New->stack+sizeof(New->stack);
  memset(New->stack, 0, sizeof(New->stack));

  // 维护当前协程的链表
  struct co* prev = co_main->prev;
  New->prev = prev;
  New->next = co_main;
  co_main->prev = New;
  prev->next = New;
  co_num++;

  return New;
}

void wrapper(){
  co_current->status = CO_RUNNING;
  co_current->func(co_current->arg);
  co_current->status = CO_DEAD;

  if(co_current->waiter!=NULL){
    co_current->waiter->status = CO_RUNNING;
    co_current->waiter = NULL;
  }

  co_yield();   // 将当前的协程执行完jmplong函数 将val值返回
}

void co_yield(){
  int val = setjmp(co_current->context);
  if(val==0){
    struct co* next = co_current->next;
    while(next->status==CO_WAITING||next->status==CO_DEAD)  next = next->next;
    co_current = next;
    if(next->status==CO_NEW){
      // 什么意思？
      if(sizeof(void*)==4) stack_switch_call(next->stackptr,wrapper,(uintptr_t)NULL);
      else{
        asm volatile("mov %0,%%rsp"::"b"((uintptr_t)next->stackptr));
        wrapper();
      }
    }
    else{
      longjmp(co_current->context, 0);
    }
  }
}

void co_wait(struct co* co){
  if(co->status==CO_DEAD){
    struct co* prev = co->prev;
    struct co* next = co->next;
    prev->next = next;
    next->prev = prev;
    free(co);
  }
  else{
    co->waiter = co_current;
    co_current->status = CO_WAITING;
    co_yield();
  }
}




// // 等待--直到协程执行完毕   并将执行完任务的协程释放
// void co_wait(struct co *co) {
//   co->status = co_RUNNING;
//   co->func(co->arg);
//   co->status = co_DEAD;

//   // List* del;
//   if(!strcmp(coList->val->name,co->name)){
//     coList=coList->next;
//   }
//   else{
//     List* tmp = coList;
//     while(!strcmp(tmp->next->val->name, co->name)){
//       tmp=tmp->next;
//     }  
//     // del = tmp->next;
//     tmp->next = tmp->next->next;

//   }
//   // free(del);
//   free(co);
//   coList->num--;
  
//   //  将协程释放并移除列表
// }


//  int setjmp(jmp_buf env);    返回值为 longjmp中的参数 val
//  void longjmp(jmp_buf env, int val);   无返回值

// // 将资源转让给协程
// void co_yield() {
// /*
//   首先先将当前执行的协程状态保存下来， 然后调用一个当前可以执行协程， 恢复其状态， 实现调换
// */

//   // 找到当前执行的协程
//   List* tmp = coList;
//   while(tmp->next!=NULL && tmp->val->status!=co_RUNNING){
//     tmp = tmp->next;
//   }
  


//   if(tmp->val->status==co_RUNNING){
//     tmp->val->status = co_WAITING;

//     List* find_p = coList;
//     // 找到可以切换的协程
//     while(find_p->next!=NULL && find_p->val->status!=co_DEAD && find_p->val->status!=co_RUNNING){
//       find_p = find_p->next;
//     }

//     tmp->val->waiter = find_p->val;
//     find_p->val->status=co_RUNNING;
    
//     setjmp(tmp->val->context);

//     longjmp(find_p->val->context,  1);
    
//   }

// }
