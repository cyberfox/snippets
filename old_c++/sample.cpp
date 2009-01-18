#include "DB.h"
#include "SimpleLog.h"
#include "Regex.h"

int main(int argc, char **argv) {
  Regex matcher("[Ff]o+");

  cout << "Is 'foo' a match: " << (matcher.match("foo") ? "true" : "false") << EOL;
  cout << "Is 'bar' a match: " << (matcher.match("bar") ? "true" : "false") << EOL;
  matcher.match("MyFooooooooolishness");
  cout << "what part of 'MyFooooooooolishness' matches: " << matcher.get_match(0) << EOL;

  cout << EOL << "This is just a sample app to be linked with the libraries." << EOL;
}
