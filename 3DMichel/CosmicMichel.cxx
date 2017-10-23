#ifndef LARLITE_COSMICMICHEL_CXX
#define LARLITE_COSMICMICHEL_CXX

#include "CosmicMichel.h"
#include "DataFormat/calorimetry.h"
#include "DataFormat/hit.h"
#include "DataFormat/track.h"
#include "DataFormat/mctrack.h"
#include "TruncMean.h"

#include "LArUtil/SpaceChargeMicroBooNE.h"
#include "LArUtil/GeometryHelper.h"

namespace larlite {

  bool CosmicMichel::initialize() {

    _w2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _t2cm  = larutil::GeometryHelper::GetME()->TimeToCm();

    if (_tree_rc) delete _tree_rc;
    _tree_rc = new TTree("rctree","rctree");
    _tree_rc->Branch("_dqdx_v","std::vector<double>",&_dqdx_v);
    _tree_rc->Branch("_dqdx_trunc_v","std::vector<double>",&_dqdx_trunc_v);
    _tree_rc->Branch("_x_v","std::vector<double>",&_x_v);
    _tree_rc->Branch("_y_v","std::vector<double>",&_y_v);
    _tree_rc->Branch("_z_v","std::vector<double>",&_z_v);
    _tree_rc->Branch("_rr_v","std::vector<double>",&_rr_v);
    _tree_rc->Branch("_xe_rc",&_xe_rc,"xe_rc/D");
    _tree_rc->Branch("_ye_rc",&_ye_rc,"ye_rc/D");
    _tree_rc->Branch("_ze_rc",&_ze_rc,"ze_rc/D");
    _tree_rc->Branch("_xe_mc",&_xe_mc,"xe_mc/D");
    _tree_rc->Branch("_ye_mc",&_ye_mc,"ye_mc/D");
    _tree_rc->Branch("_ze_mc",&_ze_mc,"ze_mc/D");
    _tree_rc->Branch("_dmin",&_dmin,"dmin/D");
    _tree_rc->Branch("_rr_max",&_rr_max,"rr_max/D");
    _tree_rc->Branch("_qnear",&_qnear,"qnear/D");
    
    // mc tree
    if (_tree_mc) delete _tree_mc;
    _tree_mc = new TTree("mctree","mctree");
    _tree_mc->Branch("_xe_mc",&_xe_mc,"xe_mc/D");
    _tree_mc->Branch("_ye_mc",&_ye_mc,"ye_mc/D");
    _tree_mc->Branch("_ze_mc",&_ze_mc,"ze_mc/D");
    _tree_mc->Branch("_energy_mc",&_energy_mc,"energy_mc/D");
    
    _tmean._rad = 10;
    
    return true;
  }
  
