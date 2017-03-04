#ifndef LARLITE_SAVEVTXTRACK_CXX
#define LARLITE_SAVEVTXTRACK_CXX

#include "SaveVtxTrack.h"
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"

namespace larlite {

  bool SaveVtxTrack::initialize() {

    event_file.open("numucc_sel2_5E19_info.txt");

    return true;
  }
  
  bool SaveVtxTrack::analyze(storage_manager* storage) {
  
    auto run = storage->run_id();
    auto event = storage->event_id();
    auto subrun = storage->subrun_id();

    auto ev_vtx = storage->get_data<event_vertex>("numuCC_vertex");
    auto ev_trk = storage->get_data<event_track>("numuCC_track");

    // grab vertex
    auto const& vtx = ev_vtx->at(0);
    // grab track
    auto const& trk = ev_trk->at(0);


    event_file << run << " " << subrun << " " << event << " "
	       << vtx.X() << " " << vtx.Y() << " " << vtx.Z() << " "
	       << trk.Vertex().X() << " " << trk.Vertex().Y() << " " << trk.Vertex().Z() << " "
	       << trk.End().X() << " " << trk.End().Y() << " " << trk.End().Z()
	       << "\n";

    std::cout << "could not find vertex for event " << event << std::endl;

    return true;
  }

  bool SaveVtxTrack::finalize() {


    return true;
  }

}
#endif
