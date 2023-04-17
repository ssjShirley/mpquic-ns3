## figure 8b cwnd with OLIA
import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = "Times New Roman"

i = 0
dir = 'cwnd-1-1'
file = open(dir+'/scheduler'+str(i)+'-cwnd-change-0.txt', 'r')
lines = file.readlines()
goodput = []
c_time = []
for line in lines:
    temp = line.split('\t')
    c_time.append(float(temp[0]))
    c_time.append(int(temp[1])/1460)
    c_time.append(int(temp[2])/1460)
    goodput.append(c_time)
    c_time = []
dataTotal0 = pd.DataFrame (goodput, columns = ['Time', 'cwnd1', 'cwnd2'])        


file = open(dir+'/scheduler'+str(i)+'-cwnd-change-1.txt', 'r')
lines = file.readlines()
goodput = []
c_time = []
for line in lines:
    temp = line.split('\t')
    c_time.append(float(temp[0]))
    c_time.append(int(temp[1])/1460)
    c_time.append(int(temp[2])/1460)
    goodput.append(c_time)
    c_time = []
dataTotal1 = pd.DataFrame (goodput, columns = ['Time', 'cwnd1', 'cwnd2'])  

plt.figure(figsize=(8, 6))
plt.grid(linestyle="--") 
ax = plt.gca()

plt.plot(dataTotal0['Time'], dataTotal0['cwnd2'], color="green", label="P0", linewidth=4)
plt.plot(dataTotal1['Time'], dataTotal1['cwnd2'], color="red", label="P1", linewidth=4)

plt.xticks(fontsize=18, fontweight='bold')  
plt.yticks(fontsize=18, fontweight='bold')
plt.ylabel("Congestion Window (Segment)", fontsize=20, fontweight='bold')
plt.xlabel("Time (s)", fontsize=20, fontweight='bold')


plt.legend(loc=0, numpoints=1)
leg = plt.gca().get_legend()
ltext = leg.get_texts()
plt.setp(ltext, fontsize=20, fontweight='bold') 

plt.savefig('./cwnd_OLIA.pdf', format='pdf')
plt.savefig('./cwnd_OLIA.png', format='png')
plt.show()