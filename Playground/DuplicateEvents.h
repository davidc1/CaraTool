/**
 * \file DuplicateEvents.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class DuplicateEvents
 *
 * @author davidc1
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_DUPLICATEEVENTS_H
#define LARLITE_DUPLICATEEVENTS_H

#include "Analysis/ana_base.h"

struct EventID {
  int run;
  int subrun;
  int event;
};

namespace larlite {
  /**
     \class DuplicateEvents
     User custom analysis class made by SHELL_USER_NAME
   */
  class DuplicateEvents : public ana_base{
  
  public:

    /// Default constructor
    DuplicateEvents(){ _name="DuplicateEvents"; _fout=0;}

    /// Default destructor
    virtual ~DuplicateEvents(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

  protected:

    std::vector<EventID> _event_db;
    
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
