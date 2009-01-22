#ifndef URL_H
#define URL_H

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

#define EOL "\n"

class URL {
 private:
  //  Examples are using: http://me:pa$$word@example.com:8080/special/test.html?foo=bar&bar=baz#anchor
  string m_protocol;    //  e.g. http
  string m_user;        //  e.g. me:pa$$word
  string m_host;        //  e.g. example.com
  string m_port;        //  e.g. 8080
  string m_destination; //  e.g. /special/test.html
  string m_params;      //  e.g. foo=bar&bar=baz
  string m_index;       //  e.g. anchor
  bool m_relative;      //  Relative or absolute URL? (e.g. false, above)

  string check_anchor(const string &url) {
    size_t index_i = url.find('#');
    if(index_i != -1) {
      m_index = url.substr(index_i+1);
      return url.substr(0, index_i);
    }
    return url;
  }

  string check_params(const string &url) {
    size_t index_i = url.find('?');
    if(index_i != -1) {
      m_params = url.substr(index_i+1);
      return url.substr(0, index_i);
    }
    return url;
  }

  string strip_user(const string &url) {
    size_t user_i = url.find('@');
    m_user = url.substr(0, user_i);
    return url.substr(user_i+1);
  }

  void set_port() {
    if(m_protocol == "http") { m_port = "80"; } else
    if(m_protocol == "https") { m_port = "443"; } else
    if(m_protocol == "ftp") { m_port = "21"; } else
    if(m_protocol == "mailto") { m_port = "25"; } else
    if(m_protocol == "news") { m_port = "119"; } else
    if(m_protocol == "telnet") { m_port = "23"; } else
    if(m_protocol == "file") { m_port = ""; }
  }

  void set_protocol(const string &proto) {
    m_protocol = proto;
    set_port();
  }

  void parse(const string &url) {
    string processed_url = url;
    //  Indexes into the url string.

    //  First we trim off the hash/index into the document, as it could
    //  be almost anything, and could confuse later parsing.
    processed_url = check_anchor(processed_url);
    processed_url = check_params(processed_url);
    if(processed_url.find_first_of(":./@") == -1) {
      m_host = processed_url;
      return;
    }

    int user_i = processed_url.find('@');
    int protocol_i = processed_url.find(':');
    if(user_i != -1 && (protocol_i == -1 || user_i < protocol_i)) {
      processed_url = strip_user(processed_url);
    } else {
      if(protocol_i != -1) {
        //  We know that there's a : in the url, and we know it's before an '@'.
        //  It could be user:pass@host, it could be protocol:user:pass@host, and
        //  it could be protocol:host.
        if(user_i == -1) {
          set_protocol(processed_url.substr(0, protocol_i));
          processed_url = processed_url.substr(protocol_i+1);
          if(processed_url.substr(0, 2) == "//") processed_url = processed_url.substr(2);
          if(m_protocol == "file" || m_protocol == "news" ||
             m_protocol == "mailto" || m_protocol == "telnet") {
            //  Strip out '//' if there happen to be any.
            m_destination = processed_url;
            return;
          }
        } else {
          //  user:pass@host, protocol:user@host, or protocol:user:pass@host?
          //  (Note that the first two are usually nearly impossible to
          //  determine the difference between without making unpleasant
          //  assumptions.)
          string protocol = processed_url.substr(0, protocol_i);
          //  If it's followed by "//" then it's a protocol, because
          //  '//' would be quoted in a password.
          if(processed_url.substr(protocol_i+1, 2) == "//") {
            set_protocol(protocol);
            processed_url = processed_url.substr(protocol_i + 3);
            //  If it's a file, everything else is the destination (no host, etc.)
            if(m_protocol == "file" || m_protocol == "news" ||
               m_protocol == "mailto" || m_protocol == "telnet") {
              m_destination = processed_url;
              return;
            }
            //  Otherwise, it had a user info block, so pull that out next.
            processed_url = strip_user(processed_url);
          } else {
            //  It's virtually impossible to tell the difference between
            //  mailto:user@host and user:pass@host, unfortunately.  This
            //  is where we make assumptions, such as the username being
            //  used will never be 'news' or 'mailto'.
            if(protocol == "news" || protocol == "mailto") {
              set_protocol(protocol);
              m_destination = processed_url.substr(protocol_i + 1);
              return;
            } else {
              //  It must be user:pass@host, so treat it as such.
              //  Otherwise, it had a user info block, so pull that out next.
              processed_url = strip_user(processed_url);
            }
          }
        }
      }
    }

    //  We should now be pointing at the 'host' part, optionally followed by a path or port.
    int port_i = processed_url.find(':');
    int dest_i = processed_url.find('/');

    //  We have a port?
    if(port_i != -1) {
      //  And no destination?  (http://www.example.com:8080)
      if(dest_i == -1) {
        m_port = processed_url.substr(port_i+1);
        m_host = processed_url.substr(0, port_i);
        return;
      } else {
        m_port = processed_url.substr(port_i+1, (dest_i-(port_i+1)));
        m_host = processed_url.substr(0, port_i);
        m_destination = processed_url.substr(dest_i);
        return;
      }
    }

    if(dest_i != -1) {
      m_host = processed_url.substr(0, dest_i);
      m_destination = processed_url.substr(dest_i);
      return;
    }
    m_host = processed_url;
  }

 public:
  URL(const string &url) : m_protocol("http"), m_port("80"), m_relative(false) {
    load(url);
  }

  void load(const string &url) {
    set_protocol("http");
    m_relative = false;
    m_user = "";
    m_host = "";
    m_params = "";
    m_destination = "";
    m_index = "";

    parse(url);
  }

  void dump() {
    cerr << "Protocol:\t" << get_protocol() << EOL;
    cerr << "User:\t" << get_user() << EOL;
    cerr << "Host:\t" << get_host() << EOL;
    cerr << "Port:\t" << get_port() << EOL;
    cerr << "Params:\t" << get_params() << EOL;
    cerr << "Dest:\t" << get_destination() << EOL;
    cerr << "Index:\t" << get_index() << EOL;
  }

  string get_protocol() { return m_protocol; }
  string get_user() { return m_user; }
  string get_host() { return m_host; }
  string get_port() { return m_port; }
  int get_port_number() { return atol(m_port.c_str()); }
  string get_params() { return m_params; }
  string get_destination() { return m_destination; }
  string get_index() { return m_index; }
};

#endif //  URL_H
