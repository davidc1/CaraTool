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

for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("ana.root")
my_proc.set_output_file("out.root")


ana = fmwk.MCTruthScanner()
ana.showAllMCPart(False)
ana.setVerbose(False)

# set the number of pi0s requested
my_proc.add_process(ana)


my_proc.enable_event_alignment(False)

#my_proc.enable_filter(True)

ctr = 0

my_proc.run()
sys.exit()

while my_proc.process_event():

    print
    print
    print 'Entry %i'%ctr
    print
    print

    incommand = raw_input("Continue: any key. Quit: q \n")

    ctr += 1

    if (incommand == "q"):
        break

sys.exit(0);
