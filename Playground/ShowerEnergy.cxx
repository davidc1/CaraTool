#ifndef SHOWERENERGY_CXX
#define SHOWERENERGY_CXX

#include "ShowerEnergy.h"

namespace larlite {

  ShowerEnergy::ShowerEnergy()
    : _shr_tree(nullptr)
    , _ide_tree(nullptr)
  {
    _name = "ShowerEnergy";
    _fout = 0;
    // keep track of IDE ID
    _id = 0;
    LArProp = (::larutil::LArProperties*)(::larutil::LArProperties::GetME());
  }

  bool ShowerEnergy::initialize() {

    // Prepare IDE tree
    if (_ide_tree)
      delete _ide_tree;
    _ide_tree = new TTree("_ide_tree","ide_tree");
    _ide_tree->Branch("_x",&_x,"x/D");
    _ide_tree->Branch("_y",&_y,"y/D");
    _ide_tree->Branch("_z",&_z,"z/D");
    _ide_tree->Branch("_e",&_e,"e/D");
    _ide_tree->Branch("_q",&_q,"q/D");
    _ide_tree->Branch("_pl",&_pl,"pl/I");
    _ide_tree->Branch("_t",&_t,"t/I");
    _ide_tree->Branch("_id",&_id,"id/I");
    _ide_tree->Branch("_eboxU",&_eboxU,"eboxU/D");
    _ide_tree->Branch("_eboxV",&_eboxV,"eboxV/D");
    _ide_tree->Branch("_eboxY",&_eboxY,"eboxY/D");
    _ide_tree->Branch("_ebrkU",&_ebrkU,"ebrkU/D");
    _ide_tree->Branch("_ebrkV",&_ebrkV,"ebrkV/D");
    _ide_tree->Branch("_ebrkY",&_ebrkY,"ebrkY/D");
    _ide_tree->Branch("_qLife",&_qLife,"qLife/D");
    _ide_tree->Branch("_theta",&_theta,"theta/D");
    _ide_tree->Branch("_pitchU",&_pitchU,"pitchU/D");
    _ide_tree->Branch("_pitchV",&_pitchV,"pitchV/D");
    _ide_tree->Branch("_pitchY",&_pitchY,"pitchY/D");
    _ide_tree->Branch("_phi",&_phi,"phi/D");

    // Prepare Shower Tree
    if (_shr_tree)
      delete _shr_tree;
    _shr_tree = new TTree("_shr_tree","shr_tree");
    _shr_tree->Branch("_E",&_E,"E/D");
    _shr_tree->Branch("_Edep",&_Edep,"Edep/D");
    _shr_tree->Branch("_EideU",&_EideU,"EideU/D");
    _shr_tree->Branch("_EideV",&_EideV,"EideV/D");
    _shr_tree->Branch("_EideY",&_EideY,"EideY/D");
    _shr_tree->Branch("_EboxU",&_EboxU,"EboxU/D");
    _shr_tree->Branch("_EboxV",&_EboxV,"EboxV/D");
    _shr_tree->Branch("_EboxY",&_EboxY,"EboxY/D");
    _shr_tree->Branch("_EbrkU",&_EbrkU,"EbrkU/D");
    _shr_tree->Branch("_EbrkV",&_EbrkV,"EbrkV/D");
    _shr_tree->Branch("_EbrkY",&_EbrkY,"EbrkY/D");
    _shr_tree->Branch("_QideU",&_QideU,"QideU/D");
    _shr_tree->Branch("_QideV",&_QideV,"QideV/D");
    _shr_tree->Branch("_QideY",&_QideY,"QideY/D");
    _shr_tree->Branch("_pitchU",&_pitchU,"pitchU/D");
    _shr_tree->Branch("_pitchV",&_pitchV,"pitchV/D");
    _shr_tree->Branch("_pitchY",&_pitchY,"pitchY/D");
    _shr_tree->Branch("_X",&_X,"X/D");
    _shr_tree->Branch("_Y",&_Y,"Y/D");
    _shr_tree->Branch("_Z",&_Z,"Z/D");
    _shr_tree->Branch("_px",&_px,"px/D");
    _shr_tree->Branch("_py",&_py,"py/D");
    _shr_tree->Branch("_pz",&_pz,"pz/D");
    _shr_tree->Branch("_theta",&_theta,"theta/D");
    _shr_tree->Branch("_phi",&_phi,"phi/D");

    return true;
  }
  
