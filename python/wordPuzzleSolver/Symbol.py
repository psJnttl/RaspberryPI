class Symbol:
    def __init__(self, sym='a', coords=0, points=2):
        self.symbol = sym    # 'a', 'ed', 'in-', '-est', 'm/i'.
        self.symbolType = self.DetermineSymbolType(sym)
        if self.symbolType == 'eitherOr': self.eoState = 'either'
        else: self.eoState = ''
        self.gaIndex = coords # not x,y but index
        self.points = points

    def SymbolChars(self):
        if self.symbolType == 'letter' or self.symbolType == 'digram' or self.symbolType == 'path':
            return self.symbol
        elif self.symbolType == '1stDigram' or self.symbolType == 'lastDigram':
            return self.symbol.strip('-')
        elif self.symbolType == 'eitherOr':
            if self.eoState == 'either': return self.symbol[:self.symbol.find('/')]
            elif self.eoState == 'or': return self.symbol[self.symbol.find('/') +1 :]

    def SymbolLength(self):
        if self.symbolType == 'letter' or self.symbolType == 'digram' or self.symbolType == 'path':
            return len(self.symbol)
        elif self.symbolType == '1stDigram' or self.symbolType == 'lastDigram':
            return len( self.symbol.strip('-') )
        elif self.symbolType == 'eitherOr':
            return 1  # assuming there won't be 'as/be' etc. only 'm/i'

    def EndsWith(self): # in case symbol is one of the word's ending with: -ed, -est
        if self.symbolType == 'lastDigram':
            return True
        return False        

    def Coordinates(self): 
        return self.gaIndex

    def Points(self):
        return self.points

    def IsNextSymbol(self, word, wordIndex):
        result = 0
        if self.symbolType == 'letter' or self.symbolType == 'digram' :
            result = self.IsNextLetterOrDigram(word, wordIndex)
        elif self.symbolType == '1stDigram':
            result = self.IsNext1stDigram(word, wordIndex)
        elif self.symbolType == 'lastDigram':
            result = self.IsNextLastDigram(word, wordIndex)
        elif self.symbolType == 'eitherOr':
            result = self.IsNextEitherOr(word, wordIndex)
        return result # 0:fail, 1...4 nbr letters match depending on symbol type

    def IsNextLetterOrDigram(self, word, wordIndex): # fail: 0, OK: amount of bytes consumed from word
        testWord = word[wordIndex:]
        foundAt = testWord.find(self.symbol)  # one or many chars
        if foundAt == 0: # yes, it's next in the word
            return len(self.symbol)
        else: return 0

    def IsNext1stDigram(self, word, wordIndex):
        if wordIndex != 0: # doesn't start with == fail
            return 0
        foundAt = word.find(self.symbol[:-1])  # drop tailing '-'
        if foundAt == 0:
            return len(self.symbol[:-1])
        return 0
    
    def IsNextLastDigram(self, word, wordIndex):
        if 0 == word[wordIndex:].find(self.symbol[1:]):
            return len(self.symbol) -1  # take '-' into account
        return 0

    def IsNextEitherOr(self, word, wordIndex): # test both, every time
        testWord = word[wordIndex:]
        sym = self.symbol[:self.symbol.find('/')]
        if 0 == testWord.find(sym):
            return 1
        sym = self.symbol[self.symbol.find('/')+1:]
        if 0 == testWord.find(sym):
            return 1
        return 0

    def ToggleEoState(self):
        if self.eoState == 'either': self.eoState = 'or'
        elif self.eoState == 'or': self.eoState = 'either'
        else: self.eoState = ''

    def Copy(self):  # references don't fly very far
        newSymbol = Symbol()
        newSymbol.symbol = self.symbol; newSymbol.symbolType = self.symbolType; newSymbol.eoState = self.eoState
        newSymbol.gaIndex = self.gaIndex; newSymbol.points = self.points
        return newSymbol

    def DetermineSymbolType(self, sym): # letter, digram, 1stDigram, lastDigram, eitherOr, path
        result = False
        if sym.isdigit():
            return 'path'
        elif len(sym) == 1 and sym.isalpha() :
            result = 'letter'
        elif len(sym) > 1 and len(sym) < 5: 
            if len(sym) == 3 and 1 == sym.find('/') :
                if True == sym[0].isalpha() and True == sym[2].isalpha():
                    result = 'eitherOr'
            elif 0 == sym.find('-') and sym[1:].isalpha() :
                result = 'lastDigram'
            elif (len(sym) - 1) == sym.find('-') and sym[:-1].isalpha():
                result = '1stDigram'
            elif sym.isalpha():
                result = 'digram'
        return result

    def SymbolType(self):
        return self.symbolType
