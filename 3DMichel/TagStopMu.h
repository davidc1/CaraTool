/**
 * \file TagStopMu.h
 *
 * \ingroup 3DMichel
 * 
 * \brief Class def header for a class TagStopMu
 *
 * @author davidc1
 */

/** \addtogroup 3DMichel

    @{*/

#ifndef LARLITE_TAGSTOPMU_H
#define LARLITE_TAGSTOPMU_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class TagStopMu
     User custom analysis class made by SHELL_USER_NAME
   */
  class TagStopMu : public ana_base{
  
  public:

    /// Default constructor
    TagStopMu(){ _name="TagStopMu"; _fout=0;}

    /// Default destructor
    virtual ~TagStopMu(){}

    /** IMPLEMENT in TagStopMu.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in TagStopMu.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in TagStopMu.cc! 
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
