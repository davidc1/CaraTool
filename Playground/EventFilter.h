/**
 * \file EventFilter.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class EventFilter
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_EVENTFILTER_H
#define LARLITE_EVENTFILTER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class EventFilter
     User custom analysis class made by SHELL_USER_NAME
   */
  class EventFilter : public ana_base{
  
  public:

    /// Default constructor
    EventFilter(){ _name="EventFilter"; _fout=0;}

    /// Default destructor
    virtual ~EventFilter(){ RunEvent_v.clear(); }

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();
    
    void addEvent(const int run, const int event);

  protected:

    std::vector<std::pair<int,int> > RunEvent_v;
    
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
