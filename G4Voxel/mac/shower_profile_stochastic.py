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

RL = 14.167
EC = 32.84

def Profile(path,energy,N,clear=False):


    r_v = np.linspace(0,1500,50)
    r_v = np.array(r_v)
    f_v = []
    for r in r_v:
        f_v.append([])

    INPATH = path+'/%03iMeV/'%energy

    infiles = os.listdir(INPATH)

    ctr = 0

    xstart = 0
    ystart = 0
    zstart = 0

    for fname in infiles:

        if (ctr >= N): break

        if os.path.isfile(INPATH+fname):

            if (fname.find("~") >= 0): continue
            
            fin = open(INPATH+fname,'r')
            
            print 'Reading file %s'%(INPATH+fname)

            linectr = 0

            ana.Clear()

            for line in fin:

                words = line.split()
                
                if (len(words) < 8): continue

                if (words[0].isdigit() == False):
                    continue
            

                try:

                    ID   = int(words[0])
                    edep = float(words[1])
                    xpos0 = float(words[2])# + 10000
                    ypos0 = float(words[3])# + 10000
                    zpos0 = float(words[4])# + 10000
                    xpos1 = float(words[5])# + 10000
                    ypos1 = float(words[6])# + 10000
                    zpos1 = float(words[7])# + 10000

                    if (clear and (linectr == 0) ):
                        print 'reset start @ [%i,%i,%i]'%(int(xpos0),int(ypos0),int(zpos0))
                        xstart = xpos0
                        ystart = ypos0
                        zstart = zpos0

                    xpos0 -= xstart
                    xpos1 -= xstart
                    ypos0 -= ystart
                    ypos1 -= ystart
                    zpos0 -= zstart
                    zpos1 -= zstart

                    linectr += 1
                
                    ana.AddG4Step(xpos0,ypos0,zpos0,xpos1,ypos1,zpos1,edep)

                except:
                    print 'error @ file %s/%s @ line %s'%(INPATH,fname,line)
                    
            ctr += 1
            
            for i,r in enumerate(r_v):
                f_v[i].append(ana.Cluster(200.,r))

    r_v /= 10.

    # get radial profile with band including stochasticity
    f_v_median = []
    f_v_low    = []
    f_v_high   = []
    for i in xrange(len(f_v)):
        vec  = np.array(f_v[i])
        nval = len(vec)
        median = np.median(vec)
        # sort the vector and find value @ 25% and 75% of entries
        vecsorted = sorted(vec)
        flow = vecsorted[int(nval*(1./4))]
        fhih = vecsorted[int(nval*(3./4))]
        f_v_median.append(median)
        f_v_low.append(flow)
        f_v_high.append(fhih)
        print '@ r = %i median = %.02f in range [%.02f,%.02f]'%(r_v[i],median,flow,fhih)

    return r_v,f_v_median,f_v_low,f_v_high



import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 16})

fig = plt.figure(figsize=(6,6))

plt.xlabel('$t = x/X')

#r_v_diff,f_v_diff = Profile('/a/data/amsterdam/dcaratelli/Geant/gamma/'    ,600,1000,clear=True)
#plt.plot(r_v_diff,f_v_diff,'b--',lw=2,label='photons')

#r_v_diff,f_v_diff = Profile('/a/data/amsterdam/dcaratelli/Geant/electrons/',600,1000)
#plt.plot(r_v_diff,f_v_diff,'r--',lw=2,label='electrons')

#r_v_diff,f_v_diff = Profile('/a/data/amsterdam/dcaratelli/Geant/gamma/'    ,600,1000)
#plt.plot(r_v_diff,f_v_diff,'g--',lw=2,label='photons [RL]')

r_v,f_v,fl_v,fh_v = Profile('/a/data/amsterdam/dcaratelli/Geant/electrons/',600,1000)
print 'len : [%i,%i,%i,%i]'%(len(r_v),len(f_v),len(fl_v),len(fh_v))
plt.plot(r_v,f_v,'r--',lw=3,label='median')
plt.fill_between(r_v,fl_v,fh_v, facecolor='r',alpha=0.5,label='50% interval')

#r_v,f_v,fl_v,fh_v = Profile('/a/data/amsterdam/dcaratelli/Geant/gamma/'    ,600,100)
#plt.plot(r_v,f_v,'g--',lw=2,label='photons [RL]')
#plt.fill_between(r_v,fl_v,fh_v, facecolor='g',alpha=0.5)

plt.grid()
plt.legend(loc=4,fontsize=16)
plt.xlabel('Distance [cm]'        ,fontsize=16,fontweight='bold')
plt.ylabel('Frac. of Total Charge',fontsize=16,fontweight='bold')
plt.title('600 MeV Electrons'     ,fontsize=16,fontweight='bold')
plt.savefig('showerprofilestochastic.png')
plt.show()

sys.exit()

