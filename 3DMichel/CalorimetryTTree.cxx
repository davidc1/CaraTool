#ifndef LARLITE_CALORIMETRYTTREE_CXX
#define LARLITE_CALORIMETRYTTREE_CXX

#include "CalorimetryTTree.h"
#include "DataFormat/calorimetry.h"
#include "DataFormat/hit.h"
#include "DataFormat/track.h"
#include "DataFormat/mctrack.h"
#include "TruncMean.h"

namespace larlite {

  bool CalorimetryTTree::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree(_name.c_str(),_name.c_str());
    _tree->Branch("_dqdx_v","std::vector<double>",&_dqdx_v);
    _tree->Branch("_dqdx_trunc_v","std::vector<double>",&_dqdx_trunc_v);
    _tree->Branch("_x_v","std::vector<double>",&_x_v);
    _tree->Branch("_y_v","std::vector<double>",&_y_v);
    _tree->Branch("_z_v","std::vector<double>",&_z_v);
    _tree->Branch("_rr_v","std::vector<double>",&_rr_v);
    _tree->Branch("_xe_rc",&_xe_rc,"xe_rc/D");
    _tree->Branch("_ye_rc",&_ye_rc,"ye_rc/D");
    _tree->Branch("_ze_rc",&_ze_rc,"ze_rc/D");
    _tree->Branch("_xe_mc",&_xe_mc,"xe_mc/D");
    _tree->Branch("_ye_mc",&_ye_mc,"ye_mc/D");
    _tree->Branch("_ze_mc",&_ze_mc,"ze_mc/D");
    _tree->Branch("_rr_max",&_rr_max,"rr_max/D");
    _tree->Branch("_dmin",&_dmin,"dmin/D");

    _tmean._rad = 10;
    
    return true;
  }
  
  bool CalorimetryTTree::analyze(storage_manager* storage) {

    // start by grabbing the calorimetry
    auto ev_calo = storage->get_data<event_calorimetry>("pandoraCosmiccalo");

    // grab associated track
    larlite::event_track* ev_trk;
    auto calo_trk_ass = storage->find_one_ass(ev_calo->id(),ev_trk,ev_calo->name());

    // load mctracks
    auto ev_mct = storage->get_data<event_mctrack>("mcreco");

    // grab the muon (assume single-particle in TPC stopping sample)
    if (!ev_mct or (ev_mct->size() != 1)){
      print(larlite::msg::kWARNING,__FUNCTION__,"MCTracks != 1");
      return true;
    }

    auto mct = ev_mct->at(0);
    auto end = mct.End();
    _xe_mc = end.X();
    _ye_mc = end.Y();
    _ze_mc = end.Z();
    
    if (!ev_trk or (ev_trk->size() == 0)){
      print(larlite::msg::kWARNING,__FUNCTION__,"no tracks");
      return true;
    }

    // choose longest reconstructed track in event
    size_t trk_idx = 0;
    double Lmax = 0.;

    for (size_t i=0; i < ev_trk->size(); i++) {
      
      auto const& trk = ev_trk->at(i);

      if (trk.Length() > Lmax) { Lmax = trk.Length(); trk_idx = i; }

    }// for all tracks

    auto trk = ev_trk->at(trk_idx);
    _xe_rc = trk.End().X();
    _ye_rc = trk.End().Y();
    _ze_rc = trk.End().Z();

    std::cout << "longest track length: " << Lmax << std::endl;

    // loop through calorimetry, find the one we care about
    for (size_t i=0; i < ev_calo->size(); i++) {

      auto calo = ev_calo->at(i);

      if (calo.PlaneID().Plane != 2) continue;
      
      // is it associated with the track we want?
      if (calo_trk_ass.at(i).size() != 1) continue;
      if (calo_trk_ass.at(i).at(0) != trk_idx) continue;

      std::cout << "found associated calo object!" << std::endl;

      auto dqdx_v  = calo.dQdx();
      auto xyz_v   = calo.XYZ();
      auto rr_v    = calo.ResidualRange();

      _x_v.clear();
      _y_v.clear();
      _z_v.clear();
      _rr_v.clear();
      _dqdx_v.clear();
      _dqdx_trunc_v.clear();

      _rr_max = 0.;
      
      _x_v.reserve(dqdx_v.size());
      _y_v.reserve(dqdx_v.size());
      _z_v.reserve(dqdx_v.size());
      _rr_v.reserve(dqdx_v.size());
      _dqdx_v.reserve(dqdx_v.size());
      _dqdx_trunc_v.reserve(dqdx_v.size());

      for (size_t p=0; p < dqdx_v.size(); p++) {

	_x_v.push_back( xyz_v[p].X() );
	_y_v.push_back( xyz_v[p].Y() );
	_z_v.push_back( xyz_v[p].Z() );
	_dqdx_v.push_back( dqdx_v[p] );
	_rr_v.push_back( rr_v[p] );

	_dmin = sqrt( (_ye_rc - _ye_mc)*(_ye_rc - _ye_mc) + (_ze_rc - _ze_mc)*(_ze_rc - _ze_mc) );
	
	if (rr_v[p] > _rr_max) { _rr_max = rr_v[p]; }

      }// for all points in calorimetry

      _tmean.CalcTruncMean(_rr_v,_dqdx_v,_dqdx_trunc_v);
      
    }// for all calo objects

    _tree->Fill();
    
    return true;
  }

  bool CalorimetryTTree::finalize() {

    if (_fout and _tree){
      _fout->cd();
      _tree->Write();
    }
    
    return true;
  }

}
#endif
