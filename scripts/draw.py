## instantaneous goodput

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

schedulerTypes = [0]

topdir = "../results-wns3/"

def helpone(num,i):
    # dir = 'dynamic0-67'
    # dir = 'dynamic1-16'
    dir = '../results-wns3/one-'+str(num)
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
    dir = '../results-wns3/two-'+str(num)
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
    dir = '../results-wns3/four-'+str(num)
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

for j in range(1,51):
    one = helpone(j,0)
    two = helptwo(j,0)
    four = helpfour(j,0)  

    plt.figure(figsize=(8, 5))
    plt.grid(linestyle="--")  # 设置背景网格线为虚线
    ax = plt.gca()
    # ax.spines['top'].set_visible(False)  # 去掉上边框
    # ax.spines['right'].set_visible(False)  # 去掉右边框


    plt.plot(one['Time'], one['goodput'], color="green", label="one", linewidth=4)
    plt.plot(two['Time'], two['goodput'], color="red", label="two", linewidth=4)
    plt.plot(four['Time'], four['goodput'], color="blue", label="four", linewidth=4)
    # plt.plot(ecf['Time'], ecf['goodput'], color="c", label="ECF", linewidth=4)
    # plt.plot(mab['Time'], mab['goodput'], color="orange", label="MAB", linewidth=4)

    # group_labels = ['1-1', '1-2', '1-3', '1-4', '1-5', '1-6', '1-7', '1-8', '1-9']  # x轴刻度的标识
    plt.xticks(fontsize=20, fontweight='bold')  # 默认字体大小为10
    plt.yticks(fontsize=20, fontweight='bold')
    plt.title(j, fontsize=12, fontweight='bold')  # 默认字体大小为12
    plt.ylabel("Instantaneous Goodput (Mbps)", fontsize=20, fontweight='bold')
    plt.xlabel("Time (s)", fontsize=20, fontweight='bold')

    plt.legend(loc=0, numpoints=1)
    leg = plt.gca().get_legend()
    ltext = leg.get_texts()
    plt.setp(ltext, fontsize=20, fontweight='bold')  # 设置图例字体的大小和粗细

    plt.savefig('plot/'+str(j)+'.png', format='png') 
    plt.close()
    # plt.show()


# dir = str(sys.argv[1])

 


# plt.figure(figsize=(10, 5))
# plt.grid(linestyle="--")  # 设置背景网格线为虚线
# ax = plt.gca()
# ax.spines['top'].set_visible(False)  # 去掉上边框
# ax.spines['right'].set_visible(False)  # 去掉右边框


# plt.plot(rr['Time'], rr['goodput'], color="green", label="RR", linewidth=2)
# plt.plot(minrtt['Time'], minrtt['goodput'], color="red", label="Min-RTT", linewidth=2)
# plt.plot(blest['Time'], blest['goodput'], color="blue", label="BLEST", linewidth=2)
# plt.plot(ecf['Time'], ecf['goodput'], color="yellow", label="ECF", linewidth=2)
# # plt.plot(pee['Time'], pee['goodput'], color="black", label="Peekaboo", linewidth=2)
# plt.plot(mab['Time'], mab['goodput'], color="orange", label="MAB", linewidth=2)

# # group_labels = ['1-1', '1-2', '1-3', '1-4', '1-5', '1-6', '1-7', '1-8', '1-9']  # x轴刻度的标识
# plt.xticks(fontsize=12, fontweight='bold')  # 默认字体大小为10
# plt.yticks(fontsize=12, fontweight='bold')
# plt.title(dir, fontsize=12, fontweight='bold')  # 默认字体大小为12
# plt.ylabel("Instantaneous Goodput (Mbps)", fontsize=15, fontweight='bold')
# plt.xlabel("Time (s)", fontsize=15, fontweight='bold')
# # plt.xlim(0.9, 6.1)  # 设置x轴的范围
# # plt.ylim(0, 5)

# # # plt.legend()          #显示各曲线的图例
# plt.legend(loc=0, numpoints=1)
# # leg = plt.gca().get_legend()
# # ltext = leg.get_texts()
# # plt.setp(ltext, fontsize=12, fontweight='bold')  # 设置图例字体的大小和粗细

# plt.savefig(dir+'.png', format='png')  # 建议保存为svg格式,再用在线转换工具转为矢量图emf后插入word中
# plt.show()