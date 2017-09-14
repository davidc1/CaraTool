#ifndef LARLITE_ADDCLUSTERS2PI0_CXX
#define LARLITE_ADDCLUSTERS2PI0_CXX

#include "AddClusters2Pi0.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"
#include "DataFormat/cluster.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/event_ass.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"

namespace larlite {

  bool AddClusters2Pi0::initialize() {

    return true;
  }
  
  bool AddClusters2Pi0::analyze(storage_manager* storage) {

    auto ev_hit = storage->get_data<event_hit>("gaushit");

    auto ev_vtx = storage->get_data<event_vertex> ("hc");
    auto ev_pfp = storage->get_data<event_pfpart> ("hc");
    auto ev_cls = storage->get_data<event_cluster>("hc");

    // create a new pfpart -> cluster association
    larlite::event_ass* ev_pfpart_cluster_ass_v = storage->get_data<event_ass>( ev_pfp->name() );
    // create a new cluster -> hit association
    larlite::event_ass* ev_cluster_hit_ass_v    = storage->get_data<event_ass>( ev_cls->name() );

    // vector to hold associated hits to cluster
    std::vector< std::vector< unsigned int> > clus_hit_ass_holder;
    // vector to hold pfpart -> cluster ass
    std::vector< std::vector< unsigned int> > pfpart_clus_ass_holder;

    //set event ID through storage manager
    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());

    std::cout << "run : " << storage->run_id() << ", evt : " << storage->event_id() << std::endl;
    
    std::pair<int,int> evtinfo = std::make_pair(storage->run_id(),storage->event_id());

    if (_vtx_hits.find(evtinfo) == _vtx_hits.end()){
      return false;
    }

    auto const& evtvtx = _vtx_hits[evtinfo];
    auto const& evthit = _hit_idx_v[evtinfo];
    
    // create vertex
    std::vector<int> vtxhitidxv;
    for (auto const& idx : evtvtx)
      if (std::find(vtxhitidxv.begin(),vtxhitidxv.end(),idx) == vtxhitidxv.end()) { vtxhitidxv.push_back(idx); }
    
    if (vtxhitidxv.size() != 2) {
      print(larlite::msg::kERROR,__FUNCTION__,"num. vertices != 2");
      return false;
    }

    auto geom = ::larutil::Geometry::GetME();
    auto geoH = ::larutil::GeometryHelper::GetME();
    auto detProp = ::larutil::DetectorProperties::GetME();
    
    Double_t y,z;
    // vertex hits
    auto h1 = ev_hit->at(vtxhitidxv[0]);
    auto h2 = ev_hit->at(vtxhitidxv[1]);
    // plane offset for these two planes
    double * origin;
    origin = new double[3];
    geom->PlaneOriginVtx( h1.WireID().Plane, origin);
    float planeOffset1 = origin[0];
    geom->PlaneOriginVtx( h2.WireID().Plane, origin);
    float planeOffset2 = origin[0];
    geom->IntersectionPoint(h1.WireID().Wire,h2.WireID().Wire,
			    h1.WireID().Plane,h2.WireID().Plane,
			    y,z);
    double x1 = (h1.PeakTime() - detProp->TriggerOffset()) * geoH->TimeToCm() + planeOffset1;
    double x2 = (h2.PeakTime() - detProp->TriggerOffset()) * geoH->TimeToCm() + planeOffset2;
    double x = (x1+x2)/2;
    
    double xyz[3] = {x,y,z};
    larlite::vertex vtx(xyz);
    std::cout << "vertex coordinates : [" << x << ", " << y << ", " << z << "]" << std::endl;
    ev_vtx->emplace_back(vtx);

    

    // vectors of cluster indices
    std::vector< std::vector<unsigned int> > cluster_v(12,std::vector<unsigned int>());

    std::map<int,bool> usedhits;
    
