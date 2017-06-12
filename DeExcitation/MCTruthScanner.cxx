#ifndef LARLITE_MCTRUTHSCANNER_CXX
#define LARLITE_MCTRUTHSCANNER_CXX

#include "MCTruthScanner.h"

#include <iomanip>

#include "DataFormat/mctruth.h"

namespace larlite {

  bool MCTruthScanner::initialize() {

    return true;
  }
  
  bool MCTruthScanner::analyze(storage_manager* storage) {

    auto ev_truth = storage->get_data<event_mctruth>("generator");

    for (size_t i=0; i < ev_truth->size(); i++) {

      auto truth = ev_truth->at(i);

      auto neutrino = truth.GetNeutrino();

      std::cout << "Neutrino PDG = " << neutrino.Nu().PdgCode() << std::endl
		<< "Interaction type [0 = CC & 1 = NC] = " << neutrino.CCNC() << std::endl
		<< "Nuclear target PDG code = " << neutrino.Target() << std::endl
		<< std::endl;

      // get outgoing particles
      
      auto parts = truth.GetParticles();
      
      for (auto const& part : parts) {

	std::cout << "\t status code : " << part.StatusCode()
		  << "\t PDG : " << std::setw(10) << part.PdgCode() 
		  << "\t w/ energy " << part.Trajectory().at(0).E() 
		  << std::endl;

      }// for all outgoing particles

      std::cout << std::endl << std::endl << std::endl;

      //return true;

      // grab mcparticles
      auto ev_mcpart = storage->get_data<event_mcpart>("largeant");

      for (size_t i=0; i < ev_mcpart->size(); i++) {

	auto const& part = ev_mcpart->at(i);
	auto const& step = part.Trajectory().at(0);

	if (part.Trajectory().at(0).E() * 1000 < 0.2) continue;

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
	
      }// for all particles

      std::cout << std::endl << std::endl << std::endl;
      
    }// for all mctruth objects
  
    return true;
  }

  bool MCTruthScanner::finalize() {
  
    return true;
  }

}
#endif