  bool ShowerEnergy::analyze(storage_manager* storage) {

    // clear trees
    clearShrTree();
    _EideU = _EideV = _EideY = 0;
    _EboxU = _EboxV = _EboxY = 0;
    _EbrkU = _EbrkV = _EbrkY = 0;
    _QideU = _QideV = _QideY = 0;
    clearIDETree();
  
    // get Showers
    auto evt_shower = storage->get_data<event_mcshower>(_ShowerProducer.c_str());

    if (!evt_shower){
      std::cout << "This shower Type does not exist! Exiting..." << std::endl;
      return false;
    }

    // get simchannels
    auto evt_simch = storage->get_data<event_simch>("largeant");

    if (!evt_simch){
      std::cout << "No simch! This is a problem..." << std::endl;
      return false;
    }

    // choose events with a single shower
    if (evt_shower->size() != 1){
      std::cout << "Event has != 1 showers! Exit..." << std::endl;
      return true;
    }

    // make simch map
    //std::cout << "make map" << std::endl;
    makeMap(evt_simch);
    //std::cout << "make done!" << std::endl;

    // get mcshower's info
    auto const& shr = evt_shower->at(0);
    auto const& start = shr.DetProfile();
    _X  = start.X();
    _Y  = start.Y();
    _Z  = start.Z();
    _px = start.Px();
    _py = start.Py();
    _pz = start.Pz();
    _Edep = start.E();
    _E  = shr.Start().E();


    // get dx for shower
    _theta = acos(_py/sqrt(_px*_px+_py*_py+_pz*_pz));
    _phi   = atan(_pz/_px);
    // calculate pitch
    //_pitchU = larutil::GeometryUtilities::GetME()->CalculatePitch(0,phi,theta);
    //_pitchV = larutil::GeometryUtilities::GetME()->CalculatePitch(1,phi,theta);
    //_pitchY = larutil::GeometryUtilities::GetME()->CalculatePitch(2,phi,theta);
    _pitchU = larutil::GeometryUtilities::GetME()->PitchInView(0,_phi,_theta);
    _pitchV = larutil::GeometryUtilities::GetME()->PitchInView(1,_phi,_theta);
    _pitchY = larutil::GeometryUtilities::GetME()->PitchInView(2,_phi,_theta);

    // loop over voxels
    std::map<::radius::Point,std::pair<double,double> >::iterator it;
    for (it = _vxlMap.begin(); it != _vxlMap.end(); it++){
      auto const& pos = it->first;
      _x = pos[0];
      _y = pos[1];
      _z = pos[2];
      _e = (it->second).first;
      _q = (it->second).second;
      double ttick = 3200-(_x/256.)*3200.;
      _qLife =  _q/_calo.LifetimeCorrection(ttick);
      _eboxY = 0.03*LArProp->ModBoxCorrection(_qLife/(0.03));
      _ebrkY = 0.03*LArProp->BirksCorrection(_qLife/(0.03));      
      _EboxY += _eboxY;
      _EbrkY += _ebrkY;
      _ide_tree->Fill();
    }
    
    
    /*
    // save all simchannel information
    for (size_t i=0; i < evt_simch->size(); i++){
      auto const& simch = evt_simch->at(i);
      // get channel and plane info
      UInt_t ch = simch.Channel();
      _pl = larutil::Geometry::GetME()->ChannelToPlane(ch);
      double dedx;
      // get map of TDC -> vector<ides> for this simch object
      const std::map<unsigned short, std::vector<larlite::ide> > ideMap = simch.TDCIDEMap();
      // create iterator to loop through ides
      std::map<unsigned short, std::vector<larlite::ide> >::const_iterator it;
      for (it = ideMap.begin(); it != ideMap.end(); it++){
	auto const& ideVec = it->second;
	// get time-tick
	_t = it->first-3200;
	_id += 1;
	for (auto const& ide : ideVec){
	  _e = ide.energy;
	  _x = ide.x;
	  _y = ide.y;
	  _z = ide.z;
	  _q = ide.numElectrons;
	  // time needs to be in time-ticks
	  double ttick = 3200-(_x/256.)*3200.;
	  // lifetime correction using time reconstructed from x position
	  _qLife = _q/_calo.LifetimeCorrection(ttick);
	  // lifetime correction using time from Tick
	  //_qLife = _q*_calo.LifetimeCorrection(_t);
	  if (_pl == 0) {
	    _EideU += _e;
	    _QideU += _qLife;
	    dedx = LArProp->ModBoxCorrection(_qLife/(_pitchU));
	    _eboxU = (_pitchU)*dedx;
	    _EboxU += _eboxU;
	    dedx = LArProp->BirksCorrection(_qLife/(_pitchU));
	    _ebrkU = (_pitchU)*dedx;
	    _EbrkU += _ebrkU;
	  }
	  if (_pl == 1) {
	    _EideV += _e;
	    _QideV += _qLife;
	    dedx = LArProp->ModBoxCorrection(_qLife/(_pitchV));
	    _eboxV = (_pitchV)*dedx;
	    _EboxV += _eboxV;
	    dedx = LArProp->BirksCorrection(_qLife/(_pitchV));
	    _ebrkV = (_pitchV)*dedx;
	    _EbrkV += _ebrkV;
	  }
	  if (_pl == 2) {
	    _EideY += _e;
	    _QideY += _qLife;
	    dedx = LArProp->ModBoxCorrection(_qLife/(_pitchY));
	    _eboxY = (_pitchY)*dedx;
	    _EboxY += _eboxY;
	    dedx = LArProp->BirksCorrection(_qLife/(_pitchY));
	    _ebrkY = (_pitchY)*dedx;
	    _EbrkY += _ebrkY;
	  }
	  _ide_tree->Fill();
	}// for all ides
      }// for all ideMaps
    }// for all simchannels
    */

	
    _shr_tree->Fill();

    return true;
  }

