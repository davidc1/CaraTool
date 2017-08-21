#ifndef LARLITE_RADIALDIST_CXX
#define LARLITE_RADIALDIST_CXX

#include "RadialDist.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"

#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/track.h"

namespace larlite {

  bool RadialDist::initialize() {

    if (_tree) {delete _tree;}
    _tree = new TTree("tree","tree");
    _tree->Branch("_q",&_q,"q/D");
    _tree->Branch("_r",&_r,"r/D");
    

    _w2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _t2cm  = larutil::GeometryHelper::GetME()->TimeToCm();
    
    return true;
  }
  
  bool RadialDist::analyze(storage_manager* storage) {

    auto geom = larutil::Geometry::GetME();
    auto geoH = larutil::GeometryHelper::GetME();

    _trk2D_v.clear();

    double *origin;
    origin = new double[3];
    geom->PlaneOriginVtx(2,origin);

    auto ev_trk  = storage->get_data<event_track>("pandoraCosmic");
    auto ev_clus = storage->get_data<event_cluster>("spallation");
    larlite::event_hit *ev_hit = nullptr;
    auto ass_clus_hit_v = storage->find_one_ass(ev_clus->id(),ev_hit, ev_clus->name());

    // loop through all tracks and
    for (size_t t=0; t < ev_trk->size(); t++) {

      auto const& trk = ev_trk->at(t);

      // vector of 2D points
      std::vector< std::pair<double,double> > trk2D;

      // loop through all track points
      for (size_t nstep=0; nstep < trk.NumberTrajectoryPoints(); nstep++) {

	auto const& pt3D = trk.LocationAtPoint(nstep);

	auto const& pt2D = geoH->Point_3Dto2D(pt3D,2);
	
	auto const& pt = geoH->Point_3Dto2D(pt3D,2);
	
	trk2D.push_back( std::make_pair(pt.w, pt.t + 800 * _t2cm - origin[0] ) ); 

      }// for all trajectory points

      _trk2D_v.push_back( trk2D );
      
    }// for all tracks

    // loop through all clusters and find their COM
    for (size_t c=0; c < ev_clus->size(); c++) {

      // grab associated hits
      auto const& hit_idx_v = ass_clus_hit_v.at(c);

      // position
      double comW = 0;
      double comT = 0;

      // charge
      double Q;

      for (auto const& hit_idx : hit_idx_v) {

	auto const& hit = ev_hit->at(hit_idx);

	comW += hit.WireID().Wire   * _w2cm;
	comT += hit.PeakAmplitude() * _t2cm;

	Q += hit.Integral();
	
      }

      comW /= hit_idx_v.size();
      comT /= hit_idx_v.size();

      // find 2D track weighted R
      _r = R(comW,comT);
      _q = Q;
      _tree->Fill();
      
    }// for all clusters
  
    return true;
  }

  bool RadialDist::finalize() {

    _tree->Write();

    return true;
  }


  double RadialDist::R(const double& comW, const double& comT) {

    double Rtot = 0.;

    for (auto const& trk2D : _trk2D_v) {

      double ddmin = 10e6;
      
      for (auto const& pt : trk2D) {

	double dd = (pt.first - comW) * (pt.first - comW) + (pt.second - comT) * (pt.second - comT);

	if (dd < ddmin) { ddmin = dd; }

      }// for all 2D points

      Rtot += 1. / sqrt(ddmin);
      
    }// for all 2D tracks


    return 1./Rtot;
  }

}
#endif
