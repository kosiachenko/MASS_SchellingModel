# Simulation of Schelling's Model of Segregation using MASS library

## 1. Summary of application and parallelization strategies
### What is a MASS library

MASS is a parallel-computing library for multi-agent and spatial simulation over a cluster of computing nodes. 
The main components of the MASS library are “places” and “agents”. “Places” is a matrix of elements that are
dynamically allocated over a cluster of computing nodes. Each element is called a place, is
pointed to by a set of network-independent matrix indices, and is capable of exchanging
information with any other places. On the other hand, “agents” are a set of execution instances
that can reside on a place, migrate to any other places with matrix indices, (thus as duplicating
themselves), and interact with other agents as well as multiple places. MASS library is developed by a reserch group in the University of Washington, Bothell, WA (http://depts.washington.edu/dslab/MASS/).

### What my program does

My program implements the Schelling's Model of Segregation. This agent-based
model was created in 1971 by the economist Thomas Schelling to help explain
how individuals might self-segregate by nationality, race, income or any other
attribute, even when they have no explicit desire to do so. The steps of the
algorithm are described on this web-page:
http://nifty.stanford.edu/2014/mccown-schelling-model-segregation

### General program architecture
* My program uses two layers:
  * Houses (derived from Place class in MASS library) represents the spots agents might
occupy;
  * Residents (derived from Agent class in MASS library) represent the agents who move
between Houses.
* Each agent has a certain binary property, which in my visualization is
represented by color of the cell(green or black) and in my code by number (1
or 2). In real life in can represent a certain characteristic of the individual –
race, nationality, income, etc.
* Agents also have a threshold (or tipping point), which represents the % of
their neighbors that need to be of the same type than they are in order for the
agent to be satisfied with his place.
* At each step of the simulation each agent calculates the actual proportion of
the neighbors of the same type as they are and compare it to their threshold
value. If the actual % of similar neighbors is more than the threshold, the
agent is considered satisfied with his place and stays where he is. If he is not
satisfied with the place – he moves to any other free cell.

### Implementation details:
Two of the technical ideas I had to come up with while implementing my project
were finding an empty spot for an unsatisfied agent and collision free migration.
* __Finding an empty spot.__
On every turn of the iteration agents calculate if they are satisfied with their
spot or not. If not – they need to move to a random empty spot. So I need a
list of available empty spots in my system. Because MASS library doesn’t
provide a way to get information from ALL places in the grid (see issue 2 in
“MASS programmability analysis” below), I came up with the following
algorithm. First, agents would get information about vacant spots in their
extended neighborhood(48 cells around). If there are some vacant spots
there – they randomly select one of them and migrate there. If there are no
empty places in the agent’s neighborhood, he migrates to a random place in
the matrix (without information if it’s occupied or not).
* __Collision free migration.__
Because MASS doesn’t allow to use collision free migration option together
with the Place’s outMessage, I had to develop a way to deal with agent
collisions. So I came up with a partial collision free migration algorithm:
agents can temporarily stay in an occupied cell, but have to move further at
the next turn.
On each turn Agent would make a call to its place and get an ID of its primary
occupant (the one who is at index [0] in this place’s agents vector). This way
they identify if they are primary occupants of this place or not. If they are not,
their flag is set to ‘unsatisfied’ regardless of their neighbors color and they
should move.

## 2. MASS programmability analysis
In general it was very interesting working with MASS as it forces you to think it
terms of a very specific framework and find unusual ways to achieve what you
want using MASS tools.
Also it manages the threads and processed seamlessly and provides a very good
performance improvements for significantly large problems (see Performance
section for more details).
However, I faces a number of problems while coding using MASS, some of them
due to insufficient documentation and some due to some implementation issues.
I summarized the problems together with proposed solutions to these problems
in the table below.

|  |MASS problems encountered	|How I coped with them	|Possible changes to MASS to address the issue|
|---|---|---|---|
|__1__	|The limitation on __collision free migration__ when the program uses Place’s `outMessage`. In my program I needed both collision-free migration and communication between places, so it was a problem for me.|	I absolutely couldn’t do without outMessage. So I implemented my own __partial collision free migration__ (agents can temporarily stay in an occupied cell, but have to move further at the next turn).	|Fix this limitation by implementing collision-free migration through a different mechanism (not involving Place’s `outMessage`).|
|__2__	|No way to collect information from __ALL__ the cells in the Places array: __1)__ `getOutMessage` doesn’t work across ranks; __2)__ `exchangeAll` takes as an argument a set of relative coordinates, which are different for all cells; __3)__ I tried passing to the `exchangeAll` function a vector of all possible relative coordinates for any cell in a Places matrix (which would be of size (2* sizeX -1)*(2* sizeY -1)), but this solution doesn’t work either, because the result is saved in an `inMessage` vector, and it’s size is limited to sizeX*sizeY by MASS library	|I came up with a way to implement my system without getting information from all the cells. In order to find an empty spot for an unsatisfied agent I would first search for empty spots in the agent’s neighborhood, and if nothing found – move him to a random place in the matrix (without information if it’s occupied or not). This approach slows down the stabilization of the system, but actually works ok – all the agents eventually find an unoccupied place they like and the system reaches stability.	|__Option 1__: Edit the documentation to clearly indicate that MASS is not suitable for systems that require to collect information from ALL places/agents at some point of the execution; it’s good for systems with only local communications between places/agents. __Option 2__: Add a function similar to exchangeAll, but which takes absolute coordinates of the cells to collect data from. This would allow to collect data from all the cells in the Places matrix. __Option 3__ (would fix the issue, but will be very cumbersome to use): increase the size of the inMessage vector to 4*sizeX*sizeY, so that the user could pass to exchangeAll a vector of all the possible relative coordinates and inMessage vector would have enough space to store the replies.|
|__3__	|`map()` function does not work according to what is described it the spec (i.e. _“system-provided map() method distributes agents over places uniformly”_) in case #agents < #places. It just fills the beginning of the grid with agents and leaves the end empty.	|I overloaded the `map()` function to distribute agents evenly throughout the grid.	|__Option 1__: edit the description in spec to let users know that in case #agents < #places they will not be distributed uniformly throughout the grid. __Option 2__ (more convenient to uses): fix the implementation so that agents are evenly distributed throughout the Places grid.|
|__4__	|`migratableData` needs to be specifically set, which is a non-obvious step and can lead to a weird system behavior if not performed.	|I manually managed the `migratableData`.	|I think, during agent migration the library should copy all the data members of the agent to a new instance by default. It will be an added convenience to the library users, and will eliminate confusion due to an unexpected system behavior.|

