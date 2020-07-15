//
//  main.cpp
//  DBhw3
//
//  Created by 蕭楚澔 on 2020/6/1.
//  Copyright © 2020 Bob. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <pthread.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

class variable{
public:
    int value;
    //different from pdf, using semaphore to implement 2PL
    sem_t read_write_lock;
};

class single_job{
public:
    int write_var;
    vector<pair<bool, int>> read_var;
    vector<pair<bool, int>> num;
};

sem_t semaphore;
mutex phase_lock;
variable var[100];
int thread_size;

void* exec(void* args){
    sem_wait(&semaphore);
    //After getting the semaphore, access the job list and perform it. You will implement 2PL in this step.
    single_job *job = (single_job *)args;
    vector<pair<bool, int>> read = job->read_var;
    vector<pair<bool, int>> number = job->num;
    int write = job->write_var;

//-----growing phase starts
    //var[write] takes thread_size semaphore, if cannot take, wait
    for(int j=0; j<thread_size; j++){
        sem_wait(&var[write].read_write_lock);
    }

    //var[read[j]] takes one semaphore, if cannot take, wait
    //if read variable is the same as write variable or is already been read, just ignore
    vector<int> is_reading;
    for(int j=0; j<read.size(); j++){
        if(write != read[j].second && find(is_reading.begin(), is_reading.end(), read[j].second) == is_reading.end()){
            is_reading.push_back(read[j].second);
            sem_wait(&var[read[j].second].read_write_lock);
        }
    }
    //now, growing phase is finished
    phase_lock.unlock();
//-----growing phase ends

//-----variable operation starts
    int ans = 0;
    for(int j=0; j<read.size(); j++){
        if(read[j].first == true){
            ans+=var[read[j].second].value;
        }
        else{
            ans-=var[read[j].second].value;
        }
    }
//-----variable operation ends

//-----number operation starts
    for(int j=0; j<number.size(); j++){
        if(number[j].first == true){
            ans+=number[j].second;
        }
        else{
            ans-=number[j].second;
        }
    }
    var[write].value = ans;
//-----number operation ends

//-----shrinking phase starts
    //give back semaphore
    for(int j=0; j<thread_size; j++){
        sem_post(&var[write].read_write_lock);
    }

    vector<int> is_read;
    for(int j=0; j<read.size(); j++){
        if(write != read[j].second && find(is_read.begin(), is_read.end(), read[j].second) == is_read.end()){
            is_read.push_back(read[j].second);
            sem_post(&var[read[j].second].read_write_lock);
        }
    }
//-----shrinking phase ends
    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    //Read the required thread number from argv, and create the threads. Make these thread wait the “job semaphore”.
    stringstream ss(argv[1]);
    ss>>thread_size;
    pthread_t thread[thread_size];

    //Read the variable number and initial values.
    int N;
    cin>>N;
    for(int i=0; i<N; i++){
        int v;
        cin>>v;
        var[i].value = v;
    }
    
    //Read and parse the equation, pack this equation to a single job (defined by yourself, you can use struct or class to pack them).
    string s;
	int k=-1;
    vector<single_job> job_list;
    sem_init(&semaphore, 0, 0);
    for(int i=0; i<100; i++){
        sem_init(&var[i].read_write_lock, 0, thread_size);
    }
    int count = 0;
    bool flag = false;
    while(getline(cin, s)){
		k++;
        single_job job;
        bool isLeft = true;
        bool isNum = true;
        bool greaterThanZero = true;
        int temp=0;
        for(int i=0; i<s.size(); i++){
            if(isLeft == true){
                if(s[i] == '='){
                    job.write_var = temp;
                    temp = 0;
                    isLeft = false;
                }
                else if(s[i] >= '0' && s[i] <= '9'){
                    temp *= 10;
                    temp += (int)s[i] - (int)'0';
                }
            }
            else{
                pair<bool, int> p;
                if(s[i] == '$'){
                    isNum = false;
                }
                else if(i == s.size()-1 && isNum == true){
                    temp *= 10;
                    temp += (int)s[i] - (int)'0';
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.num.push_back(p);
                }
                else if(i == s.size()-1 && isNum == false){
                    temp *= 10;
                    temp += (int)s[i] - (int)'0';
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.read_var.push_back(p);
                }
                else if(s[i] == '+' && isNum == true){
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.num.push_back(p);
                    greaterThanZero = true;
                    temp = 0;
                }
                else if(s[i] == '+' && isNum == false){
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.read_var.push_back(p);
                    greaterThanZero = true;
                    temp = 0;
                    isNum = true;
                }
                else if(s[i] == '-' && isNum == true){
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.num.push_back(p);
                    greaterThanZero = false;
                    temp = 0;
                }
                else if(s[i] == '-' && isNum == false){
                    p.first = greaterThanZero;
                    p.second = temp;
                    job.read_var.push_back(p);
                    greaterThanZero = false;
                    temp = 0;
                    isNum = true;
                }
                else if(s[i] >= '0' && s[i] <= '9'){
                    temp *= 10;
                    temp += (int)s[i]-(int)'0';
                }
            }
        }
        //Put this job into a job list (also defined by yourself, you can use vector or other data type to maintain it).
		if(flag == true){
            job_list.push_back(job);
            //trigger a “job semaphore”.
            sem_post(&semaphore);
        }
        else{
            flag = true;
        }
    }
    count = 0;
    for(int i=0; i<job_list.size(); i++){
        //One of your thread will get the job semaphore.
        phase_lock.lock();
        pthread_create(&thread[count], NULL, exec, &job_list[i]);
        if(count+1 == thread_size){
            count == 0;
        }
        else{
            count++;
        }
    }
    
    //At the end of your main program, after parsing the input data, wait the finished semaphore many times, so that we could know whether all of these jobs have finished or not.
    for(int i=0; i<thread_size; i++){
        pthread_join(thread[i], NULL);
    }
    
    //Write the result variable values to the output file.
    string file_name;
    stringstream sss(argv[2]);
    sss>>file_name;
    ofstream myfile(file_name);
    if(myfile.is_open()){
        for(int i=0; i<N; i++){
            myfile<<'$';
            myfile<<i;
            myfile<<" = ";
            myfile<<var[i].value<<endl;
        }
        myfile.close();
    }
    else{
        cout<<"Unable to open file";
    }
    return 0;
}

