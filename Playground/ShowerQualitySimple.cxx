#ifndef LARLITE_SHOWERQUALITYSIMPLE_CXX
#define LARLITE_SHOWERQUALITYSIMPLE_CXX

#include "ShowerQualitySimple.h"
#include "DataFormat/shower.h"
#include "DataFormat/mcshower.h"
#include <TTree.h>

namespace larlite {

  ShowerQualitySimple::ShowerQualitySimple() 
    : _mc_tree(nullptr)
    , _reco_tree(nullptr)
  {
    _fout = 0;
    _name = "ShowerQualitySimple";
    _producer_name = "showerreco";
    _minE = 100;
  }

  bool ShowerQualitySimple::initialize() {

    _evt = 0;

    if (_mc_tree) { delete _mc_tree; }
    _mc_tree = new TTree("_mc_tree","MC Shower Info Tree");
    _mc_tree->Branch("_evt",&_evt,"evt/I");
    _mc_tree->Branch("_pdg_mc",&_pdg_mc,"pdg_mc/I");
    _mc_tree->Branch("_e_det_mc",&_e_det_mc,"e_det_mc/D");
    _mc_tree->Branch("_x_det_mc",&_x_det_mc,"x_det_mc/D");
    _mc_tree->Branch("_y_det_mc",&_y_det_mc,"y_det_mc/D");
    _mc_tree->Branch("_z_det_mc",&_z_det_mc,"z_det_mc/D");
    _mc_tree->Branch("_px_det_mc",&_px_det_mc,"px_det_mc/D");
    _mc_tree->Branch("_py_det_mc",&_py_det_mc,"py_det_mc/D");
    _mc_tree->Branch("_pz_det_mc",&_pz_det_mc,"pz_det_mc/D");
    _mc_tree->Branch("_e_sim_mc",&_e_sim_mc,"e_sim_mc/D");
    _mc_tree->Branch("_x_sim_mc",&_x_sim_mc,"x_sim_mc/D");
    _mc_tree->Branch("_y_sim_mc",&_y_sim_mc,"y_sim_mc/D");
    _mc_tree->Branch("_z_sim_mc",&_z_sim_mc,"z_sim_mc/D");
    _mc_tree->Branch("_px_sim_mc",&_px_sim_mc,"px_sim_mc/D");
    _mc_tree->Branch("_py_sim_mc",&_py_sim_mc,"py_sim_mc/D");
    _mc_tree->Branch("_pz_sim_mc",&_pz_sim_mc,"pz_sim_mc/D");
    _mc_tree->Branch("_containment",&_containment,"containment/D");

    if (_reco_tree) { delete _reco_tree; }
    _reco_tree = new TTree("_reco_tree","RECO Shower Info Tree");
    _reco_tree->Branch("_evt",&_evt,"evt/I");
    _reco_tree->Branch("_best_mcs",&_best_mcs,"best_mcs/I");
    _reco_tree->Branch("_dedx",&_dedx,"dedx/D");
    _reco_tree->Branch("_dist",&_dist,"dist/D");
    _reco_tree->Branch("_dir",&_dir,"dir/D");
    _reco_tree->Branch("_e_reco",&_e_reco,"e_reco/D");
    _reco_tree->Branch("_x_reco",&_x_reco,"x_reco/D");
    _reco_tree->Branch("_y_reco",&_y_reco,"y_reco/D");
    _reco_tree->Branch("_z_reco",&_z_reco,"z_reco/D");
    _reco_tree->Branch("_px_reco",&_px_reco,"px_reco/D");
    _reco_tree->Branch("_py_reco",&_py_reco,"py_reco/D");
    _reco_tree->Branch("_pz_reco",&_pz_reco,"pz_reco/D");
    _reco_tree->Branch("_pdg_best",&_pdg_best,"pdg_best/I");
    _reco_tree->Branch("_e_det_best",&_e_det_best,"e_det_best/D");
    _reco_tree->Branch("_x_det_best",&_x_det_best,"x_det_best/D");
    _reco_tree->Branch("_y_det_best",&_y_det_best,"y_det_best/D");
    _reco_tree->Branch("_z_det_best",&_z_det_best,"z_det_best/D");
    _reco_tree->Branch("_px_det_best",&_px_det_best,"px_det_best/D");
    _reco_tree->Branch("_py_det_best",&_py_det_best,"py_det_best/D");
    _reco_tree->Branch("_pz_det_best",&_pz_det_best,"pz_det_best/D");
    _reco_tree->Branch("_e_sim_best",&_e_sim_best,"e_sim_best/D");
    _reco_tree->Branch("_x_sim_best",&_x_sim_best,"x_sim_best/D");
    _reco_tree->Branch("_y_sim_best",&_y_sim_best,"y_sim_best/D");
    _reco_tree->Branch("_z_sim_best",&_z_sim_best,"z_sim_best/D");
    _reco_tree->Branch("_px_sim_best",&_px_sim_best,"px_sim_best/D");
    _reco_tree->Branch("_py_sim_best",&_py_sim_best,"py_sim_best/D");
    _reco_tree->Branch("_pz_sim_best",&_pz_sim_best,"pz_sim_best/D");

    return true;
  }
  
