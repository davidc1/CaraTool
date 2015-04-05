#ifndef MUONCALORIMETRY_CXX
#define MUONCALORIMETRY_CXX

#include "MuonCalorimetry.h"

namespace larlite {

  bool MuonCalorimetry::initialize() {


    // Initialize Geometry Utilities
    //    if(!_GUtil) _GUtil = (larutil::GeometryUtilities*)(larutil::GeometryUtilities::GetME());

    // Setup CalAlgos
    _CaloAlgBox.setUseModBox(true);
    _CaloAlgBirks.setUseModBox(false);

    // wire and time -> cm conversion constants
    //    _w2cm = _GUtil->WireToCm();
    //    _t2cm = _GUtil->TimeToCm();

    // detector width from geometry
    _detWidth = ::larutil::Geometry::GetME()->DetHalfWidth()*2;

    /// Set TPC
    _TpcBox = geoalgo::AABox(0, -(::larutil::Geometry::GetME()->DetHalfHeight()), 0,
			     2*(::larutil::Geometry::GetME()->DetHalfWidth()),
			     ::larutil::Geometry::GetME()->DetHalfHeight(),
			     ::larutil::Geometry::GetME()->DetLength());

    // set dimensionality for track point holders
    _trackU = new geoalgo::Trajectory(0,3);
    _trackV = new geoalgo::Trajectory(0,3);
    _trackY = new geoalgo::Trajectory(0,3);

    // set producer names
    _hitProducer = "gaushit";
    _mctrkProducer = "mcreco";

    // Particle Tree
    if (_part_tree) { delete _part_tree; }
    _part_tree = new TTree("part_tree","Particle Info");
    _part_tree->Branch("_contained",&_contained,"contained/I");
    _part_tree->Branch("_Emc",&_Emc,"Emc/D");
    _part_tree->Branch("_ElinearAmp",&_ElinearAmp,"ElinearAmp/D");
    _part_tree->Branch("_ElinearArea",&_ElinearArea,"ElinearArea/D");
    _part_tree->Branch("_EBirksAmp",&_EBirksAmp,"EBirksAmp/D");
    _part_tree->Branch("_EBirksArea",&_EBirksArea,"EBirksArea/D");
    _part_tree->Branch("_EBoxAmp",&_EBoxAmp,"EBoxAmp/D");
    _part_tree->Branch("_EBoxArea",&_EBoxArea,"EBoxArea/D");

    // Tree
    if (_hit_tree_U) { delete _hit_tree_U; }
    _hit_tree_U = new TTree("hit_tree_U","hit tree U");
    _hit_tree_U->Branch("_muE",&_muE,"muE/D");
    _hit_tree_U->Branch("_resRange",&_resRange,"resRange/D");
    _hit_tree_U->Branch("_hitToTrackDist",&_hitToTrackDist,"hitToTrackDist/D");
    _hit_tree_U->Branch("_dEdx_U_BoxAmp",&_dEdx_U_BoxAmp,"dEdx_U_BoxAmp/D");
    _hit_tree_U->Branch("_dEdx_U_BoxArea",&_dEdx_U_BoxArea,"dEdx_U_BoxArea/D");
    _hit_tree_U->Branch("_dEdx_U_BirksAmp",&_dEdx_U_BirksAmp,"dEdx_U_BirksAmp/D");
    _hit_tree_U->Branch("_dEdx_U_BirksArea",&_dEdx_U_BirksArea,"dEdx_U_BirksArea/D");

    if (_hit_tree_V) { delete _hit_tree_V; }
    _hit_tree_V = new TTree("hit_tree_V","hit tree V");
    _hit_tree_V->Branch("_muE",&_muE,"muE/D");
    _hit_tree_V->Branch("_resRange",&_resRange,"resRange/D");
    _hit_tree_V->Branch("_hitToTrackDist",&_hitToTrackDist,"hitToTrackDist/D");
    _hit_tree_V->Branch("_dEdx_V_BoxAmp",&_dEdx_V_BoxAmp,"dEdx_V_BoxAmp/D");
    _hit_tree_V->Branch("_dEdx_V_BoxArea",&_dEdx_V_BoxArea,"dEdx_V_BoxArea/D");
    _hit_tree_V->Branch("_dEdx_V_BirksAmp",&_dEdx_V_BirksAmp,"dEdx_V_BirksAmp/D");
    _hit_tree_V->Branch("_dEdx_V_BirksArea",&_dEdx_V_BirksArea,"dEdx_V_BirksArea/D");

    if (_hit_tree_Y) { delete _hit_tree_Y; }
    _hit_tree_Y = new TTree("hit_tree_Y","hit tree Y");
    _hit_tree_Y->Branch("_muE",&_muE,"muE/D");
    _hit_tree_Y->Branch("_resRange",&_resRange,"resRange/D");
    _hit_tree_Y->Branch("_hitToTrackDist",&_hitToTrackDist,"hitToTrackDist/D");
    _hit_tree_Y->Branch("_dEdx_Y_BoxAmp",&_dEdx_Y_BoxAmp,"dEdx_Y_BoxAmp/D");
    _hit_tree_Y->Branch("_dEdx_Y_BoxArea",&_dEdx_Y_BoxArea,"dEdx_Y_BoxArea/D");
    _hit_tree_Y->Branch("_dEdx_Y_BirksAmp",&_dEdx_Y_BirksAmp,"dEdx_Y_BirksAmp/D");
    _hit_tree_Y->Branch("_dEdx_Y_BirksArea",&_dEdx_Y_BirksArea,"dEdx_Y_BirksArea/D");

    // Histograms
    _hdEdx_U_BoxAmp = new TH2D("_hdEdx_U_BoxAmp","dEdx vs. Residual Range for Muon Tracks - U Plane - Box & AMP",100,0,200,100,0,10);
    _hdEdx_U_BoxArea = new TH2D("_hdEdx_U_BoxArea","dEdx vs. Residual Range for Muon Tracks - U Plane - Box & Area",100,0,200,100,0,10);
    _hdEdx_U_BirksAmp = new TH2D("_hdEdx_U_BirksAmp","dEdx vs. Residual Range for Muon Tracks - U Plane - Birks & AMP",100,0,200,100,0,10);
    _hdEdx_U_BirksArea = new TH2D("_hdEdx_U_BirksArea","dEdx vs. Residual Range for Muon Tracks - U Plane - Birks & Area",100,0,200,100,0,10);
    _hdEdx_V = new TH2D("_hdEdx_V","dEdx vs. Residual Range for Muon Tracks - V Plane",100,0,200,100,0,10000);
    _hdEdx_Y = new TH2D("_hdEdx_Y","dEdx vs. Residual Range for Muon Tracks - Y Plane",100,0,200,100,0,10000);

    return true;
  }
  
