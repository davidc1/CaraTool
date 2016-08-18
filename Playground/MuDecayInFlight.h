/**
 * \file MuDecayInFlight.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class MuDecayInFlight
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_MUDECAYINFLIGHT_H
#define LARLITE_MUDECAYINFLIGHT_H

#include "Analysis/ana_base.h"
#include "TTree.h"
#include <map>

namespace larlite {
  /**
     \class MuDecayInFlight
     User custom analysis class made by SHELL_USER_NAME
   */
  class MuDecayInFlight : public ana_base{
  
  public:

    /// Default constructor
  MuDecayInFlight() : _tree(nullptr) { _name="MuDecayInFlight"; _fout=0;}

    /// Default destructor
    virtual ~MuDecayInFlight(){}

    /** IMPLEMENT in MuDecayInFlight.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MuDecayInFlight.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MuDecayInFlight.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    TTree* _tree;
    double _energy;
    double _el_x, _el_y, _el_z;
    double _mu_x, _mu_y, _mu_z;
    int    _mu_pdg;
    int    _parent_trkid, _mu_trkid, _min_trkid;
    double _mu_energy;
    double _dist, _dist_min;
    std::string _process;

    std::map<int,int> _trackIDMap;
    
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
