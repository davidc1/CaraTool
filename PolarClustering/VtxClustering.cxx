
#ifndef LARLITE_VTXCLUSTERING_CXX
#define LARLITE_VTXCLUSTERING_CXX

#include "VtxClustering.h"
#include "DataFormat/cluster.h"
#include <math.h>

namespace larlite {

  VtxClustering::VtxClustering()
    : _polar_tree(nullptr)
  {
    _name="VtxClustering";
    _fout=0;
    _bin_width = 7.2 / 180. * 3.1415;
    _thresh = 20;
    _max_vtx_dist = 100;
    // define PI
    PI = 3.1415;
    // geometry, to be used across the module
    _geom = ::larutil::Geometry::GetME();
    // geometry helper, to be used across the module
    _geomH = ::larutil::GeometryHelper::GetME();
    // get w -> cm
    _w2cm = _geomH->WireToCm();
    // get t -> cm
    _t2cm = _geomH->TimeToCm();
  }

  bool VtxClustering::initialize() {

    _bin_width_deg = _bin_width * 180. / PI;
    _n_bins = (size_t)(2*PI / _bin_width);

    vtx_w_cm = {0.,0.,0.};
    vtx_t_cm = {0.,0.,0.};

    if (_polar_tree) delete _polar_tree;
    _polar_tree = new TTree("_polar_tree","Polar Tree");
    _polar_tree->Branch("_angle_v", "std::vector<double>",&_angle_v );
    _polar_tree->Branch("_radius_v","std::vector<double>",&_radius_v);
    _polar_tree->Branch("_charge_v","std::vector<double>",&_charge_v);
    _polar_tree->Branch("_pl_v",    "std::vector<int>",   &_pl_v    );

    return true;
  }
  
  bool VtxClustering::analyze(storage_manager* storage) {

    // grab input clusters to be used in clustering
    auto *ev_clus_in = storage->get_data<event_cluster>(_cluster_producer);

    // grab vtx to be used to guide polar-coordinates
    auto *ev_vtx = storage->get_data<event_vertex>(_vtx_producer);

    // grab clusters to be produced
    auto *ev_clus_out = storage->get_data<event_cluster>("polar");

    // produce cluster -> hit association
    auto *ev_clus_hit_ass = storage->get_data<event_ass>("polar");

    // produce cluster -> vertex association
    auto *ev_clus_vtx_ass = storage->get_data<event_ass>("polar");

    storage->set_id(storage->run_id(), storage->subrun_id(), storage->event_id());

    // if no input cluster -> continue
    if ( !ev_clus_in or (ev_clus_in->size() == 0) ){
      std::cout << "No Input Cluster -> return" << std::endl;
      return false;
    }

    // grab hits to be used in clustering
    larlite::event_hit *ev_hit = nullptr;
    auto const& ass_cluster_hit_v = storage->find_one_ass(ev_clus_in->id(), ev_hit, ev_clus_in->name());

    // if no hits or vertex -> exit
    if ( !ev_hit or (ev_hit->size() == 0) ){
      std::cout << "No Hits -> return" << std::endl;
      return false;
    }
    
    if ( !ev_vtx or (ev_vtx->size() == 0) ){
      std::cout << "No Vertex -> return" << std::endl;
      return false;
    }

    // clear everything
    _HitMap_v.clear();
    _HitMap_v.resize(3);

    _angle_v.clear();
    _radius_v.clear();
    _charge_v.clear();
    _pl_v.clear();

    _PolarBin_v.clear();
    for (size_t pl=0; pl < 3; pl++)
      _PolarBin_v.push_back( std::vector<std::vector<size_t> >( _n_bins, std::vector<size_t>{}) );
    
    _PolarClusters_v.clear();
    _PolarClusters_v.resize(3);


    // get vertex projection on the 3 planes
    // if more than 1 vertex in event -> throw warning
    if (ev_vtx->size() > 1)
      print(msg::kWARNING,__FUNCTION__,"More than 1 vertex. Not sure what to do...");
    projectVtx( ev_vtx->at(0) );

    // fill polar hit maps
    fillPolarHitMap(ass_cluster_hit_v,ev_hit);

    // Bin in Polar angles
    PolarBinning();

    // Find polar clusters
    FindPolarClusters();

    // finally, save clusters

    // vector for assocaitions
    std::vector<std::vector<unsigned int> > cluster_hit_ass_v;

    for (size_t pl=0; pl < _PolarClusters_v.size(); pl++){
      
      auto const& polarClusters = _PolarClusters_v[pl];
      
      // for each cluster create a larlite::cluster
      for (size_t i=0; i < polarClusters.size(); i++){
	if (polarClusters[i].size() > 0){
	  larlite::cluster clus;
	  clus.set_n_hits(polarClusters[i].size());
	  clus.set_view( _geom->PlaneToView(pl) );
	  // vector for associations
	  ev_clus_out->push_back(clus);
	  std::vector<unsigned int> clus_hit_ass;
	  for (auto const& hitidx : polarClusters[i])
	    clus_hit_ass.push_back(hitidx);
	  cluster_hit_ass_v.push_back(clus_hit_ass);
	}
      }// for all polar clusters
    }// for all planes

    // save cluster -> hit associations
    ev_clus_hit_ass->set_association(ev_clus_out->id(),product_id(data::kHit,ev_hit->name()),cluster_hit_ass_v);
    
    return true;
  }

