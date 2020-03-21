void Exit(){
  asm(
    "li a7, 1;"
     "ecall"
  );
}