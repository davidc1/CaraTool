/**
 * \file Coverage.h
 *
 * \ingroup Spallation
 * 
 * \brief Class def header for a class Coverage
 *
 * @author david
 */

/** \addtogroup Spallation

    @{*/

#ifndef LARLITE_COVERAGE_H
#define LARLITE_COVERAGE_H

#include "Analysis/ana_base.h"

#include "TTree.h"

namespace larlite {
  /**
     \class Coverage
     User custom analysis class made by SHELL_USER_NAME
   */
  class Coverage : public ana_base{
  
  public:

    /// Default constructor
    Coverage()
      : _tree(nullptr)
      { _name="Coverage"; _fout=0;}

    /// Default destructor
    virtual ~Coverage(){}

    /** IMPLEMENT in Coverage.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in Coverage.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in Coverage.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    // vector storing all 3D track 2D projected trajectories
    std::vector < std::vector< std::pair<double,double> > > _trk2D_v;

    std::pair<double,double> R(const double& comW, const double& comT);

    double _t2cm, _w2cm;

    TTree* _tree;
    double _q, _r, _rsum, _rhit;
    double _w, _t;
    
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
