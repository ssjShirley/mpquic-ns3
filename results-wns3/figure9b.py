## figure 9b stable instantaneous throughput
import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = "Times New Roman"

def help(i):
    file = open('scheduler-133'+'/scheduler'+str(i)+'-queue.txt', 'r')
    finish_time = float(file.readlines()[-1].split('\t')[0])
    file = open('scheduler-133'+'/scheduler'+str(i)+'-rx.txt', 'r')
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


rr = help(0)  
minrtt = help(1)
blest = help(2)  
ecf = help(3)
peek = help(4)

plt.figure(figsize=(8, 6))
plt.grid(linestyle="--")
ax = plt.gca()

plt.plot(rr['Time'], rr['goodput'], color="green", label="RR", linewidth=4)
plt.plot(minrtt['Time'], minrtt['goodput'], color="red", label="MRTT", linewidth=4)
plt.plot(blest['Time'], blest['goodput'], color="blue", label="BLEST", linewidth=4)
plt.plot(ecf['Time'], ecf['goodput'], color="c", label="ECF", linewidth=4)
plt.plot(peek['Time'], peek['goodput'], color="orange", label="Peekaboo", linewidth=4)

plt.xticks(fontsize=20, fontweight='bold')
plt.yticks(fontsize=20, fontweight='bold')
plt.ylabel("Instantaneous Throughput (Mbps)", fontsize=20, fontweight='bold')
plt.xlabel("Time (s)", fontsize=20, fontweight='bold')
plt.ylim(2, 12)

plt.legend(loc=0, numpoints=1)
leg = plt.gca().get_legend()
ltext = leg.get_texts()
plt.setp(ltext, fontsize=20, fontweight='bold') 

plt.savefig('./ins_throughput_scheduler.pdf', format='pdf')
plt.savefig('./ins_throughput_scheduler.png', format='png') 
plt.close()
