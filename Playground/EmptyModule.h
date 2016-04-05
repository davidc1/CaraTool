/**
 * \file EmptyModule.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class EmptyModule
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_EMPTYMODULE_H
#define LARLITE_EMPTYMODULE_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class EmptyModule
     User custom analysis class made by SHELL_USER_NAME
   */
  class EmptyModule : public ana_base{
  
  public:

    /// Default constructor
    EmptyModule(){ _name="EmptyModule"; _fout=0;}

    /// Default destructor
    virtual ~EmptyModule(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

  protected:
    
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
