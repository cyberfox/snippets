#include "Regex.h"

Regex::Regex(const string &regex, bool silent) : m_silent(silent) {
  int rval = regcomp(&m_preg, regex.c_str(), REG_EXTENDED | PCRE_DOTALL);
  if(rval != 0) {
    handle_regex_error(rval);
  }
}

bool Regex::match(const string &against) {
  int rval = regexec(&m_preg, against.c_str(), MAX_MATCH, m_matches, 0);

  if(rval == REG_NOMATCH) {
    //  Clear the subexpressions on a match failure.
    for(int i=0; i<MAX_MATCH; i++) {
      m_subexps[i] = "";
    }
    return false;
  }

  if(rval != 0) {
    handle_regex_error(rval);
    return false;
  }

  for(int i=0; i<MAX_MATCH; i++) {
    regmatch_t this_match = m_matches[i];

    if(this_match.rm_so != -1) {
      m_subexps[i] = against.substr(this_match.rm_so, this_match.rm_eo-this_match.rm_so);
    } else {
      m_subexps[i] = "";
    }
  }

  return true;
}
