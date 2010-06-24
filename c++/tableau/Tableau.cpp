#include "Tableau.h"
#include <math.h>

/** 
 * Compare two floats, by taking their bit-value and comparing how far apart they are.
 *
 * Algorithm taken from http://bit.ly/95BC1M.  It's not the optimal algorithm given, but it's
 * an easily comprehensible one.
 *
 * @param x - The first value to compare
 * @param y - The second value to compare
 * 
 * @return -1 if X < Y
 *          0 if X ~= Y
 *          1 if X > Y
 */
int Tableau::compareFloat(float x, float y) {
  if(x == y) return 0;

  if(sizeof(float) == sizeof(int)) {
    // Extract the two float values as integers (not casting, but
    // converting the raw bits to an integer).
    int delta = *(int*)&x - *(int*)&y;
    //  If they're within one adjacent float, we'll call them equal (fudge factor)
    if(abs(delta) <= 1) return 0;
  }

  if(x < y) return -1;
  return 1;
}

/** 
 * Do a binary search through a sorted array for a specific float value.
 * 
 * @param search - The value to search for
 * @param sorted - The array to search in
 * @param length - The length of the array provided
 * 
 * @return - true if the value (or a very close float) exists in the array, false otherwise.
 */
bool Tableau::find(float search, float sorted[], int length) {
  if(length == 0) return false;
  float midpoint = length/2;
  float size = midpoint;

  do {
    size /= 2;
    switch(compareFloat(search, sorted[(int)midpoint])) {
      case 0: return true;
      case -1: midpoint -= size;
        break;
      case 1: midpoint += size;
        break;
    }
  } while(round(size) > 0);
  return false;
}

/** 
 * Reverse a string in place.
 * 
 * @param forward - null terminated string to reverse
 * 
 * @return - The string passed in, reversed. Will return NULL if NULL was passed in.
 */
char *Tableau::reverse(char *forward) {
  if(forward == NULL) return forward;

  char *base = forward;
  char *step = forward;

  while(*step != '\0') step++;

  char ch;
  while(--step > base) {
    ch = *step;
    *step = *base;
    *base = ch;
    base++;
  }
  return forward;
}
