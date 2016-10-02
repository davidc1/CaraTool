#ifndef LARLITE_SHOWERTREE_CXX
#define LARLITE_SHOWERTREE_CXX

#include "ShowerTree.h"
#include "DataFormat/mcshower.h"
#include "LArUtil/LArUtilException.h"
#include "LArUtil/Geometry.h"


namespace larlite {

  ShowerTree::ShowerTree()
    : _tree(nullptr)
  {
    _name = "ShowerTree";
    _fout = 0;
    _debug = false;
    _verbose = false;
  }

  bool ShowerTree::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("_tree","mcshower tree");

    _tree->Branch("_michel_x",&_michel_x,"michel_x/D");
    _tree->Branch("_michel_y",&_michel_y,"michel_y/D");
    _tree->Branch("_michel_z",&_michel_z,"michel_z/D");
    _tree->Branch("_michel_px",&_michel_px,"michel_px/D");
    _tree->Branch("_michel_py",&_michel_py,"michel_py/D");
    _tree->Branch("_michel_pz",&_michel_pz,"michel_pz/D");
    
    _tree->Branch("_e_truth",&_e_truth,"e_truth/D");
    _tree->Branch("_e_dep",&_e_dep,"e_dep/D");
    _tree->Branch("_e_elec",&_e_elec,"e_elec/D");
    _tree->Branch("_e_lost",&_e_lost,"e_lost/D");
    _tree->Branch("_chrage",&_charge,"charge/D");
    _tree->Branch("_process",&_process);

    _tree->Branch("_ide_elec",&_ide_elec,"ide_elec/D");
    _tree->Branch("_ide_shower",&_ide_shower,"ide_shower/D");
    
    _tree->Branch("_photon_edep","std::vector<double>",&_photon_edep);
    _tree->Branch("_photon_x","std::vector<double>",&_photon_x);
    _tree->Branch("_photon_y","std::vector<double>",&_photon_y);
    _tree->Branch("_photon_z","std::vector<double>",&_photon_z);

    _tree->Branch("_electron_energy","std::vector<double>",&_electron_energy);
    _tree->Branch("_electron_x","std::vector<double>",&_electron_x);
    _tree->Branch("_electron_y","std::vector<double>",&_electron_y);
    _tree->Branch("_electron_z","std::vector<double>",&_electron_z);


    _tree->Branch("_ioni_e",&_ioni_e,"ioni_e/D");
    _tree->Branch("_gamma_e",&_gamma_e,"gamma_e/D");
    _tree->Branch("ide_e","std::vector<double>",&_ide_e_v);
    _tree->Branch("ide_x","std::vector<double>",&_ide_x_v);
    _tree->Branch("ide_y","std::vector<double>",&_ide_y_v);
    _tree->Branch("ide_z","std::vector<double>",&_ide_z_v);

