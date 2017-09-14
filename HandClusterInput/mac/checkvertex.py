#
# Example PyROOT script to run analysis module, ana_base.
# The usage is same for inherited analysis class instance.
#

# Load libraries
import os, ROOT, sys
from larlite import larlite as fmwk



    

# Now import ana_processor & your class. For this example, ana_base.
#from ROOT import larlite as fmwk#

# Create ana_processor instance
my_proc=fmwk.ana_processor()

# Specify IO mode
#my_proc.set_io_mode(fmwk.storage_manager.kBOTH)
#my_proc.set_io_mode(storage_manager.WRITE)
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

for x in xrange(len(sys.argv)-2):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("checkvertex.root")
my_proc.set_output_file("")

hittxt = open(sys.argv[-1],'r')

ana = fmwk.CheckVertex()
#ana.clearHits()

for line in hittxt:
    words = line.split()
    run    = int(words[0])
    evt    = int(words[1])
    hitidx = int(words[2])
    shridx = int(words[3])
    #print 'adding hit idx %i @ shr %i'%(hitidx,shridx)
    ana.addHit(run,evt,shridx,hitidx)
my_proc.add_process(ana)


my_proc.run()
sys.exit()
