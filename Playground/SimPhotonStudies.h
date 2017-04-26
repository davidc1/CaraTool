/**
 * \file SimPhotonStudies.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class SimPhotonStudies
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SIMPHOTONSTUDIES_H
#define LARLITE_SIMPHOTONSTUDIES_H

#include "Analysis/ana_base.h"
#include "TTree.h"

namespace larlite {
  /**
     \class SimPhotonStudies
     User custom analysis class made by SHELL_USER_NAME
   */
  class SimPhotonStudies : public ana_base{
  
  public:

    /// Default constructor
    SimPhotonStudies()
      : _tree(nullptr)
      { _name="SimPhotonStudies"; _fout=0;}

    /// Default destructor
    virtual ~SimPhotonStudies(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void setNTracks(int n)  { _n_tracks  = n; }
    void setNShowers(int n) { _n_showers = n; }

  protected:

    TTree* _tree;
    double _photons;
    double _edep, _qdep;

    int _n_tracks;
    int _n_showers;
    int _nshr, _ntrk;
    int _pdg;
    
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
