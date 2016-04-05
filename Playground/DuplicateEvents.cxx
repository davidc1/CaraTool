#ifndef LARLITE_DUPLICATEEVENTS_CXX
#define LARLITE_DUPLICATEEVENTS_CXX

#include "DuplicateEvents.h"
#include "DataFormat/mctrack.h"

namespace larlite {

  bool DuplicateEvents::initialize() {

    _event_db.clear();

    return true;
  }
  
  bool DuplicateEvents::analyze(storage_manager* storage) {

    auto ev_mct = storage->get_data<event_mctrack>("mcreco");
    
    if (!ev_mct || (ev_mct->size() == 0) )
      return true;

    auto mct = ev_mct->at(0);
    if (mct.size() == 0)
      return true;
    auto pdg = mct.PdgCode();
    auto end = mct[mct.size()-1];
    auto eng = mct.Start().E();

    auto evt = storage->event_id();
    auto run = storage->run_id();
    auto sub = storage->subrun_id();

    EventID id;
    id.run    = run;
    id.subrun = sub;
    id.event  = evt;

    int duplicate = 0;

    for (size_t i=0; i < _event_db.size(); i++){
      auto evtmp = _event_db[i];
      if ( (evtmp.run == id.run) and (evtmp.subrun == id.subrun) and (evtmp.event == id.event) ){
	std::cout << "Next Event is Duplicate" << std::endl;
	duplicate = 1;
      }
    }
    if (duplicate == 0)
      _event_db.push_back(id);
    
    std::cout << "[Run, Subrun, Event] -> [" << run << ", " << sub << ", " << evt << "]" 
	      << " ..... " 
	      << "[PDG, Estart] -> [" << pdg << ", " << eng << "]"
	      << " ..... "
	      << "[endX, endY, endZ] -> [" << end.X() << "," << end.Y()  << "," << end.Z() << "]" 
	      << std::endl;
  
    return true;
  }

  bool DuplicateEvents::finalize() {

    return true;
  }

}
#endif
