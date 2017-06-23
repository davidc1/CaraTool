#ifndef LARLITE_MCTRUTHSCANNER_CXX
#define LARLITE_MCTRUTHSCANNER_CXX

#include "MCTruthScanner.h"

#include <iomanip>

#include "DataFormat/mctruth.h"

namespace larlite {

  bool MCTruthScanner::initialize() {

    _neta = 0;
    _nstable = 0;
    _entry = 0;

    return true;
  }
  
  bool MCTruthScanner::analyze(storage_manager* storage) {

    auto ev_truth = storage->get_data<event_mctruth>("generator");

    for (size_t i=0; i < ev_truth->size(); i++) {

      auto truth = ev_truth->at(i);

      auto neutrino = truth.GetNeutrino();

      if (_verbose)
	std::cout << "Neutrino PDG = " << neutrino.Nu().PdgCode() << std::endl
		  << "Interaction type [0 = CC & 1 = NC] = " << neutrino.CCNC() << std::endl
		  << "Nuclear target PDG code = " << neutrino.Target() << std::endl
		  << std::endl;
      
      // get outgoing particles
      
      auto parts = truth.GetParticles();

      int npi0 = 0;
      bool foundeta = false;
      
      for (auto const& part : parts) {

	if (part.PdgCode() == 331){
	  _neta += 1;
	  foundeta = true;
	  if (part.StatusCode() == 1)
	    _nstable += 1;
	}


	if (part.PdgCode() == 111) { npi0 += 1; }
	
	if (_verbose || part.PdgCode() == 331)
	  std::cout << "\t status code : " << part.StatusCode()
		    << "\t PDG : " << std::setw(10) << part.PdgCode() 
		    << "\t w/ energy " << part.Trajectory().at(0).E()
		    << "\t start proces : " << part.Process() 
		    << "\t end process : " << part.EndProcess()
		    << "\t @ entry " << _entry
		    << std::endl;
	
      }// for all outgoing particles

      if (foundeta) { std::cout << "also there were " << npi0 << " pi0s" << std::endl; }

      _entry += 1;

      if (_verbose)
	std::cout << std::endl << std::endl << std::endl;

      if (_display_mcpart == false) return true;

      // grab mcparticles
      auto ev_mcpart = storage->get_data<event_mcpart>("largeant");

      for (size_t i=0; i < ev_mcpart->size(); i++) {

	auto const& part = ev_mcpart->at(i);
	auto const& step = part.Trajectory().at(0);

	if (part.Trajectory().at(0).E() * 1000 < 0.2) continue;

	if (_verbose) {
	  std::cout << "\t status code : " << part.StatusCode()
		    << "\t PDG : " << std::setw(10) << part.PdgCode() 
		    << "\t Process : " << std::setw(15) << part.Process()
		    << "\t @ [" << std::setw(10) << step.X() << ", " << std::setw(10) << step.Y() << ", " << std::setw(10) << step.Z() << "] ";
	  if ( (part.Mother() > 0) && (part.Mother() < ev_mcpart->size()) )
	    std::cout << "\t parent PDG : " << std::setw(10) << ev_mcpart->at(part.Mother()).PdgCode();
	  else
	    std::cout << "\t parent PDG : " << std::setw(10) << " unknown";
	  std::cout << "\t w/ energy " << step.E() * 1000. << " MeV"
		    << std::endl;
	}
	
      }// for all particles

      if (_verbose)
	std::cout << std::endl << std::endl << std::endl;
      
    }// for all mctruth objects
  
    return true;
  }

  bool MCTruthScanner::finalize() {
  
    std::cout << _neta << " eta particles, " << _nstable << " of which are stable" << std::endl;

    return true;
  }

}
#endif
