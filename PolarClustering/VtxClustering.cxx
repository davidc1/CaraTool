#ifndef LARLITE_VTXCLUSTERING_CXX
#define LARLITE_VTXCLUSTERING_CXX

#include "VtxClustering.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"
#include "DataFormat/cluster.h"
#include "LArUtil/GeometryHelper.h"
#include <math.h>

namespace larlite {

  VtxClustering::VtxClustering()
  {
    _name="VtxClustering";
    _fout=0;
    _bin_width = 3./180. * 3.1415;
    _thresh = 15;
  }

  bool VtxClustering::initialize() {

    _bin_width_deg = _bin_width * 180. / 3.1415;

    _binU = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});
    _binV = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});
    _binY = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});



    return true;
  }
  
  bool VtxClustering::analyze(storage_manager* storage) {

    // grab hits to be used in clustering
    auto *ev_hit = storage->get_data<event_hit>(_hit_producer);

    // grab vtx to be used to guide polar-coordinates
    auto *ev_vtx = storage->get_data<event_vertex>(_vtx_producer);

    // grab clusters to be produced
    auto *ev_clus = storage->get_data<event_cluster>("polar");

    // produce cluster -> hit association
    auto *ev_clus_hit_ass = storage->get_data<event_ass>("polar");

    // produce cluster -> vertex association
    auto *ev_clus_vtx_ass = storage->get_data<event_ass>("polar");

    storage->set_id(storage->run_id(), storage->subrun_id(), storage->event_id());

    // if no hits or vertex -> exit
    if ( !ev_hit or (ev_hit->size() == 0) ){
      std::cout << "No Hits -> return" << std::endl;
      return false;
    }
    
    if ( !ev_vtx or (ev_vtx->size() == 0) ){
      std::cout << "No Vertex -> return" << std::endl;
      _has_vtx = false;
      return false;
    }

    _has_vtx = true;
    
    // clear maps
    _Umappol.clear();
    _Vmappol.clear();
    _Ymappol.clear();
    _Umapcm.clear();
    _Vmapcm.clear();
    _Ymapcm.clear();

    // geometry helper, to be used across the module
    auto const* geom = larutil::GeometryHelper::GetME();
        
    // get w -> cm
    auto const& w2cm = geom->WireToCm();
    // get t -> cm
    auto const& t2cm = geom->TimeToCm();

    // get vertex position on the various planes (in cm)
    auto const& vtx = ev_vtx->at(0);

    std::vector<double> xyz = {vtx.X() + 960 * t2cm ,vtx.Y(),vtx.Z()};

    vtx_w_cm = {0.,0.,0.};
    vtx_t_cm = {0.,0.,0.};

    auto const& vtx_U = geom->Point_3Dto2D(xyz,0);
    vtx_w_cm[0] = vtx_U.w;
    vtx_t_cm[0] = vtx_U.t;

    auto const& vtx_V = geom->Point_3Dto2D(xyz,1);
    vtx_w_cm[1] = vtx_V.w;
    vtx_t_cm[1] = vtx_V.t;

    auto const& vtx_Y = geom->Point_3Dto2D(xyz,2);
    vtx_w_cm[2] = vtx_Y.w;
    vtx_t_cm[2] = vtx_Y.t;

    std::cout << "vtx w " << vtx_w_cm[2] << std::endl;


    std::cout << "hits : " << ev_hit->size() << std::endl;

    // loop through hits
    for (size_t i=0; i < ev_hit->size(); i++){

      auto const& hit = ev_hit->at(i);

      // get plane
      auto const& pl = hit.WireID().Plane;

      // get the hit position in polar coordinates
      double hit_t_cm = hit.PeakTime() * t2cm;
      double hit_w_cm = hit.WireID().Wire * w2cm;

      double r = sqrt( (hit_t_cm - vtx_t_cm[pl]) * (hit_t_cm - vtx_t_cm[pl]) +
		       (hit_w_cm - vtx_w_cm[pl]) * (hit_w_cm - vtx_w_cm[pl]) );

      double cos_theta = (hit_w_cm - vtx_w_cm[pl]) / r;
      double sin_theta = (hit_t_cm - vtx_t_cm[pl]) / r;

      double theta_rad = atan2(sin_theta,cos_theta);

      std::pair<double,double> pol_coord;
      pol_coord = std::make_pair(r,theta_rad);

      std::pair<double,double> cm_coord;
      cm_coord = std::make_pair(hit_w_cm,hit_t_cm);
      
      if (pl==0){
	_Umappol[i] = pol_coord;
	_Umapcm[i]  = cm_coord;
      }
      if (pl==1){
	_Vmappol[i] = pol_coord;
	_Vmapcm[i]  = cm_coord;
      }
      if (pl==2){
	_Ymappol[i] = pol_coord;
	_Ymapcm[i]  = cm_coord;
      }

    }// finish loop over hits


    _binU = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});
    _binV = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});
    _binY = std::vector<std::vector<size_t> >( (int)(2*3.1415 / _bin_width), std::vector<size_t>{});


    // now histogram based on the polar angle
    // U plane
    for (auto const& hit : _Umappol){
      // hit index
      size_t idx = hit.first;
      // hit angle
      double angle = hit.second.second;
      double rad   = hit.second.first;
      // ignore hits w/ radius < 1 (too close to vertex)
      if (rad < 1)
	continue;
      // calculate bin this falls into
      size_t bin = ((angle + 3.1415) / 2*3.1415) / _bin_width;
      if ( (bin > 0) and (bin < _binU.size()) )
	_binU[bin].push_back(idx);
      else
	std::cout << "Hit w/ angle " << angle << " is out of range!" << std::endl;
    }// for U-plane hits

    // now histogram based on the polar angle
    // U plane
    for (auto const& hit : _Vmappol){
      // hit index
      size_t idx = hit.first;
      // hit angle
      double angle = hit.second.second;
      double rad   = hit.second.first;
      // ignore hits w/ radius < 1 (too close to vertex)
      if (rad < 1)
	continue;
      // calculate bin this falls into
      size_t bin = ((angle + 3.1415) / 2*3.1415) / _bin_width;
      if ( (bin > 0) and (bin < _binU.size()) )
	_binV[bin].push_back(idx);
      else
	std::cout << "Hit w/ angle " << angle << " is out of range!" << std::endl;
    }// for U-plane hits
    
    // now histogram based on the polar angle
    // Y plane
    for (auto const& hit : _Ymappol){
      // hit index
      size_t idx = hit.first;
      // hit angle
      double angle = hit.second.second;
      double rad   = hit.second.first;
      // ignore hits w/ radius < 1 (too close to vertex)
      if (rad < 1)
	continue;
      // calculate bin this falls into
      size_t bin = ((angle + 3.1415) / 2*3.1415) / _bin_width;
      if ( (bin > 0) and (bin < _binU.size()) )
	_binY[bin].push_back(idx);
      else
	std::cout << "Hit w/ angle " << angle << " is out of range!" << std::endl;
    }// for U-plane hits

    std::vector<std::vector<size_t> > polarClustersU;
    FindPolarClusters(_binU,polarClustersU);
    std::vector<std::vector<size_t> > polarClustersV;
    FindPolarClusters(_binV,polarClustersV);
    std::vector<std::vector<size_t> > polarClustersY;
    FindPolarClusters(_binY,polarClustersY);

    // finally, save clusters

    // vector for assocaitions
    std::vector<std::vector<unsigned int> > cluster_hit_ass_v;

    //U plane
    // for each cluster create a larlite::cluster
    for (size_t i=0; i < polarClustersU.size(); i++){
      if (polarClustersU[i].size() > 0){
	larlite::cluster clus;
	clus.set_n_hits(polarClustersU[i].size());
	clus.set_view(larlite::geo::View_t::kU);
	//clus.set_planeID(2);
	// vector for associations
	ev_clus->push_back(clus);
	std::vector<unsigned int> clus_hit_ass;
	for (auto const& hitidx : polarClustersU[i])
	  clus_hit_ass.push_back(hitidx);
	cluster_hit_ass_v.push_back(clus_hit_ass);
      }
    }

    // V plane
    // for each cluster create a larlite::cluster
    for (size_t i=0; i < polarClustersV.size(); i++){
      if (polarClustersV[i].size() > 0){
	larlite::cluster clus;
	clus.set_n_hits(polarClustersV[i].size());
	clus.set_view(larlite::geo::View_t::kV);
	//clus.set_planeID(2);
	// vector for associations
	ev_clus->push_back(clus);
	std::vector<unsigned int> clus_hit_ass;
	for (auto const& hitidx : polarClustersV[i])
	  clus_hit_ass.push_back(hitidx);
	cluster_hit_ass_v.push_back(clus_hit_ass);
      }
    }

    // Y plane
    // for each cluster create a larlite::cluster
    for (size_t i=0; i < polarClustersY.size(); i++){
      if (polarClustersY[i].size() > 0){
	larlite::cluster clus;
	clus.set_n_hits(polarClustersY[i].size());
	clus.set_view(larlite::geo::View_t::kW);
	//clus.set_planeID(2);
	// vector for associations
	ev_clus->push_back(clus);
	std::vector<unsigned int> clus_hit_ass;
	for (auto const& hitidx : polarClustersY[i])
	  clus_hit_ass.push_back(hitidx);
	cluster_hit_ass_v.push_back(clus_hit_ass);
      }
    }

    ev_clus_hit_ass->set_association(ev_clus->id(),product_id(data::kHit,ev_hit->name()),cluster_hit_ass_v);    
       

    return true;
  }

  bool VtxClustering::finalize() {

  
    return true;
  }


  void VtxClustering::FindPolarClusters(const std::vector<std::vector<size_t> > polarHits,
					std::vector<std::vector<size_t> >& polarClusters){

    // boolean to indicate if we currently
    // are within a cluster (above threshold)
    bool inClus = false;

    // reset cluster vector
    polarClusters.clear();

    // vector of pairs to keep track of cluster bounds
    std::vector<std::pair<size_t,size_t> > clus_bounds;

    // values to define angle of cluster start & end
    size_t clus_start = 0;
    size_t clus_end   = 0;

    for (size_t a = 0; a < polarHits.size(); a++){

      auto nhits = polarHits[a].size();
      if (nhits > _thresh){
	if (inClus == false)
	  clus_start = a;
	inClus = true;
      }// if above threshold
      else{
	if (inClus == true){
	  clus_end = a;
	  if ( (clus_end > clus_start) and (clus_end <= polarHits.size()) )
	    clus_bounds.push_back(std::make_pair(clus_start,clus_end));
	}
	inClus = false;
      }// if below threshold
    }// for all polar angles

    // start adding hit indices to each cluster list
    for (auto const& bounds : clus_bounds){
      auto start = bounds.first;
      auto end   = bounds.second;
      // expand by 1 bin if we can
      if (start > 0)
	start -= 1;
      if (end < polarHits.size()-1)
	end += 1;
      std::cout << "found new cluster between angles " << start << " and " << end << std::endl;
      std::vector<size_t> clusHitsIdx_v;
      for (size_t a = start; a <= end; a++){
	auto anglehit_v = polarHits[a];
	for (auto const& hit_idx : anglehit_v)
	  clusHitsIdx_v.push_back(hit_idx);
      }// for all angles in the cluster
      polarClusters.push_back(clusHitsIdx_v);
      std::cout << "created cluster w/ " << clusHitsIdx_v.size() << " hits" << std::endl;
    }// for all clusters

    return;
  }

}
#endif
