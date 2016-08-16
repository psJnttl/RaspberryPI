from Symbol import *
from Word import *
import platform
if platform.system() == 'Linux':
    vocabPath = '/var/www/html/worDaDe2/vocabulary/'
else: 
    vocabPath = 'C:\\media\\python\\worDaDe2\\vocabulary\\'


class Vocabulary:
    def __init__(self):
        self.nbrWords = self.LoadVocabulary()
        self.wordIndex = 0
        self.lastFailedAt = ''
        self.nextChar = ''
        self.skippedWords = 0
        self.failedSymbolType = ''

    def LoadVocabulary(self, folderName=vocabPath):
        nameList = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']
        numberOfWords = 0
        self.theVocabulary = dict()
        for name in nameList:
            filename = folderName + name + '.txt'
            theFile = open(filename, 'r')
            tmpList = theFile.readlines()
            theFile.close()
            destList = list()
            for aWord in tmpList:
                destList.append(aWord.strip('\n\r'))  # files saved on Win require '\r'
                numberOfWords += 1
            self.theVocabulary[name] = destList
        print "vocabulary loaded from ", folderName
        return numberOfWords

    def NumberWords(self):
        return self.nbrWords

    def UseSubVocabulary(self, sym): 
        self.ResetFailedAt()
        symbol = sym.SymbolChars()
        if len(symbol) > 1: symbol = symbol[:1]   # Symbol could be a digram.
        if symbol in self.theVocabulary:
            self.activeDictionary = self.theVocabulary[symbol]
            self.lastWordIndex = len(self.activeDictionary)
            symbol = sym.SymbolChars()
            if len(symbol) > 1: # if digram, FFWD
                for i in range(0, self.lastWordIndex):
                    if 0 == self.activeDictionary[i].find(symbol):
                        self.wordIndex = i
                        self.skippedWords += i
                        break
            else:
                self.wordIndex = 0

    def GetNextWord(self):
        result = False
        if self.wordIndex < self.lastWordIndex:
            for i in range(self.wordIndex, self.lastWordIndex):
                if 0 != self.activeDictionary[i].find(self.lastFailedAt):
                    self.skippedWords += i - self.wordIndex
                    self.wordIndex = i                    
                    break # skip until word doesn't start with what failed last
            result = self.activeDictionary[self.wordIndex]
            self.wordIndex += 1
        return result

    def FailsAt(self, partial, nextWord, wordIndex): #
        if '' !=  self.nextChar:   # failed at special tile
            if wordIndex + len(self.nextChar) > len(nextWord): # avoid exception due to out of bounds
                fragment = '1234567890123456' # anything but a word in the vocabulary
            else:
                fragment = partial.WordAsStr() + nextWord[ wordIndex : wordIndex + len(self.nextChar) ]
        else:
            fragment = partial.WordAsStr() + nextWord[wordIndex:wordIndex+1]   #nextSymbol  
        if len(fragment) > len(self.lastFailedAt):
            if 0 == len(self.lastFailedAt) or 0 == fragment.find(self.lastFailedAt): # new is a continuation of the old OR there was none
                self.lastFailedAt = fragment

    def SetSpecialTile(self, symbol): # get contents of special tile
        self.nextChar = symbol.SymbolChars()
        self.failedSymbolType = symbol.SymbolType()  # coupling gets tighter, not good

    def ResetFailedAt(self):
        self.lastFailedAt = ''
        self.nextChar = ''
        self.failedSymbolType = ''
    def SkippedWords(self):
        return self.skippedWords
