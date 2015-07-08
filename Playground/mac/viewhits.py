# Load libraries
import sys, os
from ROOT import *
from ROOT import gSystem
from ROOT import larlite as fmwk

# vertex finder
# Create ana_processor instance
vtxproc=fmwk.ana_processor()

# Specify IO mode
vtxproc.set_io_mode(fmwk.storage_manager.kREAD)

for x in xrange(len(sys.argv)-1):
    vtxproc.add_input_file(sys.argv[x+1])

vtxproc.set_ana_output_file("vtxfinder.root")

# Create analysis class instance. For this example, ana_base.
# To show how one can run multiple analysis modules at once,
# we make multiple ana_base instance.

vtxfinder = fmwk.VertexFinder()
vtxfinder.setCellSize(2.)
vtxfinder.setRadius(1.)
vtxfinder.setPlane(2)

vtxproc.add_process(vtxfinder)

vtxproc.run()

# Create ana_processor instance
my_proc=fmwk.ana_processor()

# Specify IO mode
#my_proc.set_io_mode(fmwk.storage_manager.kBOTH)
#my_proc.set_io_mode(fmwk.storage_manager.kWRITE)
my_proc.set_io_mode(fmwk.storage_manager.kREAD)


for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])


#is there a way to disable ana_proc from creating an output file at all?
my_proc.set_ana_output_file("")

my_ana = fmwk.HitViewer()
my_ana.setMinQ(200)

my_proc.add_process(my_ana)


gStyle.SetOptStat(0)
gStyle.SetTitleFontSize(0.1)
gStyle.SetTitleOffset(0.4,"X")
gStyle.SetTitleSize(0.08,"X")
gStyle.SetTitleOffset(0.4,"Y")
gStyle.SetTitleSize(0.08,"Y")
gStyle.SetLabelSize(0.08,"X")
gStyle.SetLabelSize(0.08,"Y")
gStyle.SetLabelSize(0.08,"Z")
gStyle.SetOptLogz(1)

c=TCanvas("c","Wire v. Time Hit Viewer",900,600)
c.Divide(1,3)

while my_proc.process_event():

    #vtxproc.process_event()

    currentview = 0;
    #First fill the 6 pads on the main canvas with stuff
    for pad in xrange(1,4,1):
        
        c.cd(pad)
        h = my_ana.GetHisto_Hits(int(currentview))
        h.SetTitleFont(62,'X')
        h.SetTitleFont(62,'Y')
        h.Draw("COLZ")
    
        currentview = currentview + 1
        c.Update()
    

    usrinput = raw_input("Hit Enter: next evt  ||  q: exit viewer\n")
    if ( usrinput == "q" ):
        sys.exit(0)
    
# done!
