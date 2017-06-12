import ROOT
from larlite import larlite as ll
import numpy as np
import matplotlib.pyplot as plt

f = ROOT.TFile("ll_nu.root")

simch_tree = f.Get("simch_largeant_tree")
part_tree  = f.Get("mcpart_largeant_tree")

N = simch_tree.GetEntries()

PartColor = {}
PartColor[22]   = "r"
PartColor[2112] = "g"
PartColor[2212] = "k"
PartColor[13] = "y"
PartColor[-13] = "y"

origin = []

for n in xrange(N):

    simch_tree.GetEntry(n)
    part_tree.GetEntry(n)

    ev_simch = simch_tree.simch_largeant_branch
    ev_part  = part_tree.mcpart_largeant_branch

    nsimch = ev_simch.size()

    if (nsimch == 0): continue

    fig = plt.figure(figsize=(12,12))

    fig.gca()

    ide_x = []
    ide_y = []
    ide_z = []

    origin = [-100,-200,2000]
    
    for i in xrange(nsimch):
        
        simch =ev_simch.at(i)

        ide_v = simch.TrackIDsAndEnergies(0,9600)

        for j in xrange(ide_v.size()):
            
            ide = ide_v.at(j)

            ide_x.append(float(ide.x))
            ide_y.append(float(ide.y))
            ide_z.append(float(ide.z))

    plt.scatter(ide_z,ide_x,color='b')

    for i in xrange(ev_part.size()):

        part = ev_part.at(i)

        pdg = part.PdgCode()

        if ( (part.StatusCode() == 1) and (part.Process() == "primary") ):
            
            nuvtx = part.Trajectory().at(0)
            
            origin = [nuvtx.X(),nuvtx.Y(),nuvtx.Z()]
            print 'Origin @ ',origin

        if (pdg in PartColor):

            COL = PartColor[pdg]

            # get path
            p_x_v = []
            p_y_v = []
            p_z_v = []

            traj = part.Trajectory()

            for k in xrange(traj.size()):
                
                step = traj.at(k)

                p_x_v.append( step.X() )
                p_y_v.append( step.Y() )
                p_z_v.append( step.Z() )

            plt.plot(p_z_v,p_x_v,'--',color=COL, lw=2)

    if ( (origin[0] < 0) or (origin[0] > 256.) or (origin[1] < -116) or (origin[1] > 116) or (origin[2] < 0) or (origin[2] > 1036.) ):
        continue

    plt.xlim([ origin[2]-50, origin[2]+50 ])
    plt.ylim([ origin[0]-50, origin[0]+50 ])
    plt.grid()
    plt.savefig("ev_%03i.png"%n)


