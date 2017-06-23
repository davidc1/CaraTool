/**
 * \file MCTruthScanner.h
 *
 * \ingroup DeExcitation
 * 
 * \brief Class def header for a class MCTruthScanner
 *
 * @author davidc1
 */

/** \addtogroup DeExcitation

    @{*/

#ifndef LARLITE_MCTRUTHSCANNER_H
#define LARLITE_MCTRUTHSCANNER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class MCTruthScanner
     User custom analysis class made by SHELL_USER_NAME
   */
  class MCTruthScanner : public ana_base{
  
  public:

    /// Default constructor
    MCTruthScanner(){ _name="MCTruthScanner"; _fout=0;_display_mcpart=false;}

    /// Default destructor
    virtual ~MCTruthScanner(){}

    /** IMPLEMENT in MCTruthScanner.cc!
o        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MCTruthScanner.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MCTruthScanner.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void showAllMCPart(bool on) { _display_mcpart = on; }
    void setVerbose(bool on) { _verbose = on; }

  protected:

    int _neta, _nstable;

    bool _display_mcpart;

    bool _verbose;

    int _entry;
    
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
