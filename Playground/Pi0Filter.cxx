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

    size_t pi0_idx = 0;

    for (size_t i=0; i < part_v.size(); i++){
      auto const& part = part_v[i];
      //std::cout << "found " << part.PdgCode() << std::endl;
      if ( (part.PdgCode() == 111) and (part.StatusCode() == 1) ){
	std::cout << "found " << part.PdgCode() << "\tw/ energy : " << part.Trajectory().at(0).E() <<  std::endl;
	n_pi0 += 1;
	pi0_idx = i;
	  }
    }
    
    if (n_pi0 != _n_pi0s)
      return false;
    
    // is the vertex in the TPC?
    auto const& vtx = part_v.at(pi0_idx).Trajectory().at(0);
    if ( (vtx.X() > 0) and (vtx.X() < 256) and (vtx.Y() > -116) and (vtx.Y() < 116) and (vtx.Z() > 0) and (vtx.Z() < 1036) ){
      std::cout << "Vertex @ x : " << vtx.X() << "\t y : " << vtx.Y() << "\t z : " << vtx.Z() << std::endl;
      return true;
    }
    
    return false;
  }
  
  bool Pi0Filter::finalize() {
    
    return true;
  }

}
#endif
