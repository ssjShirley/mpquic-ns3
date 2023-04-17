## figure 13 unstable received bytes
import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = "Times New Roman"

def drawU(i):
    file = open('schedulerU-79'+'/scheduler'+str(i)+'-queue.txt', 'r')
    finish_time = float(file.readlines()[-1].split('\t')[0])
    file = open('schedulerU-79'+'/scheduler'+str(i)+'-rx.txt', 'r')
    lines = file.readlines()
    goodput = []
    c_time = []
    for line in lines:
        temp = line.split('\t')
        c_time.append(float(temp[1]))
        c_time.append(float(temp[2])/1024/1024)
        c_time.append(temp[0])
        c_time.append(i)
        goodput.append(c_time)
        if (c_time[0] > finish_time):
            break
        c_time = []
    dataTotal = pd.DataFrame (goodput, columns = ['Time', 'goodput', 'path', 'scheduler'])        

    toDrop =dataTotal.loc[dataTotal["path"] == '3'].index.tolist()
    one = dataTotal.drop(toDrop)
    toDrop =dataTotal.loc[dataTotal["path"] == '1'].index.tolist()
    two = dataTotal.drop(toDrop)

    plt.figure(figsize=(10, 10))
    plt.grid(linestyle="--") 
    ax = plt.gca()

    plt.plot(one['Time'], one['goodput'], color="green", label="P0", linewidth=6)
    plt.plot(two['Time'], two['goodput'], color="red", label="P1", linewidth=6)

    plt.xticks(fontsize=20, fontweight='bold') 
    plt.yticks(fontsize=20, fontweight='bold')
    plt.ylabel("Received Bytes (MB)", fontsize=24, fontweight='bold')
    plt.xlabel("Time (s)", fontsize=24, fontweight='bold')
    plt.xlim(0, 14)
    plt.ylim(0, 5)


    plt.legend(loc=0, numpoints=1)
    leg = plt.gca().get_legend()
    ltext = leg.get_texts()
    plt.setp(ltext, fontsize=20, fontweight='bold')

    plt.savefig('./received_bytes_scheduler_unstable'+str(i)+'.pdf', format='pdf',bbox_inches ="tight",pad_inches = 0,transparent = False) 
    plt.savefig('./received_bytes_scheduler_unstable'+str(i)+'.png', format='png')  
    plt.close()

for i in range(0,5):
    drawU(i)