import sys,os

import ROOT

from larlite import larlite as ll

from ROOT import vx

import numpy as np

a = vx.Voxel(0,0,0,3.0)
print a
print 'voxel [x,y,z] = [%.02f,%.02f,%.02f] w/ half-length %.02f and energy %.02f'%(a.x(),a.y(),a.z(),a.HalfLength(),a.Energy())

frac_01MeV = []
frac_05MeV = []
frac_06MeV = []
frac_07MeV = []
frac_08MeV = []
frac_09MeV = []
frac_10MeV = []

ANGLE_V = np.arange(5,45,1)
RAD_V   = np.arange(20,250,2)
#ANGLE_V = [5,10,15,20,30,45]
#RAD_V   = [20,40,60,100,150,200,300]

FRAC_V = []

for a in enumerate(ANGLE_V):
    
    FRAC_V.append( [] )

    for r in enumerate(RAD_V):

        FRAC_V[-1].append([])

ENERGY = int(sys.argv[-2])
INPATH = sys.argv[-1]
infiles = os.listdir(INPATH)

SPACING = 3.0

OUTFILE = open('ana_cluster.txt','a')

ana = vx.VoxelizeTracks(SPACING)
ana.setVerbose(False)

MIP = 2.105168 / SPACING

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

        if (ana.TotalEnergy() < (ENERGY * 0.8)): continue

        for i,angle in enumerate(ANGLE_V):
            for j,radius in enumerate(RAD_V):

                frac = ana.Cluster(angle,radius*10.)
                
                FRAC_V[i][j].append(frac)

        ana.Clear()

        ctr += 1

for i,angle in enumerate(ANGLE_V):
    for j,radius in enumerate(RAD_V):

        frac_v = FRAC_V[i][j]

        sorted_frac = np.sort(frac_v)
        
        print '@ angle %i and radius %i avg. frac is %.02f'%(angle,radius,np.median(np.array(frac_v)))

        mean = np.mean(sorted_frac)
        std  = np.std(sorted_frac)
        med  = np.median(sorted_frac)
        N    = len(sorted_frac)
        low  = sorted_frac[int(N/6.)]
        high = sorted_frac[int(5*N/6.)]
        diff = high-low
        OUTFILE.write('%i %i %i %.03f %.03f %.03f %.03f \n'%(ENERGY,angle,radius,mean,std,med,diff))
        print 'Angle : %i \t Radius : %i'%(angle,radius)
        print '\t mean : %.03f \t std : %.03f \t median : %.03f \t width : %.03f'%(mean,std,med,diff)
        

OUTFILE.close()
sys.exit(0)

