import sys,os

import numpy as np
import matplotlib.pyplot as plt

lines = []

fin = open(sys.argv[-1],'r')

dedx_v = []

for line in fin:

    words = line.split()

    ID   = int(words[0])
    edep = float(words[1])
    dx   = float(words[8])

    if (dx == 0): continue

    dedx_v.append(edep/(dx/10.))

fig = plt.figure(figsize=(10,10))
plt.hist(dedx_v,bins=np.linspace(0,5,50),histtype='step',lw=2)
plt.grid()
plt.show()
    

