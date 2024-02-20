2.16    实现了co.c函数， 但是当make test时出现了段错误， 后续检查错误

2.20    实现co.c文件    难点主要是：栈切换的汇编代码， 

static inline void stack_switch_call(void *sp, void *entry, uintptr_t arg) {
asm volatile (
#if __x86_64__
    "movq %0, %%rsp; movq %2, %%rdi; jmp *%1" : : "b"((uintptr_t)sp),     "d"(entry), "a"(arg)
#else
    "movl %0, %%esp; movl %2, 4(%0); jmp *%1" : : "b"((uintptr_t)sp - 8), "d"(entry), "a"(arg)
#endif
  );
}

保证了在不同机器上的编译问题，


协程存储的主要思路是维护了一个协程的链表用于上下文切换，  同时在文件开始时，应该声明一个全局的主线程 ， 
用next和prev进行对链表的遍历


栈帧的初始化也是一个难点
 // 栈帧的初始化
  New->stackptr = New->stack+sizeof(New->stack);
  memset(New->stack, 0, sizeof(New->stack));
要保证栈指针的位置， 以及初始化

协程的上下文切换主要是依赖了setjmp和jmplong函数实现的   #include<setjmp.h>
