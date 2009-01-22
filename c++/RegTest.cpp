#include <stdio.h>

#include "Regex.h"

int main(int argc, char **argv) {
  Regex foo("ar");
  if(foo.match("Zarf!")) {
    printf("Successfully matched 'ar' in 'Zarf!'\n");
    printf("Subexpression 0 == %s\n", foo.get_match(0).c_str());
  }

  string barTest = "Zarf is with you again!  This Zarf is even longer!";
  Regex bar("(Z)(ar)(f.*?!)");
  if(bar.match(barTest)) {
    printf("Successfully matched a more complex regex, including a non-greedy component.\n");
    printf("Last is: %s\n", bar.get_match(3).c_str());
    printf("Middle is: %s\n", bar.get_match(2).c_str());
    printf("First is: %s\n", bar.get_match(1).c_str());

    string replaced = bar.replace(barTest, "in", 2);
    printf("Substitution test: %s\n", replaced.c_str());
    replaced = bar.replace_all(barTest, "in", 2);
    printf("Substitution test: %s\n", replaced.c_str());
  }

  string tryingTest = "Zarf is with you again\nThen what?  ($linkpref{join.php?invite=913241})\r\n<a href=\"$linkpref{/friends.php?uid=213}\">";
  Regex link_prefix("\\$linkpref{(.*?)}", true);
  while(link_prefix.match(tryingTest)) {
    tryingTest = link_prefix.replace(tryingTest, "&next=" + link_prefix.get_match(1));
  }
  printf("Trying difficult test: %s\n", tryingTest.c_str());
}
