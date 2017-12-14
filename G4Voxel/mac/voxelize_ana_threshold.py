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

THRESH_V = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.733,0.766,0.8,0.833,0.866,0.9,1.1,1.5]

FRAC_V = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]

ENERGY = int(sys.argv[-2])
INPATH = sys.argv[-1]
infiles = os.listdir(INPATH)

SPACING = 3.0

OUTFILE = open('ana.txt','a')


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

        print ana.getSummary()

        if (ana.TotalEnergy() < (ENERGY * 0.8)): continue

        for i,thresh in enumerate(THRESH_V):

            frac = ana.applyThreshold(thresh * MIP)
            
            if (frac > 1.0): print 'Error: frac = %.02f'%frac
            
            FRAC_V[i].append(frac)

            #print 'Frac collected @ thresh = %.01f MIP [%.02f MeV] = %.03f'%(thresh,thresh*MIP,frac)

        ana.Clear()

        ctr += 1
#ana.WriteTree()

# sort each array, compute average, mean, std, and interval including half the entries
for i, thresh in enumerate(THRESH_V):

    sorted_frac = np.sort(FRAC_V[i])

    mean = np.mean(sorted_frac)
    std  = np.std(sorted_frac)
    med  = np.median(sorted_frac)
    N    = len(sorted_frac)
    low  = sorted_frac[int(N/6.)]
    high = sorted_frac[int(5*N/6.)]
    diff = high-low
    OUTFILE.write('%i %.03f %.03f %.03f %.03f %.03f \n'%(ENERGY,thresh,mean,std,med,diff))
    print 'Thresh : %.01f'%thresh
    print '\t mean : %.03f \t std : %.03f \t median : %.03f \t width : %.03f'%(mean,std,med,diff)
    

OUTFILE.close()
sys.exit()
