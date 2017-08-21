#ifndef LARLITE_FILTERCOSMICTRACKS_CXX
#define LARLITE_FILTERCOSMICTRACKS_CXX

#include "FilterCosmicTracks.h"
#include "DataFormat/track.h"
#include "DataFormat/hit.h"

namespace larlite {

  bool FilterCosmicTracks::initialize() {


    return true;
  }
  
  bool FilterCosmicTracks::analyze(storage_manager* storage) {
  
    auto ev_trk = storage->get_data<event_track>("pandoraCosmic");

    if (!ev_trk) {
      print(larlite::msg::kWARNING,__FUNCTION__,"no tracks");
      return false;
    }

    larlite::event_hit *ev_hit = nullptr;
    auto ass_trk_hit_v = storage->find_one_ass(ev_trk->id(), ev_hit, ev_trk->name());

    for (auto const& ass_trk_hit : ass_trk_hit_v) {

      for (auto const& hit_idx : ass_trk_hit) {

	ev_hit->at(hit_idx).set_goodness(-1.0);

      }// for all hits 
      
    }// for all track -> hit associations
    
    return true;
  }

  bool FilterCosmicTracks::finalize() {

    return true;
  }

}
#endif
