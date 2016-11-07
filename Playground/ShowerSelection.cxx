#ifndef LARLITE_SHOWERSELECTION_CXX
#define LARLITE_SHOWERSELECTION_CXX

#include "ShowerSelection.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"

#include "DataFormat/mctruth.h"
#include "DataFormat/cluster.h"
#include "DataFormat/vertex.h"
#include "DataFormat/hit.h"

namespace larlite {

  ShowerSelection::ShowerSelection()
    : _tree(nullptr)
  {
    _name = "ShowerSelection";
    _fout = 0;
  }

  bool ShowerSelection::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("_tree","shower selection tree");
    _tree->Branch("_pi0_energy",&_pi0_energy,"pi0_energy/D");
    _tree->Branch("_clus_nhits_v"  ,"std::vector<int>"   ,&_clus_nhits_v  );
    _tree->Branch("_clus_vtxdist_v","std::vector<double>",&_clus_vtxdist_v);
    _tree->Branch("_clus_plane_v"  ,"std::vector<int>"   ,&_clus_plane_v  );
								     
    _wire2cm  = larutil::GeometryHelper::GetME()->WireToCm();
    _time2cm  = larutil::GeometryHelper::GetME()->TimeToCm();
    
    return true;
  }
  
  bool ShowerSelection::analyze(storage_manager* storage) {

    auto ev_mctruth = storage->get_data<event_mctruth>("generator");
    auto ev_clus    = storage->get_data<event_cluster>("rawcluster");
    auto *ev_vtx    = storage->get_data<event_vertex> ("mcvertex");
    // get hits associated to clusters
    larlite::event_hit* ev_hit = nullptr;
    auto const& ass_cluster_hit_v = storage->find_one_ass(ev_clus->id(), ev_hit, ev_clus->name());

    if ( (!ev_mctruth) or (ev_mctruth->size() == 0) )
      return false; 
    if ( (!ev_clus)    or (ev_clus->size()    == 0) )
      return false; 
    if ( (!ev_vtx)     or (ev_vtx->size()     == 0) )
      return false; 

    // get all MCParticles
    auto mctruth = ev_mctruth->at(0);

    auto part_v = mctruth.GetParticles();

    int n_pi0 = 0;

    size_t pi0_idx = 0;

    for (size_t i=0; i < part_v.size(); i++){
      auto const& part = part_v[i];
      if ( (part.PdgCode() == 111) and (part.StatusCode() == 1) and (part.Trajectory().size() != 0) ){
	//std::cout << "found " << part.PdgCode() << "\tw/ energy : " << part.Trajectory().at(0).E() <<  std::endl;
	n_pi0 += 1;
	pi0_idx = i;
      }
    }

    _pi0_energy = -1;
    if (n_pi0 == 1)
      _pi0_energy = part_v[pi0_idx].Trajectory().at(0).E();

    std::vector<double> vtx_w_cm = {0.,0.,0.};
    std::vector<double> vtx_t_cm = {0.,0.,0.};
    auto const& vtx = ev_vtx->at(0);
    auto geoH = larutil::GeometryHelper::GetME();
    std::vector<double> xyz = {vtx.X(), vtx.Y(), vtx.Z()};
    for (size_t pl = 0; pl < 3; pl++){
      auto const& pt = geoH->Point_3Dto2D(xyz,pl);
      vtx_w_cm[pl] = pt.w;
      vtx_t_cm[pl] = pt.t + 800 * _time2cm;
    }

    _clus_nhits_v.clear();
    _clus_vtxdist_v.clear();
    _clus_plane_v.clear();
    
    // loop through all clusters
    for (auto const& clus_idx_v : ass_cluster_hit_v){

      // get number of hits
      _clus_nhits_v.push_back( clus_idx_v.size() );

      // get the plane
      int pl = ev_hit->at( clus_idx_v[0] ).WireID().Plane;
      _clus_plane_v.push_back( pl );

      // find the distance from the vertx
      double dmin = 100000;
      for (auto const& hit_idx : clus_idx_v){
	auto const& hit = ev_hit->at( hit_idx );
	double w = hit.WireID().Wire * _wire2cm;
	double t = hit.PeakTime() * _time2cm;
	double dd = (w - vtx_w_cm[pl]) * (w - vtx_w_cm[pl]) + (t - vtx_t_cm[pl]) * (t - vtx_t_cm[pl]);
	if (dd < dmin) dmin = dd;
      }// for all hits in cluster
      _clus_vtxdist_v.push_back( dmin );

    }//for all clusters

    _tree->Fill();
    
    return true;
  }

  bool ShowerSelection::finalize() {

    _fout->cd();
    _tree->Write();

    return true;
  }

}
#endif
