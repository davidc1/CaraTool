/**
 * \file CreateTruthVertex.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class CreateTruthVertex
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_CREATETRUTHVERTEX_H
#define LARLITE_CREATETRUTHVERTEX_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class CreateTruthVertex
     User custom analysis class made by SHELL_USER_NAME
   */
  class CreateTruthVertex : public ana_base{
  
  public:

    /// Default constructor
    CreateTruthVertex(){ _name="CreateTruthVertex"; _fout=0;}

    /// Default destructor
    virtual ~CreateTruthVertex(){}

    /** IMPLEMENT in CreateTruthVertex.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CreateTruthVertex.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CreateTruthVertex.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    double _driftVel;
    
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
