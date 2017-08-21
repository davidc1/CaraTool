/**
 * \file FilterCosmicTracks.h
 *
 * \ingroup Spallation
 * 
 * \brief Class def header for a class FilterCosmicTracks
 *
 * @author david
 */

/** \addtogroup Spallation

    @{*/

#ifndef LARLITE_FILTERCOSMICTRACKS_H
#define LARLITE_FILTERCOSMICTRACKS_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class FilterCosmicTracks
     User custom analysis class made by SHELL_USER_NAME
   */
  class FilterCosmicTracks : public ana_base{
  
  public:

    /// Default constructor
    FilterCosmicTracks(){ _name="FilterCosmicTracks"; _fout=0;}

    /// Default destructor
    virtual ~FilterCosmicTracks(){}

    /** IMPLEMENT in FilterCosmicTracks.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in FilterCosmicTracks.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in FilterCosmicTracks.cc! 
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