  bool MuonCalorimetry::analyze(storage_manager* storage) {
    /*

    // get hits
    auto evt_hits = storage->get_data<event_hit>(_hitProducer.c_str());
    if (!evt_hits){
      std::cout << "This hit Type does not exist! Exiting..." << std::endl;
      return false;
    }

    // get MC tracks
    auto evt_mctrk = storage->get_data<event_mctrack>(_mctrkProducer.c_str());
    if (!evt_mctrk){
      std::cout << "This track Type does not exist! Exiting..." << std::endl;
      return false;
    }  
    if (evt_mctrk->size() > 1){
      std::cout << "More than one track! Not interested in this case! Move to next event..." << std::endl;
      return false;
    }
    
    // get track
    mctrack t = evt_mctrk->at(0);
    // take 3D track points and project onto 2D track objects
    fillTrajectoryPoints(t);
    // check containment: if particle starts & ends in TPC -> contained
    geoalgo::Point_t tStart(t.Start().X(),t.Start().Y(),t.Start().Z());
    geoalgo::Point_t tEnd(t.End().X(),t.End().Y(),t.End().Z());
    if ( _TpcBox.Contain(tStart) && _TpcBox.Contain(tEnd) ) { _contained = 1; }
    else { _contained = 0; }
    // get pitch from track direction
    mcstep trkS = t.Start();
    _Emc = trkS.E();
    double theta = (180/3.14)*asin(trkS.Py()/sqrt(trkS.Px()*trkS.Px()+trkS.Py()*trkS.Py()+trkS.Pz()*trkS.Pz()));
    double phi   = (180/3.14)*asin(trkS.Px()/sqrt(trkS.Px()*trkS.Px()+trkS.Pz()*trkS.Pz()));
    double pitch_U = _GUtil->PitchInView(0,phi,theta);
    double pitch_V = _GUtil->PitchInView(1,phi,theta);
    double pitch_Y = _GUtil->PitchInView(2,phi,theta);

    // clear energy holders
    _ElinearAmp  = 0;
    _ElinearArea = 0;
    _EBirksAmp   = 0;
    _EBirksArea  = 0;
    _EBoxAmp     = 0;
    _EBoxArea    = 0;

    // now go through hits
    // for each hit figure out residual range by:
    //   1) finding closest point from hit to track
    //   2) measuring distance from that point to the end of the track
    for (auto &h : *evt_hits){
      // point of closest approach of hit on track
      geoalgo::Point_t HitOnTrack(3);
      // residual range
      // hit 2D point object
      geoalgo::Point_t hitcm(h.Wire()*_w2cm,(h.PeakTime()-3200)*_t2cm,0.);

      // figure out the plane
      if (h.View() == larlite::geo::View_t::kU){
	int idx = 0; // keep track of segment at which hit is found to get muon energy
	HitOnTrack = _geoAlgo.ClosestPt(hitcm,*_trackU,idx);
	_hitToTrackDist = HitOnTrack.Dist(hitcm);
	// now figure out residual range
	_resRange = HitOnTrack.Dist(_trackU->at(_trackU->size()-1));
	_muE = t.at(idx).E();
	// get dEdx information
	_dEdx_U_BoxAmp     = _CaloAlgBox.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_U,0);
	_dEdx_U_BoxArea    = _CaloAlgBox.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_U,0);
	_dEdx_U_BirksAmp   = _CaloAlgBirks.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_U,0);
	_dEdx_U_BirksArea  = _CaloAlgBirks.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_U,0);
	_hit_tree_U->Fill();
	_hdEdx_U_BoxAmp->Fill(_resRange,_dEdx_U_BoxAmp);
	_hdEdx_U_BoxArea->Fill(_resRange,_dEdx_U_BoxArea);
	_hdEdx_U_BirksAmp->Fill(_resRange,_dEdx_U_BirksAmp);
	_hdEdx_U_BirksArea->Fill(_resRange,_dEdx_U_BirksArea);
	if (_debug){
	  std::cout << "Hit coordinates: [" << h.Wire()*_w2cm << ", " << (h.PeakTime()-_tDiff)*_t2cm << "]" << std::endl;
	  std::cout << "Distance from Hit to Track Trajectory: " << _hitToTrackDist << std::endl;
	  std::cout << "residual range: " << _resRange << std::endl;
	}
      }
      if (h.View() == larlite::geo::View_t::kV){
	HitOnTrack = _geoAlgo.ClosestPt(hitcm,*_trackV);
	_hitToTrackDist = HitOnTrack.Dist(hitcm);
	// now figure out residual range
	_resRange = HitOnTrack.Dist(_trackV->at(_trackV->size()-1));
	// get dEdx information
	_dEdx_V_BoxAmp     = _CaloAlgBox.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_V,1);
	_dEdx_V_BoxArea    = _CaloAlgBox.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_V,1);
	_dEdx_V_BirksAmp   = _CaloAlgBirks.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_V,1);
	_dEdx_V_BirksArea  = _CaloAlgBirks.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_V,1);
	_hit_tree_V->Fill();
	if (_debug){
	  std::cout << "Hit coordinates: [" << h.Wire()*_w2cm << ", " << (h.PeakTime()-_tDiff)*_t2cm << "]" << std::endl;
	  std::cout << "Distance from Hit to Track Trajectory: " << _hitToTrackDist << std::endl;
	  std::cout << "residual range: " << _resRange << std::endl;
	}
      }
      if (h.View() == larlite::geo::View_t::kW){
	HitOnTrack = _geoAlgo.ClosestPt(hitcm,*_trackY);
	_hitToTrackDist = HitOnTrack.Dist(hitcm);
	// now figure out residual range
	_resRange = HitOnTrack.Dist(_trackY->at(_trackY->size()-1));
	// get dEdx information
	_dEdx_Y_BoxAmp     = _CaloAlgBox.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_Y,2);
	_dEdx_Y_BoxArea    = _CaloAlgBox.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_Y,2);
	_dEdx_Y_BirksAmp   = _CaloAlgBirks.dEdx_AMP(h.Charge(true),h.PeakTime()-_tDiff,pitch_Y,2);
	_dEdx_Y_BirksArea  = _CaloAlgBirks.dEdx_AREA(h.Charge(false),h.PeakTime()-_tDiff,pitch_Y,2);
	_hit_tree_Y->Fill();
	if (_debug){
	  std::cout << "Hit coordinates: [" << h.Wire()*_w2cm << ", " << (h.PeakTime()-_tDiff)*_t2cm << "]" << std::endl;
	  std::cout << "Distance from Hit to Track Trajectory: " << _hitToTrackDist << std::endl;
	  std::cout << "residual range: " << _resRange << std::endl;
	}
	
	// get particle energy
	double ampElectrons  = _CaloAlgBox.ElectronsFromADCPeak(h.Charge(true),2);
	double areaElectrons = _CaloAlgBox.ElectronsFromADCArea(h.Charge(false),2);
	ampElectrons  *= _CaloAlgBox.LifetimeCorrection(h.PeakTime()-_tDiff);
	areaElectrons *= _CaloAlgBox.LifetimeCorrection(h.PeakTime()-_tDiff);
	ampElectrons  *= 1.e3 / (::larutil::kGeVToElectrons);
	areaElectrons *= 1.e3 / (::larutil::kGeVToElectrons);
	_ElinearAmp += ampElectrons;
	_ElinearArea += areaElectrons;

	_EBoxAmp  += _dEdx_Y_BoxAmp*pitch_Y;
	_EBoxArea += _dEdx_Y_BoxArea*pitch_Y;

	_EBirksAmp  += _dEdx_Y_BirksAmp*pitch_Y;
	_EBirksArea += _dEdx_Y_BirksArea*pitch_Y;

	_EBoxAmp  += _dEdx_Y_BoxAmp*pitch_Y;
	_EBoxArea += _dEdx_Y_BoxArea*pitch_Y;

      }

    }// finish loop over hits

    _part_tree->Fill();
    */
    return true;
  }

