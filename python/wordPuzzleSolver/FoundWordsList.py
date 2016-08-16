'''
contains a list of Words found during searchPath
--
+ InsertWord(
+ WordsFound()  returns a list of words found
- SortList  before solutionlist returned it's sorted based on points and alphabets
   public?
'''
from Word import *
import json

class FoundWordsList():
    def __init__(self):
        self.wordList = []
        self.sortedList = []

    def InsertWord(self, word):
        duplicate = False
        for aWord in self.wordList:
            #if aWord.WordAsStr() == word.WordAsStr(): # a duplicate!
            if aWord.InitWord() == word.InitWord(): # a duplicate!
                duplicate = True
                if word.Points > aWord.Points():      # can happen with digram, either/or
                    self.RemoveWord(aWord)
                    self.wordList.append(word.Copy())
        if False == duplicate:
            self.wordList.append(word.Copy())

    def WordsFound(self):
        for aWord in self.wordList:
            aWord.Print()

    def RemoveWord(self, word): # retain everything but
        tmpList = []
        for item in self.wordList:
            if item.InitWord() != word.InitWord():
                tmpList.append(item)
        self.wordList = tmpList

    def Sort(self): # based on 1. points, 2. in case equal points: alphabetically
        self.sortedList = []
        for aWord in self.wordList:
            word = aWord.InitWord()
            points = aWord.Points()
            path = aWord.Path()
            self.sortedList.append((points, word, path))
        self.sortedList.sort(reverse=True) # sort based on descencing points amount
        currentPoints=0; previousPoints=0; blockSize=1
        for i in range(len(self.sortedList)): # indexing needed to access items in list
            currentPoints, currentWord, currentPath = self.sortedList[i]
            if currentPoints == previousPoints:
                blockSize += 1
            else: #
                if blockSize > 1: # block is limited from sortedList[i-blockSize] to sortedList[i-1]
                    startIndex = i-blockSize
                    endIndex = i    #;print startIndex, endIndex
                    for j in range(startIndex, endIndex):
                        #print 'j: ', j
                        for k in range(j+1, endIndex):
                            #print 'k: ', k
                            pA, wordA, pthA = self.sortedList[j]; pB, wordB, pthB = self.sortedList[k]
                            #print wordA, wordB  # no equals, duplicates have been removed
                            if wordA > wordB: 
                                tmp = self.sortedList[j]
                                self.sortedList[j] = self.sortedList[k]
                                self.sortedList[k] = tmp
                blockSize = 1
            previousPoints = currentPoints
        print 'Words found: ', len(self.sortedList)
        for item in self.sortedList:
            print item

    def saveFindingsToFileJSON(self, filename): # [(pts,word,path),(pts,word,path),]
        self.Sort()
        filePtr = open(filename, 'w')
        dstList = list()
        for item in self.sortedList:
            points, aWord, path = item
            tmpDict = dict()
            tmpDict['points'] = points
            tmpDict['word'] = aWord
            tmptmpDict = dict()
            tmptmpDict['path'] = path
            tmpDict['wordPath'] = tmptmpDict
            dstList.append(tmpDict)
        filePtr.write( json.dumps(dstList) )
        filePtr.write('\n')
        filePtr.close()