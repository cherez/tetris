#include <vector>

typedef __uint128_t state;

const int WIDTH = 6;
const int HEIGHT = 6;

state left_wall, right_wall, bottom_wall;

struct Well
{
    state s;
    Well(state s = 0);
    bool lost() const;
    bool has_line() const;
};

struct Tetromino
{
    std::vector<state> states;
};

struct Piece
{
    Tetromino* shape;
    int orientation;
    int location;

    state get_state() const;
    Piece(Tetromino* t, int o, int l);
};

std::vector<Tetromino> tetrominos;

void draw_state(state s);

void make_walls();
void make_pieces();
set<state> positions(Well w, Tetromino* t);
vector<state> landings(Well w, Tetromino* t);
bool find_winner(Well w);
bool piece_wins(Well w, Tetromino& t);
