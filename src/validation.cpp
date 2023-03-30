#include<iostream>
#include<fstream>
using namespace std;

void validation(string selectedCase, string fileName){
    ifstream File1;
    ifstream File2;
    string line1, line2;
    File1.open("./sorted_ans/" + selectedCase + "/" + fileName);
    File2.open("./result/" + selectedCase + "/" + fileName);
    int match = 1;
    int lineCount=0;
    if(File1.is_open() && File2.is_open()){
        while(getline(File1,line1)){
            getline(File2,line2);
            lineCount++;
            if(line1 != line2){
                cout << "does not match" << endl;
                cout << "first line not match: " << lineCount << endl;
                match = 0;
                break;
            }
        }
        if(match) cout <<  fileName << " match!" << endl;
        File1.close();
        File2.close();
    }
    else{
        cout << "fail to open sorted_ans: " << fileName << endl;
    }
}