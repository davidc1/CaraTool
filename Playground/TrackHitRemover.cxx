#ifndef LARLITE_TRACKHITREMOVER_CXX
#define LARLITE_TRACKHITREMOVER_CXX

#include "TrackHitRemover.h"

namespace larlite {

  TrackHitRemover::TrackHitRemover(){
    _name = "TrkHitRemover";
    _fout = 0;
    _trackProducer  = "";
    _showerProducer = "";
    _outHitProducer = "outhits";
    _verbose = false;
    _getShowerHits = false;
  }

  bool TrackHitRemover::initialize() {

    return true;
  }
  
  bool TrackHitRemover::analyze(storage_manager* storage) {

    // clear vectors to be filled with hit indices:
    _trk_hit_indices.clear();
    _shr_hit_indices.clear();
    _out_hits.clear();


    // ************************************
    // Now get hits associated with showers
    if (_getShowerHits){
      std::cout << "looking for shower  hits" << std::endl;
      // Get shower
      auto ev_shower = storage->get_data<event_shower>(_showerProducer);
      if(!ev_shower) {
	print(msg::kERROR,__FUNCTION__,Form("Did not find shower produced by \"%s\"",_showerProducer.c_str()));
	return false;
      }

      std::cout << "there are " << ev_shower->size() << " showers" << std::endl;
      
      // get associated clusters
      event_cluster* ev_cluster = nullptr;
      auto const& ass_cluster_v = storage->find_one_ass(ev_shower->id(),ev_cluster,ev_shower->name());
      
      if (!ev_cluster){
	print(msg::kERROR,__FUNCTION__, Form("No associated cluster found to a shower produced by \"%s\"", _showerProducer.c_str()));
	return false;
      }
      else if(ev_cluster->size()<1) {
	print(msg::kERROR,__FUNCTION__,Form("There are 0 clusters in this event! Skipping......"));      
	return false;
      }
      
      // get associated hits
      event_hit* ev_hit_shr = nullptr;
      auto const& ass_hit_v = storage->find_one_ass(ev_cluster->id(),ev_hit_shr,ev_cluster->name());
      
      if (!ev_hit_shr){
	print(msg::kERROR,__FUNCTION__, Form("No associated hit found to a shower produced by \"%s\"", ev_cluster->name().c_str()));
	return false;
      }
      else if(ev_hit_shr->size()<1) {
	print(msg::kERROR,__FUNCTION__,Form("There are 0 hits in this event! Skipping......"));      
	return false;
      }

      _event_hit_shr = *ev_hit_shr;

      // fill vector of shower-hit index
      std::cout << "number of associated clusters: " << ass_cluster_v.size() << std::endl;
      for (size_t i=0; i < ass_cluster_v.size(); i++){
	std::vector<unsigned int> shrHits;
	for (size_t j=0; j < ass_cluster_v[i].size(); j++){
	  for (size_t k=0; k < ass_hit_v[ass_cluster_v[i][j]].size(); k++)
	    shrHits.push_back(ass_hit_v[ass_cluster_v[i][j]][k]);
	}
	_shr_hit_indices.push_back(shrHits);
      }
      
    }// if we should get hits associated with showers
    // **********************************************

    // If we did not specify a track producer
    // just move on!
    if (_trackProducer == "")
      return true;

    // Get Tracks that we want to remove
    auto ev_track = storage->get_data<event_track>(_trackProducer);
    if(!ev_track) {
      print(msg::kERROR,__FUNCTION__,Form("Did not find shower produced by \"%s\"",_trackProducer.c_str()));
      return false;
    }
    
    // get associated hits
    event_hit* ev_hit_trk = nullptr;
    auto const& ass_hit_v = storage->find_one_ass(ev_track->id(),ev_hit_trk,ev_track->name());
    
    _event_hit_trk = *ev_hit_trk;
    _ass_v = ass_hit_v;
    
    if (!ev_hit_trk){
      print(msg::kERROR,__FUNCTION__, Form("No associated hit found to a track produced by \"%s\"", _trackProducer.c_str()));
      return false;
    }
    else if(ev_hit_trk->size()<1) {
      print(msg::kERROR,__FUNCTION__,Form("There are 0 hits in this event! Skipping......"));      
      return false;
    }

      
    // loop over associated hit vector to find hits associated with tracks
    int num_ass_hits = 0;
    for (size_t t=0; t < ass_hit_v.size(); t++){
      if (_verbose) { std::cout << "track " << t << " has " << ass_hit_v[t].size() << " hits associated with it" << std::endl; }
      num_ass_hits += ass_hit_v[t].size();
    }

    // Get all hits of this type
    event_hit* all_hit = storage->get_data<event_hit>(ev_hit_trk->name());

    if (_verbose){
      std::cout << "There are " << ass_hit_v.size() << " tracks in this event" << std::endl;
      std::cout << "Hits associated with track of type " << _trackProducer.c_str() << ": " << num_ass_hits << std::endl;
      std::cout << "Hits of type  with track of type   " << ev_hit_trk->name().c_str() << ": " << all_hit->size() << std::endl;
      std::cout << std::endl;
    }

    // vector containing all indices of all hits to be removed
    std::vector<unsigned int> hits_to_remove_indices;

    // For the hits in each track, make a Cluster and calculate its parameters using CPAN
    for (size_t t=0; t < ass_hit_v.size(); t++){
      std::vector<const larlite::hit*> hits;
      for (size_t h=0; h < ass_hit_v[t].size(); h++)
	hits.push_back(&(ev_hit_trk->at(ass_hit_v[t][h])));
      ::cluster::ClusterParamsAlg cluster(hits);
      cluster.FillParams(true,true,true,true,true,false);
      // get values of quantities to be used
      auto const& mod_hit_dens = cluster.GetParams().modified_hit_density;
      auto const& multi_hit_wires = cluster.GetParams().multi_hit_wires;
      auto const& principal = TMath::Log(1-cluster.GetParams().eigenvalue_principal);
      auto const& opening_angle = cluster.GetParams().opening_angle;

      //      if ( (mod_hit_dens < 1.4) || (multi_hit_wires < 3.5) || (principal < -6) || (hits.size() > 200) ){
      if ( (hits.size() > 200) || ( opening_angle < 0.5 && hits.size() > 50) ) {
	// then we have a track
	std::vector<unsigned int> trkHitIndices;
	for (size_t h=0; h < ass_hit_v[t].size(); h++){
	  trkHitIndices.push_back(ass_hit_v[t][h]); // add hits to list of hits to remove
	  hits_to_remove_indices.push_back(ass_hit_v[t][h]);
	}
	_trk_hit_indices.push_back(trkHitIndices);
      }// if cluster is track-like
    }

    // iterator to search list of removed hits
    std::vector<unsigned int>::iterator it;
    // create new event_hit object where to store output hits
    auto ev_hits_out = storage->get_data<event_hit>(_outHitProducer);
    for (size_t n=0; n < ev_hit_trk->size(); n++){
      // find this hit
      it = std::find(hits_to_remove_indices.begin(), hits_to_remove_indices.end(), n);
      // if this hit is not found in the list to be removed
      if (it == hits_to_remove_indices.end()){
	_out_hits.push_back(n);
	ev_hits_out->push_back(ev_hit_trk->at(n));
      }
    }// for all hits
    
    std::cout << "input  hits: " << ev_hit_trk->size() << std::endl;
    std::cout << "output hits: " << ev_hits_out->size() << std::endl;
      

    return true;
  }

  bool TrackHitRemover::finalize() {

    return true;
  }

}
#endif
