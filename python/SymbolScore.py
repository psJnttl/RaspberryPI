'''
The purpose of this class is to know scoring for every symbol there is. 
Objects can query the amount of points for each symbol (string).

Currently there's only very small sample of digrams. As can be seen it can't be 
said that if digram has two letters it's always 7pts. Starting and ending with 
digrams could be 12 pts always, or not. Most likely either/or digrams are always 
20pts though.
''' 

class SymbolScore:
    def __init__(self):
        self.InitPointsTable()

    def GetScore(self, symbol):  # not class Symbol, but str
        if symbol in self.pointsTable:
            return self.pointsTable[symbol]
        else: return 1  # it's worth > 0 anyway

    def InitPointsTable(self):
        self.pointsTable = {
        'a': 2, 
        'at': 6, 
        'b': 5, 
        'c': 3, 
        'd': 3, 
        'e': 1, 
        'ed': 7,
        '-est': 12,
        'f': 5, 
        'g': 4,
        'h': 4, 
        'i': 2, 
        'i/m': 20,
        'in-': 12, 
        'j': 10, 
        'k': 6, 
        'l': 3,
        'll': 8, 
        'm': 4, 
        'n': 2, 
        'o': 2, 
        'p': 4, 
        'q': 10, 
        'r': 2, 
        's': 2,
        't': 2, 
        'u': 4,
        'u/f': 20, 
        'v': 6, 
        'w': 6, 
        'x': 9, 
        'y': 5, 
        'z':8  }
