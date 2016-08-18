#ifndef LARLITE_COPYVTX_CXX
#define LARLITE_COPYVTX_CXX

#include "CopyVtx.h"
#include "DataFormat/vertex.h"

namespace larlite {

  bool CopyVtx::initialize() {

    return true;
  }
  
  bool CopyVtx::analyze(storage_manager* storage) {
  
    auto event  = storage->event_id();
    auto subrun = storage->subrun_id();
    auto run    = storage->run_id();

    auto *ev_vtx = storage->get_data<event_vertex>("numuCC_vertex");

    storage->set_id(run, subrun, event);

    // find the vertex associated with this event
    for (size_t i=0; i < RunEvent_v.size(); i++){
      
      auto evinfo = RunEvent_v[i];

      auto const& A = RunVertex_v[i];

      if ( (evinfo.first == run) and (evinfo.second == event) ){

	std::cout << run << " " << subrun << " " << event << " "
		  << A.X() << " " << A.Y() << " " << A.Z() << "\n";

	// create the vertex object
	double* B = new double[3];//{A.X(),A.Y(),A.Z()};//new double[3];
	B[0] = A.X();
	B[1] = A.Y();
	B[2] = A.Z();
	vertex vtx(B);

	ev_vtx->emplace_back(vtx);
	break;
      }
    }
    
    return true;
  }

  bool CopyVtx::finalize() {

    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    //
    // Say you made a histogram pointer h1 to store. You can do this:
    //
    // if(_fout) { _fout->cd(); h1->Write(); }
    //
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    //
  
    return true;
  }

  void CopyVtx::addEvent(const int run, const int event)
  {

    std::pair<int,int> newevent;

    newevent = std::make_pair(run,event);
    
    RunEvent_v.push_back(newevent);

    return;
  };

  
  void CopyVtx::addVertex(const double x, const double y, const double z)
  {
    
    TVector3 vtx(x,y,z);

    RunVertex_v.push_back(vtx);

    return;
  }

}
#endif
