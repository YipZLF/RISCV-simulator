/* lib.h - self implemented lib
 * Author: Ye Zilingfeng
 */
void Exit(){
  asm(
    "li a7, 1;"
     "ecall"
  );
}