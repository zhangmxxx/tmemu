#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cassert>
using namespace std;

/* const */
const int MAX_LEN = (1 << 20);
const int DEF_NUM = 7;

/* macro definition */
#define STRINGIFY(s)  #s
#define TOSTRING(s) STRINGIFY(s)
#define Assert(cond) \
({ if (!(cond)) { \
  cerr << "syntax error" << endl; \
  if (verbose) { cerr << " @ " << __FILE__ << ":" << TOSTRING(__LINE__) << "  \n"; } \
  exit(1); \
}})
#define IDX(i) abs(i - (MAX_LEN >> 1))

/* struct */
struct transition {
  int _state, state_; // state
  string _sym, sym_; // tape symbol combinition
  string dir; // moving direction
};

void Split(const string &src, vector<string> &ret, const string& c);
void extract(const string &src, vector<string> &ret);
void check_state(string& state);
int parse_tmfile(string &filename);
void check_input(string &input);
void init_emulator(string &input);
int state_id(string &state);
int trans_id(int state, string &syms);
void run_emulator();
void print_tape(int id);
void print_tape_runtime(int id, int len);
int strtoint(string &input);
bool isnum(string &input);
int digits(int num);


/* emulator options */
bool verbose = false;

/* turing machine definitions */
static vector<string> Q;
static set<char> S, G;
static int q0;
static char B;
static set<int> F;
static int N;
static vector<transition> delta;
static bool defined[DEF_NUM];

/* runtime variables */
static vector<vector<char>> tapes;
static vector<int> cur;
bool accept = false;

int main(int argc, char* argv[]){
  string file = "", input = "";
  /* parse args */
  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout <<  "usage: turing [-v|--verbose] [-h|--help] <tm> <input>" << endl;
      return 0;
    }
    else if (arg == "--verbose" || arg == "-v") { verbose = true; }
    else if (file == "") { file = arg; }
    else if (input == "") { input = arg; }
    else {
      cerr <<  "usage: turing [-v|--verbose] [-h|--help] <tm> <input>" << endl;
      return 1;
    }
  }
  
  /* parse tm file */
  if (parse_tmfile(file) == -1) {
    cerr << "Unable to open file: " << file << endl;
    return 1;
  }

  /* check input */
  check_input(input);

  /* init emulator */
  init_emulator(input);

  /* run */
  run_emulator();
}

int parse_tmfile(string &filename) {
  ifstream infile(filename);
  if (!infile.is_open()) return -1;
  for (int i = 0; i < DEF_NUM; ++i) defined[i] = false;
  string line;
  while (getline(infile, line)) {
    string::size_type end = 0;
    while (end < line.length() && line[end] != ';') { end++; } end--;
    while (end >= 0 && line[end] == ' ') { end--; } end++;
    line = line.substr(0, end);
    /* empty line */
    if (line.length() == 0) { continue; }

    /* tm definition */
    if (line[0] == '#') {
      vector<string> elements; elements.clear();
      Assert(line.length() >= 6);
      if (line[1] == 'Q') {
        Assert(line.substr(0, 5) == "#Q = ");
        Assert(!defined[0]); defined[0] = true;
        Assert(line[5] == '{' && line.back() == '}');
        extract(line.substr(5), elements);
        for (auto elem : elements) { check_state(elem); Q.push_back(elem); }
      }
      else if (line[1] == 'S') {
        Assert(line.substr(0, 5) == "#S = ");
        Assert(!defined[1]); defined[1] = true;
        Assert(line[5] == '{' && line.back() == '}');
        extract(line.substr(5), elements);
        for (auto elem : elements) {
          Assert(elem.length() == 1);
          S.insert(elem[0]);
        }
      }
      else if (line[1] == 'G') {
        Assert(line.substr(0, 5) == "#G = ");
        Assert(!defined[2]); defined[2] = true;
        Assert(line[5] == '{' && line.back() == '}');
        extract(line.substr(5), elements);
        for (auto elem : elements) {
          Assert(elem.length() == 1);
          G.insert(elem[0]);
        }
      }
      else if (line[1] == 'q') {
        Assert(line.substr(0, 6) == "#q0 = ");
        Assert(!defined[3]); defined[3] = true;
        extract(line.substr(6), elements);
        Assert(elements.size() == 1);
        q0 = state_id(elements[0]);
      }
      else if (line[1] == 'B') {
        Assert(line.substr(0, 6) == "#B = _");
        Assert(!defined[4]); defined[4] = true;
        B = '_';
      }
      else if (line[1] == 'F') {
        Assert(line.substr(0, 5) == "#F = ");
        Assert(!defined[5]); defined[5] = true;
        Assert(line[5] == '{' && line.back() == '}');
        extract(line.substr(5), elements);
        for (auto elem : elements) F.insert(state_id(elem));
      }
      else if (line[1] == 'N') {
        Assert(line.substr(0, 5) == "#N = ");
        Assert(!defined[6]); defined[6] = true;
        extract(line.substr(5), elements);
        Assert(elements.size() == 1 && isnum(elements[0]));
        N = strtoint(elements[0]);
      }
      else { Assert(0); }
    }
    /* transition functions */
    else {
      vector<string> part; part.clear();
      Split(line, part, " ");
      transition t;
      Assert(part.size() == 5);
      t._state = state_id(part[0]); t.state_ = state_id(part[4]);
      t._sym = part[1]; t.sym_ = part[2];
      t.dir = part[3];
      Assert(t._sym.length() == N && t.sym_.length() == N && t.dir.length() == N);
      delta.push_back(t);
    }
  }

  /* do some check for the turing machine */
  // S \subseteq G
  for (int i = 0; i < DEF_NUM; ++i) Assert(defined[i]);
  Assert(B == '_');
  Assert(G.find('*') == G.end());
  Assert(S.find('*') == S.end());

  infile.close();
  return 0;
}

