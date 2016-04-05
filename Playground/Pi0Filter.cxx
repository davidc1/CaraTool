#ifndef LARLITE_PI0FILTER_CXX
#define LARLITE_PI0FILTER_CXX

#include "Pi0Filter.h"
#include "DataFormat/mctruth.h"

namespace larlite {

  bool Pi0Filter::initialize() {

    return true;
  }
  
  bool Pi0Filter::analyze(storage_manager* storage) {

    auto *ev_mctruth = storage->get_data<event_mctruth>("generator");

    if ( (!ev_mctruth) or (ev_mctruth->size() == 0) )
      return false;

    // get all MCParticles
    auto mctruth = ev_mctruth->at(0);

    auto part_v = mctruth.GetParticles();

    int n_pi0 = 0;

    for (auto const& part : part_v){
      //std::cout << "found " << part.PdgCode() << std::endl;
      if ( (part.PdgCode() == 111) and (part.StatusCode() == 1) ){
	std::cout << "found " << part.PdgCode() << "\tw/ energy : " << part.Trajectory().at(0).E() <<  std::endl;
	n_pi0 += 1;
      }
    }

    if (n_pi0 != _n_pi0s)
      return false;
  
    return true;
  }

  bool Pi0Filter::finalize() {

    return true;
  }

}
#endif
