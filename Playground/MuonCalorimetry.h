/**
 * \file MuonCalorimetry.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class MuonCalorimetry
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_MUONCALORIMETRY_H
#define LARLITE_MUONCALORIMETRY_H

#include "Analysis/ana_base.h"
#include "LArUtil/LArUtil-TypeDef.h"
#include "LArUtil/PxUtils.h"
#include "LArUtil/Geometry.h"
#include "BasicTool/GeoAlgo/GeoAlgo.h"
#include "BasicTool/GeoAlgo/GeoVector.h"
#include "BasicTool/GeoAlgo/GeoTrajectory.h"
#include "RecoTool/AnalysisAlg/CalorimetryAlg.h"
#include "DataFormat/mctrack.h"
#include <TH2D.h>

namespace larlite {
  /**
     \class MuonCalorimetry
     User custom analysis class made by david
   */
  class MuonCalorimetry : public ana_base{
  
  public:

    /// Default constructor
    MuonCalorimetry(){ _name="MuonCalorimetry"; _fout=0; _debug=false; _tDiff=0; };

    /// Default destructor
    virtual ~MuonCalorimetry(){};

    /** IMPLEMENT in MuonCalorimetry.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MuonCalorimetry.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MuonCalorimetry.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    /// Set debug mode
    void setDebug(bool on) { _debug = on; }
    
    /// Set Time-difference
    void setTimeDiff(int t) { _tDiff = t; }

    protected:


    /// Fill trajectory points for the track
    void fillTrajectoryPoints(const mctrack &t);

    // debug mode
    bool _debug;

    //time offset for extra frames
    int _tDiff;
    
    // Constants for hit w/t conversion to cm
    double _w2cm;
    double _t2cm;
    // detector width
    double _detWidth;

    // producer for hits
    std::string _hitProducer;
    // producer for mctracks
    std::string _mctrkProducer;

    // LArUtil GeometryUtilities
    //    larutil::GeometryUtilities *_GUtil;

    // Calorimetry tools
    ::calo::CalorimetryAlg _CaloAlgBox;
    ::calo::CalorimetryAlg _CaloAlgBirks;

    // GeoAlgo package
    geoalgo::GeoAlgo _geoAlgo;
    // TPC AABox
    geoalgo::AABox _TpcBox;

    // Track points for the 3 planes
    geoalgo::Trajectory_t* _trackU;
    geoalgo::Trajectory_t* _trackV;
    geoalgo::Trajectory_t* _trackY;


    // Particle Tree & variables to be stored
    TTree* _part_tree;
    int    _contained;
    double _Emc;
    double _ElinearAmp;
    double _ElinearArea;
    double _EBoxAmp;
    double _EBoxArea;
    double _EBirksAmp;
    double _EBirksArea;

    // Hit Trees & variables
    double _resRange;
    double _hitToTrackDist;
    double _muE;
    TTree* _hit_tree_U;
    double _dEdx_U_BoxAmp;
    double _dEdx_U_BoxArea;
    double _dEdx_U_BirksAmp;
    double _dEdx_U_BirksArea;
    TTree* _hit_tree_V;
    double _dEdx_V_BoxAmp;
    double _dEdx_V_BoxArea;
    double _dEdx_V_BirksAmp;
    double _dEdx_V_BirksArea;
    TTree* _hit_tree_Y;
    double _dEdx_Y_BoxAmp;
    double _dEdx_Y_BoxArea;
    double _dEdx_Y_BirksAmp;
    double _dEdx_Y_BirksArea;

    // histograms
    TH2D* _hdEdx_U_BoxAmp;
    TH2D* _hdEdx_U_BoxArea;
    TH2D* _hdEdx_U_BirksAmp;
    TH2D* _hdEdx_U_BirksArea;
    TH2D* _hdEdx_V;
    TH2D* _hdEdx_Y;

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
