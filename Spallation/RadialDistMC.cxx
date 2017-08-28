#ifndef LARLITE_RADIALDISTMC_CXX
#define LARLITE_RADIALDISTMC_CXX

#include "RadialDistMC.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"

#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/mctrack.h"

namespace larlite {

  bool RadialDistMC::initialize() {

    if (_tree) {delete _tree;}
    _tree = new TTree("tree","tree");
    _tree->Branch("_q",&_q,"q/D");
    _tree->Branch("_r",&_r,"r/D");
    _tree->Branch("_w",&_w,"w/D");
    _tree->Branch("_t",&_t,"t/D");
    _tree->Branch("_rsum",&_rsum,"rsum/D");
    _tree->Branch("_rhit",&_rhit,"rhit/D");
    

    _w2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _t2cm  = larutil::GeometryHelper::GetME()->TimeToCm();
    
    return true;
  }
  
  bool RadialDistMC::analyze(storage_manager* storage) {

    auto geom = larutil::Geometry::GetME();
    auto geoH = larutil::GeometryHelper::GetME();

    _trk2D_v.clear();

    double *origin;
    origin = new double[3];
    geom->PlaneOriginVtx(2,origin);

    auto ev_mctrk = storage->get_data<event_mctrack>("mcreco");
    auto ev_clus  = storage->get_data<event_cluster>("spallation");
    larlite::event_hit *ev_hit = nullptr;
    auto ass_clus_hit_v = storage->find_one_ass(ev_clus->id(),ev_hit, ev_clus->name());

    // loop through all tracks and
    for (size_t t=0; t < ev_mctrk->size(); t++) {

      auto const& mctrk = ev_mctrk->at(t);

      if (mctrk.size() == 0) continue;

      // vector of 2D poin
      std::vector< std::pair<double,double> > trk2D;

      double time = mctrk.at(0).Position().T();

      // loop through all track points
      for (size_t nstep=0; nstep < mctrk.size(); nstep++) {

	auto const& pt3D = mctrk.at(nstep).Position().Vect();

	//std::cout << "3D step : " << pt3D.X() << ", " << pt3D.Y() << ", " << pt3D.Z() << "\t\t";

	auto const& pt2D = geoH->Point_3Dto2D(pt3D,2);
	
	auto const& pt = geoH->Point_3Dto2D(pt3D,2);
	
	trk2D.push_back( std::make_pair(pt.w, pt.t + (800 * _t2cm) + origin[0] + (time/1000.)*2*_t2cm ) ); 

	//std::cout << "2D coordinates : " << trk2D.at(trk2D.size()-1).first << ", " << trk2D.at(trk2D.size()-1).second << std::endl;

      }// for all trajectory points

      _trk2D_v.push_back( trk2D );
      
    }// for all tracks

    // loop through all clusters and find their COM
    for (size_t c=0; c < ev_clus->size(); c++) {

      // grab associated hits
      auto const& hit_idx_v = ass_clus_hit_v.at(c);

      if (hit_idx_v.size() == 0) continue;
      if (ev_hit->at(hit_idx_v[0]).WireID().Plane != 2) continue;

      // position
      double comW = 0;
      double comT = 0;

      // charge
      double Q = 0;

      for (auto const& hit_idx : hit_idx_v) {

	auto const& hit = ev_hit->at(hit_idx);

	comW += hit.WireID().Wire * _w2cm;
	comT += hit.PeakTime()    * _t2cm;

	Q += hit.Integral();
	
      }

      comW /= hit_idx_v.size();
      comT /= hit_idx_v.size();

      _w = comW;
      _t = comT;

      // find 2D track weighted R
      auto rinfo = R(comW,comT);
      _r    = rinfo.first.first;
      _rsum = rinfo.first.second;
      // index of closest track
      auto trkidx = rinfo.second;
      // grab closest track and figure out distance of nearest hit
      _rhit = 1e6;
      auto const& trk2D = _trk2D_v.at(trkidx);
      for (auto const& pt : trk2D) {
	for (auto const& hit_idx : hit_idx_v) {
	  auto const& hit = ev_hit->at(hit_idx);
	  auto const& hw = hit.WireID().Wire * _w2cm;
	  auto const& ht = hit.PeakTime() * _t2cm;
	  double dd = (hw - pt.first) * (hw - pt.first) + (hw - pt.first) * (hw - pt.first);
	  if (dd < _rhit) { _rhit = dd; }
	}// for all hits in cluster
      }// for all points in track
      _rhit = sqrt(_rhit);

      _q = Q;
      _tree->Fill();
      
    }// for all clusters
  
    return true;
  }

  bool RadialDistMC::finalize() {

    _tree->Write();

    return true;
  }


  std::pair< std::pair<double,double>, size_t> RadialDistMC::R(const double& comW, const double& comT) {

    //std::cout << "COM : " << comW << ", " << comT << std::endl;

    double Rtot = 0.;

    double Rmin = 1e6;

    // index of nearest track
    size_t ntrk;

    for (size_t t=0; t < _trk2D_v.size(); t++) {
      
      auto const& trk2D = _trk2D_v.at(t);

      double ddmin = 10e6;
      
      for (auto const& pt : trk2D) {

	double dd = (pt.first - comW) * (pt.first - comW) + (pt.second - comT) * (pt.second - comT);

	if (dd < ddmin) { ddmin = dd; }

      }// for all 2D points

      //std::cout << "\t distance to track : " << sqrt(ddmin) << std::endl;

      Rtot += 1. / sqrt(ddmin);

      if (sqrt(ddmin) < Rmin) { Rmin = sqrt(ddmin); ntrk = t; }
      
    }// for all 2D tracks

    //std::cout << "Rmin = " << Rmin << std::endl;
    //std::cout << "effective dist : " << 1./Rtot << std::endl;

    return std::make_pair( std::make_pair(Rmin,1./Rtot), ntrk);
  }

}
#endif
