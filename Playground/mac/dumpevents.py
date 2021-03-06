#
# Example PyROOT script to run analysis module, ana_base.
# The usage is same for inherited analysis class instance.
#

# Load libraries
import os, ROOT, sys
from larlite import larlite
import time

# Now import ana_processor & your class. For this example, ana_base.
from ROOT import larlite as fmwk

# Create ana_processor instance
my_proc=fmwk.ana_processor()

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)
#my_proc.set_io_mode(storage_manager.WRITE)
#my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("pi0filter.root")
my_proc.set_output_file("filtered_bnb.root")


ana = fmwk.DumpEvents()
# set the number of pi0s requested
my_proc.add_process(ana)

my_proc.enable_filter(True)
my_proc.run()
sys.exit(0);
