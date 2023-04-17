## figure 7b instantaneous throughput

import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = "Times New Roman"

schedulerTypes = [0]

topdir = "./"

def helpone(num,i):
    dir = topdir+'one-'+str(num)
    file = open(dir+'/scheduler'+str(i)+'-queue.txt', 'r')
    finish_time = float(file.readlines()[-1].split('\t')[0])
    file = open(dir+'/scheduler'+str(i)+'-rx.txt', 'r')
    lines = file.readlines()
    goodput = []
    c_time = []
    for line in lines:
        temp = line.split('\t')
        if (temp[0] == '1' and c_time == []):
            c_time.append(float(temp[1]))
            c_time.append(float(temp[5]))
            c_time.append(i)
            goodput.append(c_time)
            if (c_time[0] > finish_time):
                break
            c_time = []
    dataTotal = pd.DataFrame (goodput, columns = ['Time', 'goodput', 'scheduler'])        
    return dataTotal

def helptwo(num,i):
    dir = topdir+'two-'+str(num)
    file = open(dir+'/scheduler'+str(i)+'-queue.txt', 'r')
    finish_time = float(file.readlines()[-1].split('\t')[0])
    file = open(dir+'/scheduler'+str(i)+'-rx.txt', 'r')
    lines = file.readlines()
    goodput = []
    c_time = []
    for line in lines:
        temp = line.split('\t')
        if (temp[0] == '1' and c_time == []):
            c_time.append(float(temp[1]))
            c_time.append(float(temp[5]))
        if (temp[0] == '3' and len(c_time) == 2):
            c_time[1] = c_time[1] + float(temp[5])
            c_time.append(i)
            goodput.append(c_time)
            if (c_time[0] > finish_time):
                break
            c_time = []
    dataTotal = pd.DataFrame (goodput, columns = ['Time', 'goodput', 'scheduler'])        
    return dataTotal


def helpfour(num,i):
    dir = topdir+'four-'+str(num)
    file = open(dir+'/scheduler'+str(i)+'-queue.txt', 'r')
    finish_time = float(file.readlines()[-1].split('\t')[0])
    file = open(dir+'/scheduler'+str(i)+'-rx.txt', 'r')
    lines = file.readlines()
    goodput = []
    c_time = []
    for line in lines:
        temp = line.split('\t')
        if (temp[0] == '1' and c_time == []):
            c_time.append(float(temp[1]))
            c_time.append(float(temp[5]))
        if (temp[0] == '3' and len(c_time) == 2):
            c_time[1] = c_time[1] + float(temp[5])
        if (temp[0] == '4' and len(c_time) == 2):
            c_time[1] = c_time[1] + float(temp[5])
        if (temp[0] == '5' and len(c_time) == 2):
            c_time[1] = c_time[1] + float(temp[5])
            c_time.append(i)
            goodput.append(c_time)
            if (c_time[0] > finish_time):
                break
            c_time = []
    dataTotal = pd.DataFrame (goodput, columns = ['Time', 'goodput', 'scheduler'])        
    return dataTotal

## take seed=39 as the example figure
j = 39
one = helpone(j,0)
two = helptwo(j,0)
four = helpfour(j,0)  

plt.figure(figsize=(10, 8))
plt.grid(linestyle="--")  
ax = plt.gca()

plt.plot(one['Time'], one['goodput'], color="green", label="one path", linewidth=4)
plt.plot(two['Time'], two['goodput'], color="red", label="two paths", linewidth=4)
plt.plot(four['Time'], four['goodput'], color="blue", label="four paths", linewidth=4)

plt.xticks(fontsize=24, fontweight='bold')  
plt.yticks(fontsize=24, fontweight='bold')

plt.ylabel("Instantaneous Throughput (Mbps)", fontsize=26, fontweight='bold')
plt.xlabel("Time (s)", fontsize=26, fontweight='bold')

plt.legend(loc=4, numpoints=1)
leg = plt.gca().get_legend()
ltext = leg.get_texts()
plt.setp(ltext, fontsize=26, fontweight='bold')

plt.savefig('./ins_throughput_scalable.pdf', format='pdf')
plt.savefig('./ins_throughput_scalable.png', format='png')
plt.show()
