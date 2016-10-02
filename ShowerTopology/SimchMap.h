/**
 * \file SimchMap.h
 *
 * \ingroup visualization
 * 
 * \brief Class def header for a class SimchMap
 *
 * @author david
 */

/** \addtogroup visualization

    @{*/

#ifndef LARLITE_SIMCHMAP_H
#define LARLITE_SIMCHMAP_H

#include "Analysis/ana_base.h"
#include "TTree.h"

namespace larlite {
  /**
     \class SimchMap
     User custom analysis class made by SHELL_USER_NAME
   */
  class SimchMap : public ana_base{
  
  public:

    /// Default constructor
    SimchMap();

    /// Default destructor
    virtual ~SimchMap(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    std::vector<double> getIDEX() { return _ide_x; }
    std::vector<double> getIDEY() { return _ide_y; }
    std::vector<double> getIDEZ() { return _ide_z; }
    std::vector<double> getIDEE() { return _ide_e; }


  protected:

    TTree *_tree;

    double _shr_E;
    double _shr_x;
    double _shr_y;
    double _shr_z;
    double _shr_px;
    double _shr_py;
    double _shr_pz;
    double _ioni_E;
    std::vector<double> _ide_e, _ide_x, _ide_y, _ide_z;

  
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
