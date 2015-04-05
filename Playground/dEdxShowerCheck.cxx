#ifndef DEDXSHOWERCHECK_CXX
#define DEDXSHOWERCHECK_CXX

#include "dEdxShowerCheck.h"

namespace larlite {

  bool dEdxShowerCheck::initialize() {

    // Initialize Geometry Utilities
    //    if(!_Gser) _Gser = (larutil::GeometryUtilities*)(larutil::GeometryUtilities::GetME());
    
    // Setup CalAlgos
    _CaloAlgBox.setUseModBox(true);
    _CaloAlgBirks.setUseModBox(false);

    _hitProducer = "gaushit";
    _mcshrProducer = "mcreco";
    _mctrkProducer = "mcreco";

    _mode = true;

    _hit_tree = new TTree("hit_tree","hit_tree");
    _hit_tree->Branch("_hitADC",&_hitADC,"hitADC/D");
    _hit_tree->Branch("_hitArea",&_hitArea,"hitArea/D");
    _hit_tree->Branch("_hitQAmp",&_hitQAmp,"hitQAmp/D");
    _hit_tree->Branch("_hitQArea",&_hitQArea,"hitQArea/D");
    _hit_tree->Branch("_hitdEdxBoxAmp",&_hitdEdxBoxAmp,"hitdEdxBoxAmp/D");
    _hit_tree->Branch("_hitdEdxBoxArea",&_hitdEdxBoxArea,"hitdEdxBoxArea/D");
    _hit_tree->Branch("_hitdEdxBirksAmp",&_hitdEdxBirksAmp,"hitdEdxBirksAmp/D");
    _hit_tree->Branch("_hitdEdxBirksArea",&_hitdEdxBirksArea,"hitdEdxBirksArea/D");
    _hit_tree->Branch("_theta",&_theta,"theta/D");
    _hit_tree->Branch("_phi",&_phi,"phi/D");
    _hit_tree->Branch("_E",&_E,"E/D");

    _part_tree = new TTree("part_tree","particle tree");
    _part_tree->Branch("_Emc",&_Emc,"Emc/D");
    _part_tree->Branch("_ElinearAmp",&_ElinearAmp,"ElinearAmp/D");
    _part_tree->Branch("_ElinearArea",&_ElinearArea,"ElinearArea/D");
    _part_tree->Branch("_EBirksAmp",&_EBirksAmp,"EBirksAmp/D");
    _part_tree->Branch("_EBirksArea",&_EBirksArea,"EBirksArea/D");
    _part_tree->Branch("_EBoxAmp",&_EBoxAmp,"EBoxAmp/D");
    _part_tree->Branch("_EBoxArea",&_EBoxArea,"EBoxArea/D");

    return true;
  }
  
  bool dEdxShowerCheck::analyze(storage_manager* storage) {

    return true;
  }

  bool dEdxShowerCheck::finalize() {

    _hit_tree->Write();
    _part_tree->Write();
  
    return true;
  }

}
#endif
