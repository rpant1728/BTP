# Reads the runtimes from the output file for multiple executions of the code to compute the average runtime.

f = open("outputs/runtimes.txt", "r")
lines = f.read().split("\n")
sum1 = 0
sum2 = 0
i = 0
for line in lines:
    if line == "":
        break
    if(i%2 == 0):
        sum1 += float(line)
    else:
        sum2 += float(line)
    i += 1

print("Average runtime with optimization:- %f" % (2*sum1/len(lines)))
print("Average runtime without optimization:- %f" % (2*sum2/len(lines)))