  bool ShowerEnergy::finalize() {

    // Write trees & histos
    _shr_tree->Write();
    _ide_tree->Write();
  
    return true;
  }

  void ShowerEnergy::clearShrTree(){

    _E  = _max;
    _X  = _max;
    _Y  = _max;
    _Z  = _max;
    _px = _max;
    _py = _max;
    _pz = _max;
    _Edep  = _max;

    return;
  }

  void ShowerEnergy::clearIDETree(){

    _x = _y = _z = 0;
    _e = 0;
    _q = 0;

    return;
  }


  void ShowerEnergy::makeMap(const larlite::event_simch* evt_simch){

    _vxlMap.clear();

    std::pair<double,double> pair;

    for (size_t i=0; i < evt_simch->size(); i++){

      auto const simch = evt_simch->at(i);
      
      UInt_t ch = simch.Channel();
      _pl = larutil::Geometry::GetME()->ChannelToPlane(ch);
      // if not Y-plane, continue
      if (_pl != 2)
	continue;
      
      // get map of TDC -> vector<ides> for this simch object
      const std::map<unsigned short, std::vector<larlite::ide> > ideMap = simch.TDCIDEMap();
      // create iterator to loop through ides
      std::map<unsigned short, std::vector<larlite::ide> >::const_iterator it;
      for (it = ideMap.begin(); it != ideMap.end(); it++){
	auto const& ideVec = it->second;

	for (auto const& ide : ideVec){
	  
	  // lifetime correct the energy
	  
	  std::vector<double> vv = {ide.x,ide.y,ide.z};
	  ::radius::Point v(vv);
	  if (_vxlMap.find(v) == _vxlMap.end()){
	    // new voxel -> make pair
	    pair = std::make_pair(ide.energy,ide.numElectrons);
	    _vxlMap[v] = pair;
	  }
	  else{
	    // voxel already exists
	    _vxlMap[v].first  += ide.energy;
	    _vxlMap[v].second += ide.numElectrons;
	  }
	}// for all ides
      }// for all ide vectors in map
    }// for all simchannels
  }
  
}
#endif
