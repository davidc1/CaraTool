#ifndef LARLITE_COVERAGE_CXX
#define LARLITE_COVERAGE_CXX

#include "Coverage.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"

#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/track.h"

namespace larlite {

  bool Coverage::initialize() {

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
  
  bool Coverage::analyze(storage_manager* storage) {

    auto geom = larutil::Geometry::GetME();
    auto geoH = larutil::GeometryHelper::GetME();

    _trk2D_v.clear();

    double *origin;
    origin = new double[3];
    geom->PlaneOriginVtx(2,origin);

    auto ev_trk  = storage->get_data<event_track>("pandoraCosmic");

    // loop through all tracks and
    for (size_t t=0; t < ev_trk->size(); t++) {

      auto const& trk = ev_trk->at(t);

      // vector of 2D points
      std::vector< std::pair<double,double> > trk2D;

      // loop through all track points
      for (size_t nstep=0; nstep < trk.NumberTrajectoryPoints(); nstep++) {

	auto const& pt3D = trk.LocationAtPoint(nstep);

	//std::cout << "3D step : " << pt3D.X() << ", " << pt3D.Y() << ", " << pt3D.Z() << "\t\t";

	auto const& pt2D = geoH->Point_3Dto2D(pt3D,2);
	
	auto const& pt = geoH->Point_3Dto2D(pt3D,2);
	
	trk2D.push_back( std::make_pair(pt.w, pt.t + (800 * _t2cm) + origin[0] ) ); 

	//std::cout << "2D coordinates : " << trk2D.at(trk2D.size()-1).first << ", " << trk2D.at(trk2D.size()-1).second << std::endl;

      }// for all trajectory points

      _trk2D_v.push_back( trk2D );
      
    }// for all tracks

    // scan all points in TPC and measure rhit for each

    for(int w=10; w < 93; w++) {
      for (int t=10; t < 25; t++) {

	_w = w * 10;
	_t = t * 10;
	
	_rhit = R(_w,_t).first;

	_tree->Fill();

      }// for all times
    }// for all wires
      
    return true;
  }

  bool Coverage::finalize() {

    _tree->Write();

    return true;
  }


  std::pair<double,double> Coverage::R(const double& comW, const double& comT) {

    //std::cout << "COM : " << comW << ", " << comT << std::endl;

    double Rtot = 0.;

    double Rmin = 1e6;

    for (size_t t=0; t < _trk2D_v.size(); t++) {
      
      auto const& trk2D = _trk2D_v.at(t);

      double ddmin = 10e6;
      
      for (auto const& pt : trk2D) {

	double dd = (pt.first - comW) * (pt.first - comW) + (pt.second - comT) * (pt.second - comT);

	if (dd < ddmin) { ddmin = dd; }

      }// for all 2D points

      //std::cout << "\t distance to track : " << sqrt(ddmin) << std::endl;

      Rtot += 1. / sqrt(ddmin);

      if (sqrt(ddmin) < Rmin) { Rmin = sqrt(ddmin); }
      
    }// for all 2D tracks

    //std::cout << "Rmin = " << Rmin << std::endl;
    //std::cout << "effective dist : " << 1./Rtot << std::endl;

    return std::make_pair(Rmin,1./Rtot);
  }

}
#endif
