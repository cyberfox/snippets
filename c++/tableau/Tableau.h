#ifndef TABLEAU_H
#define TABLEAU_H

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

class Tableau {
 private:
  int compareFloat(float x, float y);

 public:
  bool find(float search, float sorted[], int length);

  char *reverse(char *forward);
};

#endif // TABLEAU_H
