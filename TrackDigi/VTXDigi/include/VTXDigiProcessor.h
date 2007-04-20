#ifndef VTXDigiProcessor_h
#define VTXDigiProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <gsl/gsl_rng.h>

using namespace lcio ;
using namespace marlin ;


/** ======= VTXDigiProcessor ========== <br>
 * Produces SIT & VTX TrackerHit collection from SimTrackerHit collections. <br> 
 * The positions of "digitized" TrackerHits are obtained by gaussian smearing positions
 * of SimTrackerHits in r-phi and z according to the specified point resolutions. 
 * Each created TrackerHit is assigned the type via method TrackerHitImpl::setType(int type).
 * The TrackerHit type is encoded in the following way : <br>
 * type = 100 + layer_index_vtx  for vertex hits (layer_index_vtx = 1...5) <br>
 * type = 400 + layer_index_sit  for SIT hits (layer_index_sit  = 1,2) <br>  
 * To access this type use method TrackerHit::getType() <br> 
 * <h4>Input collections and prerequisites</h4> 
 * Processor requires collections of SimTrackerHits in vertex detector and SIT <br>
 * <h4>Output</h4>
 * Processor produces collection of digitized TrackerHits in the vertex detector and SIT <br>
 * @param VTXCollectionName The name of input collection of VTX SimTrackerHits <br>
 * (default name vxd00_VXD) <br>
 * @param SITCollectionName The name of input collection of SIT SimTrackerHits <br>
 * (default name sit00_SIT) <br> 
 * @param VTXHitCollection The name of output collection of digitized VTX TrackerHits <br>
 * (default name VTXTrackerHits) <br>
 * @param SITHitCollection The name of output collection of digitized SIT TrackerHits <br>
 * (default name SITTrackerHits) <br>
 * @param PointResolutionRPhi_VTX Point resolution in r-phi for the vertex detector (in mm) <br>
 * (default value 0.004) <br>
 * @param PointResolutionZ_VTX Point resolution in z for the vertex detector (in mm) <br>
 * (default value 0.004) <br>
 * @param PointResolutionRPhi_SIT Point resolution in r-phi for SIT (in mm) <br>
 * (default value 0.01) <br>
 * @param PointResolutionZ_SIT Point resolution in z for SIT (in mm) <br>
 * (default value 0.01) <br>
 * @param RemoveDrays When this flag is set to 1 hits produced by delta-electrons are removed
 * from output collections <br>
 * (default value 0) <br>
 * @param MomentumCutForDRays The upper cut on delta-electron momentum (in MeV) <br>
 * (default value 10) <br>
 * @param Debug When this flag is set to one, debugging regime is enabled with a lot of printouts <br>
 * (default value is 0) <br>
 * <br>
 * @author A. Raspereza, MPI (Munich)
 */
class VTXDigiProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new VTXDigiProcessor ; }
  
  
  VTXDigiProcessor() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;
  
  
 protected:

  std::string _colNameVTX ;
  std::string _colNameSIT ;
  std::string _outColNameVTX;
  std::string _outColNameSIT;

  int _nRun ;
  int _nEvt ;
  int _debug;
  int _removeDRays;
  float _pointResoRPhi,_pointResoRPhi_VTX,_pointResoRPhi_SIT;
	float _pointResoZ,_pointResoZ_VTX,_pointResoZ_SIT;
  float _momCut;

  gsl_rng * r ;


} ;

#endif



