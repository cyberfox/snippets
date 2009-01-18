#include <pcre.h>
#include <pcreposix.h>
#include <string>

using namespace std;

//  A max of 10 sub-expressions
#define MAX_MATCH 10

class Regex {
  private:
    bool m_silent;
    regex_t m_preg;
    regmatch_t m_matches[MAX_MATCH];  //  Storage for potential matches.
    string m_subexps[MAX_MATCH];

    void handle_regex_error(int error_value) {
        char errbuf[256];
        regerror(error_value, &m_preg, errbuf, sizeof(errbuf));
        if(!m_silent) printf("Error: \x07%s\n", errbuf);
    }

  public:
    Regex(const string &regex, bool silent=false);

    ~Regex() {
      regfree(&m_preg);
    }

    bool match(const string &against);
    string get_match(int subexp) {
      return m_subexps[subexp];
    }

    size_t get_start(int subexp=0) {
      return m_matches[subexp].rm_so;
    }

    size_t get_end(int subexp=0) {
      return m_matches[subexp].rm_eo;
    }

    string replace(const string &against, const string &newtext, int subexp = 0) {
      return against.substr(0, get_start(subexp)) + newtext + against.substr(get_end(subexp));
    }

    string replace_all(const string &against, const string &newtext, int subexp = 0) {
      string result(against);
      while(match(result)) {
        result.replace(get_start(subexp), get_end(subexp)-get_start(subexp), newtext);
        //        result = replace(result, newtext, subexp);
      }
      return result;
    }
};
