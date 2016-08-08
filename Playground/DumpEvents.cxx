#ifndef LARLITE_DUMPEVENTS_CXX
#define LARLITE_DUMPEVENTS_CXX

#include "DumpEvents.h"

namespace larlite {

  bool DumpEvents::initialize() {

    _fileout.open("evt_list.txt");

    return true;
  }
  
  bool DumpEvents::analyze(storage_manager* storage) {
  
    int run    = storage->run_id();
    int subrun = storage->subrun_id();
    int event  = storage->event_id();

    _fileout << run << " " << subrun << " " << event << std::endl;
  
    return true;
  }

  bool DumpEvents::finalize() {

    _fileout.close();

    return true;
  }

}
#endif
