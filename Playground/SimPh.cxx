#ifndef LARLITE_SIMPH_CXX
#define LARLITE_SIMPH_CXX

#include "SimPh.h"
#include "DataFormat/simphotons.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  bool SimPh::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("_tree","tree");
    _tree->Branch("_photons",&_photons,"photons/D");

    return true;
  }
  
  bool SimPh::analyze(storage_manager* storage) {
  
    auto ev_simph = storage->get_data<event_simphotons>("largeant");

    auto ev_mcshower = storage->get_data<event_mcshower>("mcreco");
    if (ev_mcshower->size() != _n_showers) return true;

    auto ev_mctrack = storage->get_data<event_mctrack>("mcreco");
    if (ev_mctrack->size() != _n_tracks) return true;

    auto trk = ev_mctrack->at(0);

    /*
      std::cout << "Det profile : " << trk.at(0).E() - trk.at(trk.size()-1).E() << std::endl;
    std::cout << "last point @ [ " 
	      << trk.at(trk.size()-1).X() << ", "
	      << trk.at(trk.size()-1).Y() << ", "
	      << trk.at(trk.size()-1).Z() << " ]" << std::endl << std::endl;
    */

    //    std::cout << ev_simph->size() << std::endl;

    _photons = 0.0;

    for (size_t i=0; i < ev_simph->size(); i++) {

      auto const& simph = ev_simph->at(i);
      
      for (auto const& ph : simph)
	
	_photons += ph.Energy;
      
    }

    _tree->Fill();
    
    return true;
  }

  bool SimPh::finalize() {

    _tree->Write();

    return true;
  }

}
#endif
