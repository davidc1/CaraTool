#ifndef LARLITE_TAGSTOPMU_CXX
#define LARLITE_TAGSTOPMU_CXX

#include "TagStopMu.h"

// Tag stopping muons by removing
// those that end below Y = -100
// those that have a flash nearby
// save output tracks to producer
// to select and be able to study
// misid in evd

#include "DataFormat/track.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/opflash.h"
#include "DataFormat/mctrack.h"

#include "LArUtil/SpaceChargeMicroBooNE.h"
#include "LArUtil/GeometryHelper.h"

namespace larlite {

  bool TagStopMu::initialize() {

    return true;
  }
  
  bool TagStopMu::analyze(storage_manager* storage) {

    // load mctracks
    auto ev_mct = storage->get_data<event_mctrack>("mcreco");
    // grab flash
    auto ev_flash = storage->get_data<event_opflash>("simpleFlashCosmic");
    // grab tracks
    auto ev_track = storage->get_data<event_track>("pandoraCosmic");
    
    // output track producer
    auto ev_outtrack = storage->get_data<event_track>("pandoraCosmicStop");

    //set event ID through storage manager
    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());

    // save true muon stopping points
    std::vector< std::vector<double> > mu_end_pt_v;
    auto SCE = larutil::SpaceChargeMicroBooNE();

    // grab the muon (assume single-particle in TPC stopping sample)
    if (!ev_mct or (ev_mct->size() == 0)){
      print(larlite::msg::kWARNING,__FUNCTION__,"no MCTracks");
      return true;
    }
    
    for (size_t i=0; i < ev_mct->size(); i++) {
      auto mct = ev_mct->at(i);
      if ( (mct.PdgCode() == 13) or (mct.PdgCode() == -13) ) {
	if (mct.size() == 0) continue;
	auto enter = mct.at(0);
	auto end = mct.End();
	if (end.E() > 106.) continue;
	double _xe_mc = end.X();
	double _ye_mc = end.Y();
	double _ze_mc = end.Z();
	if ( (_xe_mc < 256.) and (_xe_mc > 0) and (_ye_mc < 116.) and
	     (_ye_mc > -116.) and (_ze_mc < 1036.) and (_ze_mc > 0.) ) {
	  std::vector<double> endpt{_xe_mc,_ye_mc,_ze_mc};
	  auto endptSCE = SCE.GetPosOffsets(endpt[0],endpt[1],endpt[2]);
	  endptSCE[0] += endpt[0];
	  endptSCE[1] += endpt[1];
	  endptSCE[2] += endpt[2];
	  mu_end_pt_v.push_back( endptSCE );
	}// if in FV
      }// if muon
    }// for all mctracks

    for (size_t i=0; i < ev_track->size(); i++) {

      auto const& trk = ev_track->at(i);

      if (trk.Length() < 150.) continue;
      if (trk.End().Y() < -100) continue;
      if ( (trk.End().X() < -40) || (trk.End().X() > 290) ) continue;
      if ( (trk.End().Z() < 30) || (trk.End().Z() > 1000.) ) continue;


      // closest flash time:
      double tmin = 10000.;
      for (size_t f=0; f < ev_flash->size(); f++) {
	auto const& flash = ev_flash->at(f);
	double ttrk = fabs(trk.End().X() / 0.1114 - flash.Time());
	if (ttrk < tmin) tmin = ttrk;
	ttrk = fabs((trk.End().X()-256.) / 0.1114 - flash.Time());
	if (ttrk < tmin) tmin = ttrk;
      }


      if (tmin < 10) continue;

      // find the closest true muon end-point
      double ddmin = 100000.;
      int idxmin = 0;
      for (size_t k=0; k < mu_end_pt_v.size(); k++) {
	auto endpt = mu_end_pt_v[k];
	double dd = sqrt( (trk.End().Y() - endpt[1])*(trk.End().Y() - endpt[1]) + (trk.End().Z() - endpt[2])*(trk.End().Z() - endpt[2]) );
	if (dd < ddmin) { ddmin = dd; }
      }// for all MC tracks

      std::cout << "Track end @ " << trk.End().X() << ", "
		<< trk.End().Y() << ", " 
		<< trk.End().Z() 
		<< " w/ time " << tmin 
		<< "\t dmin " << ddmin 
		<< std::endl;
      
      ev_outtrack->emplace_back(trk);
      
    }
    
    return true;
  }

  bool TagStopMu::finalize() {

    return true;
  }

}
#endif
