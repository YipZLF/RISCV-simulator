#include"lib.h"
#define SIZE 8
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
/*
void Init(){
    srand(1);
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }
}

void Print(){
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            printf("%d ",A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            printf("%d ",B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for(int i = 0 ; i < SIZE ; ++i){
        for(int j = 0 ;j < SIZE;++j){
            printf("%d ",C[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
*/
int main(){
    //Print();
    MatMul();
    Exit();
    //Print();
}
