#include <iostream>
#include <iomanip>
#include <assert.h>
#include "Tableau.h"

using namespace std;

#define EOL "\n"

void test_nearby_precision(Tableau &t) {
  float f = 2.0f;
  int x = *(int *)&f;
  x += 1;
  float g = *(float *)&x;
  x -= 2;
  float h = *(float *)&x;
  cout << EOL;
  cout << "Floats bracketing 2.0: [" << g << ", " << f << ", " << h << "]" << EOL;
  float close[] = {g, h};

  cout << "Finding 2.0 in an array of two floats bracketing it (one float step away on either side): " << t.find(2.0f, close, 2) << EOL;
  cout << "Matching a float two steps away from " << h << " should fail: " << t.find(h, close, 1) << EOL;
  cout << "Matching a float one step away from " << f << " should succeed: " << t.find(f, close, 1) << EOL;
}

void test_binary_search_float(Tableau &t) {
  cout << "Binary Search of a sorted float array\n";
  cout << "-------------------------------------\n";

  float empty[] = {};
  cout << boolalpha;
  cout << setprecision(10);
  cout << "Finding 0.1 in an empty array: " << t.find(0.1, empty, 0) << EOL;

  float single[] = {0.5f};
  cout << "Finding 0.5 in [0.5]: " << t.find(0.5f, single, 1) << EOL;
  cout << "Finding 0.6 in [0.5]: " << t.find(0.6f, single, 1) << EOL;

  float several[] = {1.0f, 5.5f, 8.9f, 100.5f};
  cout << "Finding 5.5 in [1.0, 5.5, 8.9, 100.5]: " << t.find(5.5f, several, 4) << EOL;
  cout << "Finding 6.5 in [1.0, 5.5, 8.9, 100.5]: " << t.find(6.5f, several, 4) << EOL;
  cout << "Finding 1.0 (first) in [1.0, 5.5, 8.9, 100.5]: " << t.find(1.0f, several, 4) << EOL;
  cout << "Finding 100.5 (last) in [1.0, 5.5, 8.9, 100.5]: " << t.find(100.5f, several, 4) << EOL;
  cout << "Finding 17.8 / 2.0 in [1.0, 5.5, 8.9, 100.5]: " << t.find(17.8f / 2.0f, several, 4) << EOL;

  float negatives[] = {-973412341.123f, -109.8f, -77.77f, -8.9f, -2.0f, 0.0f, 12.8f, 37.779f, 887112.88f};
  cout << "Finding -77.77 in [-973412341.123, -109.8, -77.77, -8.9, -2.0, 0.0, 12.8, 37.779, 887112.88]: " << t.find(-77.77f, negatives, 8) << EOL;
  cout << "Finding -108.9 in [-973412341.123, -109.8, -77.77, -8.9, -2.0, 0.0, 12.8, 37.779, 887112.88]: " << t.find(-108.9f, negatives, 6) << EOL;
  cout << "Finding 0.0 in [-973412341.123, -109.8, -77.77, -8.9, -2.0, 0.0, 12.8, 37.779, 887112.88]: " << t.find(0.0f, negatives, 6) << EOL;

  float difficult[] = { 0.0f, 0.33333333333f, 1.999999881f, 2.0f, 10000.0f, 10000.000977f};
  cout << "Finding 1/3 in [0.0, 0.33333333333, 1.999999881, 2.0, 10000.0, 10000.000977]: " << t.find(1.0f/3.0f, difficult, 5) << EOL;
  cout << "Finding 1/6 in [0.0, 0.33333333333, 1.999999881, 2.0, 10000.0, 10000.000977]: " << t.find(1.0f/6.0f, difficult, 5) << EOL;
}

char *empty = strdup("");
char *single = strdup("a");
char *two = strdup("ab");
char *three = strdup("abb");
char *palindrome = strdup("abba");
char *five = strdup("abba!");

void test_string_reversal(Tableau &t) {
  cout << EOL;
  cout << "String Reversal\n";
  cout << "---------------\n";
  assert(t.reverse(NULL) == NULL);
  cout << "Reversed NULL\n";
  assert(strcmp(t.reverse(empty), "")==0);
  cout << "Reversed empty string\n";
  assert(strcmp(t.reverse(single), "a")==0);
  cout << "Reversed a single character\n";
  assert(strcmp(t.reverse(two), "ba")==0);
  cout << "Reversed 'ab' to 'ba'\n";
  assert(strcmp(t.reverse(three), "bba")==0);
  cout << "Reversed 'abb' to 'bba'\n";
  assert(strcmp(t.reverse(palindrome), "abba")==0);
  cout << "Reversed a palindrome\n";
  assert(strcmp(t.reverse(five), "!abba")==0);
  cout << "Reversed five characters 'abba!' to '!abba'\n";
}

int main(char **argv, int argc) {
  if(sizeof(float) != sizeof(int)) {
    cout << "The float comparison code will not work for nearby floats; float and int are different sizes.\n";
  }

  Tableau t;
  test_binary_search_float(t);
  test_nearby_precision(t);
  test_string_reversal(t);
}
