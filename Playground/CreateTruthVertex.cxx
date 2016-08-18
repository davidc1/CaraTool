#ifndef LARLITE_CREATETRUTHVERTEX_CXX
#define LARLITE_CREATETRUTHVERTEX_CXX

#include "CreateTruthVertex.h"
#include "DataFormat/vertex.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/LArProperties.h"

namespace larlite {

  bool CreateTruthVertex::initialize() {

    double efield   = larutil::LArProperties::GetME()->Efield(); // kV/cm
    double temp     = larutil::LArProperties::GetME()->Temperature(); // Kelvin
    _driftVel       = larutil::LArProperties::GetME()->DriftVelocity(efield,temp); // [cm/us]

    return true;
  }
  
  bool CreateTruthVertex::analyze(storage_manager* storage) {

    auto ev_mctrk   = storage->get_data<event_mctrack>("mcreco");
    auto ev_mcshr   = storage->get_data<event_mcshower>("mcreco");
    auto ev_vtx_trk = storage->get_data<event_vertex>("mcvtx_trk");
    auto ev_vtx_shr = storage->get_data<event_vertex>("mcvtx_shr");

    storage->set_id( ev_mctrk->run(), ev_mctrk->subrun(), ev_mctrk->event_id() );

    if (!ev_mctrk or (ev_mctrk->size() == 0) )
      return false;

    // if a track ends in the TPC, save the end-point as a vertex
    for (size_t i=0; i < ev_mctrk->size(); i++){

      auto const& mctrack = ev_mctrk->at(i);

      if ( (mctrack.PdgCode() != 13) and (mctrack.PdgCode() != -13) )
	continue;
      
      if (mctrack.size() < 2)
	continue;

      auto const& end = mctrack.at( mctrack.size() - 1 );

      if (end.E() > 106.)
      	continue;

      // get time of track
      double time = end.T() / 1000.;

      std::cout << "time is " << time <<  std::endl;

      double *xyz = new double[3];
      xyz[0] = end.X() + time * _driftVel;
      xyz[1] = end.Y();
      xyz[2] = end.Z();

      larlite::vertex vtx(xyz);
      ev_vtx_trk->emplace_back(vtx);
      
    }// for all MCTracks

    // vertex for all showers which deposit energy
    for (size_t i=0; i < ev_mcshr->size(); i++){

      auto const& mcshower = ev_mcshr->at(i);

      if (mcshower.DetProfile().E() < 1.)
	continue;

      auto const& start = mcshower.Start();

      // get time of track
      double time = start.T() / 1000.;

      std::cout << "time is " << time <<  std::endl;

      double *xyz = new double[3];
      xyz[0] = start.X() + time * _driftVel;
      xyz[1] = start.Y();
      xyz[2] = start.Z();

      larlite::vertex vtx(xyz);
      ev_vtx_shr->emplace_back(vtx);
      
    }// for all MCTracks
  
    return true;
  }

  bool CreateTruthVertex::finalize() {

    return true;
  }

}
#endif
