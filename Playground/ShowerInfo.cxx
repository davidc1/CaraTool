#ifndef SHOWERINFO_CXX
#define SHOWERINFO_CXX

#include "ShowerInfo.h"

namespace larlite {

  bool ShowerInfo::initialize() {

    // Prepare Histograms
    _hBestIP = new TH1D("hBestIP","Smallest IP in Evt.",100,0,100);

    // Prepare IP tree
    if (_ipTree)
      delete _ipTree;
    _ipTree = new TTree("IPTree","");
    _ipTree->Branch("_IP",&_IP,"IP/D");
    _ipTree->Branch("_rad1",&_rad1,"rad1/D");
    _ipTree->Branch("_rad2",&_rad1,"rad2/D");

    // Prepare Shower Tree
    if (_shrTree)
      delete _shrTree;
    _shrTree = new TTree("ShowerTree","");
    _shrTree->Branch("_E",&_E,"E/D");
    _shrTree->Branch("_x",&_x,"x/D");
    _shrTree->Branch("_y",&_y,"y/D");
    _shrTree->Branch("_z",&_z,"z/D");
    _shrTree->Branch("_px",&_px,"px/D");
    _shrTree->Branch("_py",&_py,"py/D");
    _shrTree->Branch("_pz",&_pz,"pz/D");
    _shrTree->Branch("_UdEdx",&_UdEdx,"UdEdx/D");
    _shrTree->Branch("_VdEdx",&_VdEdx,"VdEdx/D");
    _shrTree->Branch("_YdEdx",&_YdEdx,"YdEdx/D");
    

    return true;
  }
  
  bool ShowerInfo::analyze(storage_manager* storage) {
  
    // get Showers
    auto evt_shower = storage->get_data<event_shower>(_ShowerProducer.c_str());

    if (!evt_shower){
      std::cout << "This shower Type does not exist! Exiting..." << std::endl;
      return false;
    }
	

    // loop over showers
    for (unsigned int i=0; i < evt_shower->size(); i++ ){
      shower s = evt_shower->at(i);
      //Clear Shr Tree
      clearShrTree();
      //Get variables
      _E = s.Energy()[0];
      geoalgo::Point_t shrS = s.ShowerStart();
      geoalgo::Vector_t shrD = s.Direction();
      _x  = shrS[0];
      _y  = shrS[1];
      _z  = shrS[2];
      _px = shrD[0];
      _py = shrD[1];
      _pz = shrD[2];
      _UdEdx = s.dEdx()[0];
      _VdEdx = s.dEdx()[1];
      _YdEdx = s.dEdx()[2];
      // Fill Tree
      _shrTree->Fill();
    }

    // Now calculate distance backwards for each pair of showers
    // and keep track of best pair
    double minIP = _max;
    for (unsigned int s1=0; s1 < evt_shower->size(); s1++ ){
      geoalgo::Point_t shr1S = evt_shower->at(s1).ShowerStart();
      geoalgo::Vector_t shr1D = evt_shower->at(s1).Direction();
      shr1D*=-1;
      geoalgo::HalfLine_t s1Line(shr1S,shr1D);
      for (unsigned int s2=s1+1; s2 < evt_shower->size(); s2++ ){
	geoalgo::Point_t shr2S = evt_shower->at(s2).ShowerStart();
	geoalgo::Vector_t shr2D = evt_shower->at(s2).Direction();
	shr2D *= -1;
	geoalgo::HalfLine_t s2Line(shr2S,shr2D);
	
	// clear tree
	clearIPTree();
	// Get distance backwards
	geoalgo::Point_t PoCA1(3);
	geoalgo::Point_t PoCA2(3);
	_IP = gA.SqDist(s1Line,s2Line,PoCA1,PoCA2);
	_rad1 = shr1S.Dist(PoCA1);
	_rad2 = shr2S.Dist(PoCA2);
	_ipTree->Fill();
	if (_IP < minIP) { minIP = _IP; }
      }
    }
    _hBestIP->Fill(minIP);
    
	
  
    return true;
  }

  bool ShowerInfo::finalize() {

    // Write trees & histos
    _shrTree->Write();
    _ipTree->Write();
    _hBestIP->Write();
  
    return true;
  }

  void ShowerInfo::clearShrTree(){

    _E  = _max;
    _x  = _max;
    _y  = _max;
    _z  = _max;
    _px = _max;
    _py = _max;
    _pz = _max;

    return;
  }

  void ShowerInfo::clearIPTree(){

    _IP   = _max;
    _rad1 = _max;
    _rad2 = _max;

    return;
  }

}
#endif
