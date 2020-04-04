/* lib.h - self implemented lib
 * Author: Ye Zilingfeng
 */
#define SYS_CALL(which, arg0,arg1,arg2)({ \
  register unsigned long long int a0 asm("a0") = (unsigned long long int)arg0;\
  register unsigned long long int a1 asm("a1") = (unsigned long long int)arg1;\
  register unsigned long long int a2 asm("a2") = (unsigned long long int)arg2;\
  register unsigned long long int a7 asm("a7") = (unsigned long long int)which;\
  asm volatile ("ecall"                                                         \
                :                                                               \
                :"r"(a0),"r"(a1),"r"(a2),"r"(a7)                                \
                :);                                                             \
})

void Exit(){
  register unsigned long long int a7 asm("a7") = (unsigned long long int)1;
  asm volatile(
     "ecall"
  );
}

void Print_s(char* c){
  register unsigned long long int a7 asm("a7") = (unsigned long long int)2;
  register unsigned long long int a0 asm("a0") = (unsigned long long int)c;
  asm volatile(
     "ecall"
  );
}

void Print_d(long long int a){
  register unsigned long long int a7 asm("a7") = (unsigned long long int)3;
  register unsigned long long int a0 asm("a0") = (unsigned long long int)a;
  asm volatile(
     "ecall"
  );
}