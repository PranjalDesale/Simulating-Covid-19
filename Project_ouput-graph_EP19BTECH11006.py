import matplotlib.pyplot as plt
import csv

time=[]
s=[]
i=[]
r=[]


with open('output.txt', 'r') as outputfile:
    plots= csv.reader(outputfile, delimiter=' ')
    for row in plots:
        time.append(int(row[0]))
        s.append(int(row[1]))
        i.append(int(row[2]))
        r.append(int(row[3]))

scale=10 #can change scale on x axis using this
xmin, xmax = plt.xlim()
plt.xlim(xmin*scale,xmax*scale)

plt.plot(time, s, 'g') # plotting time, s separately 
plt.plot(time, i, 'r') # plotting time, i separately 
plt.plot(time, r, 'b') # plotting time, r separately 

plt.title('Data from pandemic modelling')

plt.xlabel('Time')
plt.ylabel('Number')

plt.show()