/**
 * \file FilterShower.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class FilterShower
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_FILTERSHOWER_H
#define LARLITE_FILTERSHOWER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class FilterShower
     User custom analysis class made by SHELL_USER_NAME
   */
  class FilterShower : public ana_base{
  
  public:

    /// Default constructor
    FilterShower(){ _name="FilterShower"; _fout=0;}

    /// Default destructor
    virtual ~FilterShower(){}

    /** IMPLEMENT in FilterShower.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in FilterShower.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in FilterShower.cc! 
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
