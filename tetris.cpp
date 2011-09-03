#include "tetris.h"
#include <iostream>
#include <map>


using namespace std;

Well::Well(state s)
{
    this->s = s;
}

bool Well::lost() const
{
    return s >> (WIDTH * HEIGHT);
}

bool Well::has_line() const
{
    state line = bottom_wall;
    while(s & line)
    {
        if((s & line) == line)
        {
            return true;
        }
        line <<= WIDTH;
    }
    return false;
}

Piece::Piece(Tetromino* t, int o, int l)
{
    shape = t;
    orientation = o;
    location = l;
}

state Piece::get_state() const
{
    if(location >= 0)
    {
        return shape->states[orientation] << location;
    }
    return shape->states[orientation] >> (-location);
}

void draw_state(state s)
{
    for(int i = 0; i < WIDTH+2; i++)
        cout << '-';
    cout << '\n';
    for(int i = 0; i < HEIGHT+4; i++)
    {
        cout << '|';
        for(int j = 0; j < WIDTH; j++)
        {
            if(s & 1)
            {
                cout << 'x';
            }
            else
            {
                cout << '.';
            }
            s /= 2;
        }
        cout << '|' << endl;
    }
    return;
}

void make_walls()
{
    bottom_wall = left_wall = right_wall = 0;
    for(int i = 0; i <HEIGHT+4; i++)
    {
        left_wall <<= WIDTH;
        right_wall <<= WIDTH;
        left_wall |= 1;
        right_wall |= (1 << (WIDTH-1));
    }
    bottom_wall = (1 << WIDTH) -1;
}

void make_pieces()
{
    Tetromino o;
    o.states.push_back((6 << WIDTH)| (6 << (2 * WIDTH)));
    tetrominos.push_back(o);

    Tetromino i;
    i.states.push_back(15 << WIDTH);
    i.states.push_back(2 | (2 << WIDTH) | (2 << (2 * WIDTH)) | (2 << (3 * WIDTH)));
    tetrominos.push_back(i);

    Tetromino j;
    j.states.push_back((14 << WIDTH) | (2 << (2 * WIDTH)));
    j.states.push_back(4 | (4 << WIDTH) | (12 << (2 * WIDTH)));
    j.states.push_back(8 | (14 << WIDTH));
    j.states.push_back(6 | (4 << WIDTH) | (4 << (2 * WIDTH)));
    tetrominos.push_back(j);

    Tetromino l;
    l.states.push_back((7 << WIDTH) | (4 << (2 * WIDTH)));
    l.states.push_back(6 | (2 << WIDTH) | (2 << (2 * WIDTH)));
    l.states.push_back(1 | (7 << WIDTH));
    l.states.push_back(2 | (2 << WIDTH) | (3 << (2 * WIDTH)));
    tetrominos.push_back(l);

    Tetromino s;
    s.states.push_back((3 << WIDTH) | (6 << (2 * WIDTH)));
    s.states.push_back(2 | (3 << WIDTH) | (1 << (2 * WIDTH)));
    tetrominos.push_back(s);

    Tetromino z;
    z.states.push_back((6 << WIDTH) | (3 << (2 * WIDTH)));
    z.states.push_back(1 | (3 << WIDTH) | (2 << (2 * WIDTH)));
    tetrominos.push_back(z);

    Tetromino t;
    t.states.push_back((7 << WIDTH) | (2 << (2 * WIDTH)));
    t.states.push_back(2 | (6 << WIDTH) | (2 << (2 * WIDTH)));
    t.states.push_back(2 | (7 << WIDTH));
    t.states.push_back(2 | (3 << WIDTH) | (2 << (2 * WIDTH)));
    tetrominos.push_back(t);
}

set<state> positions(Well w, Tetromino* t)
{
    int location = WIDTH * HEIGHT;
    Piece start(t, 0, location);
    set<state> states;
    vector<Piece> open;
    open.push_back(start);
    states.insert(start.get_state());
    while(open.size())
    {
        Piece n = open.back();
        open.pop_back();
        state s = n.get_state();
        if(!(s & left_wall))
        {
            Piece left(n.shape, n.orientation, n.location-1);
            state l = left.get_state();
            if(!states.count(l))
            {
                if(!(w.s & l))
                {
                    states.insert(l);
                    open.push_back(left);
                }
            }
        }
        if(!(s & right_wall))
        {
            Piece right(n.shape, n.orientation, n.location+1);
            state r = right.get_state();
            if(!states.count(r))
            {
                if(!(w.s & r))
                {
                    states.insert(r);
                    open.push_back(right);
                }
            }
        }
        if(!(s & bottom_wall))
        {
            Piece bottom(n.shape, n.orientation, n.location-WIDTH);
            state b = bottom.get_state();
            if(!states.count(b))
            {
                if(!(w.s & b))
                {
                    states.insert(b);
                    open.push_back(bottom);
                }
            }
        }
        Piece rotated(n.shape,
                (n.orientation+1) % n.shape->states.size(),
                n.location);
        state r = rotated.get_state();
        if(!(r & left_wall && r & right_wall))
        {
            if(!states.count(r))
            {
                states.insert(r);
                open.push_back(rotated);
            }
        }
    }
    return states;
}

vector<state> landings(Well w, Tetromino* t)
{
    set<state> result = positions(w, t);
    vector<state> ret;
    set<state>::iterator it;
    for(it = result.begin(); it != result.end(); it++)
    {
        if(*it & (bottom_wall | (w.s << WIDTH)))
        {
            ret.push_back(*it);
        }
    }
    return ret;
}

bool find_winner(Well w)
{
    static map<state, bool> winning_states;
    if(winning_states.count(w.s))
    {
        return winning_states[w.s];
    }
    for(int i = 0; i < tetrominos.size(); i++)
    {
        if(piece_wins(w, tetrominos[i]))
        {
            winning_states[w.s] = true;
            return true;
        }
    }
    winning_states[w.s] = false;
    return false;
}

bool piece_wins(Well w, Tetromino& t)
{
    vector<state> l = landings(w, &t);
    vector<state>::iterator it;
    for(it = l.begin(); it != l.end(); it++)
    {
        Well w2(w.s | *it);
        if(w2.lost())
        {
            continue;
        }
        if(w2.has_line())
        {
            return false;
        }
        if(!find_winner(w2))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    make_walls();
    make_pieces();
    Well w;
    cout << find_winner(w) << endl;
    return 0;
}
