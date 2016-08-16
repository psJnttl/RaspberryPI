from SymbolScore import *
from Vocabulary import *
from FoundWordsList import *
from GaStack import *
from Symbol import *
from Word import *

import platform
if platform.system() == 'Linux':
    srvResultPath = './results/worDaDe_findings.txt'
else: 
    srvResultPath = '.\\results\\worDaDe_findings.txt'


X_SIZE = 4
Y_SIZE = 4
MAX_WORD_LENGTH = X_SIZE * Y_SIZE

class GameArea:
    def __init__(self, gameAreaAsList=[]):
        print 'now instantiating GameArea'
        self.symbolScore = SymbolScore()
        self.theVocabulary = Vocabulary()
        print 'Vocabulary size: ', self.theVocabulary.NumberWords()
        self.gameArea = []
        self.tmpGameArea = []
        if len(gameAreaAsList) != 16:
            print 'Game Area size is not correct!'
        else: 
            self.InsertSymbols(gameAreaAsList)
        self.skippedWords = 0

    def InsertSymbols(self, gameArea):
        self.gameArea = []; k = 0
        for i in range(Y_SIZE):
            tmpListX = []
            for j in range(X_SIZE):
                newSymbol = Symbol(gameArea[k], k, self.symbolScore.GetScore(gameArea[k]))
                tmpListX.append(newSymbol)
                k += 1
            self.gameArea.append(tmpListX)

    def InsertSymbolAt(self, gameArea, symbol, gY, gX):
        if gY < 0 or gX < 0 or gY >= Y_SIZE or gX >= X_SIZE:
            return
        line = gameArea[gY]
        line[gX] = symbol 

    def Solve(self):
        self.wordList = FoundWordsList()
        for i in range(Y_SIZE):
            tmpX = self.gameArea[i]  # collect line
            for j in range(X_SIZE):  # x-coords
                if tmpX[j].EndsWith() == True: # ends-with digrams
                    print 'skipping tile '
                    continue  # can't start with this symbol
                self.theVocabulary.UseSubVocabulary(tmpX[j])
                self.ProcessVocabulary(tmpX[j], i, j)
                if tmpX[j].SymbolType() == 'eitherOr':
                    tmpX[j].ToggleEoState()  # either -> or
                    self.theVocabulary.UseSubVocabulary(tmpX[j])
                    self.theVocabulary.ResetFailedAt()
                    self.ProcessVocabulary(tmpX[j], i, j)
        self.PrintGameArea(self.gameArea)
        self.wordList.saveFindingsToFileJSON(srvResultPath) # also prints via Sort()
        print 'skipped ', self.theVocabulary.SkippedWords(), ' words'

    def ProcessVocabulary(self, symbol, gY, gX):
        peaCounter = 0; lastWord = ''; nextWord = ''
        while True:
            nextWord = self.theVocabulary.GetNextWord()
            if nextWord == False:
                break
            self.theVocabulary.ResetFailedAt()
            self.tmpGameArea = self.CopyGameArea(self.gameArea)
            self.word = Word(nextWord)  # path through wilderness
            self.gaStack = GaStack()    # GameArea stack for backtracking
            self.SearchPath(nextWord, 0, gY, gX)
            if True == self.word.IsComplete():
                self.wordList.InsertWord(self.word)
                self.theVocabulary.ResetFailedAt()
        return

    def CopyGameArea(self, gameArea): # a copy of gameArea is used
        tmpGameArea = []
        for i in range(Y_SIZE):
            tmpXdst = []
            tmpXsrc = gameArea[i]
            for j in range(X_SIZE):
                newSymbol = tmpXsrc[j].Copy()
                tmpXdst.append(newSymbol)
            tmpGameArea.append(tmpXdst)
        return tmpGameArea

    def PrintGameArea(self, gameArea): # DEBUG purposes only
        for i in range(Y_SIZE):
            line = []
            tmpX = gameArea[i]
            for j in range(X_SIZE):
                line.append(tmpX[j].SymbolChars())
            print line

    def FetchGameAreaSymbol(self, gameArea, gY, gX):
        line = gameArea[gY]
        symbol = line[gX]
        return symbol

    def SearchPath(self, nextWord, wordIndex, gY, gX):
        if gY >= Y_SIZE or gY < 0 or gX >= X_SIZE or gX < 0 or wordIndex >= MAX_WORD_LENGTH:
            return
        tileSymbol = self.FetchGameAreaSymbol(self.tmpGameArea, gY, gX)   # no easier way ?
        status = tileSymbol.IsNextSymbol(nextWord, wordIndex)
        if status == 0:   #  tile symbol not next in word
            symbolLength = tileSymbol.SymbolLength()
            if symbolLength > 1: # digram
                 self.theVocabulary.SetSpecialTile(tileSymbol)
            return
        elif status > 0:
            if len(nextWord) == (wordIndex + status):  # word compete
                self.word.InsertSymbol(tileSymbol.Copy())
                self.word.SetComplete()
                return
            else:  # word only partially found
                nro = Symbol(str(wordIndex),0,0)
                self.gaStack.Push(self.CopyGameArea(self.tmpGameArea))  # store for backtracking
                self.InsertSymbolAt(self.tmpGameArea, nro, gY, gX)  # mark game area
                wordIndex += status
                self.word.InsertSymbol(tileSymbol.Copy())

        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY, gX+1) # E
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY+1, gX+1 ) # SE
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY+1, gX) # S
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY+1, gX-1) # SW
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY, gX-1) # W
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY-1, gX-1) # NW
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY-1, gX) # N
        if False == self.word.IsComplete():
            self.SearchPath(nextWord, wordIndex, gY-1, gX+1) # NE

        if False == self.word.IsComplete():
            self.tmpGameArea = self.gaStack.Pop()
            self.theVocabulary.FailsAt(self.word, nextWord, wordIndex)
            self.word.RemoveLastSymbol()            

        return
