from tkinter import *
import re


# Maps integers to corresponding colors, used in data grid UI
colorMap = {
    -2: "green",
    -1: "black",
    0: "white",
    1: "red",
    2: "blue"
}


# Data structure for the data grid UI, consists of a frame to which it is attached and a matrix of entries
class Table:  
    def __init__(self, frame): 
        self.frame = frame
        self.entries = []
        

    # Populates the entries matrix 
    def add(self, data):
        for i in range(len(data)): 
            temp = []
            for j in range(len(data[0])): 
                entry = Entry(self.frame, width=20, bg=colorMap[data[i][j]]) 
                entry.grid(row=i, column=j) 
                temp.append(entry)
            self.entries.append(temp)


    # Called periodically to update the entries of the UI to show the new robot positions
    def update(self, data):
        for i in range(len(data)): 
            for j in range(len(data[0])): 
                self.entries[i][j].configure(bg=colorMap[data[i][j]])


# Stores the specifications of the game
class Game():
    def __init__(self, _m, _n, _k, _p, _pos1, _pos2, _pos3, _obs):
        self.m = _m
        self.n = _n
        self.k = _k
        self.p = _p
        self.pos1 = _pos1
        self.pos2 = _pos2
        self.pos3 = _pos3
        self.obstacles = _obs
        self.data = []
        self.positions = []
        self.init_data_array()


    # Initializes the data grid. Each cell of the grid is assigned a number which denotes the colour it 
    # would be assigned in the UI
    def init_data_array(self):
        for i in range(self.n):
            temp = []
            for j in range(self.m):
                temp.append(0)
            self.data.append(temp)
        for obstacle in self.obstacles:
            self.data[obstacle[1]][obstacle[0]] = -1
        self.data[self.pos1[1]][self.pos1[0]] = 1
        self.data[self.pos2[1]][self.pos2[0]] = 2
        self.data[self.pos3[1]][self.pos3[0]] = -2


    # Stores the sequence of Robot 1 and Robot 2 positions for all the possible admissible strategies
    def set_positions(self, contents):
        lines = contents.split("\n")
        for line in lines:
            if(line == ""):
                break
            player1_positions = [(self.pos1[0], self.pos1[1])]
            player2_positions = [(self.pos2[0], self.pos2[1])]

            states = line[:-1].split(" : ")[2].split(" ")
            for state in states:
                x1, y1, x2, y2, k = map(int, state.split("~"))
                player1_positions.append((x1, y1))
                player2_positions.append((x2, y2))
            self.positions.append((player1_positions, player2_positions))


# Used to display the admissible strategies visually
class Visualizer():
    def __init__(self, game):
        self.game = game
        self.root = Tk() 
        self.root.title("Admissible Strategies")
        self.frame = Frame(self.root)
        self.frame1 = Frame(self.root)
        self.grid = Table(self.frame) 
        self.curr_strategy = 0
        self.index = 0
        self.button = Button(self.frame1, text="Next Strategy", fg="red", command=self.next_strategy)


    # Components of the UI (Grid and Button) are activated
    def add_components(self):
        self.frame.pack()
        self.frame1.pack()
        self.grid.add(self.game.data)
        self.button.pack()
        self.clock()
        self.button.config(state="disabled")
        self.root.mainloop()


    # Program moves on to displaying next strategy
    def next_strategy(self):
        self.button.config(state="disabled")
        self.index = 0
        self.curr_strategy += 1
        self.clock()


    # Function called periodically after 1 second intervals, updates the grid UI with new positions
    def clock(self):
        # If all moves of a strategy have been displayed
        if self.index == len(self.game.positions[self.curr_strategy][0]):
            # If all strategies have been displayed
            if self.curr_strategy == len(self.game.positions)-1:
                self.button.config(command=quit)
                self.button.config(text="Quit")
                self.button.config(state="normal")
            else:
                self.button.config(state="normal")
            return
        # Clear old positions, add new positions
        player1_positions, player2_positions = self.game.positions[self.curr_strategy]
        new_x1, new_y1 = player1_positions[self.index]
        new_x2, new_y2 = player2_positions[self.index]
        old_x1, old_y1, old_x2, old_y2 = 0, 0, 0, 0
        if self.index != 0:
            old_x1, old_y1 = player1_positions[self.index-1]
            old_x2, old_y2 = player2_positions[self.index-1]
        else:
            if self.curr_strategy != 0:
                old_x1, old_y1 = self.game.positions[self.curr_strategy-1][0][-1]
                old_x2, old_y2 = self.game.positions[self.curr_strategy-1][1][-1]  
        self.game.data[old_y1][old_x1] = 0
        self.game.data[old_y2][old_x2] = 0
        self.game.data[self.game.pos3[1]][self.game.pos3[0]] = -2
        self.game.data[new_y1][new_x1] = 1
        self.game.data[new_y2][new_x2] = 2
        self.grid.update(self.game.data)
        self.index+=1
        self.root.after(1000, self.clock)


# Reads a file and returns its contents
def read_file(filename):
    with open(filename) as f:
        contents = f.read()
    return contents


# Creates a game instance using contents of input file
def create_game(contents):
    lines = contents.split("\n")
    m, n, k, p =  map(int, lines[0].split(" "))
    x1, y1, x2, y2, x3, y3 = map(int, lines[1].split(" "))
    obs = set()
    for i in range(p):
        x, y = map(int, lines[i+2].split(" "))
        obs.add((x, y))
    game = Game(m, n, k, p, (x1, y1), (x2,  y2), (x3, y3), obs)
    return game


# Read input and out files, create game instance and visualize the admissible strategies using tkinter
input_filename = sys.argv[1]
output_filename = sys.argv[2]
contents = read_file(input_filename)
game = create_game(contents)
contents = read_file(output_filename)
if re.match(r"^\s*$", contents):
    print("No Admissible Strategy Found!")
else:
    game.set_positions(contents)
    visualizer = Visualizer(game)
    visualizer.add_components()

# Note that the visualization stops whenever Robot 1 reaches a node in the winning set