import sys,os

import ROOT

from larlite import larlite as ll

from ROOT import vx

import numpy as np

a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())

ENERGY_V = [600]

SPACING = 3.0

OUTFILE = open('ana_cluster.txt','a')

ana = vx.VoxelizeTracks(SPACING)
ana.setVerbose(False)

MIP = 2.105168 / SPACING

RL = 141.67
EC = 32.84

conversion_d = []

ENERGY_V = [600]

for i,energy in enumerate(ENERGY_V):

    INPATH = '/a/data/amsterdam/dcaratelli/Geant/gamma/%03iMeV/'%energy

    infiles = os.listdir(INPATH)

    ctr = 0

    for fname in infiles:

        if (ctr >= 1000): break

        if os.path.isfile(INPATH+fname):

            if (fname.find("~") >= 0): continue
            
            fin = open(INPATH+fname,'r')
            
            print 'Reading file %s'%(INPATH+fname)
            
            for line in fin:

                words = line.split()
                
                if (len(words) < 8): continue

                if (words[0].isdigit() == False):
                    continue
            
                ID   = int(words[0])
                edep = float(words[1])
                xpos0 = float(words[2])
                ypos0 = float(words[3])
                zpos0 = float(words[4])
                xpos1 = float(words[5])
                ypos1 = float(words[6])
                zpos1 = float(words[7])

                dgamma = np.sqrt(xpos0**2+ypos0**2+zpos0**2)
                conversion_d.append(dgamma/10.)

                break
                

import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 16})

fig = plt.figure(figsize=(6,6))
BINS = np.linspace(0,100,25)
plt.hist(conversion_d,bins=BINS,histtype='step',lw=2)
plt.grid()
plt.legend(loc=2,fontsize=16)
#plt.savefig('showerprofile.png')
plt.show()

sys.exit()

