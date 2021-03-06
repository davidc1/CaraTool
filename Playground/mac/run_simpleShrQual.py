import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from ROOT import larlite as fmwk


# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root files
for x in xrange(len(sys.argv)):
    if x < 1:
        continue
    my_proc.add_input_file(sys.argv[x])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify analysis output root file name
my_proc.set_ana_output_file("simple_shr_qual.root");

# Specify data output root file name
my_proc.set_output_file("outfile.root")

# Create analysis unit
module = fmwk.ShowerQualitySimple()
module.setProducerName("showerreco")
module.setMinE(100)

my_proc.add_process(module)

print
print  "Finished configuring ana_processor. Start event loop!"
print

my_proc.run()

sys.exit(0)

