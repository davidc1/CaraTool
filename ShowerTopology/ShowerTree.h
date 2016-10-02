/**
 * \file ShowerTree.h
 *
 * \ingroup visualization
 * 
 * \brief Class def header for a class ShowerTree
 *
 * @author david caratelli
 */

/** \addtogroup visualization

    @{*/

#ifndef LARLITE_SHOWERTREE_H
#define LARLITE_SHOWERTREE_H

#include "Analysis/ana_base.h"
#include "DataFormat/treenode.h"
#include "DataFormat/mcpart.h"
#include "DataFormat/simch.h"
#include "TTree.h"

namespace larlite {
  /**
     \class ShowerTree
     User custom analysis class made by SHELL_USER_NAME
   */
  class ShowerTree : public ana_base{
  
  public:

    /// Default constructor
    ShowerTree();

    /// Default destructor
    virtual ~ShowerTree(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void AddChildren(treenode& node, const larlite::event_mcpart *parts);

    void SetDebug(bool on) { _debug = on; }
    void SetVerbose(bool on) { _verbose = on; }

  protected:

    // map connecting trackID -> e/gamma
    std::map<int,int> _pdgMap;

    // map connecting trackID -> parentID    
    std::map<int,int> _parentMap;

    // map connecting trackID -> index
    std::map<int,int> _idxMap;

    // diagram function
    void getShowerDiagram(treenode& node, larlite::event_mcpart *parts,
			  std::string& ret, std::string prefix);

    // get the deposited energy of this shower
    void SumLostEnergy(treenode& node, larlite::event_mcpart *parts,
		       int PDGignore, double& energy);

    // get IDEs associated to a shower
    double getShowerIDEs(const std::vector<unsigned int>& g4_v,
			 const event_simch* simch_v);
    
    bool _debug;
    bool _verbose;

    int _calls;

    std::vector<int> _scanned;

    // list of trackIDs associated with the EM shower
    std::vector<unsigned int> _g4_trackid_v;

    // list of IDEs associated with the shower
    std::vector<larlite::ide> _shower_ide_v;

    // total IDE charge
    double _ide_q_tot, _ide_q_elec;
    // total lifetime-corrected IDE charge
    double _ide_qcorr_tot, _ide_qcorr_elec;

    double _michel_x,  _michel_y,  _michel_z;
    double _michel_px, _michel_py, _michel_pz;

    // IDE energy deposited by electron only, and full shower respectively
    double _ide_elec, _ide_shower;
    
    std::vector<double> _photon_edep;
    std::vector<double> _photon_x, _photon_y, _photon_z;

    std::vector<double> _electron_energy;
    std::vector<double> _electron_x, _electron_y, _electron_z;

    TTree *_tree;
    double _e_truth,  _e_dep, _e_elec, _e_lost;
    double _charge;
    std::string _process;

    std::vector<double> _ide_e_v, _ide_x_v, _ide_y_v, _ide_z_v;
    double _ioni_e;
    double _gamma_e;
    
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