    return true;
  }
  
  bool ShowerTree::analyze(storage_manager* storage) {

    _scanned.clear();
    _g4_trackid_v.clear();
    
    auto ev_mcsh  = storage->get_data<event_mcshower>("mcreco");
    if ( (!ev_mcsh) or (ev_mcsh->size() == 0) ){
      std::cout << "No MCShower info in this file" << std::endl;
      return false;
    }
    
    auto ev_mcpart = storage->get_data<event_mcpart>("largeant");
    if ( (!ev_mcpart) or (ev_mcpart->size() == 0) ){
      std::cout << "No MCPart info in this file" << std::endl;
      return false;
    }

    auto ev_simch = storage->get_data<event_simch>("largeant");
    if (!ev_simch or (ev_simch->size() == 0)){
      std::cout << "no Simch info in this file" << std::endl;
      return false;
    }
    
    if (_debug)
      std::cout << "number of MCParticles : " << ev_mcpart->size() << std::endl;
    _calls = 0;
    
    // clear maps to be used in this event
    _pdgMap.clear();
    _parentMap.clear();
    _idxMap.clear();

    for (size_t i=0; i < ev_mcpart->size(); i++){
      
      auto const& part = ev_mcpart->at(i);
      _pdgMap[part.TrackId()]    = part.PdgCode();
      _parentMap[part.TrackId()] = part.Mother();
      _idxMap[part.TrackId()]    = i;
    }// for all MCParticles
    
    bool hasMichel = false;
    
    for (auto const& mcs : *ev_mcsh){

      
      /*
      if ( ( ( mcs.MotherPdgCode() == 13 ) || ( mcs.MotherPdgCode() == -13 ) ) &&              // parent is a muon
	   ( ( mcs.Process() == "muMinusCaptureAtRest" ) || ( mcs.Process() == "Decay" ) ) &&  // process is consistent with michel decay
	   ( mcs.DetProfile().E()/mcs.Start().E()  > 0.5 ) ) {                                 // at least 50% of energy is contained
	hasMichel = true;
      }
      */

      if ( ( ( mcs.MotherPdgCode() == -13 ) && (mcs.PdgCode() == -11) ) &&  // parent is a muon
	   ( mcs.Process() == "Decay" ) &&                                  // process is consistent with michel decay
	   ( mcs.DetProfile().E()/mcs.Start().E()  > 0.9 ) ) {              // at least 50% of energy is contained
	hasMichel = true;
      }
      
      if (hasMichel == false){
	std::cout << "no Michel electron -> quit..." << std::endl;
	continue;
	//return true;
      }

      _michel_x  = mcs.Start().X();
      _michel_y  = mcs.Start().Y();
      _michel_z  = mcs.Start().Z();
      _michel_px = mcs.Start().Px();
      _michel_py = mcs.Start().Py();
      _michel_pz = mcs.Start().Pz();

      // tree node starts w/ the MCShower as a primary
      treenode shower(mcs.TrackID());
      _g4_trackid_v.push_back(mcs.TrackID());
      shower.setPrimary(true);
      
      // fill graph for this mcshower
      try {
	AddChildren(shower,ev_mcpart);
      }
      catch (const std::exception& e) {
	std::cerr << e.what() << '\n';
      }

      _photon_edep.clear();
      _photon_x.clear();
      _photon_y.clear();
      _photon_z.clear();
      _electron_energy.clear();
      _electron_x.clear();
      _electron_y.clear();
      _electron_z.clear();

      // get particle graph with particle info
      std::string diagram;
      
      auto const& idx = shower.getNodeIndex();
      
      auto& traj = ev_mcpart->at(_idxMap[idx]).Trajectory();
      
      diagram += ( " [Idx] = " + std::to_string(idx) +
		   " ... [PDG] = " + std::to_string(_pdgMap[idx]) +
		   " ... [Proc] = " + ev_mcpart->at(_idxMap[idx]).Process() +
		   " ... [EDep] = " + std::to_string( 1000.* ( traj.at(0).E() - traj.at(traj.size()-1).E() ) ) +
		   "\n" );
      
      std::string prefix = "";//\t";
      
      getShowerDiagram(shower,ev_mcpart,diagram,prefix);

      if (_verbose)
	std::cout << diagram << std::endl;
      
      // get the shower's deposited energy by electrons only
      double energy = 0;
      SumLostEnergy(shower,ev_mcpart,22,energy);
      
      _e_truth = mcs.Start().E();
      _e_dep   = mcs.DetProfile().E();
      _e_lost  = energy;
      _e_elec  = mcs.End().E();
      _charge  = mcs.Charge(2);
      _process = mcs.Process();
      
      if (_verbose)
	std::cout << std::endl
		  << "*****************************" << std::endl
		  << "Total Mcsower Start()      energy : " << _e_truth << std::endl
		  << "Total Mcsower DetProfile() energy : " << _e_dep << std::endl
		  << "Total Mcshower primary     energy : " << _e_elec << std::endl
		  << "energy lost to photons [MeV]      : " << _e_lost << std::endl
		  << "*****************************" << std::endl << std::endl;

      if (_debug){
	std::cout << "TrackID : " << mcs.TrackID() << std::endl
		  << "All track IDs :" << std::endl;
	for (auto const& id : mcs.DaughterTrackID())
	  std::cout << id << "  ";
	std::cout << std::endl;
      }

      // list g4 particke ids associated with the shower
      if (_debug){
	std::cout << std::endl
		  << "g4 trackIDs associated to this shower : " << std::endl;
	for (auto const& g4id : _g4_trackid_v)
	  std::cout << g4id << " . ";
	std::cout << std::endl;
      }


      // get IDE vector for this shower
      _ide_shower = getShowerIDEs(_g4_trackid_v,ev_simch);
      // get IDE vector for electron only
      std::vector<unsigned int> _g4_elec{mcs.TrackID()};
      _ide_elec  = getShowerIDEs(_g4_elec,ev_simch);
      

      _ioni_e  = 0;
      _gamma_e = 0;
      _ide_x_v.clear();
      _ide_y_v.clear();
      _ide_z_v.clear();
      _ide_e_v.clear();
      
      // MOVE ON TO IDE PART
      
      // get shower trackid
      auto michelID = mcs.TrackID();

      for (size_t i=0; i < ev_simch->size(); i++){
	
	auto const& simch = ev_simch->at(i);

	if (simch.Channel() < 4800)
	  continue;
	
	auto const& all_ide = simch.TrackIDsAndEnergies(0,19600);
	
	for (size_t j=0; j < all_ide.size(); j++){
	  
	  auto const& ide = all_ide[j];

	  if (std::find(_g4_trackid_v.begin(),_g4_trackid_v.end(),ide.trackID) == _g4_trackid_v.end())
	    continue;
	  
	  // if trackID is same as primary michel -> add to ionization energy
	  if (ide.trackID == michelID)
	    _ioni_e += ide.energy;
	  
	  else{
	    _ide_x_v.push_back(ide.x);
	    _ide_y_v.push_back(ide.y);
	    _ide_z_v.push_back(ide.z);
	    _ide_e_v.push_back(ide.energy);
	    _gamma_e += ide.energy;
	  }

	}// for all IDes
	
      }// for all simchannels
      
      _tree->Fill();
      
    }// for all mcsowers
    
    return true;
  }

  bool ShowerTree::finalize() {

    _fout->cd();
    _tree->Write();

    return true;
  }

  void ShowerTree::AddChildren(treenode& node, const larlite::event_mcpart *parts){

    _calls += 1;
    //std::cout << "\t calls : " << _calls << std::endl;
    
    // get vector of daughters for this particle
    
    // node for this particle
    auto const& node_id = node.getNodeIndex();

    if (_debug)
      std::cout << "examining particle with node id " << node_id << std::endl;
    
    // find the MCPart with this index
    auto const& mcpart = parts->at(_idxMap[node_id]);

    // get all daughters of this mcpart
    auto const& children = mcpart.Daughters();

    if (_debug)
      std::cout << "this many children : " << children.size() << std::endl;

    for (auto const& child_id : children){

      if (_debug)
	std::cout << "new child ID " << child_id << std::endl;

      // if child id same to this id -> skip
      if (child_id == node_id)
	throw larutil::LArUtilException("Child has same trackID as particle...quit...");

      if (std::find(_scanned.begin(),_scanned.end(),child_id) != _scanned.end() && (_scanned.size() > 0) )
	throw larutil::LArUtilException("already examined this particle id! quit");
      
      _scanned.push_back(child_id);
      
      // add the children to the parent node
      //for (size_t i=0; i < children.size(); i++){

      _g4_trackid_v.push_back(child_id);
      
      treenode child(child_id);
      child.setParentId(node.getNodeIndex());
      child.setAncestorId(node.getAncestorId());

      // recursively add children for this new node
      if (_debug)
	std::cout << "add child for this particle" << std::endl;
      
      AddChildren(child,parts);

      // now that the sub-node is complete,
      // add to the parent
      node.AddChild(child);

    }// for all children

    return;
  }


  void ShowerTree::getShowerDiagram(treenode& node, larlite::event_mcpart *parts,
					    std::string& ret, std::string prefix){

    prefix += "\t";

    for (auto& child : node.getChildren()){

      auto const& idx = child.getNodeIndex();

      auto const& trj = parts->at(_idxMap[idx]).Trajectory();

      auto pdg  = _pdgMap[idx];
      auto proc = parts->at(_idxMap[idx]).Process();
      auto edep = 1000. * ( trj.at(0).E() - trj.at(trj.size()-1).E() );

      // if 1st generation
      if (prefix == "\t"){
	// count the photons
	if (pdg == 22)
	  _photon_edep.push_back( edep );
      }

      // save info for "charge pockets" produced
      if ( (pdg == 11) || (pdg == -11) ){
	_electron_energy.push_back( edep );
	_electron_x.push_back( trj.at(0).X() );
	_electron_y.push_back( trj.at(0).Y() );
	_electron_z.push_back( trj.at(0).Z() );
      }

      ret += ( prefix + 
	       " [Idx] = " + std::to_string(idx) +
	       " ... [PDG] = " + std::to_string( pdg ) +
	       " ... [Proc] = " + proc + 
	       " ... [EDep] = " + std::to_string( edep ) +
	       "\n" );
      
      getShowerDiagram(child,parts,ret,prefix);
      
    }// for all children
    
    return;  
  }

  void ShowerTree::SumLostEnergy(treenode& node, larlite::event_mcpart *parts,
					 int PDGignore, double& energy){
    
    for (auto& child : node.getChildren()){
      
      auto const& idx = child.getNodeIndex();
      
      auto pdg = parts->at(_idxMap[idx]).PdgCode();

      // if we find a particle that we want to ignore (i.e. a photon)
      if (pdg == PDGignore){
	auto& part   = parts->at(_idxMap[idx]);
	auto& mctraj = part.Trajectory();
	//std::cout << "Ignoring photon w/ id " << part.TrackId()
	//	  << " and E = " << mctraj.at(0).E()*1000. << std::endl;
	energy += mctraj.at(0).E() * 1000.;
	//continue;
      }
      
      //SumLostEnergy(child,parts,PDGignore,energy);

    }// for all children

    return;
  }


  double ShowerTree::getShowerIDEs(const std::vector<unsigned int>& g4_v,
				   const event_simch* simch_v)
  {
    
    double ide_energy = 0;
    
    auto const& geo = larutil::Geometry::GetME();
    
    for(size_t i=0; i < simch_v->size(); i++){
      
      auto const& sch = simch_v->at(i);
      
      auto const ch = sch.Channel();
      size_t plane = geo->ChannelToPlane(ch);
      
      if (plane != 2)
	continue;

      for(auto const& time_ide : sch.TDCIDEMap()) {
	
	auto const& time  = time_ide.first;
	auto const& ide_v = time_ide.second;
	
	for(auto const& ide : ide_v) {
	  
	  int ide_trk_id = ide.trackID;
	  
	  // is this track-id in the list?
	  if (std::find(g4_v.begin(),g4_v.end(),ide_trk_id) != g4_v.end())
	    ide_energy += ide.energy;
	  
	}// for all IDEs
      }// for all time-slices in the simchannel
    }// for all simchannels
    
    return ide_energy;
  }

  
}
#endif
