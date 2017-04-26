#ifndef LARLITE_SIMPHOTONSTUDIES_CXX
#define LARLITE_SIMPHOTONSTUDIES_CXX

#include "SimPhotonStudies.h"
#include "DataFormat/simphotons.h"
#include "DataFormat/simch.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  bool SimPhotonStudies::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("_tree","tree");
    _tree->Branch("_photons",&_photons,"photons/D");
    _tree->Branch("_edep",&_edep,"edep/D");
    _tree->Branch("_qdep",&_qdep,"qdep/D");
    _tree->Branch("_nshr",&_nshr,"nshr/I");
    _tree->Branch("_ntrk",&_ntrk,"ntrk/I");
    _tree->Branch("_pdg",&_pdg,"pdg/I");

    return true;
  }
  
  bool SimPhotonStudies::analyze(storage_manager* storage) {
  
    auto ev_simph = storage->get_data<event_simphotons>("largeant");

    auto ev_simch = storage->get_data<event_simch>("largeant");

    auto ev_mcshower = storage->get_data<event_mcshower>("mcreco");
    auto ev_mctrack  = storage->get_data<event_mctrack> ("mcreco");
    
    if ( (ev_mcshower->size() != _n_showers) && (ev_mctrack->size() != _n_tracks) ) return true;
    if ( (ev_mcshower->size() == _n_showers) && (ev_mctrack->size() == _n_tracks) ) return true;

    _nshr = ev_mcshower->size();
    _ntrk = ev_mctrack->size();

    if (_nshr == 1)
      _pdg = 11;

    if (_ntrk == 1) 
      _pdg = ev_mctrack->at(0).PdgCode();

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

    _edep = _qdep = 0;

    // grab all the simchannel info
    for (size_t i=0; i < ev_simch->size(); i++) {

      auto const& simch = ev_simch->at(i);

      auto const& idemap = simch.TDCIDEMap();

      for (auto const& idevec : idemap) {
	
	for (auto const& ide : idevec.second) {
	  
	  _edep += ide.energy;
	  _qdep += ide.numElectrons;

	}// for all ides on this channel
      }// for all channels in map
    }// for all simchannels in event

    _tree->Fill();
    
    return true;
  }

  bool SimPhotonStudies::finalize() {

    _tree->Write();

    return true;
  }

}
#endif
