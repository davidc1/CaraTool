#
# Example PyROOT script to run analysis module, ana_base.
# The usage is same for inherited analysis class instance.
#

# Load libraries
import os, ROOT, sys
from ROOT import *
from larlite import larlite as fmwk

# Create ana_processor instance
my_proc=fmwk.ana_processor()

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

for x in xrange(len(sys.argv)-2):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_ana_output_file("ana.root")
my_proc.set_output_file("filtered_events.root")

ev_filter = fmwk.EventFilter()

event_txt = open(sys.argv[-1])
for line in event_txt:
    words = line.split()
    run   = int(words[0])
    event = int(words[1])
    print 'adding event %i %i'%(run,event)
    ev_filter.addEvent(run,event)

my_proc.add_process(ev_filter)

my_proc.enable_filter(True)

my_proc.run()
sys.exit(0);
