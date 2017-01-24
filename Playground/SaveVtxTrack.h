/**
 * \file SaveVtxTrack.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class SaveVtxTrack
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SAVEVTXTRACK_H
#define LARLITE_SAVEVTXTRACK_H

#include "Analysis/ana_base.h"
#include <fstream>

namespace larlite {
  /**
     \class SaveVtxTrack
     User custom analysis class made by SHELL_USER_NAME
   */
  class SaveVtxTrack : public ana_base{
  
  public:

    /// Default constructor
    SaveVtxTrack(){ _name="SaveVtxTrack"; _fout=0;}

    /// Default destructor
    virtual ~SaveVtxTrack(){}

    /** IMPLEMENT in SaveVtxTrack.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in SaveVtxTrack.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in SaveVtxTrack.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    std::ofstream event_file;
    
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