void check_input(string &input) {
  for (int i = 0; i < input.size(); ++i) {
    if (S.find(input[i]) == S.end()) {
      if (verbose) {
        cerr << "Input: " << input << endl;
        cerr << "==================== ERR ====================\n";
        cerr << "error: Symbol \"" << input[i] << "\" in input is not defined in the set of input symbols\n";
        cerr << "Input: " << input << endl;
        for (int _ = 0; _ < i + 7; ++_) cerr << ' '; cerr << "^\n";
        cerr << "==================== END ====================\n";
      }
      else cerr << "illegal input string" << endl;
      exit(1);
    }
  }
  if (verbose) {
    cout << "Input: " << input << endl;
    cout << "==================== RUN ====================\n";
  }
}

void init_emulator(string &input) {
  for (int i = 0; i < N; ++i) {
    tapes.push_back(vector<char>(MAX_LEN, B));
    cur.push_back((MAX_LEN >> 1));
  }

  /* place input */
  int pos = cur[0];
  for (int i = 0; i < input.length(); ++i) {
    tapes[0][pos] = input[i];
    pos++;
  }
}

int state_id(string &state) {
  for (int i = 0; i < Q.size(); ++i) {
    if (Q[i] == state) return i; 
  }
  Assert(0);
}

/* assert at most one valid transition */
// TODO: add support for multi-way trainsition in "do if not exists" practice
int trans_id(int state, string &syms) {
  for (int i = 0; i < delta.size(); ++i) {
    transition t = delta[i];
    if (t._state == state) {
      bool flag = true;
      for (int j = 0; j < N; ++j) {
        if (t._sym[j] == '*') continue;
        if (t._sym[j] != syms[j]) { flag = false; break; }
      }
      if (flag) return i;
    }
  }
  return -1;
}

void run_emulator() {
  int step = 0, state = q0;
  while(1) {
    if (verbose) {
      int len = digits(N) + 6;

      printf("%-*s: %d\n", len, "Step", step);
      printf("%-*s: ", len, "State");
      cout << Q[state] << endl; // weird string
      printf("%-*s: ", len, "Acc");
      if (accept) cout << "Yes" << endl;
      else cout << "No" << endl;
      for (int i = 0; i < N; ++i) print_tape_runtime(i, digits(N) + 1);
      cout << "---------------------------------------------" << endl;
    }

    string syms = "";
    for (int i = 0; i < N; ++i) syms += tapes[i][cur[i]];

    int transition_id = trans_id(state, syms);
    if (transition_id == -1) break; // halt

    transition t = delta[trans_id(state, syms)];
    
    /* change state */
    state = t.state_;
    if (F.find(state) != F.end()) accept = true;

    for (int id = 0; id < N; ++id) {
      /* rewrite symbol */
      if (!(t._sym[id] == '*' && t.sym_[id] == '*')) {
        tapes[id][cur[id]] = t.sym_[id];
      }
      /* move */
      if (t.dir[id] == 'l') { cur[id]--; Assert(cur[id] >= 0); }
      else if (t.dir[id] == 'r') { cur[id]++; Assert(cur[id] < MAX_LEN); }
      else if (t.dir[id] == '*') ;
      else Assert(0);
    }
    step++;
  }
  if (verbose) {
    if (accept) cout << "ACCEPTED" << endl;
    else cout << "UNACCEPTED" << endl;
    cout << "Result: ";
    print_tape(0);
    cout << endl;
    cout << "==================== END ===================="   << endl;
  }
  else {
    if (accept) cout << "(ACCEPTED) ";
    else cout << "(UNACCEPTED) ";
    print_tape(0);
    cout << endl;
  }
}

void print_tape(int id) {
  int pos1 = 0, pos2 = MAX_LEN - 1;
  while (pos1 < MAX_LEN && tapes[id][pos1] == B) pos1++;
  while (pos2 >= 0 && tapes[id][pos2] == B) pos2--;
  // if (pos1 > pos2) pos1 = pos2 = cur[id];
  // pos1 = min(pos1, cur[id]); pos2 = max(pos2, cur[id]);
  // should not output extra _
  for (int i = pos1; i <= pos2; ++i) cout << tapes[id][i];
}

void print_tape_runtime(int id, int len) {
  // idx, tape_content, head
  printf("Index%-*d: ", len, id);
  int pos1 = 0, pos2 = MAX_LEN - 1;
  while (pos1 < MAX_LEN && tapes[id][pos1] == B) pos1++;
  while (pos2 >= 0 && tapes[id][pos2] == B) pos2--;
  if (pos1 > pos2) pos1 = pos2 = cur[id];
  pos1 = min(pos1, cur[id]); pos2 = max(pos2, cur[id]);

  for (int i = pos1; i <= pos2; ++i) cout << IDX(i) << " ";
  cout << endl;
  printf("Tape%-*d: ", len + 1, id);
  for (int i = pos1; i <= pos2; ++i) printf("%-*c", digits(IDX(i)) + 1, tapes[id][i]);
  cout << endl;
  printf("Head%-*d: ", len + 1, id);
  for (int i = pos1; i <= pos2; ++i) {
    if (i == cur[id]) { cout << '^' << endl; break; }
    printf("%-*c", digits(IDX(i)) + 1, ' ');
  }
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

void check_state(string &state) {
  for (auto c : state) {
    Assert((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
  }
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

bool isnum(string &input) {
  // rule out negtive numbers
  for (auto i : input) {
    if (!(i >= '0' && i <= '9')) return false;
  }
  return true;
}

int digits(int num) {
  if (!num) return 1;
  int ret = 0;
  while (num) { ret++; num /= 10; }
  return ret;
}