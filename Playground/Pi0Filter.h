/**
 * \file Pi0Filter.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class Pi0Filter
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_PI0FILTER_H
#define LARLITE_PI0FILTER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class Pi0Filter
     User custom analysis class made by SHELL_USER_NAME
   */
  class Pi0Filter : public ana_base{
  
  public:

    /// Default constructor
    Pi0Filter(){ _name="Pi0Filter"; _fout=0; _n_pi0s = 1; }

    /// Default destructor
    virtual ~Pi0Filter(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void setNPi0s(int n) { _n_pi0s = n; }

  protected:

    // number of Pi0s requested in the neutrino interaction
    int _n_pi0s;
    
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
