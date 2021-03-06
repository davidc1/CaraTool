import sys

import numpy as np

from ROOT import *
#gSystem.Load("libMy_Repo_scratch_area.so")

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk

# prepare viewer for GeoAlgo objects
from ROOT import geoalgo
import matplotlib.pyplot as plt



# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("simchmap.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
simchmap = fmwk.SimchMap()
my_proc.add_process(simchmap)

print
print  "Finished configuring ana_processor. Start event loop!"
print

#while my_proc.process_event():
#    input = raw_input('type to continue')
my_proc.run()

# done!
print
print "Finished running ana_processor event loop!"
print

#sys.exit(0)