  bool ShowerQualitySimple::analyze(storage_manager* storage) {

    _evt += 1;
  
    // Retrieve mcshower data product
    auto ev_mcs = storage->get_data<event_mcshower>("mcreco");
    
    if(!ev_mcs || !(ev_mcs->size())) {
      print(msg::kERROR,__FUNCTION__,"MCShower data product not found!");
      return false;
    }

    // Get shower
    auto ev_shr = storage->get_data<event_shower>(_producer_name);
    if(!ev_shr) {
      print(msg::kERROR,__FUNCTION__,
	    Form("Did not find shower produced by \"%s\"",_producer_name.c_str())
	    );
      return false;
    }

    // Loop through mcshowers and recoshowers
    for (size_t i=0; i < ev_mcs->size(); i++){
      if (ev_mcs->at(i).DetProfile().E() < _minE) { continue; }
      auto const& mcs = ev_mcs->at(i);
      auto const& det = mcs.DetProfile();
      auto const& sim = mcs.Start();
      _pdg_mc     = mcs.PdgCode();
      _e_det_mc   = det.E();
      _x_det_mc   = det.X();
      _y_det_mc   = det.Y();
      _z_det_mc   = det.Z();
      _px_det_mc  = det.Px();
      _py_det_mc  = det.Py();
      _pz_det_mc  = det.Pz();
      _e_sim_mc   = sim.E();
      _x_sim_mc   = sim.X();
      _y_sim_mc   = sim.Y();
      _z_sim_mc   = sim.Z();
      _px_sim_mc  = sim.Px();
      _py_sim_mc  = sim.Py();
      _pz_sim_mc  = sim.Pz();
      _containment = _e_det_mc/_e_sim_mc;
      _mc_tree->Fill();
    }
    
    // loop through reco showers
    for (size_t j=0; j < ev_shr->size(); j++){
      
      auto const& shr = ev_shr->at(j);
      auto bestplane = shr.best_plane();
      _dedx    = shr.dEdx()[bestplane];
      _e_reco  = shr.Energy()[bestplane];
      _x_reco  = shr.ShowerStart()[0];
      _y_reco  = shr.ShowerStart()[1];
      _z_reco  = shr.ShowerStart()[2];
      _px_reco = shr.Direction()[0];
      _py_reco = shr.Direction()[1];
      _pz_reco = shr.Direction()[2];

      // loop through all mc again and compare -> try and find the best one
      _best_mcs = -1;
      double best_dist = 1036;
      for (size_t i=0; i < ev_mcs->size(); i++){
	if (ev_mcs->at(i).DetProfile().E() < _minE) { continue; }
	auto const& mcs = ev_mcs->at(i);
	auto const& det = mcs.DetProfile();
	auto const& sim = mcs.Start();
	double dist = sqrt ( (det.X()-_x_reco)*(det.X()-_x_reco) + 
			     (det.X()-_x_reco)*(det.X()-_x_reco) +
			     (det.X()-_x_reco)*(det.X()-_x_reco) );
	if (dist < best_dist){
	  best_dist = dist;
	  _best_mcs  = i;
	}
      }
      if (_best_mcs >= 0){
	auto const& mcs = ev_mcs->at(_best_mcs);
	auto const& det = mcs.DetProfile();
	auto const& sim = mcs.Start();
	_pdg_best     = mcs.PdgCode();
	_e_det_best   = det.E();
	_x_det_best   = det.X();
	_y_det_best   = det.Y();
	_z_det_best   = det.Z();
	_px_det_best  = det.Px();
	_py_det_best  = det.Py();
	_pz_det_best  = det.Pz();
	_e_sim_best   = sim.E();
	_x_sim_best   = sim.X();
	_y_sim_best   = sim.Y();
	_z_sim_best   = sim.Z();
	_px_sim_best  = sim.Px();
	_py_sim_best  = sim.Py();
	_pz_sim_best  = sim.Pz();
	_dist = sqrt ( (det.X()-_x_reco)*(det.X()-_x_reco) + 
		       (det.X()-_x_reco)*(det.X()-_x_reco) +
		       (det.X()-_x_reco)*(det.X()-_x_reco) );
	_dir = ( ( (det.Px()*_px_reco) + (det.Py()*_py_reco) + (det.Pz()*_pz_reco) ) / 
		 sqrt ( (_px_reco*_px_reco) + (_py_reco*_py_reco) + (_pz_reco*_pz_reco) ) );
      }// if we found a best shower
      _reco_tree->Fill();

    }
    
    return true;
  }
  
  bool ShowerQualitySimple::finalize() {
    
    if (_fout){
      if (_mc_tree) { _mc_tree->Write(); }
      if (_reco_tree) { _reco_tree->Write(); }
    }


    return true;
  }

}
#endif
