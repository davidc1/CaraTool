#ifndef LARLITE_COSMICT0_CXX
#define LARLITE_COSMICT0_CXX

#include "CosmicT0.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/track.h"


namespace larlite {

  bool CosmicT0::initialize() {

    return true;
  }
  
  bool CosmicT0::analyze(storage_manager* storage) {

    auto *ev_track = storage->get_data<event_track>("pandoraCosmic");
    auto *ev_mctrk = storage->get_data<event_mctrack>("mcreco");

    if (!ev_track or (ev_track->size() == 0)){
      return true;
    }
    if (!ev_mctrk or (ev_mctrk->size() == 0)){
      return true;
    }
  
    return true;
  }

  bool CosmicT0::finalize() {

    return true;
  }

}
#endif
