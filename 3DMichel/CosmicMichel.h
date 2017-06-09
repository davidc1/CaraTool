/**
 * \file CosmicMichel.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class CosmicMichel
 *
 * @author david caratelli
 */

/** \addtogroup 3DMichel

    @{*/

#ifndef LARLITE_COSMICMICHEL_H
#define LARLITE_COSMICMICHEL_H

#include "Analysis/ana_base.h"
#include "TTree.h"
#include "TruncMean.h"

namespace larlite {
  /**
     \class CosmicMichel
     User custom analysis class made by SHELL_USER_NAME
   */
  class CosmicMichel : public ana_base{
  
  public:

    /// Default constructor
    CosmicMichel()
      : _tree_rc(nullptr)
      , _tree_mc(nullptr)
      { _name="CosmicMichel"; _fout=0;}

    /// Default destructor
    virtual ~CosmicMichel(){}

    /** IMPLEMENT in CosmicMichel.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CosmicMichel.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CosmicMichel.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    TTree *_tree_rc;
    std::vector<double> _x_v, _y_v, _z_v;
    std::vector<double> _dqdx_v, _dqdx_trunc_v;
    std::vector<double> _rr_v;
    double _xe_rc, _ye_rc, _ze_rc;
    double _dmin;
    double _rr_max;

    TTree *_tree_mc;
    double _xe_mc, _ye_mc, _ze_mc;
    double _energy_mc;

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
