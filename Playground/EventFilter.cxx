#ifndef LARLITE_EVENTFILTER_CXX
#define LARLITE_EVENTFILTER_CXX

#include "EventFilter.h"

namespace larlite {

  bool EventFilter::initialize() {

    _out_file.open("selected_events.txt");

    return true;
  }
  
  bool EventFilter::analyze(storage_manager* storage) {

    auto event  = storage->event_id();
    auto subrun = storage->subrun_id();
    auto run    = storage->run_id();

    bool found = false;
    
    // loop through all events we want to save
    // and see if this event is in the list
    for (size_t i=0; i < RunEvent_v.size(); i++){
      
      auto evinfo = RunEvent_v[i];

      if ( (evinfo.first == run) and (evinfo.second == event) ){
	std::cout << "Found event " << event << " @ run " << run << std::endl;
	_out_file << run << " " << subrun << " " << event << std::endl;
	//	  << vtx.X() << " " << vtx.Y() << " " << vtx.Z() << "\n";
	found = true;
	break;
      }
    }

    if (found == true)
      return true;
  
    return false;
  }

  bool EventFilter::finalize() {

    _out_file.close();

    return true;
  }

  void EventFilter::addEvent(const int run, const int event)
  {

    std::pair<int,int> newevent;

    newevent = std::make_pair(run,event);
    
    RunEvent_v.push_back(newevent);

    return;
  };

}
#endif
