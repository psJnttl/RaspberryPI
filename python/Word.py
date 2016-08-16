from Symbol import *
from GameArea import *

class Word:
    def __init__(self, initWord):
        self.Reset()
        self.initWord = initWord

    def Reset(self):
        self.points = 0
        self.path = []
        self.symbolList = []
        self.wordString = ''
        self.complete = False
        self.initWord = ''

    def InsertSymbol(self, symbol): # a reference of symbol, need to make a copy
        newSymbol = Symbol(symbol.SymbolChars(), symbol.Coordinates(), symbol.Points())
        self.points += symbol.Points()
        self.symbolList.append(newSymbol)
        self.path.append(symbol.Coordinates())

    def RemoveLastSymbol(self): # can only remove the last
        symbol = self.symbolList.pop()
        self.path.pop()
        self.points -= symbol.Points()

    def Path(self):
        return self.path

    def Points(self):
        points = self.points
        if len(self.wordString) == 5:
            points = points * 3 / 2    # on purpose: no decimals and rounding down
        elif len(self.wordString) > 5 and len(self.wordString) < 8: 
            points = points * 2
        elif len(self.wordString) >= 8: 
            points = points * 5 / 2
        return points

    def IsComplete(self):
        return self.complete
    def SetComplete(self):
        self.wordString = self.WordAsStr()
        self.complete = True

    def Copy(self):  # return a copy of self
        newWord = Word(self.InitWord())
        newWord.points = self.points
        newWord.wordString = self.wordString
        for step in self.path:
            newWord.path.append(step)
        for symbol in self.symbolList:
            newWord.symbolList.append(symbol)
        return newWord

    def WordAsStr(self):
        tmpStr = ''
        for symbol in self.symbolList:
            tmpStr += symbol.SymbolChars()
        return tmpStr

    def InitWord(self):
        return self.initWord
