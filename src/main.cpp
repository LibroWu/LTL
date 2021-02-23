//
// Created by Libro on 2021/2/23.
//
#include <iostream>
#include "priority_queue.hpp"
#include <queue>
#include <ctime>
#include <cstdlib>

using namespace std;
int main(){
    srand(time(NULL));
    sjtu::priority_queue<int> que1,que2,que3;
    priority_queue<int> STD;
    int n = 100000;
    for (int i = 0; i < n; ++i) {
        int op,x;
        op=rand()%2;
        switch (op) {
            case 1:
                x=rand();
                que1.push(x);
                STD.push(x);
                break;
            case 0:
                x=rand();
                que2.push(x);
                STD.push(x);
                break;
        }
    }
    /*
    que1.show(true, nullptr, true);
    cout<<"division line\n";
    que2.show(true, nullptr, true);
    cout<<"division line\n";*/
    que1.merge(que2);
    que3=que1;
    //que1.show(true, nullptr, true);
    for (int i=0;i<n;++i){
        cout<<que3.top()<<' '<<abs(STD.top())<<'\n';
        if (que3.top()!=abs(STD.top())) return 1;
        que3.pop();
        STD.pop();
    }
    cout<<que2.empty();
}
