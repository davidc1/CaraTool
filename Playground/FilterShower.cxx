#ifndef LARLITE_FILTERSHOWER_CXX
#define LARLITE_FILTERSHOWER_CXX

#include "FilterShower.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  bool FilterShower::initialize() {

    return true;
  }
  
  bool FilterShower::analyze(storage_manager* storage) {
  
    auto ev_shr = storage->get_data<event_mcshower>("mcreco");

    if (ev_shr->size() != 1) return false;

    auto const& shr = ev_shr->at(0);

    if ( shr.DetProfile().E() > 400. ) return false;

    return true;
  }

  bool FilterShower::finalize() {

    return true;
  }

}
#endif
