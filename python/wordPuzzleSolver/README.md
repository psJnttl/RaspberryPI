The worDaDe is a word puzzle solver for MSFT's Wordament mobile game. Solves all puzzles including 
digrams (starts-with, ends-with) and either/or tiles. Has been designed to run on a server. Client 
sends request from web page to PHP script which is turn calls worDaDe.py with all the 16 parameters, 
one for each tile (4x4, left-right, up-down) in the  puzzle. Solving is done by checking if a word in 
the vocabulary has a path in the puzzle, according to rules of the game. All results are stored sorted 
first by points accrued and then alphabetically in descending order. Result is written in JSON format and 
PHP script will send it back to client.

PHP script and client side Javascript code are not included. 

For quick testing just comment first four lines from worDaDe.py and generate game area with this:

ga = ['s', 'a', 's', 'e', 'e', 'm' ,'at', 'l', 's', 'h', 'e', 'c', 's', 'o', 'd', 'r']

gameArea = GameArea(ga)

Vocabulary size is now (18-Aug-2016) 12k words, plenty of room for improvement. I've found that many 
dictionaries available online contain a lot of rubbish, not real words, that'll just slow the search 
process down.
