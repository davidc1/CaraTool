#ifndef LARLITE_VERTEXFINDER_CXX
#define LARLITE_VERTEXFINDER_CXX

#include "VertexFinder.h"
#include "DataFormat/hit.h"
#include "DataFormat/mctruth.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"
#include <math.h>

namespace larlite {

  bool VertexFinder::initialize() {

    _wire2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _time2cm  = larutil::GeometryHelper::GetME()->TimeToCm();
    _detwidth = 2*larutil::Geometry::GetME()->DetHalfWidth();

    if (!_vtx_tree) { _vtx_tree = new TTree("_vtx_tree","Vertex Tree"); }
    _vtx_tree->Branch("_distY",&_distY,"distY/D");
    _vtx_tree->Branch("_distV",&_distV,"distV/D");
    _vtx_tree->Branch("_distU",&_distU,"distU/D");
    _vtx_tree->Branch("_distYZ",&_distYZ,"distYZ/D");
    _vtx_tree->Branch("_distX",&_distX,"distX/D");
    _vtx_tree->Branch("_dist3D",&_dist3D,"dist3D/D");
    _vtx_tree->Branch("_area",&_area,"area/D");
    _vtx_tree->Branch("_xAvg",&_xAvg,"xAvg/D");
    _vtx_tree->Branch("_yAvg",&_yAvg,"yAvg/D");
    _vtx_tree->Branch("_zAvg",&_zAvg,"zAvg/D");
    _vtx_tree->Branch("_nu_x",&_nu_x,"nu_x/D");
    _vtx_tree->Branch("_nu_y",&_nu_y,"nu_y/D");
    _vtx_tree->Branch("_nu_z",&_nu_z,"nu_z/D");
    _vtx_tree->Branch("_QvsTheta","std::vector<double>",&_QvsTheta);
    _vtx_tree->Branch("_nProtons",&_nProtons,"nProtons/I");
    _vtx_tree->Branch("_evt",&_evt,"evt/I");
    
    _evt = 0;

    return true;
  }
  
  bool VertexFinder::analyze(storage_manager* storage) {

    auto evt_hits = storage->get_data<event_hit>("gaushit");

    if (!evt_hits){
      std::cout << "No hits!" << std::endl;
      return false;
    }

    // get neutrino vtx info
    auto evt_truth = storage->get_data<event_mctruth>("generator");
    if (!evt_truth or !evt_truth->size()){
      std::cout << "No mctruth!" << std::endl;
      return false;
    }
    auto const& truth     = evt_truth->at(0);
    auto const& nu        = truth.GetNeutrino();
    auto const& firstStep = nu.Nu().Trajectory().back();
    _nu_z = firstStep.Z();
    _nu_x = firstStep.X()+_detwidth;
    _nu_y = firstStep.Y();
    // count the number of protons (that start from < 1 cm of neutrino)
    auto const& parts = truth.GetParticles();
    _nProtons = 0;
    for (auto p : parts){
      if ( (p.PdgCode() == 2212) and ((p.Trajectory().front().E()-p.Mass()) > 0.02) ){
	auto const& ps = p.Trajectory().front();
	double d = sqrt( (ps.X()+_detwidth-_nu_x)*(ps.X()+_detwidth-_nu_x) +
			 (ps.Y()-_nu_y)*(ps.Y()-_nu_y) +
			 (ps.Z()-_nu_z)*(ps.Z()-_nu_z) );
	if (d < 1)
	  _nProtons += 1;
      }// if proton > 20 MeV
    }// for all parts
    

    if (_verbose){ std::cout << "Event: " << _evt << std::endl; }

    // prepare 3-element vector to store best hit on each plane
    std::vector<::larlite::hit> bestH_v;

    for (int plane=0; plane < 3; plane++){
      
      // get best hit on a plane
      size_t bestHitQ = GetBestHit(evt_hits,plane);
      
      if (bestHitQ == 9999)
	return false;
      
      auto const& bestH = evt_hits->at(bestHitQ);
      bestH_v.push_back(bestH);
      double t = bestH.PeakTime()*_time2cm;
      double w = bestH.WireID().Wire*_wire2cm;

      double dist = sqrt( (_nu_z-w)*(_nu_z-w) + (_nu_x-t)*(_nu_x-t) );
      if (plane == 0) _distU = dist;
      if (plane == 1) _distV = dist;
      if (plane == 2) _distY = dist;
      
      if (_verbose){

	std::cout << "Plane: " << plane << std::endl;
	std::cout << "Best Hit    : [" << w << ", " << t << "]" << std::endl;
	std::cout << "Neutrino pos: [" << _nu_z << ", " << _nu_x << "]" << std::endl; 
	std::cout << "dist: " << dist << std::endl;
      }

      if (plane == 2){
	// now loop through hit-list and fill vector with angle to start point direction
	_QvsTheta.clear();
	_QvsTheta = std::vector<double>(360,0.);
	for (size_t h=0; h < evt_hits->size(); h++){
	  auto const& hit = evt_hits->at(h);
	  double t2 = hit.PeakTime()*_time2cm;
	  double w2 = hit.WireID().Wire*_wire2cm;
	  if ( ((w2-w) == 0) and ((t2-t) == 0) )
	    continue;
	  double c = (t2-t)/sqrt( (w2-w)*(w2-w) + (t2-t)*(t2-t) );
	  double angle = (180/3.14)*acos(c);
	  if ( (w2-w) < 0){
	    if (c < 0)
	      angle += 90;
	    else
	      angle += 270;
	  }
	  //      std::cout << angle << std::endl;
	  _QvsTheta[int(angle)] += hit.Integral();
	}
      }// if plane == 2
    }// loop over planes

    // Now reconstruct a 3D vertex from the 3 vertices on each plane
    double yUV, zUV;
    double yUY, zUY;
    double yVY, zVY;
    larutil::Geometry::GetME()->IntersectionPoint( bestH_v[0].WireID().Wire, bestH_v[1].WireID().Wire,
						   0, 1, yUV, zUV);
    larutil::Geometry::GetME()->IntersectionPoint( bestH_v[1].WireID().Wire, bestH_v[2].WireID().Wire,
						   1, 2, yVY, zVY);
    larutil::Geometry::GetME()->IntersectionPoint( bestH_v[0].WireID().Wire, bestH_v[2].WireID().Wire,
						   0, 2, yUY, zUY);

    // average the points
    _yAvg = (yUV+yUY+yVY)/3.;
    _zAvg = (zUV+zUY+zVY)/3.;

    // area from the 3 points (measure of uncertainty)
    _area = ( zUV*(yUY-zVY) + zUY*(yVY-yUV) + zVY*(yUV-yUY) ) / 2.;
    if (_area < 0) { _area *= -1; }
     
    _distYZ = sqrt( (_yAvg-_nu_y)*(_yAvg-_nu_y) + (_zAvg-_nu_z)*(_zAvg-_nu_z) );

    // best time average
    _xAvg  = (bestH_v[0].PeakTime()+bestH_v[1].PeakTime()+bestH_v[2].PeakTime())*_time2cm/3.;
    _distX = _nu_x-_xAvg;
    if (_distX < 0) _distX *= -1;
    
    _dist3D = sqrt ( _distX*_distX + _distYZ*_distYZ );

    _vtx_tree->Fill();

    _evt += 1;

    return true;
  }

