import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

Mode=["Linear","MThread","DMThread"]
sizes=[10,25,50,75,100,125,150,175,200,225,250]
individuals=[1,2,4,6,8,10,12,14,16]
threadQ=[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]
data = {}

# graphs
sizeAnalisis = 125 # should be 200
indAnalisis = 6
biggestSize = 125
biggestInd = 16

for x in Mode:
  data[x] = {}
  #print("Results/" + x)
  for size in sizes:
    data[x][size] = {}
    for ind in individuals:
      if(x == "Linear"):
        data[x][size][ind] = 0
        # once ...
        # print("0:Results/" + x + "/tempTime_"+str(size)+"_"+str(ind))
        file = open("Results/" + x + "/tempTime_"+str(size)+"_"+str(ind))
        total = 0
        counter = 0
        for l in file:
            total += float(l.strip())
            counter+=1
        data[x][size][ind] = (total /counter)

        file.close()
      else:
        data[x][size][ind] = {}
        for tq in threadQ:
          data[x][size][ind][tq] = 0
          # print("Results/" + x + "/tempTime_"+str(size)+"_"+str(ind)+"_"+str(tq))
          file = open("Results/" + x + "/tempTime_"+str(size)+"_"+str(ind)+"_"+str(tq))
          counter = 0
          total = 0
          for l in file:
            total += float(l.strip())
            counter+=1
          data[x][size][ind][tq] =(total /counter)
          
          file.close()

# print(data)

# now we have all the data ready to make the things !!!
# Lets start with the graphs of the normal one

# Usume we have the np

sns.set_theme()

# Make the time cost, from individuals[0] to individuals[n] with size 200 [lines with threads]
# Make the time cost, from size[0] to size[n] with individuals 200 [lines with threads]
# Finally with the end will be used with threads from threadQ[0] to ThreadQ[n] size 200 ind = 0 [horizontal line with 1 thread] [horizontal line with max posible speedup]

# Lastly a latex table with the result condensed in a latex table, with the [250 Size  16 ind] [1thread | 4 | 6 | 8 |] the first will include the normal time, and the rest will be the speedup 250 and 100
# column_names = ["Iterations", "FevBest", "FevWorst", "FevAvg"]
# df0 = pd.DataFrame(columns=column_names)


# first graph

dataLinear = []
dataT = [[],[],[]]
dataDM= [[],[],[]]
threadOptions = [2,4,6,8,10,12,14,16]
for x in individuals:
  # linear
  dataLinear.append(data["Linear"][sizeAnalisis][x])
  # threads
  dataT[0].append(data["MThread"][sizeAnalisis][x][threadOptions[0]])
  dataT[1].append(data["MThread"][sizeAnalisis][x][threadOptions[1]])
  dataT[2].append(data["MThread"][sizeAnalisis][x][threadOptions[2]])
  # halve thing
  dataDM[0].append(data["DMThread"][sizeAnalisis][x][threadOptions[0]])
  dataDM[1].append(data["DMThread"][sizeAnalisis][x][threadOptions[1]])
  dataDM[2].append(data["DMThread"][sizeAnalisis][x][threadOptions[2]])

y = np.array(individuals)
plt.figure(figsize=(10,6))
plt.ylabel("Time (s)")
plt.xlabel("Number of individuals")

points = np.array(dataLinear)
plt.plot(y, points, color = "r")


points = np.array(dataT[0])
plt.plot(y,points, color = "green", linestyle = "--")

points = np.array(dataT[1])
plt.plot(y,points, color = "green", linestyle = "-.")

points = np.array(dataT[2])
plt.plot(y,points, color = "green", linestyle = ":")


points = np.array(dataDM[0])
plt.plot(y,points, color = "blue", linestyle = "--")

points = np.array(dataDM[1])
plt.plot(y,points, color = "blue", linestyle = "-.")

points = np.array(dataDM[2])
plt.plot(y,points, color = "blue", linestyle = ":")

