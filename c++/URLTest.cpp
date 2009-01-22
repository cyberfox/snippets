#include "URL.h"

bool fail(const string &msg, const string &s, const string &d) {
  cerr << "Failed to properly parse the " << msg << " portion (\"" << s << "\" vs. \"" << d << "\")!" << EOL;
  cerr << "---\n";
}

bool u_test(URL test, string p, string h, string po, string d="", string u="", string pa="", string i="") {
  if(test.get_protocol() != p) { test.dump(); return fail("protocol", p, test.get_protocol()); }
  if(test.get_host() != h) { test.dump(); return fail("host", h, test.get_host()); }
  if(test.get_port() != po) { test.dump(); return fail("port", po, test.get_port()); }
  if(test.get_user() != u) { test.dump(); return fail("user", u, test.get_user()); }
  if(test.get_params() != pa) { test.dump(); return fail("params", pa, test.get_params()); }
  if(test.get_destination() != d) { test.dump(); return fail("destination/path", d, test.get_destination()); }
  if(test.get_index() != i) { test.dump(); return fail("index/hash", i, test.get_index()); }
}

int main(int argc, char **argv)
{
  URL foo("http://me:pa$$word@example.com:8080/special/test.html?foo=bar&bar=baz#anchor");
  u_test(foo, "http", "example.com", "8080", "/special/test.html", "me:pa$$word", "foo=bar&bar=baz", "anchor");
  foo.load("mailto:mrs@best.com");
  u_test(foo, "mailto", "", "25", "mrs@best.com");
  foo.load("user:pass@best.com");
  u_test(foo, "http", "best.com", "80", "", "user:pass");
  foo.load("user:pass@best.com:9090");
  u_test(foo, "http", "best.com", "9090", "", "user:pass");
  foo.load("zarf");
  u_test(foo, "http", "zarf", "80");
  foo.load("zarf@intranet");
  u_test(foo, "http", "intranet", "80", "", "zarf");
  foo.load("http://www.vixen.com");
  u_test(foo, "http", "www.vixen.com", "80");
  foo.load("http://www.vixen.com/quickie.html");
  u_test(foo, "http", "www.vixen.com", "80", "/quickie.html");
  foo.load("http://www/quickie.html");
  u_test(foo, "http", "www", "80", "/quickie.html");
  foo.load("file:///home/mrs");
  u_test(foo, "file", "", "", "/home/mrs");
  foo.load("ftp://mrs@vixen.com/pub/00README.TXT");
  u_test(foo, "ftp", "vixen.com", "21", "/pub/00README.TXT", "mrs");
  foo.load("http://www.example.com:8080");
  u_test(foo, "http", "www.example.com", "8080");
  foo.load("/quickie.cgi");
  u_test(foo, "http", "", "80", "/quickie.cgi");

  foo.load("http://me:pa$$word@example.com:8080/special/test.html?foo=bar&bar=baz#anchor");
  foo.dump();

  return 0;
}
