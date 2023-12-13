#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cassert>
using namespace std;

// 其实只需要在运行时考虑verbose. 其余的一律cerr.
/* const */
const string help_msg = "usage: turing [-v|--verbose] [-h|--help] <tm> <input>";
const string parse_err_msg = "syntax error";
const string input_err_msg = "illegal input string";

/* macro definition */
#define STRINGIFY(s)  #s
#define TOSTRING(s) STRINGIFY(s)
#define Assert(cond) \
({ if (!(cond)) { \
  cerr << parse_err_msg << endl; \
  cerr << " @ " << __FILE__ << ":" << TOSTRING(__LINE__) << "  \n"; \
  exit(1); \
}})


bool verbose = false;

struct transition {
  string _state, state_; // state
  string _sym, sym_; // tape symbol combinition
  string dir; // moving direction
};

void Split(const string &src, vector<string> &ret, const string& c);
void extract(const string &src, vector<string> &ret);
int parse_tmfile(string &filename);
bool check_input(string &input);
int strtoint(string &input);

/* turing machine definitions */
static set<string> Q;
static set<char> S, G;
static string q0;
static char B;
static set<string> F;
static int N;
static int tapenum;
static vector<transition> delta;

int main(int argc, char* argv[]){
  string file = "", input = "";
  /* parse args */
  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << help_msg << endl;
      return 0;
    }
    else if (arg == "--verbose" || arg == "-v") { verbose = true; }
    else if (file == "") { file = arg; }
    else if (input == "") { input = arg; }
    else {
      cerr << help_msg << endl;
      return 1;
    }
  }
  
  /* parse tm file */
  int ret = parse_tmfile(file);
  if (ret == -1) {
    cerr << "Unable to open file: " << file << endl;
    return 1;
  }

  cerr << "Q:\n";
  for (auto state : Q) {
    cerr << state << " ";
  }
  cerr << endl;

  cerr << "S:\n";
  for (auto state : S) {
    cerr << state << " ";
  }
  cerr << endl;

  cerr << "G:\n";
  for (auto state : G) {
    cerr << state << " ";
  }
  cerr << endl;

  cerr << "S:\n";
  cerr << q0 << endl;

  cerr << "B:\n";
  cerr << B << endl;

  cerr << "F:\n";
  for (auto state : F) {
    cerr << state << " ";
  }
  cerr << endl;

  cerr << "N:\n";
  cerr << tapenum << endl;

  for (auto t : delta) {
    cerr << t._state << " " << t._sym << " " << t.sym_ << " " << t.dir << " " << t.state_ << endl;
  }

  if (!check_input(input)) {
    cerr << input_err_msg << endl;
    return 1;
  }
}

int parse_tmfile(string &filename) {
  ifstream infile(filename);
  if (!infile.is_open()) return -1;
  string line;
  /* for each line in file */
  while (getline(infile, line)) {
    vector<string> part; part.clear();
    /* cut off comment */
    string::size_type end = 0;
    while (end < line.length() && line[end] != ';') end++;
    line = line.substr(0, end);
    /* empty line */
    if (line.length() == 0) { continue; }

    Split(line, part, " ");
    /* tm definition */
    if (line[0] == '#') {
      vector<string> elements; elements.clear();
      extract(part[2], elements);
      for (auto elem : elements) {
        bool is_char = elem.length() == 1;
        if (part[0] == "#Q") { Q.insert(elem); }
        else if (part[0] == "#S") { Assert(is_char); S.insert(elem[0]); }
        else if (part[0] == "#G") { Assert(is_char); G.insert(elem[0]); }
        else if (part[0] == "#q0") { q0 = elem; }
        else if (part[0] == "#B") { Assert(is_char); B = elem[0]; }
        else if (part[0] == "#F") { F.insert(elem); }
        else if (part[0] == "#N") { tapenum = strtoint(elem); }
        else { Assert(0); }
      }
    }
    else {
      transition t;
      Assert(part.size() == 5);
      t._state = part[0]; t.state_ = part[4];
      t._sym = part[1]; t.sym_ = part[2];
      t.dir = part[3];
      Assert(t._sym.length() == tapenum && t.sym_.length() == tapenum && t.dir.length() == tapenum);
      delta.push_back(t);
    }
  }

  /* do some check for the turing machine */
  // S \subseteq G
  Assert(B == '_');
  Assert(G.find('*') == G.end());
  Assert(S.find('*') == S.end());

  infile.close();
  return 0;
}

bool check_input(string &input) {
  for (int i = 0; i < input.size(); ++i) {
    if (S.find(input[i]) == S.end()) return false;
  }
  return true;
}

void Split(const string &src, vector<string> &ret, const string& c){
  string::size_type pos1, pos2;
  pos2 = src.find(c);
  pos1 = 0;
  while (string::npos != pos2) {
    ret.push_back(src.substr(pos1, pos2 - pos1));
    pos1 = pos2 + c.size();
    pos2 = src.find(c, pos1);
  }
  if (pos1 != src.length()) ret.push_back(src.substr(pos1));
}

void extract(const string &src, vector<string> &ret) {
  string ct = src;
  if (ct[0] == '{') ct = ct.substr(1, ct.length() - 2);
  Split(ct, ret, ",");
}

int strtoint(string &input) {
  int ans = 0;
  for (int i = 0; i < input.length(); ++i) {
    ans *= 10;
    ans += input[i] - '0';
  }
  return ans;
}