    // loop through map of clustered hits, assigning them to the
    // appropriate cluster
    for (auto const& hitinfo : evthit) {

      auto const& idx = hitinfo.second;

      // if hit already used -> skip
      if (usedhits.find(idx) != usedhits.end()) continue;

      usedhits[idx] = true;
      
      auto const& shr = hitinfo.first;
      auto const& hit = ev_hit->at(idx);

      auto plane = hit.WireID().Plane;

      cluster_v[ shr*3 + plane ].push_back(idx);
      
    }// for all hits to be clustered

    for (size_t i=0; i < cluster_v.size(); i++) {

      auto const clus_idx_v = cluster_v.at(i);
      
      if (clus_idx_v.size() == 0) continue;

      auto const& view = ev_hit->at( clus_idx_v[0] ).View();

      // project vertex to 2D.
      auto const& vtx2D = geoH->Point_3Dto2D(xyz,(unsigned int)view);
      // find nearest 2D hit to vertex. this will be start point on this cluster
      // same for end-point
      double sw,st,ew,et;
      double dmin = 100000;
      double dmax = 0;
      for (auto const& idx : clus_idx_v) {
	auto const& h = ev_hit->at(idx);
	geom->PlaneOriginVtx( h.WireID().Plane, origin);
	float planeOffset = origin[0];
	double hw = h.WireID().Wire * geoH->WireToCm();
	double ht = (h.PeakTime() - detProp->TriggerOffset() ) * geoH->TimeToCm() + planeOffset;
	double dd = sqrt( (hw-vtx2D.w)*(hw-vtx2D.w) + (ht-vtx2D.t)*(ht-vtx2D.t) );
	if (dd < dmin) { dmin = dd; sw = h.WireID().Wire; st = h.PeakTime(); }
	if (dd > dmax) { dmax = dd; ew = h.WireID().Wire; et = h.PeakTime(); }
      }
    
      // new cluster
      larlite::cluster clus;
      clus.set_n_hits( clus_idx_v.size() );

      clus.set_view( view );
      clus.set_planeID( larlite::geo::PlaneID(0,0,view) );
      clus.set_start_wire ( sw, 1);
      clus.set_start_tick ( st, 1);
      clus.set_end_wire   ( ew, 1);
      clus.set_end_tick   ( et, 1);
      clus.set_start_angle( 10 * 3.14 / 180.);
      clus_hit_ass_holder.push_back( clus_idx_v );

      ev_cls->emplace_back( clus );
      
    }

    std::cout << "created " << ev_cls->size() << "clusters" << std::endl;

    if (ev_cls->size() >= 2) {
      larlite::pfpart pfp1;
      std::vector<unsigned int> pfp_clus_v1 = {0,1};
      ev_pfp->emplace_back(pfp1);
      pfpart_clus_ass_holder.push_back(pfp_clus_v1);
    }

    if (ev_cls->size() >= 4) {
      larlite::pfpart pfp2;
      std::vector<unsigned int> pfp_clus_v2 = {2,3};
      ev_pfp->emplace_back(pfp2);
      pfpart_clus_ass_holder.push_back(pfp_clus_v2);
    }

    if (ev_cls->size() >= 6) {
      larlite::pfpart pfp3;
      std::vector<unsigned int> pfp_clus_v3 = {4,5};
      ev_pfp->emplace_back(pfp3);
      pfpart_clus_ass_holder.push_back(pfp_clus_v3);
    }

    if (ev_cls->size() == 8) {
      larlite::pfpart pfp4;
      std::vector<unsigned int> pfp_clus_v4 = {6,7};
      ev_pfp->emplace_back(pfp4);
      pfpart_clus_ass_holder.push_back(pfp_clus_v4);
    }


    ev_cluster_hit_ass_v   ->set_association( ev_cls->id(), product_id(data::kHit,ev_hit->name()),       clus_hit_ass_holder    );
    ev_pfpart_cluster_ass_v->set_association( ev_pfp->id(),  product_id(data::kCluster,ev_cls->name()),  pfpart_clus_ass_holder );    
      
  
    return true;
  }

  bool AddClusters2Pi0::finalize() {

    return true;
  }

}
#endif
