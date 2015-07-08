#
# Example PyROOT script to run analysis module, ana_base.
# The usage is same for inherited analysis class instance.
#

# Load libraries
import os, ROOT, sys
from ROOT import *
from ROOT import gSystem
import time
import matplotlib.pyplot as plt
import numpy as np

plt.ion()

# function that given hit charge returns marker size
def markerSize(Q):
    return (Q/10.)

gSystem.Load("libLArLite_Base")
gSystem.Load("libLArLite_Analysis")
gSystem.Load("libLArLite_LArUtil")
gSystem.Load("libBasicTool_GeoAlgo")

# Now import ana_processor & your class. For this example, ana_base.
from ROOT import larlite as fmwk

w2cm = larutil.GeometryUtilities.GetME().WireToCm()
t2cm = larutil.GeometryUtilities.GetME().TimeToCm()

# Create ana_processor instance
my_proc=fmwk.ana_processor()

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("hitremover.root")
my_proc.set_output_file("out_hits.root")

# Create analysis class instance. For this example, ana_base.
# To show how one can run multiple analysis modules at once,
# we make multiple ana_base instance.

my_ana = fmwk.TrackHitRemover()
outhitproducer = 'outhits'
my_ana.setOutHitProducer(outhitproducer)
my_ana.setGetShowerHits(True)
my_ana.setShowerProducer("showerreco")
my_proc.add_process(my_ana)
my_proc.set_data_to_write(fmwk.data.kHit,outhitproducer)
#my_proc.run()
#sys.exit()

# track colors
trk_c = ['r','g','c','m','b']

fig, (axU, axV, axY) = plt.subplots(nrows=3,figsize=(15,12))

while my_proc.process_event():

    hits = my_ana.getShrHits()
    out_hit_indices = my_ana.getOutHits()
    hit_t = [[],[],[]]
    hit_w = [[],[],[]]
    hit_s = [[],[],[]]
    axU.clear()
    axV.clear()
    axY.clear()
    fig.gca()
    # loop over all hits and color them blue
    for h in xrange(hits.size()):
    #for h in out_hit_indices:
        pl = hits.at(h).View()
        hit_w[pl].append(hits.at(h).WireID().Wire*w2cm)
        hit_t[pl].append(hits.at(h).PeakTime()*t2cm)
        hit_s[pl].append(markerSize(hits.at(h).Integral()))
    axU.scatter(hit_w[0],hit_t[0],s=hit_s[0],color='b')
    axV.scatter(hit_w[1],hit_t[1],s=hit_s[1],color='b')
    axY.scatter(hit_w[2],hit_t[2],s=hit_s[2],color='b')

    # now get hits associated with track-like clusters and add them
    shr_hits = my_ana.getShrHitIndices()

    ctr = 0
    for t in xrange(shr_hits.size()):
        trk_hit_t = [[],[],[]]
        trk_hit_w = [[],[],[]]
        trk_hit_s = [[],[],[]]
        for th in xrange(shr_hits[t].size()):
            # determine plane
            pl = hits.at(shr_hits[t][th]).View()
            trk_hit_w[pl].append(hits.at(shr_hits[t][th]).WireID().Wire*w2cm)
            trk_hit_t[pl].append(hits.at(shr_hits[t][th]).PeakTime()*t2cm)
            trk_hit_s[pl].append(markerSize(hits.at(shr_hits[t][th]).Integral()))

        axU.scatter(trk_hit_w[0],trk_hit_t[0],s=trk_hit_s[0],color=trk_c[ctr%5])
        axV.scatter(trk_hit_w[1],trk_hit_t[1],s=trk_hit_s[1],color=trk_c[ctr%5])
        axY.scatter(trk_hit_w[2],trk_hit_t[2],s=trk_hit_s[2],color=trk_c[ctr%5])
        ctr += 1

    plt.show()
    usrinput = raw_input("Hit Enter: next evt  ||  q: exit viewer\n")
    if ( usrinput == "q" ):
        sys.exit(0)

