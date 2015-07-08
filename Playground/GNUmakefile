#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = PlaygroundCint
SHLIB = libCaraTool_Playground.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += $(shell recotool-config --includes)
INCFLAGS += -I$(LARLITE_USERDEVDIR)
INCFLAGS += -I$(LARLITE_USERDEVDIR)/RecoTool/AnalysisAlg
INCFLAGS += -I$(LARLITE_USERDEVDIR)/RecoTool/ClusterRecoUtil

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# CINT flag
CINTFLAGS += "-p"

LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_AnalysisAlg
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_ClusterRecoUtil
LDFLAGS += -L$(LARLITE_LIBDIR) -lRecoTool_FANN

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE



