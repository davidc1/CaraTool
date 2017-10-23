#ifndef LARLITE_ISOLATEDENDPOINT_CXX
#define LARLITE_ISOLATEDENDPOINT_CXX

#include "IsolatedEndPoint.h"

#include "DataFormat/track.h"
#include "DataFormat/hit.h"
#include "DataFormat/mctrack.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"


namespace larlite {

  bool IsolatedEndPoint::initialize() {

    _w2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _t2cm  = larutil::GeometryHelper::GetME()->TimeToCm();

    if (_tree) delete _tree;
    _tree = new TTree("tree","tree");
    _tree->Branch("_z",&_z,"z/D");
    _tree->Branch("_x",&_y,"y/D");
    _tree->Branch("_x",&_y,"x/D");
    _tree->Branch("_q",&_q,"q/D");
    _tree->Branch("_dmin",&_dmin,"dmin/D");

		      

    return true;
  }
  
  bool IsolatedEndPoint::analyze(storage_manager* storage) {

    auto ev_trk = storage->get_data<event_track>("pandoraCosmic");
    auto ev_mct = storage->get_data<event_mctrack>("mcreco");

    larlite::event_hit* ev_hit;

    auto trk_hit_ass = storage->find_one_ass(ev_trk->id(),ev_hit,ev_trk->name());

    auto SCE = larutil::SpaceChargeMicroBooNE();

    // no associated hits? somthing qent wrong
    if (!ev_hit or (ev_hit->size() == 0)){
      print(larlite::msg::kWARNING,__FUNCTION__,"no hits associated to tracks...");
      return true;
    }

    std::vector< std::vector<double> > mu_end_pt_v;
	
    for (size_t i=0; i < ev_mct->size(); i++) {
      auto mct = ev_mct->at(i);
      if ( (mct.PdgCode() == 13) or (mct.PdgCode() == -13) ) {
	if (mct.size() == 0) continue;
	auto end = mct.at(mct.size()-1);
	std::vector<double> endpt{end.X(),end.Y(),end.Z()};
	auto endptSCE = SCE.GetPosOffsets(endpt[0],endpt[1],endpt[2]);
	endptSCE[0] += endpt[0];
	endptSCE[1] += endpt[1];
	endptSCE[2] += endpt[2];
	mu_end_pt_v.push_back( endptSCE );
	}// if in FV
      }// if muon
    
    // split hits in cells 10x10 cm to easily locate them
    
    // create a map which connects position (x,z) in 10-cm blocks to hit index
    std::map< std::pair<int,int>,std::vector<int> > _hitmap;
    for (int i=-10; i < 35; i++){
      for (int k=-1; k < (int)(1036./10.)+1; k++){
	auto pos = std::make_pair(i,k);
	_hitmap[pos] = std::vector<int>();
      }
    }
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

    // done storing hits in easy to retrieve map

    // start looping through reconstructed tracks
    // for each one, identify the end-point (taken as the lowest 3D point)

    auto geoH = larutil::GeometryHelper::GetME();

    for (int t=0; t < ev_trk->size(); t++) {

      auto const& trk = ev_trk->at(t);

      if (trk.Length() < 150.) continue;

      auto const& endpt = trk.End();

      auto const& pt = geoH->Point_3Dto2D(endpt,2);

      int endw = pt.w/10;
      int endt = (pt.t + 800 * _t2cm)/10;

      // grab hits associated to this track
      auto const& ass_hit_v = trk_hit_ass.at(t);

      std::vector<int> nearbyhit_v;
      double nearbyhit_q = 0;
      
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
	    if (d < 5.) { nearbyhit_v.push_back(hit_idx); nearbyhit_q += hit.Integral(); }
	  }// for all hits in cell
	}// for all time cells
      }// for all wire cells

      // find the closest true muon end-point
      double ddmin = 100000.;
      for (size_t k=0; k < mu_end_pt_v.size(); k++) {

	auto mcendpt = mu_end_pt_v[k];
	
	double dd = sqrt( (endpt.Y() - mcendpt[1])*(endpt.Y() - mcendpt[1]) + (endpt.Z() - mcendpt[2])*(endpt.Z() - mcendpt[2]) );
	if (dd < ddmin) { ddmin = dd; }
      }// for all MC tracks

      /*
      std::cout << "end hit position : " << pt.w << ", " << pt.t << std::endl;
      std::cout << "there are " << nearbyhit_v.size() << " hits near this point" << std::endl;
      std::cout << "nearest track end point : " << ddmin << std::endl;
      std::cout << std::endl;
      */

      _x = endpt.X();
      _y = endpt.Y();
      _z = endpt.Z();
      _dmin = ddmin;
      _q = nearbyhit_q;
      _tree->Fill();
      
    }
    
    
    return true;
  }

  bool IsolatedEndPoint::finalize() {


    if (_fout) _fout->cd();
    if (_tree) _tree->Write();

    return true;
  }

}
#endif
