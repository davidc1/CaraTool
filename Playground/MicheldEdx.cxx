#ifndef LARLITE_MICHELDEDX_CXX
#define LARLITE_MICHELDEDX_CXX

#include "MicheldEdx.h"
#include "DataFormat/track.h"
#include "DataFormat/calorimetry.h"

namespace larlite {

  MicheldEdx::MicheldEdx()
    : _tree_dist(nullptr)
    , _tree_calo(nullptr)
  {
    _name = "MicheldEdx";
    _fout = 0;
  }
  

  bool MicheldEdx::initialize() {

    std::cout << "there are " <<  _michel_map.size() << "  michel events" << std::endl;

    if (_tree_dist) delete _tree_dist;
    _tree_dist = new TTree("_tree_dist","distance");
    _tree_dist->Branch("_dw",&_dw,"dw/D");
    _tree_dist->Branch("_dt",&_dt,"dt/D");
    _tree_dist->Branch("_d" ,&_d ,"d/D" );

    if (_tree_calo) delete _tree_calo;
    _tree_calo = new TTree("_tree_calo","calorimetry");
    _tree_calo->Branch("_rr",&_rr,"rr/D");
    _tree_calo->Branch("_x",&_x,"x/D");
    _tree_calo->Branch("_px",&_px,"px/D");
    _tree_calo->Branch("_y",&_y,"y/D");
    _tree_calo->Branch("_py",&_py,"py/D");
    _tree_calo->Branch("_z",&_z,"z/D");
    _tree_calo->Branch("_pz",&_pz,"pz/D");
    _tree_calo->Branch("_dqds" ,&_dqds ,"dqds/D" );

    return true;
  }
  
  bool MicheldEdx::analyze(storage_manager* storage) {
  
    auto ev_tracks = storage->get_data<event_track>("pandoraCosmic");

    larlite::event_calorimetry* ev_calo = nullptr;
    auto const& ass_trk_calo_v = storage->find_one_ass( ev_tracks->id(), ev_calo, "pandoraCosmiccalo");

    auto const& evt = storage->event_id();
    auto const& run = storage->run_id();
    std::pair<int,int> evtinfo(run,evt);
    
    if (_michel_map.find( evtinfo ) == _michel_map.end() ){
      std::cout << "event not matched to michel txt file...exit " << std::endl;
      return true;
    }

    // grab muon end point for this event
    auto const& mustop = _michel_map[ evtinfo ];

    if (!ev_calo) { std::cout << "NO CALO!" << std::endl; return true; }

    // loop through tracks
    for (size_t t=0; t < ev_tracks->size(); t++){

      auto const& trk = ev_tracks->at(t);

      if (trk.Length() < 60) continue;

      // does this track match the Michel?
      if ( MatchTrack(trk,mustop) == false ) continue;

      //std::cout << "found a match with distance " << _d << std::endl;

      auto const& calo_ass_v = ass_trk_calo_v.at(t);

      //std::cout << "found associated calo stuff... " << std::endl;

      // loop throguh calos
      for (auto calo_idx : calo_ass_v){

	auto const& calo = ev_calo->at(calo_idx);

	if (calo.PlaneID().Plane != 2) continue;

	// grab dqds values and xyz coordinates
	auto const& dqds_v = calo.dQdx();
	auto const& xyz_v  = calo.XYZ();
	auto const& RR_v   = calo.ResidualRange();

	if (dqds_v.size() != RR_v.size())
	  {
	    std::cout << "dqds and xyz vectors not same size...skip..." << std::endl;
	    continue;
	  }

	auto const& trkend = TrackEnd(trk);
	
	for (size_t n=0; n < dqds_v.size(); n++){

	  auto const& dqds = dqds_v.at(n);
	  auto const& RR   = RR_v.at(n);
	  auto const& xyz  = xyz_v.at(n);
	  
	  _x = xyz.X();
	  _y = xyz.Y();
	  _z = xyz.Z();

	  auto vend = trkend-xyz;
	  auto vendnorm = vend.Unit();

	  _px = vendnorm.X();
	  _py = vendnorm.Y();
	  _pz = vendnorm.Z();

	  _rr = RR;
	  
	  _dend = sqrt( ( ( _x - trkend.X() ) * ( _x - trkend.X() ) ) +
			( ( _y - trkend.Y() ) * ( _y - trkend.Y() ) ) +
			( ( _z - trkend.Z() ) * ( _z - trkend.Z() ) ) );
	  
	  _dqds = dqds;

	  _tree_calo->Fill();

	}// for all calorimetry points

      }// for all calo objects associated to the track

    }// loop through all tracks
    
    return true;
  }

  bool MicheldEdx::finalize() {

    if (_fout){ _fout->cd(); }

    _tree_dist->Write();
    _tree_calo->Write();

    return true;
  }


  TVector3 MicheldEdx::TrackEnd(const larlite::track& trk) {

    auto trk_end   = trk.End();
    auto trk_start = trk.Vertex();

    if (trk_end.Y() > trk_start.Y()){
      trk_end   = trk.Vertex();
      trk_start = trk.End();
    }

    return trk_end;
  }


  bool MicheldEdx::MatchTrack(const larlite::track& trk, const std::pair<double,double> mustop) {

    auto trk_end   = trk.End();
    auto trk_start = trk.Vertex();

    if (trk_end.Y() > trk_start.Y()){
      trk_end   = trk.Vertex();
      trk_start = trk.End();
    }

    _dt = trk_end.X() - mustop.first;
    _dw = trk_end.Z() - mustop.second;

    _d = sqrt( _dt * _dt + _dw * _dw );

    _tree_dist->Fill();

    if (_d < 2.0) return true;
    
    return false;
  }


  void MicheldEdx::AddMichel(const int& run, const int& event, const double& xpos, const double& zpos) {

    std::pair<int,int> evtinfo(run,event);
    std::pair<double,double> mustop(xpos,zpos);

    _michel_map[ evtinfo ] = mustop;
    
    return;
  }

}
#endif
