#ifndef LARLITE_EMPTYMODULE_CXX
#define LARLITE_EMPTYMODULE_CXX

#include "EmptyModule.h"

namespace larlite {

  bool EmptyModule::initialize() {

    return true;
  }
  
  bool EmptyModule::analyze(storage_manager* storage) {
  
    return true;
  }

  bool EmptyModule::finalize() {

    return true;
  }

}
#endif
