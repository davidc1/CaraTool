#ifndef LARLITE_SIMCHMAP_CXX
#define LARLITE_SIMCHMAP_CXX

#include "SimchMap.h"
#include "DataFormat/simch.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  SimchMap::SimchMap()
    : _tree(nullptr)
  {
    _name = "SimchMap";
    _fout = 0;
  }

  bool SimchMap::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("tree","shower tree");
    _tree->Branch("_shr_E",&_shr_E,"shr_E/D");
    _tree->Branch("_shr_x",&_shr_x,"shr_x/D");
    _tree->Branch("_shr_y",&_shr_y,"shr_y/D");
    _tree->Branch("_shr_z",&_shr_z,"shr_z/D");
    _tree->Branch("_shr_px",&_shr_px,"shr_px/D");
    _tree->Branch("_shr_py",&_shr_py,"shr_py/D");
    _tree->Branch("_shr_pz",&_shr_pz,"shr_pz/D");
    _tree->Branch("_ioni_E",&_ioni_E,"ioni_E/D");
    _tree->Branch("ide_e","std::vector<double>",&_ide_e);
    _tree->Branch("ide_x","std::vector<double>",&_ide_x);
    _tree->Branch("ide_y","std::vector<double>",&_ide_y);
    _tree->Branch("ide_z","std::vector<double>",&_ide_z);
    

    return true;
  }
  
  bool SimchMap::analyze(storage_manager* storage) {

    auto ev_mcsh  = storage->get_data<event_mcshower>("mcreco");
    if ( (!ev_mcsh) or (ev_mcsh->size() == 0) ){
      std::cout << "No MCShower info in this file" << std::endl;
      return false;
    }

    auto ev_simch = storage->get_data<event_simch>("largeant");
    if ( (!ev_simch) or (ev_simch->size() == 0) ){
      std::cout << "No simch info in this file" << std::endl;
      return false;
    }
    
    bool hasMichel = false;
    
    for (auto const& mcs : *ev_mcsh){
      
      
      //std::cout << "shower PDG " << mcs.PdgCode() << std::endl;
      
      if ( ( ( mcs.MotherPdgCode() == 13 ) || ( mcs.MotherPdgCode() == -13 ) ) &&              // parent is a muon
	   ( ( mcs.Process() == "muMinusCaptureAtRest" ) || ( mcs.Process() == "Decay" ) ) &&  // process is consistent with michel decay
	   ( mcs.DetProfile().E()/mcs.Start().E()  > 0.5 ) ) {                                 // at least 50% of energy is contained
	hasMichel = true;
      }
      
      if (hasMichel == false){
	std::cout << "no Michel electron -> quit..." << std::endl;
	continue;
	//return true;
      }

      _ide_e.clear();
      _ide_x.clear();
      _ide_y.clear();
      _ide_z.clear();
      _ioni_E = 0;
      
      _shr_x  = mcs.Start().X();
      _shr_y  = mcs.Start().Y();
      _shr_z  = mcs.Start().Z();
      _shr_px = mcs.Start().Px();
      _shr_py = mcs.Start().Py();
      _shr_pz = mcs.Start().Pz();
      _shr_E  = mcs.DetProfile().E();

      // get shower trackid
      auto michelID = mcs.TrackID();

      for (size_t i=0; i < ev_simch->size(); i++){
	
	auto const& simch = ev_simch->at(i);

	if (simch.Channel() < 4800)
	  continue;
	
	auto const& all_ide = simch.TrackIDsAndEnergies(0,19600);
	
	for (size_t j=0; j < all_ide.size(); j++){
	  
	  auto const& ide = all_ide[j];

	  // if trackID is same as primary michel -> add to ionization energy
	  if (ide.trackID == michelID)
	    _ioni_E += ide.energy;

	  else{
	    _ide_x.push_back(ide.x);
	    _ide_y.push_back(ide.y);
	    _ide_z.push_back(ide.z);
	    _ide_e.push_back(ide.energy);
	  }

	}// for all IDes
	
      }// for all simchannels

    }// for all MCShowers
      
    _tree->Fill();
  
    return true;
  }

  bool SimchMap::finalize() {

    if (_fout){
      if (_tree) { _tree->Write(); }
    }

    return true;
  }

}
#endif
