pieces = []
width = 6
height = 6

left_wall = right_wall = bottom_wall = 0

from collections import *

#ripped from http://programmingzen.com/2009/05/18/memoization-in-ruby-and-python/
def memoize(function):
    cache = {}
    def decorated_function(*args):
        if args in cache:
            return cache[args]
        else:
            val = function(*args)
            cache[args] = val
            return val
    return decorated_function

class Well(object):
    def __init__(self, state=0):
        self.state = state

    def __repr__(self):
        state = ''
        state += '-'*(width+2)+'\n'
        for i in range(height):
            state += '|'
            for j in range(width):
                if self.state & (1<<(i*width+j)):
                    state += 'x'
                else:
                    state += '.'
            state += '|'
            state += '\n'
        return state

    def has_line(self):
        line = (1 << width) - 1
        while(self.state & line):
            if (self.state & line) == line:
                return True
            line <<= width
        return False

    def lost(self):
        return self.state > (1 << (width*height))

    def __hash__(self):
        return hash(self.state)

    def __cmp__(self, other):
        return cmp(self.state, other.state)

class Tetromino(object):
    states = ()
    def __init__(self, states):
        self.states = states

Piece = namedtuple('Piece', ['piece', 'orientation', 'location'])

#@memoize
def piece_state(piece):
    if piece.location >= 0:
        return piece.piece.states[piece.orientation] << piece.location
    else:
        return piece.piece.states[piece.orientation] >> -piece.location


def positions(well, piece):
    location = (width*height)
    start = Piece(piece, 0, location)
    states = set()
    states.add(start)
    open = [start]
    while open:
        n = open.pop()
        #try to go left
        ps = piece_state(n)
        if not ps & left_wall:
            left = Piece(n.piece, n.orientation, n.location-1)
            if left not in states:
                if not piece_state(left) & well.state:
                    states.add(left)
                    open.append(left)
        #try to go right
        if not ps & right_wall:
            right = Piece(n.piece, n.orientation, n.location+1)
            if right not in states:
                if not piece_state(right) & well.state:
                    states.add(right)
                    open.append(right)
        #try to go down
        if not ps & bottom_wall:
            down = Piece(n.piece, n.orientation, n.location-width)
            if down not in states:
                if not piece_state(down) & well.state:
                    states.add(down)
                    open.append(down)
        #try rotation
        rotated = Piece(n.piece, (n.orientation+1) % len(n.piece.states), n.location)
        if rotated not in states:
            #slightly hackish way to see if we've crossed the edge
            r = piece_state(rotated)
            if not ( (r & left_wall) and (r & right_wall)):
                if not r & well.state:
                    states.add(rotated)
                    open.append(rotated)
    return states

def landings(well, piece):
   return [n for n in positions(well, piece) if piece_state(n) & (bottom_wall | (well.state << width))]

def make_pieces():
    global pieces
    pieces = []
    #O
    shapes = ((6 | 6<<width) << width ,)
    pieces.append(Tetromino(shapes))

    #I
    shapes = ( 15 << width, 2 | (2 << width) | (2 << (2 * width)) | (2 << (3 * width)) )
    pieces.append(Tetromino(shapes))

    #J
    shapes = ((14 << width) | (2 << (2 * width)),
            4 | (4 << width) | (12 << (2 * width)),
            8 | (14 << width),
            6 | (4 << width) | (4 << (2 * width)),
            )
    pieces.append(Tetromino(shapes))

    #L
    shapes = ((7 << width) | (4 << (2 * width)),
            6 | (2 << width) | (2 << (2 * width)),
            1 | (7 << width),
            2 | (2 << width) | (3 << (2 * width)),
            )
    pieces.append(Tetromino(shapes))

    #S
    shapes = ((3 << width) | (6 << (2 * width)),
            2 | (3 << width) | (1 << (2 * width)),
            )
    pieces.append(Tetromino(shapes))

    #Z
    shapes = ((6 << width) | (3 << (2 * width)),
            1 | (3 << width) | (2 << (2 * width)),
            )
    pieces.append(Tetromino(shapes))

    #T
    shapes = ((7 << width) | (2 << (2 * width)),
            2 | (6 << width) | (2 << (2 * width)),
            2 | (7 << width),
            2 | (3 << width) | (2 << (2 * width)),
            )
    pieces.append(Tetromino(shapes))

def make_walls():
    global bottom_wall, left_wall, right_wall
    for i in range(height+4):
        left_wall <<= width
        right_wall <<= width
        left_wall |= 1
        right_wall |= (1 << (width-1))
    bottom_wall = (1 << width) -1

@memoize
def who_wins(well):
    for p in pieces:
        if piece_wins(well, p):
            return 1
    return 0

def piece_wins(well, piece):
    for position in landings(well, piece):
        w2 = Well(well.state | piece_state(position))
        if w2.lost():
            continue
        if w2.has_line():
            return 0
        if not who_wins(w2):
            return 0
    return 1

make_walls()
make_pieces()
#while not w.lost():
#    print w
#    print w.has_line()
#    move = random.choice(landings(w, pieces[0]))
#    w.state |= piece_state(move)
#print w

print who_wins(Well(0))
