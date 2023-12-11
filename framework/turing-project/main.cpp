#include <iostream>
#include <fstream>
#include <string>
#include <set>
using namespace std;

// 其实只需要在运行时考虑verbose. 其余的一律cerr.
/* const */
const string help_msg = "usage: turing [-v|--verbose] [-h|--help] <tm> <input>";
const string parse_err_msg = "parse tmfile error! check the format. Good Luck!";
const string input_err_msg = "illegal input string";

bool verbose = false;

void Split(const string& s, vector<string>& v, const string& c);
int parse_tmfile(string &filename);
bool check_input(string &input);

/* turing machine definitions */
vector<string> Q;
set<char> S, G;
int q0; // index
char B;
set<int> F; // index
int N;

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
  else if (ret == 1) {
    cerr << parse_err_msg << endl;
    return 1;
  }
  else {;}
  /* check input */
  if (!check_input(input)) {
    cerr << input_err_msg << endl;
    return 1;
  }
}

int parse_tmfile(string &filename) {
  ifstream infile(filename);
  if (!infile.is_open()) {
    cerr << "Unable to open file: " << filename << endl;
    return -1;
  }
  string line;
  /* for each line in file */
  while (getline(infile, line)) {
    /* empty line */
    if (line.length() == 0) { continue; }
    /* comment */
    else if (line[0] == ';') { cerr << "comment: " << line << endl; }
    /* turing machine define */
    else if (line[0] == '#') {
      char type = line[1];
      string::size_type pos = 0;
      while (line[pos] != '=') { pos++; } // points to '='
      vector<string> elements; elements.clear();
      if (type == 'Q') {
        pos += 3;
        Split(line.substr(pos, line.size() - pos - 1), elements, ",");
        for (int i = 0; i < elements.size(); ++i) {
          cerr << elements[i] << endl;
        }
      }
      else if (type == 'S') {

      }
      else if (type == 'G') {

      }
      else if (type == 'q') {

      }
      else if (type == 'B') {

      }
      else if (type == 'F') {

      }
      else if (type == 'N') {

      }
      else {
        cerr << "invalid #type " << type << endl;
        return 1;
      }
    }
    /* transition functions */
    else {

    }
  }
  infile.close();
  return 0;
}

bool check_input(string &input) {
  return true;
}

void Split(const string& s, vector<string>& v, const string& c){
  string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while (string::npos != pos2) {
    v.push_back(s.substr(pos1, pos2 - pos1));
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if (pos1 != s.length()) v.push_back(s.substr(pos1));
}