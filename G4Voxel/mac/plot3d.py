import sys,os

import numpy as np
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D

lines = []

fin = open(sys.argv[-1],'r')

trkid = -1

currentinfo = [[],[],[]]

etot = 0

xmin = 1e6
xmax = -1e6
ymin = 1e6
ymax = -1e6
zmin = 1e6
zmax = -1e6

for line in fin:

    words = line.split()

    ID   = int(words[0])
    edep = float(words[1])

    if (edep == 0): continue
    
    xpos0 = float(words[2])
    ypos0 = float(words[3])
    zpos0 = float(words[4])
    xpos1 = float(words[5])
    ypos1 = float(words[6])
    zpos1 = float(words[7])
    
    if (xpos0 < xmin): xmin = xpos0
    if (xpos0 > xmax): xmax = xpos0
    if (ypos0 < ymin): ymin = ypos0
    if (ypos0 > ymax): ymax = ypos0
    if (zpos0 < zmin): zmin = zpos0
    if (zpos0 > zmax): zmax = zpos0

    etot += edep

    if (ID != trkid):

        if (len(currentinfo) != 0):

            lines.append(currentinfo)

        currentinfo = [[],[],[]]
        trkid = ID

    currentinfo[0].append((xpos0+xpos1)/2.)
    currentinfo[1].append((ypos0+ypos1)/2.)
    currentinfo[2].append((zpos0+zpos1)/2.)

print 'found %i tracks '%(len(lines))

print 'total energy = %.02f'%etot

fig = plt.figure()
ax  = fig.gca(projection='3d')

for l in lines:

    ax.plot(l[0],l[1],l[2])

ax.set_xlim(xmin*1.2,xmax*1.2)
ax.set_ylim(ymin*1.2,ymax*1.2)
ax.set_zlim(zmin*1.2,zmax*1.2)
    
plt.show()
    

