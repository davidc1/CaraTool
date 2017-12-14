import sys,os

import ROOT

from larlite import larlite as ll

import numpy as np

from ROOT import vx



a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())


ENERGY_V = [50,100,200,300,400,500,600]

dedx_v = [[],[],[],[],[],[],[]]



SPACING = 3.0

ana = vx.VoxelizeTracks(SPACING,100)
ana.setVerbose(False)

MIP = 2.105168 / SPACING

ctr = 0

for i,energy in enumerate(ENERGY_V):

    INPATH = '/a/data/amsterdam/dcaratelli/Geant/electrons/%03iMeV/'%energy

    infiles = os.listdir(INPATH)

    for fname in infiles:

        if os.path.isfile(INPATH+fname):

            fin = open(INPATH+fname,'r')

            for line in fin:

                words = line.split()

                if (len(words) < 8): continue

                if (words[0].isdigit() == False):
                    continue
            
                try:

                    ID   = int(words[0])
                    edep = float(words[1])
                    xpos0 = float(words[2]) + 10000
                    ypos0 = float(words[3]) + 10000
                    zpos0 = float(words[4]) + 10000
                    xpos1 = float(words[5]) + 10000
                    ypos1 = float(words[6]) + 10000
                    zpos1 = float(words[7]) + 10000
                    
                    ana.AddG4Step(xpos0,ypos0,zpos0,xpos1,ypos1,zpos1,edep)

                except:
                    print 'error @ file %s/%s @ line %s'%(INPATH,fname,line)

            dedx = ana.dEdx(30.)
            print 'dEdx = %.02f'%dedx
            dedx_v[i].append( dedx )
            
            ana.Clear()

            ctr += 1

import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 16})

COLORS = ['b','c','m','r','y','g','k']

fig = plt.figure(figsize=(12,6))
BINS = np.linspace(0.3,7.0,100)
for i,energy in enumerate(ENERGY_V):
    avg = np.average(np.array(dedx_v[i]))
    plt.hist(dedx_v[i],bins=BINS,histtype='step',lw=2,color=COLORS[i],label='Energy : %03i MeV. Avg. dE/dx : %.02f'%(energy,avg))
plt.grid()
plt.legend(loc=1,fontsize=16)
plt.xlabel('dE/dx [ MeV / cm ]',fontsize=20,fontweight='bold')
plt.savefig('dedx.png')
plt.show()

sys.exit()
