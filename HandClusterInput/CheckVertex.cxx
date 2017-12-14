#ifndef LARLITE_CHECKVERTEX_CXX
#define LARLITE_CHECKVERTEX_CXX

#include "CheckVertex.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"
#include "DataFormat/cluster.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/event_ass.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"

namespace larlite {

  bool CheckVertex::initialize() {

    if (_tree) delete _tree;
    _tree = new TTree("tree","tree");
    _tree->Branch("_d",&_d,"d/D");
    
    return true;
  }
  
  bool CheckVertex::analyze(storage_manager* storage) {

    auto ev_hit = storage->get_data<event_hit>("gaushit");
    auto ev_vtx = storage->get_data<event_vertex>("mcvertex");

    auto const& tvtx = ev_vtx->at(0);

    std::cout << "run : " << storage->run_id() << ", evt : " << storage->event_id() << std::endl;
    
    std::pair<int,int> evtinfo = std::make_pair(storage->run_id(),storage->event_id());

    if (_vtx_hits.find(evtinfo) == _vtx_hits.end()){
      return false;
    }

    auto const& evtvtx = _vtx_hits[evtinfo];
    
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

    auto tx = tvtx.X();
    auto ty = tvtx.Y();
    auto tz = tvtx.Z();

    _d = sqrt( (x-tx)*(x-tx) + (y-ty)*(y-ty) + (z-tz)*(z-tz) );

    _tree->Fill();

    std::cout << "DISTANCE : " << _d << std::endl;
  
    return true;
  }

  bool CheckVertex::finalize() {

    if (_fout) _fout->cd();
    if (_tree) _tree->Write();

    return true;
  }

}
#endif
