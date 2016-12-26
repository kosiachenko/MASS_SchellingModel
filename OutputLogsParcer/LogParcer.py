from __future__ import print_function
import sys
import re
import numpy as np
import matplotlib.pyplot as plt

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

# arguments: sizeX, sizeY, numTurns
numTurns = 100
sizeX = 40
sizeY = 40

#create a 3D array [][][]
states  = [[[0 for k in xrange(sizeY)] for j in xrange(sizeX)] for i in xrange(numTurns)]
movesPerTurn = [0 for k in xrange(numTurns)]

pat1 = re.compile("""^Moving the agent\. Turn\[(\d+)\]""")
pat2 = re.compile("""^Turn\[(\d+)\], cell\[(\d+)\]\[(\d+)\] \= (\d+)""")

def parseLogFile( fileName ):
    with open(fileName) as f:
        for line in f:
            #Turn[1], cell[7][6] = 1
            l = pat2.match(line)
            if l:
                turn = int(l.group(1))
                x = int(l.group(2))
                y = int(l.group(3))
                value = int(l.group(4))
                states[turn][x][y] = value
             
            #Moving the agent. Turn[1], Agent-9 at [0, 9] to move to [2, 11] a displacement of [2, 2]
            m = pat1.match(line)
            if m:
                turn = int(m.group(1))
                movesPerTurn[turn] += 1

parseLogFile('MASS_logs/master.txt')
parseLogFile('MASS_logs/PID_1_uw1-320-02.uwb.eduresult.txt')
parseLogFile('MASS_logs/PID_2_uw1-320-03.uwb.eduresult.txt')
parseLogFile('MASS_logs/PID_3_uw1-320-05.uwb.eduresult.txt')

for index in range(numTurns):
    print(index)
    for i in range(sizeX):
        for j in range(sizeY):
            print(states[index][i][j], end=" ") 
        print()

for t in range(numTurns):
    eprint("Turn ", t, " , total agent moves= ", movesPerTurn[t])

plt.figure()
plt.plot(movesPerTurn, color='r', linewidth=2.0)
plt.axis([0, 30, 0, 250])
plt.ylabel("Agents moved")
plt.xlabel('turn #')
plt.title("Agents moved per turn")

plt.matshow(states[0], fignum = 2, cmap='nipy_spectral_r')
plt.title("Agents distribution at turn 0")

plt.matshow(states[99], fignum = 3, cmap='nipy_spectral_r')
plt.title("Agents distribution after the system reached a stable state")
plt.show()