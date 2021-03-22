#include <iostream>
#include "map.hpp"
using std::cin;
using std::cout;
using sjtu::map;

int main(){
    map<int,int>::RBT rbt;
    int n;
    cin>>n;
    for (int i = 0; i < n; ++i) {
        int x,y;
        cin>>x>>y;
        rbt.insert(x,y);
        cout<<"*******\n";
        rbt.show();
        cout<<"*******\n";
    }
    cout.flush();
    rbt.SwapTwoRBNode(rbt.head,rbt.head->lch);
    rbt.show();
    rbt.SwapTwoRBNode(rbt.head,rbt.head->rch);
    rbt.show();
    cout.flush();
    /*
    for (int i = 0; i < n; ++i) {
        int x;
        cin>>x;
        cout<<rbt.get(x).first->value<<'\n';
    }*/
}