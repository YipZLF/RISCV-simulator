#include"lib.h"
#define SIZE 10
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

void Init(){
    for(int i = 0 ; i < SIZE ; ++i){
        arr[i] = SIZE-i;
    }
}

void Print(){
    for(int i = 0 ; i < SIZE ; ++i){
        Print_d(arr[i]);
    }
    Print_s("\n");
}

int main(){
    Init();
    Print();
    QuickSort(arr,0,SIZE-1);
    Print();
    Exit();
}
