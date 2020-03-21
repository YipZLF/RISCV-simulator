#include "lib.h"
#define SIZE 20
int arr[SIZE];
void QuickSort(int *arr, int s, int e){
    if(s<e){
        int key = arr[s];
        int l = s, r = e;
        while(l<r){
            while( l<r && arr[r] >= key) r--;
            if(l<r) arr[l++] = arr[r];
            while( l<r && arr[l] <= key) l++;
            if(l<r) arr[r--] = arr[l];
        }
        arr[l] = key;
        QuickSort(arr,s,l-1);
        QuickSort(arr,l+1,e);
    }
}
/*
void Init(){
    srand(1);
    for(int i = 0 ; i < SIZE ; ++i){
        arr[i] = rand() % SIZE;
    }
}
*/

int main(){
    //Init();
    QuickSort(arr,0,SIZE-1);
    Exit();
}
