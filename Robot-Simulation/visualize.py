from tkinter import *
import datetime
import time
import random

colorMap = {
    -2: "green",
    -1: "black",
    0: "white",
    1: "red",
    2: "blue"
}

class Table:  
    def __init__(self,root): 
        self.entries = []
        
    def add(self):
        for i in range(n): 
            temp = []
            for j in range(m): 
                entry = Entry(root, width=20, bg=colorMap[data[i][j]]) 
                entry.grid(row=i, column=j) 
                temp.append(entry)
            self.entries.append(temp)

    def update(self):
        for i in range(n): 
            for j in range(m): 
                self.entries[i][j].configure(bg=colorMap[data[i][j]])

with open("inputs/input2.txt") as f:
    contents = f.read()

lines = contents.split("\n")
m, n, k, p =  map(int, lines[0].split(" "))
x1, y1, x2, y2, x3, y3 = map(int, lines[1].split(" "))

data = []

for i in range(n):
    temp = []
    for j in range(m):
        temp.append(0)
    data.append(temp)


for i in range(p):
    x, y = map(int, lines[i+2].split(" "))
    data[y][x] = -1

data[y1][x1] = 1
data[y2][x2] = 2
data[y3][x3] = -2

# with open("outputs/minimal_pws.txt") as f:
#     contents = f.read()

# lines = contents.split("\n")

# # for line in lines:
# player1_positions = [(x1, y1)]
# player2_positions = [(x2, y2)]

# states = lines[0].split(" : ")[2].split(" ")
# for state in states:
#     x1, y1, x2, y2, k = map(int, state.split("~"))
#     player1_positions.append((x1, y1))
#     player2_positions.append((x2, y2))

root = Tk() 
t = Table(root) 
t.add()

# index=0

# def clock():
#     global index
#     if(index == len(player1_positions)):
#         return
#     new_x1, new_y1 = player1_positions[index]
#     new_x2, new_y2 = player2_positions[index]
#     old_x1, old_y1 = player1_positions[index-1]
#     old_x2, old_y2 = player2_positions[index-1]
#     data[old_y1][old_x1] = 0
#     data[old_y2][old_x2] = 0
#     data[new_y1][new_x1] = 1
#     data[new_y2][new_x2] = 2
#     t.update()
#     index+=1
#     root.after(1000, clock)

# clock()

root.mainloop()
