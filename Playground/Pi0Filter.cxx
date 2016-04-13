#ifndef LARLITE_PI0FILTER_CXX
#define LARLITE_PI0FILTER_CXX

#include "Pi0Filter.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  Pi0Filter::Pi0Filter()
    : _pi0_tree(nullptr)
  {
    _name = "Pi0Filter";
    _n_pi0s = 1;
  }

  bool Pi0Filter::initialize() {

    if (_pi0_tree)
      delete _pi0_tree;
    _pi0_tree = new TTree("_pi0_tree","Pi0 Kinematics Tree");
    _pi0_tree->Branch("_pi0_E",&_pi0_E,"pi0_E/D");
    _pi0_tree->Branch("_shr1_E",&_shr1_E,"shr1_E/D");
    _pi0_tree->Branch("_shr2_E",&_shr2_E,"shr2_E/D");
    _pi0_tree->Branch("_shr1_Edep",&_shr1_Edep,"shr1_Edep/D");
    _pi0_tree->Branch("_shr2_Edep",&_shr2_Edep,"shr2_Edep/D");
    _pi0_tree->Branch("_angle",&_angle,"angle/D");

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
    if ( (vtx.X() < 0) or (vtx.X() > 256) or (vtx.Y() < -116) or (vtx.Y() > 116) or (vtx.Z() < 0) or (vtx.Z() > 1036) ){
      std::cout << "Vertex @ x : " << vtx.X() << "\t y : " << vtx.Y() << "\t z : " << vtx.Z() << std::endl;
      return false;
    }

    // find the MCShowers associated to the Pi0
    auto* ev_mcs = storage->get_data<event_mcshower>("mcreco");
    size_t pi0_ID_1 = 0;
    size_t pi0_ID_2 = 0;
    size_t idx_1 = 0;
    size_t idx_2 = 0;
    size_t n_found = 0;
    for (size_t i=0; i < ev_mcs->size(); i++){
      auto const& mcs = ev_mcs->at(i);
      if ( mcs.MotherPdgCode() == 111){
	if (n_found == 0){
	  pi0_ID_1 = mcs.MotherTrackID();
	  idx_1 = i;
	  n_found += 1;
	}
	else if (n_found == 1){
	  pi0_ID_2 = mcs.MotherTrackID();
	  idx_2 = i;
	  n_found += 1;
	}
	  else
	    n_found += 1;
      }// if mother is a Pi0
    }// for all MCShowers

    if ( (n_found == 2) and (pi0_ID_1 == pi0_ID_2) ){

      auto const& shr1 = ev_mcs->at(idx_1);
      auto const& shr2 = ev_mcs->at(idx_2);

      _pi0_E  = shr1.MotherEnd().E();
      _shr1_E = shr1.Start().E();
      _shr2_E = shr2.Start().E();
      _shr1_Edep = shr1.DetProfile().E();
      _shr2_Edep = shr2.DetProfile().E();
      double mom1 = sqrt ( ( shr1.Start().Px() * shr1.Start().Px() ) +
			   ( shr1.Start().Py() * shr1.Start().Py() ) +
			   ( shr1.Start().Pz() * shr1.Start().Pz() ) );
      double mom2 = sqrt ( ( shr2.Start().Px() * shr2.Start().Px() ) +
			   ( shr2.Start().Py() * shr2.Start().Py() ) +
			   ( shr2.Start().Pz() * shr2.Start().Pz() ) );
      _angle = ( ( shr1.Start().Px() * shr2.Start().Px() ) +
		 ( shr1.Start().Py() * shr2.Start().Py() ) +
		 ( shr1.Start().Pz() * shr2.Start().Pz() ) ) / (mom1*mom2);

      _pi0_tree->Fill();
    }// if there are 2 showers from the same pi0
    
    return true;
  }
  
  bool Pi0Filter::finalize() {

    _pi0_tree->Write();
    
    return true;
  }

}
#endif
