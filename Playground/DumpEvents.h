/**
 * \file DumpEvents.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class DumpEvents
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_DUMPEVENTS_H
#define LARLITE_DUMPEVENTS_H

#include "Analysis/ana_base.h"
#include <fstream>

namespace larlite {
  /**
     \class DumpEvents
     User custom analysis class made by SHELL_USER_NAME
   */
  class DumpEvents : public ana_base{
  
  public:

    /// Default constructor
    DumpEvents(){ _name="DumpEvents"; _fout=0;}

    /// Default destructor
    virtual ~DumpEvents(){}

    /** IMPLEMENT in DumpEvents.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in DumpEvents.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in DumpEvents.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    ofstream _fileout;
    
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