# Names for each line
plt.legend(['Sequential A*', 'Parallel A* (2)','Parallel A* (4)','Parallel A* (6)', 'Parallel Bidirectional A* (2)','Parallel Bidirectional A* (4)','Parallel Bidirectional A* (6)'])
# where to save the image
plt.savefig("TimeVSindividuals.png")


# second graph
dataLinear = []
dataT = [[],[],[]]
dataDM= [[],[],[]]
threadOptions = [2,4,6,8,10,12,14,16]
for x in sizes:
  # linear
  dataLinear.append(data["Linear"][x][indAnalisis])
  # threads
  dataT[0].append(data["MThread"][x][indAnalisis][threadOptions[0]])
  dataT[1].append(data["MThread"][x][indAnalisis][threadOptions[1]])
  dataT[2].append(data["MThread"][x][indAnalisis][threadOptions[2]])
  # halve thing
  dataDM[0].append(data["DMThread"][x][indAnalisis][threadOptions[0]])
  dataDM[1].append(data["DMThread"][x][indAnalisis][threadOptions[1]])
  dataDM[2].append(data["DMThread"][x][indAnalisis][threadOptions[2]])
  
plt.figure(figsize=(10,6))
plt.ylabel("Time (s)")
plt.xlabel("Map Size")
y = np.array(sizes)

points = np.array(dataLinear)
plt.plot(y,points, color = "r")


points = np.array(dataT[0])
plt.plot(y,points, color = "green", linestyle = "--")

points = np.array(dataT[1])
plt.plot(y,points, color = "green", linestyle = "-.")

points = np.array(dataT[2])
plt.plot(y,points, color = "green", linestyle = ":")

points = np.array(dataDM[0])
plt.plot(y,points, color = "blue", linestyle = "--")

points = np.array(dataDM[1])
plt.plot(y,points, color = "blue", linestyle = "-.")

points = np.array(dataDM[2])
plt.plot(y,points, color = "blue", linestyle = ":")

# Names for each line
plt.legend(['Sequential A*', 'Parallel A* (2)','Parallel A* (4)','Parallel A* (6)', 'Parallel Bidirectional A* (2)','Parallel Bidirectional A* (4)','Parallel Bidirectional A* (6)'])
# where to save the image
plt.savefig("TimeVSMapSize.png")


# third graph
dataLinear = []
dataT = []
dataDM= []
for x in threadQ:
  dataLinear.append(data["Linear"][biggestSize][biggestInd]);
  # threads
  dataT.append(data["MThread"][biggestSize][biggestInd][x])
  # halve thing
  dataDM.append(data["DMThread"][biggestSize][biggestInd][x])
  
y = np.array(threadQ)
plt.figure(figsize=(10,6))
plt.ylabel("Time (s)")
plt.xlabel("Thread quantity")

points = np.array(dataLinear)
plt.plot(y,points, color = "r")

points = np.array(dataT)
plt.plot(y,points, color = "green")

points = np.array(dataDM)
plt.plot(y,points, color = "blue")

# points = np.array(dataDM)
# plt.plot(y, points, color = "blue", linestyle = "-.")

# plt.axhline(dataLinear)
# Names for each line
plt.legend(['Sequential A*', 'Parallel A*', "Parallel Bidirectional A*"])
# where to save the image
plt.savefig("TimeVSThreads.png")
# line if necesary
# plt.ylim(-20000, 20000)


# speedup 
lines = ""

for x in threadQ:
  seq = round(data["Linear"][biggestSize][2],4)
  speedupP = round(seq/data["MThread"][biggestSize][2][x],4)
  speedupPB = round(seq/ data["DMThread"][biggestSize][2][x],4)
  lines += " {} & {} & {} & {} \n".format("a",seq, speedupP, speedupPB)

for x in threadQ:
  seq = round(data["Linear"][biggestSize][biggestInd],4)
  speedupP = round(seq/ data["MThread"][biggestSize][biggestInd][x],4)
  speedupPB = round(seq/ data["DMThread"][biggestSize][biggestInd][x],4)
  lines += " {} & {} & {} & {} \n".format("a",seq, speedupP, speedupPB)


lt = open("LatexTable.txt","w")
lt.write(lines)
lt.close()