  bool VertexFinder::finalize() {

    if (_fout && _vtx_tree)
      _vtx_tree->Write();

    return true;
  }

  void VertexFinder::MakeHitMap(const event_hit* hitlist, int plane){

    _hitMap.clear();
    // temporary pair
    std::pair<int,int> tmpPair;
    
    for (size_t h=0; h < hitlist->size(); h++){
      auto const& hit = hitlist->at(h);
      // skip if not of plane we want
      if (hit.View() != plane)
	continue;
      double t = hit.PeakTime()*_time2cm;
      double w = hit.WireID().Wire*_wire2cm;
      // map is (i,j) -> hit list
      // i : ith bin in wire of some width
      // j : jth bin in time of some width
      int i = int(w/_cellSize);
      int j = int(t/_cellSize);
      tmpPair = std::make_pair(i,j);
      // does this entry exist in the map?
      // if yes -> append to vector
      // if no create new vector and add to map
      if (_hitMap.find(tmpPair) == _hitMap.end()){
	std::vector<size_t> aaa = {h};
	_hitMap[tmpPair] = aaa;
      }
      else
	_hitMap[tmpPair].push_back(h);
    }// for all hits

    return;
  }

  size_t VertexFinder::GetBestHit(const event_hit* evt_hits, int plane){


    MakeHitMap(evt_hits,plane);
    
    // index for best hit
    size_t bestHitQ = 9999;
    size_t bestHitN = 9999;
    // charge at best hit
    double bestCharge = -1;
    double bestNum    = 0;
    
    std::map<std::pair<int,int>, std::vector<size_t> >::iterator it;
    for (it = _hitMap.begin(); it != _hitMap.end(); it++){
      auto const& hitlist = it->second;
      for (size_t h1=0; h1 < hitlist.size(); h1++){
	auto const& hit1 = evt_hits->at(hitlist[h1]);
	double charge = hit1.Integral();
	double num    = 0;
	for (size_t h2=0; h2 < hitlist.size(); h2++){
	  if (h1 == h2) continue;
	  auto const& hit2 = evt_hits->at(hitlist[h2]);
	  double t1 = hit1.PeakTime()*_time2cm;
	  double w1 = hit1.WireID().Wire*_wire2cm;
	  double t2 = hit2.PeakTime()*_time2cm;
	  double w2 = hit2.WireID().Wire*_wire2cm;
	  double dist = sqrt( (t1-t2)*(t1-t2) + (w1-w2)*(w1-w2) );
	  if (dist < _radius){
	    charge += hit2.Integral();
	    num    += 1;
	  }
	}// for second loop
	if (charge > bestCharge){
	  bestCharge = charge;
	  bestHitQ = hitlist[h1];
	}
	if (num > bestNum){
	  bestNum = num;
	  bestHitN = hitlist[h1];
	}
      }// for first loop
    }// loop over map entries

    return bestHitQ;
  }

}
#endif
