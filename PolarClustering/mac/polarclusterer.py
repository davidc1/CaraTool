import sys
if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)


from larlite import larlite as fmwk

from larlite import larutil

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    fname = sys.argv[x+1]
    my_proc.add_input_file(fname)
    
# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify analysis output root file name
my_proc.set_ana_output_file("");

# Specify data output root file name
my_proc.set_output_file("larlite_mcclusters.root")

polar_clusterer = fmwk.VtxClustering()
polar_clusterer.setHitProducer("gaushit")
polar_clusterer.setVtxProducer("mcroi")
my_proc.add_process(polar_clusterer)

print
print  "Finished configuring ana_processor. Start event loop!"
print

import matplotlib.pyplot as plt
import numpy as np
plt.ion()

t2cm = larutil.GeometryHelper.GetME().TimeToCm()

#fig, ax = plt.subplots(1,1,projection='polar')
axp = plt.subplot(111,projection='polar')
#ax   = plt.subplot(121)

while ( my_proc.process_event() ):

    if (polar_clusterer._has_vtx == False):
        continue
    
    #ax.cla()
    axp.cla()
    #fig.gca()

    mapY   = polar_clusterer.getYMappol()
    vtx_w  = polar_clusterer.getVtxW()[2]
    vtx_t  = polar_clusterer.getVtxT()[2]

    if (mapY.size() < 100):
        continue

    diff = 960
    diff = 0
    #plt.plot(vtx_w,vtx_t + diff * t2cm ,'ro',markersize=10)
    axp.grid(True)

    hitR = []
    hitA = []
    for hit in mapY:
        hitR.append( hit.second.first  )
        hitA.append( hit.second.second )
    print len(hitR)
    print len(hitA)
    axp.plot(hitA,hitR,'ko',alpha=0.5)


    vals, bin_edges = np.histogram(hitA,bins=np.linspace(-np.pi,np.pi,50))

    bin_centers = 0.5*(bin_edges[1:]+bin_edges[:-1])

    clusters = []
    cluster_start = 0
    cluster_end = 0
    in_cluster = False
    thresh = 30
    for i,v in enumerate(vals):
        if (v > thresh):
            if not in_cluster:
                cluster_start = i
                in_cluster = True
        if (v <= thresh):
            if in_cluster:
                cluster_end = i
                in_cluster = False
                if ( (cluster_end > cluster_start) and (cluster_start != 0) ):
                    clusters.append([cluster_start,cluster_end])

    # loop through hits
    # if in a cluster region -> add them to a cluster
    cluster_hits = []
    for cl in clusters:
        cluster_hits.append([[],[]])

    for i,a in enumerate(hitA):
        for j, cl in enumerate(clusters):
            if ( (a >= bin_centers[cl[0]]) and (a <= bin_centers[cl[1]]) ):
                cluster_hits[j][0].append(hitR[i])
                cluster_hits[j][1].append(hitA[i])
                break

    colors = ['r','b','g','y','c','m']
            
    for i,cl in enumerate(cluster_hits):
        axp.plot(cl[1],cl[0],'o',color=colors[i%6],ms=8,mec=colors[i%6])
    
    #ax.hist(hitA,bins=np.linspace(-np.pi,np.pi,40))

    #fig.canvas
    #fig.canvas.draw()

    plt.show()
        
    usrinput = raw_input("Hit Enter: next channel  || int: go to channel number ||  q: exit viewer\n")                        
    if ( usrinput == "q" ):                                                          
        sys.exit(0)
    elif ( usrinput.isdigit() == True):
        chan = int(usrinput)

sys.exit()

