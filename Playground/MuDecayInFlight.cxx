#ifndef LARLITE_MUDECAYINFLIGHT_CXX
#define LARLITE_MUDECAYINFLIGHT_CXX

#include "MuDecayInFlight.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/mctrack.h"

namespace larlite {

  bool MuDecayInFlight::initialize() {

    _tree = new TTree("tree","tree");
    _tree->Branch("_dist",&_dist,"dist/D");
    _tree->Branch("_dist_min",&_dist_min,"dist_min/D");
    _tree->Branch("_parent_trkid",&_parent_trkid,"parent_trkid/I");
    _tree->Branch("_mu_trkid",&_mu_trkid,"mu_trkid/I");
    _tree->Branch("_min_trkid",&_min_trkid,"min_trkid/I");
    _tree->Branch("_mu_pdg",&_mu_pdg,"mu_pdg/I");
    _tree->Branch("_mu_x",&_mu_x,"mu_x/D");
    _tree->Branch("_mu_y",&_mu_y,"mu_y/D");
    _tree->Branch("_mu_z",&_mu_z,"mu_z/D");
    _tree->Branch("_el_x",&_el_x,"el_x/D");
    _tree->Branch("_el_y",&_el_y,"el_y/D");
    _tree->Branch("_el_z",&_el_z,"el_z/D");
    _tree->Branch("_energy",&_energy,"energy/D");
    _tree->Branch("_mu_energy",&_mu_energy,"mu_energy/D");
    _tree->Branch("_process",&_process);
    
    return true;
  }
  
  bool MuDecayInFlight::analyze(storage_manager* storage) {

    _trackIDMap.clear();

    auto *ev_mcshower = storage->get_data<event_mcshower>("mcreco");

    auto *ev_mctrack  = storage->get_data<event_mctrack>("mcreco");

    for (size_t i=0; i < ev_mctrack->size(); i++){
      auto const& trk = ev_mctrack->at(i);
      if ( (trk.PdgCode() != 13) and (trk.PdgCode() != -13) )
	continue;
      _trackIDMap[ ev_mctrack->at(i).TrackID() ] = i;
    }

    for (size_t i=0; i < ev_mcshower->size(); i++){

      auto const& mcsh = ev_mcshower->at(i);

      _process = mcsh.Process();
      _parent_trkid = mcsh.MotherTrackID();

      auto const& e_strt = mcsh.Start();

      _el_x = e_strt.X();
      _el_y = e_strt.Y();
      _el_z = e_strt.Z();

      if (mcsh.DetProfile().E() < 1)
	continue;

      // is the mcshower contained? if not -> continue
      if ( (e_strt.X() < 0) or (e_strt.X() > 256) or (e_strt.Y() < -116) or (e_strt.Y() > 116) or (e_strt.Z() < 0) or (e_strt.Z() > 1036) )
	continue;

      if ( (_process == "Decay") or (_process == "muMinusCaptureAtRest") ){

      if ( (mcsh.MotherPdgCode() != 13) and (mcsh.MotherPdgCode() != -13) )
	continue;

      // get the parent nuon
      auto const& muon = ev_mctrack->at( _trackIDMap[ mcsh.MotherTrackID() ] );

      if (muon.size() < 2)
	  continue;

      auto const& mu_end = muon.at( muon.size() -1 );
      
      _mu_x = mu_end.X();
      _mu_y = mu_end.Y();
      _mu_z = mu_end.Z();

      // is the track contained? if not -> continue
      if ( (mu_end.X() < 0) or (mu_end.X() > 256) or (mu_end.Y() < -116) or (mu_end.Y() > 116) or (mu_end.Z() < 0) or (mu_end.Z() > 1036) )
	continue;
      
      _mu_pdg = muon.PdgCode();
      _mu_trkid = muon.TrackID();

      _dist = sqrt ( pow(mu_end.X() - e_strt.X(),2) + pow(mu_end.Y() - e_strt.Y(),2) + pow(mu_end.Z() - e_strt.Z(),2) );

      // loop over all muons to find the one that stops closest
      
      _dist_min = _dist;

      for ( std::map<int,int>::iterator it=_trackIDMap.begin(); it != _trackIDMap.end(); it++){

	auto const& muon2 = ev_mctrack->at ( it->second );

	if (muon2.size() < 2)
	  continue;

	auto const& mu_end2 = muon2.at( muon2.size() -1 );

	double d = sqrt ( pow(mu_end2.X() - e_strt.X(),2) + pow(mu_end2.Y() - e_strt.Y(),2) + pow(mu_end2.Z() - e_strt.Z(),2) );

	if (d < _dist_min){
	  _dist_min = d;
	  _min_trkid = muon2.TrackID();
	}
	
      }// for all muons

      // what is the energy of the muon when it dies
      _mu_energy = muon.End().E();
      _energy   = mcsh.Start().E();
      _tree->Fill();
      
      }
      
    }
    
    return true;
  }

  bool MuDecayInFlight::finalize() {

    _tree->Write();
    
    return true;
  }

}
#endif
