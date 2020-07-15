#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include "index.h"

using namespace std;

int NodeSize = 60;

//class Index;

//class TreeNode{
//    bool isLeaf;
//    int *key;
//    int *value;
//    int size;
//    TreeNode** ptr;
//    TreeNode* parentNode;
//    friend class Index;
//public:
//    TreeNode();
//};
//
//class Index{
//public:
//    TreeNode *root;
//    Index(int num_rows, vector<int> keys, vector<int> values);
//    void Insert(int key, int value);
//    void InsertInternal(int, int, TreeNode*, TreeNode*);
//    void key_query(vector<int> query_keys);
//    int search(int key);
//    void range_query(vector<pair<int, int>> query_pairs);
//    TreeNode *search_node(int key);
//    int findMax(TreeNode*, TreeNode*, int, int);
//    void clear_index();
//    void clear(TreeNode*);
//    void display(TreeNode*);
//};

TreeNode::TreeNode(){
    key = new int[NodeSize];
    value = new int[NodeSize];
    ptr = new TreeNode*[NodeSize+1];
}

Index::Index(int num_rows, vector<int> keys, vector<int> values){
    root = NULL;
    for(int i=0; i<num_rows; i++){
        Insert(keys[i], values[i]);
    }
}

void Index::Insert(int key, int value){
    if(root == NULL){
        //create root
        root = new TreeNode;
        root->key[0] = key;
        root->value[0] = value;
        root->isLeaf = true;
        root->size = 1;
        root->parentNode = NULL;
        //finish inserting root
    }
    else{
        TreeNode* forward = root;
        TreeNode* parent = NULL;
        //B plus tree insert from the bottom, which is the leaf node
        while(forward->isLeaf == false){
            parent = forward;
            for(int i=0; i<forward->size; i++){
                if(key < forward->key[i]){
                    //if new key is smaller than current key, we have to go to it's ptr
                    forward = forward->ptr[i];
                    break;
                }
                if(i == forward->size-1){
                    //if new key is bigger than every key in this node, we have to go to ptr at the most right
                    forward = forward->ptr[i+1];
                    break;
                }
            }
        }
        //now, forward is at leaf node
        if(forward->size < NodeSize){
            //if this node is not full yet
            int i=0;
            while(key > forward->key[i] && i < forward->size){
                i++;
            }
            //make all keys bigger than new key move back a block, so that the new key could have space
            for(int j=forward->size; j>i; j--){
                forward->key[j] = forward->key[j-1];
                forward->value[j] = forward->value[j-1];
            }
            forward->key[i] = key;
            forward->value[i] = value;
            forward->size++;
            //according to the movement, the secondly last ptr(is last ptr before) should now be last ptr
            //*****
            forward->ptr[forward->size] = forward->ptr[forward->size-1];
            forward->ptr[forward->size-1] = NULL;
            forward->parentNode = parent;
            //finish inserting node
        }
        else{
            //if this node is full, which means overflow, we have to split it
            //create new leaf
            TreeNode* NewLeaf = new TreeNode;
            //create a temp node and insert new key(used to split nodes later)
            int tempNode[NodeSize+1][2];
            for(int i=0; i<NodeSize; i++){
                tempNode[i][0] = forward->key[i];
                tempNode[i][1] = forward->value[i];
            }
            int i=0;
            while(key > tempNode[i][0] && i < NodeSize){
                i++;
            }
            //make all keys bigger than new key move back a block, so that the new key could have space
            for(int j = NodeSize; j>i; j--){
                tempNode[j][0] = tempNode[j-1][0];
                tempNode[j][1] = tempNode[j-1][1];
            }
            tempNode[i][0] = key;
            tempNode[i][1] = value;
            NewLeaf->isLeaf = true;
            //start spliting
            forward->size = (NodeSize+1)/2;
            NewLeaf->size = (NodeSize+1) - ((NodeSize+1)/2);
            //now forward node should be pointing to NewLeaf
            forward->ptr[forward->size] = NewLeaf;
            //NewLeaf have to point to the leaf node that forward node point to
            NewLeaf->ptr[NewLeaf->size] = forward->ptr[NodeSize];
            forward->ptr[NodeSize] = NULL;
            //giving keys and values to new leaf nodes
            for(int i=0; i<forward->size; i++){
                forward->key[i] = tempNode[i][0];
                forward->value[i] = tempNode[i][1];
            }
            for(int i=0, j=forward->size; i<NewLeaf->size; i++, j++){
                NewLeaf->key[i] = tempNode[j][0];
                NewLeaf->value[i] = tempNode[j][1];
            }
            //deal with parent problem
            if(forward == root){
                //if forward is root, means we have two root now, which is not possible, so we have to create a new root
                TreeNode* NewRoot = new TreeNode;
                NewRoot->key[0] = NewLeaf->key[0];
                NewRoot->value[0] = NewLeaf->value[0];
                NewRoot->ptr[0] = forward;
                NewRoot->ptr[1] = NewLeaf;
                forward->parentNode = NewRoot;
                NewLeaf->parentNode = NewRoot;
                NewRoot->isLeaf = false;
                NewRoot->size = 1;
                root = NewRoot;
            }
            else{
                //forward is not root, so the new parent is internal node
                forward->parentNode = parent;
                NewLeaf->parentNode = parent;
                InsertInternal(NewLeaf->key[0], NewLeaf->value[0], parent, NewLeaf);
            }
        }
    }
}

