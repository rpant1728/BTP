x=input()

n=x.split(" ")[1][:-1]

edgeList = []
vertexList = []

print(int(n)+1)

for i in range(int(n)+1):
	x=input()
	xList=x.split(" ")
	vertexList.append(xList[0] + " " + str(int(xList[2])+1) + " " + xList[1])
	for j in xList[3].split(","):
		edgeList.append([xList[0],j])

print(len(edgeList))

for v in vertexList:
	print(v)

for i,j in edgeList:
	print(i,j)