  bool CosmicMichel::analyze(storage_manager* storage) {

    // start by grabbing the calorimetry
    auto ev_calo = storage->get_data<event_calorimetry>("pandoraCosmiccalo");

    // grab associated track
    larlite::event_track* ev_trk;
    auto calo_trk_ass = storage->find_one_ass(ev_calo->id(),ev_trk,ev_calo->name());

    // load mctracks
    auto ev_mct = storage->get_data<event_mctrack>("mcreco");

    // load hits
    larlite::event_hit* ev_hit;
    auto trk_hit_ass = storage->find_one_ass(ev_trk->id(),ev_hit,ev_trk->name());

    // no associated hits? somthing went wrong
    if (!ev_hit or (ev_hit->size() == 0)){
      print(larlite::msg::kWARNING,__FUNCTION__,"no hits associated to tracks...");
      return true;
    }

    // split hits in cells 10x10 cm to easily locate them
    ResetHitMap(ev_hit);

    // done creating hit map
    
    for (size_t h=0; h < ev_hit->size(); h++) {

      auto const& hit = ev_hit->at(h);

      // only Y-plane
      if (hit.WireID().Plane != 2) continue;
      
      auto wcm = hit.WireID().Wire * _w2cm;
      auto tcm = hit.PeakTime() * _t2cm;

      auto pos = std::make_pair((int)tcm/10,(int)wcm/10);

      _hitmap[pos].push_back( h );
      
    }


    std::vector< std::vector<double> > mu_end_pt_v;

    auto SCE = larutil::SpaceChargeMicroBooNE();

    if (_truth) {

      // grab the muon (assume single-particle in TPC stopping sample)
      if (!ev_mct or (ev_mct->size() == 0)){
	print(larlite::msg::kWARNING,__FUNCTION__,"no MCTracks");
	return true;
      }
      
      for (size_t i=0; i < ev_mct->size(); i++) {
	auto mct = ev_mct->at(i);
	if ( (mct.PdgCode() == 13) or (mct.PdgCode() == -13) ) {
	  if (mct.size() == 0) continue;
	  auto enter = mct.at(0);
	  _energy_mc = enter.E();
	  auto end = mct.End();
	  if (end.E() > 106.) continue;
	  _xe_mc = end.X();
	  _ye_mc = end.Y();
	  _ze_mc = end.Z();
	  if ( (_xe_mc < 256.) and (_xe_mc > 0) and (_ye_mc < 116.) and
	       (_ye_mc > -116.) and (_ze_mc < 1036.) and (_ze_mc > 0.) ) {
	    std::vector<double> endpt{_xe_mc,_ye_mc,_ze_mc};
	    auto endptSCE = SCE.GetPosOffsets(endpt[0],endpt[1],endpt[2]);
	    endptSCE[0] += endpt[0];
	    endptSCE[1] += endpt[1];
	    endptSCE[2] += endpt[2];
	    mu_end_pt_v.push_back( endptSCE );
	    _tree_mc->Fill();
	  }// if in FV
	}// if muon
      }

    } // if use truth

    if (!ev_trk or (ev_trk->size() == 0)){
      print(larlite::msg::kWARNING,__FUNCTION__,"no tracks");
      return true;
    }

    // loop through calorimetry, find the one we care about
    for (size_t i=0; i < ev_calo->size(); i++) {

      auto calo = ev_calo->at(i);

      if (calo.PlaneID().Plane != 2) continue;

      // grab the associated track
      if (calo_trk_ass.at(i).size() != 1) continue;

      auto trkidx = calo_trk_ass.at(i).at(0);
      auto trk = ev_trk->at( trkidx );

      _xe_rc = trk.End().X();
      _ye_rc = trk.End().Y();
      _ze_rc = trk.End().Z();

      _qnear = Qtot(5.,trk.End(),trk_hit_ass[trkidx],ev_hit);
      
      // find the closest true muon end-point
      double ddmin = 100000.;
      int idxmin = 0;
      for (size_t k=0; k < mu_end_pt_v.size(); k++) {

	auto endpt = mu_end_pt_v[k];
	
	double dd = ( (_ye_rc - endpt[1])*(_ye_rc - endpt[1]) + (_ze_rc - endpt[2])*(_ze_rc - endpt[2]) );
	if (dd < ddmin) { ddmin = dd; idxmin = k; _dmin = sqrt(dd); }
      }// for all MC tracks

      if (ddmin < 100000.) {
	_xe_mc = mu_end_pt_v[idxmin][0];
	_ye_mc = mu_end_pt_v[idxmin][1];
	_ze_mc = mu_end_pt_v[idxmin][2];
      }

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

	if (rr_v[p] > _rr_max) { _rr_max = rr_v[p]; }

      }// for all points in calorimetry

      if ( _rr_max < 100) continue;

      _tmean.CalcTruncMean(_rr_v,_dqdx_v,_dqdx_trunc_v);
      _tree_rc->Fill();

    }// for all calo objects
    
    return true;
  }

  bool CosmicMichel::finalize() {

    if (_fout and _tree_rc and _tree_mc){
      _fout->cd();
      _tree_mc->Write();
      _tree_rc->Write();
    }
    
    return true;
  }

  void CosmicMichel::ResetHitMap(larlite::event_hit* ev_hit) {

    _hitmap.clear();
    
    for (int i=-10; i < 35; i++){
      for (int k=-1; k < (int)(1036./10.)+1; k++){
	auto pos = std::make_pair(i,k);
	_hitmap[pos] = std::vector<int>();
      }
    }
    
    for (size_t h=0; h < ev_hit->size(); h++) {

      auto const& hit = ev_hit->at(h);

      // only Y-plane
      if (hit.WireID().Plane != 2) continue;
      
      auto wcm = hit.WireID().Wire * _w2cm;
      auto tcm = hit.PeakTime() * _t2cm;

      auto pos = std::make_pair((int)tcm/10,(int)wcm/10);

      _hitmap[pos].push_back( h );
      
    }

    return;
  }

  double CosmicMichel::Qtot(const double& rmax,
			    const TVector3& end,
			    const std::vector<unsigned int>& ass_hit_v,
			    larlite::event_hit* ev_hit) {

    double qtot = 0;

    auto geoH = larutil::GeometryHelper::GetME();

    auto const& pt = geoH->Point_3Dto2D(end,2);
    
    int endw = pt.w/10;
    int endt = (pt.t + 800 * _t2cm)/10;
    
    // grab points nearby
    for (int i = endw-1; i <= endw+1; i++) {
      for (int j = endt-1; j <= endt+1; j++) {
	auto endpos = std::make_pair(j,i);
	auto const& hits = _hitmap[endpos];
	for (auto const& hit_idx : hits) {
	  // if hit index associated to track -> skip
	  bool trackhit = false;
	  for (auto const& idx : ass_hit_v){
	    if (idx == hit_idx) { trackhit = true; break; }
	  }
	  if (trackhit == true) continue;
	  auto const& hit = ev_hit->at(hit_idx);
	  auto wcm = hit.WireID().Wire * _w2cm;
	  auto tcm = hit.PeakTime() * _t2cm;
	  double d = sqrt ( (wcm - pt.w) * (wcm-pt.w) + (tcm - pt.t - 800*_t2cm) * (tcm - pt.t - 800*_t2cm) );
	  if (d < 5.) { qtot += hit.Integral(); }
	}// for all hits in cell
      }// for all time cells
    }// for all wire cells

    return qtot;
    
  }

}
#endif
