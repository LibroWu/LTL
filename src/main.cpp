#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "deque.hpp"

using namespace std;

void print(vector<int>& v,sjtu::deque<int> & my){
    for (int i=0;i<v.size();++i) cout<<v[i]<<' ';
    cout<<'\n';
    while (!my.empty()){
        cout<<my.front()<<' ';
        my.pop_front();
    }
    cout<<'\n';
}

int main(){
    sjtu::deque<int> my;
    vector<int> v;
    v.clear();
    srand(0);
    int n=10000;
    for (int i=0;i<n;++i) {
        int op=rand()%6,x;
        switch (op) {
            case 0:
                x=rand();
                v.push_back(x);
                my.push_back(x);
                break;
            case 1:
                x=rand();
                v.insert(v.begin(),x);
                my.push_front(x);
                break;
            case 2:
                if (!my.empty()) {
                    v.pop_back();
                    my.pop_back();
                }
                break;
            case 3:
                if (!my.empty()) {
                    v.erase(v.begin());
                    my.pop_front();
                }
                break;
            case 4:
                if (my.empty()) break;
                if (v.front()!=my.front()) {
                    cout<<"fail\n";
                    print(v,my);
                    return 1;
                }
                break;
            case 5:
                if (my.empty()) break;
                auto it=v.end();
                --it;
                if (*it!=my.back()) {
                    cout<<"fail\n";
                    print(v,my);
                    return 1;
                }
                break;
        }
    }
}