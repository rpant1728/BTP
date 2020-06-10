import random

SIZE = 10

f = open("inputs/random_input.txt","w+")

m = int(SIZE*random.random())
n = int(SIZE*random.random())
k = int(SIZE*random.random())
p = int(5*random.random())

f.write("{} {} {} {}\n".format(m, n, k, p))

x1 = int(m*random.random())
y1 = int(n*random.random())
x2 = int(m*random.random())
y2 = int(n*random.random())
x3 = int(m*random.random())
y3 = int(n*random.random())

f.write("{} {} {} {} {} {}\n".format(x1, y1, x2, y2, x3, y3))

for i in range(p):
    x = int(m*random.random())
    y = int(n*random.random())
    f.write("{} {}\n".format(x, y))


f.close()