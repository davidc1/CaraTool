/**
 * \file CalorimetryTTree.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class CalorimetryTTree
 *
 * @author david
 */

/** \addtogroup 3DMichel

    @{*/

#ifndef LARLITE_CALORIMETRYTTREE_H
#define LARLITE_CALORIMETRYTTREE_H

#include "Analysis/ana_base.h"
#include "TTree.h"
#include "TruncMean.h"

namespace larlite {
  /**
     \class CalorimetryTTree
     User custom analysis class made by SHELL_USER_NAME
   */
  class CalorimetryTTree : public ana_base{
  
  public:

    /// Default constructor
    CalorimetryTTree()
      : _tree(nullptr)
      { _name="CalorimetryTTree"; _fout=0;}

    /// Default destructor
    virtual ~CalorimetryTTree(){}

    /** IMPLEMENT in CalorimetryTTree.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CalorimetryTTree.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CalorimetryTTree.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    TTree *_tree;
    std::vector<double> _x_v, _y_v, _z_v;
    std::vector<double> _dqdx_v, _dqdx_trunc_v;
    std::vector<double> _rr_v;
    double _xe_rc, _ye_rc, _ze_rc;
    double _xe_mc, _ye_mc, _ze_mc;
    double _rr_max;

    TruncMean _tmean;
    
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
