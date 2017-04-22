/**
 * \file SimPh.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class SimPh
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SIMPH_H
#define LARLITE_SIMPH_H

#include "Analysis/ana_base.h"
#include "TTree.h"

namespace larlite {
  /**
     \class SimPh
     User custom analysis class made by SHELL_USER_NAME
   */
  class SimPh : public ana_base{
  
  public:

    /// Default constructor
    SimPh()
      : _tree(nullptr)
      { _name="SimPh"; _fout=0;}

    /// Default destructor
    virtual ~SimPh(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void setNTracks(int n)  { _n_tracks  = n; }
    void setNShowers(int n) { _n_showers = n; }

  protected:

    TTree* _tree;
    double _photons;

    int _n_tracks;
    int _n_showers;
    
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
