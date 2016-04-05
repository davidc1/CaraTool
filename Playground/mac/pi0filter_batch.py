import os, sys

INPATH  = '/pnfs/uboone/scratch/users/davidc1/MCC7_BNB/data/'
OUTPATH = '/uboone/data/users/davidc1/MCC7_BNB/'

# Load libraries
import os, ROOT, sys
from larlite import larlite
import time

# Now import ana_processor & your class. For this example, ana_base.
from ROOT import larlite as fmwk

ctrmin = int(sys.argv[-2])
ctrmax = int(sys.argv[-1])

data_dirs = os.listdir(INPATH)

# start looping through folders

# keep track of a counter for all subdirectories
ctr = 0

# for all subdirectories
for subdir in data_dirs:
    
    fpath = INPATH+subdir+"/"
    
    if (os.path.isdir(fpath) == False):
        continue


    files = os.listdir(fpath)

    # select only larlite root files
    larlite_root_v = []

    for fname in files:

        if ( (fname.find('root') > 0) and fname.find('larlite') >= 0):
            fullpath = fpath+fname
            print fullpath
            larlite_root_v.append(fullpath)

        if ( os.path.isfile(fullpath) == False):
            continue

    # root of the file name (w/ .root)
    fout  = OUTPATH+'larlite_mcc7_bnb_1pi0_%04i.root'%ctr
    print fout

    ctr += 1

    if ( (ctr < ctrmin) or (ctr >= ctrmax) ):
        continue

    # Create ana_processor instance
    my_proc=fmwk.ana_processor()
    
    # Specify IO mode
    #my_proc.set_io_mode(fmwk.storage_manager.kREAD)
    #my_proc.set_io_mode(storage_manager.WRITE)
    my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

    for f_in in larlite_root_v:
        my_proc.add_input_file(f_in)

    my_proc.set_ana_output_file("hitenergy.root")
    my_proc.set_output_file(fout)


    ana = fmwk.Pi0Filter()
    # set the number of pi0s requested
    ana.setNPi0s(1)
    my_proc.add_process(ana)
    my_proc.add_process(fmwk.EmptyModule())
    
    my_proc.enable_filter(True)
    my_proc.run()





sys.exit(0);