void Index::InsertInternal(int NewKey, int NewValue, TreeNode* parent, TreeNode* child){
    if(parent->size < NodeSize){
        //parent node is not full, so we can easily insert node
        int i=0;
        while(NewKey > parent->key[i] && i < parent->size){
            i++;
        }
        //make space for new key, value and new pointer
        for(int j=parent->size; j>i; j--){
            parent->key[j] = parent->key[j-1];
            parent->value[j] = parent->value[j-1];
        }
        for(int j=parent->size+1; j>i+1; j--){
            parent->ptr[j] = parent->ptr[j-1];
        }
        parent->key[i] = NewKey;
        parent->value[i] = NewValue;
        parent->size++;
        parent->ptr[i+1] = child;
        child->parentNode=parent;
        //finish inserting
    }
    else{
        //parent node is full, overflow again
        //create new internal node
        TreeNode* newInternal = new TreeNode;
        //crete temp internal node
        int tempKey[NodeSize+1];
        int tempValue[NodeSize+1];
        TreeNode* tempPtr[NodeSize+2];
        //copy parent to temp
        for(int i=0; i<parent->size; i++){
            tempKey[i] = parent->key[i];
            tempValue[i] = parent->value[i];
        }
        for(int i=0; i<parent->size+1; i++){
            tempPtr[i] = parent->ptr[i];
        }
        int i=0;
        //make space for new key, value and pointer
        while(NewKey > tempKey[i] && i < parent->size){
            i++;
        }
        for(int j=NodeSize; j>i; j--){
            tempKey[j] = tempKey[j-1];
            tempValue[j] = tempValue[j-1];
        }
        tempKey[i] = NewKey;
        tempValue[i] = NewValue;
        for(int j=NodeSize+1; j>i+1; j--){
            tempPtr[j] = tempPtr[j-1];
        }
        tempPtr[i+1] = child;
        child->parentNode=tempPtr[i+1];
        
        newInternal->isLeaf = false;
        //start splitting
        parent->size = (NodeSize+1)/2;
        newInternal->size = NodeSize - ((NodeSize+1)/2);
        //give elements and pointers to new node
        int j;
        for(i=0, j=parent->size+1; i<newInternal->size; i++, j++){
            newInternal->key[i] = tempKey[j];
            newInternal->value[i] = tempValue[j];
        }
        for(i=0, j=parent->size+1; i<newInternal->size+1; i++, j++){
            newInternal->ptr[i] = tempPtr[j];
            tempPtr[j]->parentNode=newInternal;
        }
        for(i=0; i<parent->size+1; i++){
            parent->ptr[i] = tempPtr[i];
            tempPtr[i]->parentNode=parent;
        }
        for(i=0; i<parent->size; i++){
            parent->key[i] = tempKey[i];
            parent->value[i] = tempValue[i];
        }
        if(parent == root){
            //if parent is root, means we have two root now, which is not possible, so we have to create a new root
            TreeNode* NewRoot = new TreeNode;
            NewRoot->key[0] = tempKey[parent->size];
            NewRoot->value[0] = tempValue[parent->size];
            NewRoot->ptr[0] = parent;
            NewRoot->ptr[1] = newInternal;
            parent->parentNode = NewRoot;
            newInternal->parentNode = NewRoot;
            NewRoot->isLeaf = false;
            NewRoot->size = 1;
            root = NewRoot;
        }
        else{
            newInternal->parentNode = parent->parentNode;
            //do recursion until b plus tree won't overflow
            InsertInternal(tempKey[parent->size], tempValue[parent->size], parent->parentNode, newInternal);
        }
    }
}

