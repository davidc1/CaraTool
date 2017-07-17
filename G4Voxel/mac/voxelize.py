import sys,os

import ROOT

from larlite import larlite as ll

from ROOT import vx

a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())

ana = vx.VoxelizeTracks(3)
ana.setVerbose(True)

ana.AddG4Step(1.2,4.3,5.7,1.3,4.4,20.8,100.)