  bool MuonCalorimetry::finalize() {

    // Write trees
    _hit_tree_U->Write();
    _hit_tree_V->Write();
    _hit_tree_Y->Write();
    _part_tree->Write();
    
    // Write histograms
    _hdEdx_U_BoxAmp->Write();
    _hdEdx_U_BoxArea->Write();
    _hdEdx_U_BirksAmp->Write();
    _hdEdx_U_BirksArea->Write();

    return true;
  }

  void MuonCalorimetry::fillTrajectoryPoints(const mctrack &t){
    /*
    // clear trajectory-point holders
    _trackU->clear();
    _trackV->clear();
    _trackY->clear();

    // Take mctrack step points and project them
    // onto each plane.
    for (size_t pt=0; pt < t.size(); pt++){
      geoalgo::Point_t p(t.at(pt).X(), t.at(pt).Y(), t.at(pt).Z());
      ::larutil::PxPoint pU = _GUtil->Get2DPointProjectionCM(p,0);
      ::larutil::PxPoint pV = _GUtil->Get2DPointProjectionCM(p,1);
      ::larutil::PxPoint pY = _GUtil->Get2DPointProjectionCM(p,2);
      geoalgo::Point_t Uproj(pU.w, pU.t, 0.);
      geoalgo::Point_t Vproj(pV.w, pV.t, 0.) ;
      geoalgo::Point_t Yproj(pY.w, pY.t, 0.);
      
      _trackU->push_back(Uproj);
      _trackV->push_back(Vproj);
      _trackY->push_back(Yproj);
    }// for all points on track

    if (_debug){
      std::cout << "Track: " << std::endl
		<< "U Plane: [" << _trackU->at(0)[0] << ", " << _trackU->at(0)[1] << "] --> [" 
		<< _trackU->at(_trackU->size()-1)[0] << ", " << _trackU->at(_trackU->size()-1)[1] << "]" << std::endl
		<< "V Plane: [" << _trackV->at(0)[0] << ", " << _trackV->at(0)[1] << "] --> [" 
		<< _trackY->at(_trackV->size()-1)[0] << ", " << _trackV->at(_trackV->size()-1)[1] << "]" << std::endl
		<< "Y Plane: [" << _trackY->at(0)[0] << ", " << _trackY->at(0)[1] << "] --> [" 
		<< _trackY->at(_trackY->size()-1)[0] << ", " << _trackY->at(_trackY->size()-1)[1] << "]" << std::endl;
    }
    */
    return;
  }

}
#endif
