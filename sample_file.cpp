#include <iostream>
#include <fstream>
using namespace std;
int main(){
    ofstream file("sample1.txt"); 
    ofstream file1("sample2.txt");

    if(file.is_open()){
        file << "This is a sample text" << endl;
        cout << "File is created sucessfully" << endl;
    }
    if(file1.is_open()){

        file1 << "Second file" << endl;
        cout <<  "File2 created successfully" << endl;
    }
    else{
        cout << "file not created" << endl;
    }

    file.close(); 
    file1.close();

    return 0;
}