  bool VtxClustering::finalize() {

    _polar_tree->Write();
    
    return true;
  }


  void VtxClustering::FindPolarClusters(){

    // do this per-plane
    for (size_t pl=0; pl < _PolarBin_v.size(); pl++){

      auto const& polarHits = _PolarBin_v[pl];
      
      // boolean to indicate if we currently
      // are within a cluster (above threshold)
      bool inClus = false;
      
      // reset cluster vector
      _PolarClusters_v[pl].clear();
      
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
	//std::cout << "found new cluster between angles " << start << " and " << end << std::endl;
	std::vector<size_t> clusHitsIdx_v;
	for (size_t a = start; a <= end; a++){
	  auto anglehit_v = polarHits[a];
	  for (auto const& hit_idx : anglehit_v)
	    clusHitsIdx_v.push_back(hit_idx);
	}// for all angles in the cluster
	_PolarClusters_v[pl].push_back(clusHitsIdx_v);
	//std::cout << "created cluster w/ " << clusHitsIdx_v.size() << " hits" << std::endl;
      }// for all clusters
    }// for all planes
      
    return;
  }
  
  void VtxClustering::projectVtx(const larlite::vertex vtx){
    
    std::vector<double> xyz = {vtx.X(),vtx.Y(),vtx.Z()};
    
    auto const& vtx_U = _geomH->Point_3Dto2D(xyz,0);
    vtx_w_cm[0] = vtx_U.w;
    vtx_t_cm[0] = vtx_U.t + 800 * _t2cm;

    auto const& vtx_V = _geomH->Point_3Dto2D(xyz,1);
    vtx_w_cm[1] = vtx_V.w;
    vtx_t_cm[1] = vtx_V.t + 800 * _t2cm;

    auto const& vtx_Y = _geomH->Point_3Dto2D(xyz,2);
    vtx_w_cm[2] = vtx_Y.w;
    vtx_t_cm[2] = vtx_Y.t + 800 * _t2cm;

    std::cout << "Vtx @ [W,T] : " << std::endl
	      << "Pl 0 [" << vtx_w_cm[0] << ", " << vtx_t_cm[0] << "]" << std::endl
      	      << "Pl 1 [" << vtx_w_cm[1] << ", " << vtx_t_cm[1] << "]" << std::endl
      	      << "Pl 2 [" << vtx_w_cm[2] << ", " << vtx_t_cm[2] << "]" << std::endl << std::endl;
      

    return;
  }


  void VtxClustering::fillPolarHitMap(const std::vector<std::vector<unsigned int> > ass_cluster_hit_v,
				      const larlite::event_hit *ev_hit){

    // loop through cluster
    for (auto const& clus_hit_ass : ass_cluster_hit_v){

      // if there are more than 5 hits in the cluster
      //if (clus_hit_ass.size() < 5)
      //continue;
      
      // loop through hits for this cluster
      for (auto const& i: clus_hit_ass){
	
	auto const& hit = ev_hit->at(i);
	
	// get plane
	auto const& pl = hit.WireID().Plane;
	if (pl > _geom->Nplanes()){
	  print (msg::kERROR,__FUNCTION__,"Plane number not valid...");
	  continue;
	}
	
	// get the hit position in polar coordinates
	double hit_t_cm = hit.PeakTime()    * _t2cm;
	double hit_w_cm = hit.WireID().Wire * _w2cm;
	
	double r = sqrt( (hit_t_cm - vtx_t_cm[pl]) * (hit_t_cm - vtx_t_cm[pl]) +
			 (hit_w_cm - vtx_w_cm[pl]) * (hit_w_cm - vtx_w_cm[pl]) );

	// ignore any hit that is too far away from the vertexo
	if (r > _max_vtx_dist)
	  continue;
	
	double cos_theta = (hit_w_cm - vtx_w_cm[pl]) / r;
	double sin_theta = (hit_t_cm - vtx_t_cm[pl]) / r;
	double theta_rad = atan2(sin_theta,cos_theta);
	
	
	// Fill variables for output (debugging/optimizing) tree
	_angle_v.push_back(theta_rad * 180. / PI);
	_radius_v.push_back(r);
	_charge_v.push_back(hit.Integral());
	_pl_v.push_back(pl);
	
	
	// fill map accordingly
	// 1st index -> plane
	// 2nd index -> hit index in ev_hit vector
	_HitMap_v[pl][i] = std::make_pair(r,theta_rad);
	
      }// finish loop over hits in the cluster
    }// finish loop over clusters

    _polar_tree->Fill();

    return;
  }


  void VtxClustering::PolarBinning(){

    // do this separately for every plane
    for (size_t pl = 0; pl < _HitMap_v.size(); pl++){

      // get the hit map for this plane
      auto const& HitMap = _HitMap_v[pl];
      
      for (auto const& hit : HitMap){
	// hit index
	size_t idx = hit.first;
	// hit angle
	double angle  = hit.second.second;
	double radius = hit.second.first;
	// ignore hits w/ radius < 1 (too close to vertex)
	if (radius < 1)
	  continue;
	// calculate bin this falls into
	size_t bin = (angle + PI) / _bin_width;
	if ( bin < _n_bins )
	  _PolarBin_v[pl][bin].push_back(idx);
	//else
	//std::cout << "Hit w/ angle " << angle << " is out of range!" << std::endl;
      }// for hits in this plane
    }// for all planes
      
    return;
  }

}
#endif
