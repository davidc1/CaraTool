import sys,os

import ROOT

from larlite import larlite as ll

from ROOT import vx

a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())

fin = open(sys.argv[-1],'r')

ana = vx.VoxelizeTracks(3)
ana.setVerbose(False)

ctr = 0

for line in fin:

    words = line.split()

    ID   = int(words[0])
    edep = float(words[1])
    xpos0 = float(words[2]) + 10000
    ypos0 = float(words[3]) + 10000
    zpos0 = float(words[4]) + 10000
    xpos1 = float(words[5]) + 10000
    ypos1 = float(words[6]) + 10000
    zpos1 = float(words[7]) + 10000

    #print 'new segment : [%.02f,%.02f,%.02f] -> [%.02f,%.02f,%.02f]'%(xpos0,ypos0,zpos0,xpos1,ypos1,zpos1)

    ana.AddG4Step(xpos0,ypos0,zpos0,xpos1,ypos1,zpos1,edep)

    ctr += 1;

print ana.getSummary()

print '@ 0.1 MeV threshold recover : %.02f of energy'%ana.applyThreshold(0.1)
print '@ 0.5 MeV threshold recover : %.02f of energy'%ana.applyThreshold(0.5)
print '@ 1.0 MeV threshold recover : %.02f of energy'%ana.applyThreshold(1.0)
    
sys.exit()
