/**
 * \file ShowerSelection.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class ShowerSelection
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SHOWERSELECTION_H
#define LARLITE_SHOWERSELECTION_H

#include "Analysis/ana_base.h"
#include "TTree.h"

namespace larlite {
  /**
     \class ShowerSelection
     User custom analysis class made by SHELL_USER_NAME
   */
  class ShowerSelection : public ana_base{
  
  public:

    /// Default constructor
    ShowerSelection();

    /// Default destructor
    virtual ~ShowerSelection(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

  protected:

    TTree* _tree;
    double _pi0_energy;
    std::vector<int>    _clus_nhits_v;
    std::vector<double> _clus_vtxdist_v;
    std::vector<int>    _clus_plane_v;

    double _wire2cm, _time2cm;
    
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
