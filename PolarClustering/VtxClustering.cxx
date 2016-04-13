#ifndef LARLITE_VTXCLUSTERING_CXX
#define LARLITE_VTXCLUSTERING_CXX

#include "VtxClustering.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"
#include "LArUtil/GeometryHelper.h"
#include <math.h>

namespace larlite {

  bool VtxClustering::initialize() {

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

    }
      

    return true;
  }

  bool VtxClustering::finalize() {

  
    return true;
  }

}
#endif
