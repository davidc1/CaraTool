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
my_proc.set_io_mode(fmwk.storage_manager.kREAD)
#my_proc.set_io_mode(storage_manager.WRITE)
#my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("ana.root")
my_proc.set_output_file("")


ana = fmwk.MicheldEdx()

fin = open('/home/david/Desktop/michelCalib/michels_extbnb.txt','r')
for line in fin:
    words = line.split()
    run = int(words[0])
    event = int(words[2])
    wire  = int(words[3])
    tick  = int(words[4])
    xpos  = tick * 0.5 * 0.11 - 42.8
    zpos  = wire * 0.3
    ana.AddMichel(run,event,xpos,zpos)

# set the number of pi0s requested
my_proc.add_process(ana)

#my_proc.enable_filter(True)
my_proc.run()
sys.exit(0);