## 3. Performance analysis
Running time for a 32x32 places, 100 turns simulation:

|Setup |Running time (ms) |Improvement compared to sequential execution|
|---|---|---|
|1 node, 1 thread |1 420 583 |-|
|1 node, 4 threads |1 208 365 |1.2 times|
|2 nodes, 4 threads |752 667 |1.9 times|
|4 nodes, 4 threads |573 657 |__2.5 times__|
|8 nodes, 4 threads |581 723 |2.4 times|

As we can see from the chart above, there is a pretty substantial performance
improvement when increasing the number of nodes from 1 to 2 (1,9 times) and
to 4 nodes (2,5 times). However there is no further improvement when we
increase the number of nodes to 8. I re-run the test several times to account for
network issues, but got the same results every time.
I then tried to increase the size of my simulation to 100x100 places and 100
turns and got an interesting change in results:

|Setup |Running time (ms) |Improvement compared to sequential execution|
|---|---|---|
|1 node, 1 thread |27 741 056 |-|
|1 node, 4 threads |17 339 304 |1.6 times|
|2 nodes, 4 threads |12 531 210 |2.2 times|
|4 nodes, 4 threads |6 502 735 |4.3 times|
|8 nodes, 4 threads |2 152 257 |__12.9 times__|

As we can see, the performance improvement for 2 nodes and 4 nodes is even
more dramatic for this size of the problem (2,2 times and 4,3 times respectively),
but also increasing the number of nodes to 8 gives a very good boost to
performance (__12,9 times(!)__ compared to sequential & 3 times compared to 4-
node version).

So, __the conclusions__ I can draw from these experiments is that:

1. the size of the problems is extremely important to the effect of
parallelization on the performance results;
2. and that the MASS does significantly improve the performance of the
large multi-agent simulations.

## 4. Execution results
1. An example MASS log files for the system of size = 40x40, number of iteration
= 100, threshold = 30%, number of nodes = 4 and number of threads = 4 can be
found in the folder “ExecutionOutput”.
2. I also wrote a log parcing program in Python to aggregate execution output
from log files from different coned, process and visualize it. Within this script I
use ‘matplotlib’ library to: 1) visualize the distribution of the agents on the grid in
the beginning and in the end of the simulation, and 2) plot the number of agent
moves on each turn to see how system moves to a stable position with time. The
script is called _“LogParcer.py”_ and can be found in the folder _“ExecutionOutput”_.
Below are the examples of the graphic output for different threshold values:

