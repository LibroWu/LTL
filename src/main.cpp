#include <iostream>
#include "map.hpp"
using std::cin;
using std::cout;
using sjtu::map;
using std::endl;

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
    for (int i = 0; i < n; ++i) {
        int x;
        cin>>x;
        rbt.Delete(x);
        cout<<"*******\n";
        rbt.show();
        cout<<"*******\n";
    }
    /*
    for (int i = 0; i < n; ++i) {
        int x;
        cin>>x;
        cout<<rbt.get(x).first->value<<endl;
    }*/
}