void Index::key_query(vector<int> query_keys){
    vector<int> value;
    for(int i=0; i<query_keys.size(); i++){
        int ans = search(query_keys[i]);
        value.push_back(ans);
    }
    ofstream File("key_query_out.txt");
    for(int i=0; i<value.size(); i++){
        File << value[i];
        File << "\n";
    }
    File.close();
}

int Index::search(int key){
    if(root == NULL){
        return -1;
    }
    else{
        TreeNode* forward = root;
        while(forward->isLeaf == false){
            for(int i=0; i<forward->size; i++){
                if(key < forward->key[i]){
                    forward = forward->ptr[i];
                    break;
                }
                if(i == forward->size-1){
                    forward = forward->ptr[i+1];
                    break;
                }
            }
        }
        for(int i=0; i<forward->size; i++){
            if(key == forward->key[i]){
                return forward->value[i];
            }
        }
        return -1;
    }
}

void Index::range_query(vector<pair<int, int> > query_pairs){
    vector<int> value;
    for(int i=0; i<query_pairs.size(); i++){
        int first = query_pairs[i].first;
        int second = query_pairs[i].second;
        TreeNode* start = search_node(first);
        TreeNode* end = search_node(second);
        int ans = findMax(start, end, first, second);
        value.push_back(ans);
    }
    ofstream File("range_query_out.txt");
    for(int i=0; i<value.size(); i++){
        File << value[i];
        File << "\n";
    }
    File.close();
}

TreeNode* Index::search_node(int key){
    TreeNode* forward = root;
    while(forward->isLeaf == false){
        for(int i=0; i<forward->size; i++){
            if(key < forward->key[i]){
                forward = forward->ptr[i];
                break;
            }
            if(i == forward->size-1){
                forward = forward->ptr[i+1];
                break;
            }
        }
    }
    return forward;
}

int Index::findMax(TreeNode *start, TreeNode *end, int firstkey, int secondkey){
    int max = 0;
    vector<int> ans;
    if(start == end){
        if(firstkey < start->key[0] && secondkey < start->key[0]){
            return -1;
        }
        if(firstkey > start->key[start->size-1] && secondkey > start->key[start->size-1]){
            return -1;
        }
        for(int i=0; i<start->size-1; i++){
            if(firstkey > start->key[i] && secondkey < end->key[i+1]){
                return -1;
            }
        }
        for(int i=0; i<start->size; i++){
            if(firstkey < start->key[i] && secondkey > end->key[i]){
                ans.push_back(start->value[i]);
            }
        }
    }
    else{
        while(start != end){
            for(int i=0; i<start->size; i++){
                if(firstkey < start->key[i] || firstkey == start->key[i]){
                    ans.push_back(start->value[i]);
                }
            }
            start = start->ptr[start->size];
        }
        for(int i=0; i<end->size; i++){
            if(secondkey > end->key[i] || secondkey == end->key[i]){
                ans.push_back(end->value[i]);
            }
        }
    }
    for(int i=0; i<ans.size(); i++){
        if(ans[i] > max){
            max = ans[i];
        }
    }
    return max;
}

void Index::clear_index(){
    clear(root);
}

void Index::clear(TreeNode* forward){
    if(forward->isLeaf != true){
        for(int i=0; i<forward->size+1; i++){
            clear(forward->ptr[i]);
        }
    }
    delete[] forward->key;
    delete[] forward->value;
    delete[] forward->ptr;
    delete forward;
}

void Index::display(TreeNode* cursor)
{
    //depth first display
    if(cursor!=NULL)
    {
        for(int i = 0; i < cursor->size; i++)
        {
            cout<<cursor->value[i]<<" ";
        }
        cout<<"\n";
        if(cursor->isLeaf != true)
        {
            for(int i = 0; i < cursor->size+1; i++)
            {
                display(cursor->ptr[i]);
            }
        }
    }
}
