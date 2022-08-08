#include<iostream>
#include<string>
#include<stdlib.h>  //for getenv()
using namespace std;

//export TEMP_PATH=xxx
int main(){
  string temp = getenv("TEMP_PATH");
  cout << "temp:" << temp <<endl;
  return 0;
}
