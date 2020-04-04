#include "lib.h"

int Ackermann(int m,int n){
    while(m != 0){
        if(n == 0) n = 1;
        else n = Ackermann(m,n-1);
        m = m - 1;
    }
    return n+1;
}

int main(){
    int m=2,n=2,ans;
    ans = Ackermann(m,n);
    Print_d(ans);
    Exit();
}
