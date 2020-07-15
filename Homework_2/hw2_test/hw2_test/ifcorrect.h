#ifndef ifcorrect_h
#define ifcorrect_h

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void ifcorrect(){
    fstream out,ans;
    vector<int> key_query_out,key_query_ans,range_query_out,range_query_ans;
    out.open("key_query_out.txt",ios::in);
    ans.open("key_query_ans.txt",ios::in);
    string line;
    while(getline(out,line,'\n')){
        key_query_out.push_back(atoi(line.c_str()));
    };
    while(getline(ans,line,'\n')){
        key_query_ans.push_back(atoi(line.c_str()));
    };
    bool flag=1;
    for(int i=0;i<key_query_out.size();i++){
        if(key_query_ans[i]!=key_query_out[i]){
            cout<<"ERROT!! The key query "<<i<<" line is not equal"<<endl;
            flag=0;
        }
    }
    if(flag)
        cout<<"Congratulation!! Your key query answers are correct"<<endl;
    out.close();
    ans.close();


    out.open("range_query_out.txt",ios::in);
    ans.open("range_query_ans.txt",ios::in);
    while(getline(out,line,'\n')){
        range_query_out.push_back(atoi(line.c_str()));
    };
    while(getline(ans,line,'\n')){
        range_query_ans.push_back(atoi(line.c_str()));
    };
    for(int i=0;i<range_query_out.size();i++){
        if(range_query_ans[i]!=range_query_out[i]){
            cout<<"ERROT!! The range query "<<i<<" line is not equal"<<endl;
            flag=0;
        }
    }
    if(flag)
        cout<<"Congratulation!! Your range query answers are correct"<<endl;
    out.close();
    ans.close();
}
#endif
