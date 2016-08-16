#!/usr/bin/env python
import sys
from GameArea import *  # GameArea, there shouldn't anything else
import time

if __name__ == "__main__":

    if len(sys.argv) != 17:
        exit(2)
    sys.argv.pop(0) # lose cmd name at index 0
    gameArea = GameArea(sys.argv)
    start = time.clock()
    gameArea.Solve()
    end = time.clock()
    print 'Solved in ', (end - start), ' seconds'
    exit(1)
'''
Version 4  18-Aug-2016 Pasi Junttila
Cleanup, some improvements to classes GameArea, Vocabulary, Symbol. 
Puzzle now printed with results.

Version 3  11-Aug-2016 Pasi Junttila
Word skipping mechanism moved to Vocabulary class. Special tiles now also 
included in boosting.

Version 2  08-Aug-2016 Pasi Junttila
Other symbols. Starting with basic digram, 1-4 letters. No material to test 
long digrams yet. Starting & ending with done. Either/or ready but requires 
more testing.

Version 1  31-Jul-2016 Pasi Junttila
Getting started with the Object Oriented implementation. Tiles with single letter, 
saving to JSON file. Despite speed improvement O-O version still 4x slower...

The mental amount of arguments is due to the fact that this is meant to be used on 
a server. Web client sends puzzle to PHP intermediate which in turn calls this. 
The actual result is read from result folder in JSON format and sent back to client.
'''
