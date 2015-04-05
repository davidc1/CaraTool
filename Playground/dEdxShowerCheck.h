/**
 * \file dEdxShowerCheck.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class dEdxShowerCheck
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_DEDXSHOWERCHECK_H
#define LARLITE_DEDXSHOWERCHECK_H

#include "Analysis/ana_base.h"
#include "RecoTool/AnalysisAlg/CalorimetryAlg.h"

namespace larlite {
  /**
     \class dEdxShowerCheck
     User custom analysis class made by david
   */
  class dEdxShowerCheck : public ana_base{
  
  public:

    /// Default constructor
    dEdxShowerCheck(){ _name="dEdxShowerCheck"; _fout=0; _debug=false; _tDiff=0; };

    /// Default destructor
    virtual ~dEdxShowerCheck(){};

    /** IMPLEMENT in dEdxShowerCheck.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in dEdxShowerCheck.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in dEdxShowerCheck.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void setForward(double f) { _forward = f; }

    void setDebug(bool on) { _debug = on; }
    
    void setTimeDiff(int t) { _tDiff = t; }

    void setMode(bool on) { _mode = on; }
    
    protected:

    // debug mode
    bool _debug;

    // mode: True = use MCTracks, False = use MCShowers
    bool _mode;
    
    // strings to hold producer names
    std::string _hitProducer;
    std::string _mcshrProducer;
    std::string _mctrkProducer;

    // Hit Tree & variables to be stored
    TTree* _hit_tree;
    double _hitADC;
    double _hitArea;
    double _hitQAmp;
    double _hitQArea;
    double _hitdEdxBoxAmp;
    double _hitdEdxBoxArea;
    double _hitdEdxBirksAmp;
    double _hitdEdxBirksArea;
    double _theta;
    double _phi;
    double _E;

    // Particle Tree & variables to be stored
    TTree* _part_tree;
    double _Emc;
    double _ElinearAmp;
    double _ElinearArea;
    double _EBoxAmp;
    double _EBoxArea;
    double _EBirksAmp;
    double _EBirksArea;

    // Filter value: how forward-going should a shower/track be?
    double _forward;

    //integer to allow for X-displacement due to events occuring @ 2nd frame
    int _tDiff;

    // Calorimetry alg
    ::calo::CalorimetryAlg _CaloAlgBox;
    ::calo::CalorimetryAlg _CaloAlgBirks;
    
    // LArUtil GeometryUtilities
    //larutil::GeometryUtilities *_Gser;

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
