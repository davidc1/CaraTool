import sys,os

import ROOT

from larlite import larlite as ll

from ROOT import vx

import numpy as np

a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())

ENERGY_V = [50,100,200,300,400,500,600]

FRAC_V = [[],[],[],[],[],[],[]]


SPACING = 3.0

OUTFILE = open('ana_cluster.txt','a')

ana = vx.VoxelizeTracks(SPACING)
ana.setVerbose(False)

MIP = 2.105168 / SPACING



for i,energy in enumerate(ENERGY_V):

    INPATH = '/a/data/amsterdam/dcaratelli/Geant/electrons/%03iMeV/'%energy

    infiles = os.listdir(INPATH)

    ctr = 0

    for fname in infiles:

        #if (ctr >= 100): break

        if os.path.isfile(INPATH+fname):

            if (fname.find("~") >= 0): continue
            
            fin = open(INPATH+fname,'r')
            
            print 'Reading file %s'%(INPATH+fname)
            
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
                    
            if (ana.TotalEnergy() < (energy * 0.8)): continue

            f10 = ana.Cluster(10,1000.)
            f20 = ana.Cluster(20,1000.)

            FRAC_V[i].append( f10 / f20 )

            ana.Clear()

            ctr += 1

import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 16})

COLORS = ['b','c','m','r','y','g','k']

fig = plt.figure(figsize=(12,6))
BINS = np.linspace(0.0,1.0,100)
for i,energy in enumerate(ENERGY_V):
    avg = np.average(np.array(FRAC_V[i]))
    plt.hist(FRAC_V[i],bins=BINS,histtype='step',lw=2,color=COLORS[i],label='Energy %03i MeV : avg = %.02f'%(energy,avg))
plt.grid()
plt.legend(loc=2,fontsize=16)
plt.xlabel('Frac (10,100) / (20,100)',fontsize=20,fontweight='bold')
plt.savefig('frac.png')
plt.show()

sys.exit()

