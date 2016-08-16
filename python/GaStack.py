from GameArea import *

class GaStack():
    def __init__(self):
        self.stack = []

    def Push(self, gameArea): # need a copy, reference doesn't work 
        self.stack.append(gameArea)

    def Pop(self):
        return self.stack.pop()

    def Reset(self):
        self.stack = []
