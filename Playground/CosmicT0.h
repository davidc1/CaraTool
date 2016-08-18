/**
 * \file CosmicT0.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class CosmicT0
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_COSMICT0_H
#define LARLITE_COSMICT0_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class CosmicT0
     User custom analysis class made by SHELL_USER_NAME
   */
  class CosmicT0 : public ana_base{
  
  public:

    /// Default constructor
    CosmicT0(){ _name="CosmicT0"; _fout=0;}

    /// Default destructor
    virtual ~CosmicT0(){}

    /** IMPLEMENT in CosmicT0.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CosmicT0.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CosmicT0.cc! 
        Finalize method to be called after all events processed.
    */
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
