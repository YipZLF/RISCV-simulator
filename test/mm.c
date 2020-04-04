#include "lib.h"
#define SIZE 3
int A[SIZE][SIZE],B[SIZE][SIZE],C[SIZE][SIZE];

void MatMul(){
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            C[i][j] = 0;
            for(int k = 0 ;k < SIZE; ++k){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void Init(){
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            A[i][j] = i+j;
            B[i][j] = i-j;
        }
    }
}

void Print(){
  Print_s("Matrix A:\n");
  for(int i = 0 ; i < SIZE ; ++i){
      for(int j = 0 ;j < SIZE;++j){
          Print_d(A[i][j]);
      }
      Print_s("\n");
  }
  Print_s("Matrix B:\n");
  for(int i = 0 ; i < SIZE ; ++i){
      for(int j = 0 ;j < SIZE;++j){
          Print_d(B[i][j]);
      }
      Print_s("\n");
  }
  Print_s("Matrix C:\n");
  for(int i = 0 ; i < SIZE ; ++i){
      for(int j = 0 ;j < SIZE;++j){
          Print_d(C[i][j]);
      }
      Print_s("\n");
  }
}

int main(){
    Init();
    Print();
    MatMul();
    Print();
    Exit();
}
