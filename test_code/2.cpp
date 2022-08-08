#include<iostream>
using namespace std;

extern char **environ;

int main(int argc, char *argv[]) {
  cout << "PATH table:" << endl;
  for (int i = 0;;i++){
    if (environ[i] != nullptr){
        cout << "env num." << i << " " << environ[i] << endl;
    }else{
      break;
    }
  }
  return 0;
}