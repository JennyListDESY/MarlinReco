/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* 
** This file is part of the MarlinReco Project.
** Forming part of the SubPackage: BrahmsTracking.
**
** For the latest version download from Web CVS:
** www.blah.de
**
** $Id: LEPTrackingProcessor.cc,v 1.39 2008-07-02 09:01:23 aplin Exp $
**
** $Log: not supported by cvs2svn $
** Revision 1.38  2008/07/01 15:08:31  aplin
** An Error message has been added as an event parameter for the case where the PATREC fails due to too many links.
**
** Only one track collection is now produced, namely "TPCTracks", as LEPTracking now only produces hits from the TPC. FullLDCTracking is now responsible for creating tracks including hits from the silicon detectors.
**
** Revision 1.37  2008/07/01 10:34:39  aplin
** The functionality of CurlKiller has been moved into LEPTracking. It is
** now operated at increasing levels of aggression in order to try to
** save the event from not not having ANY TPC Tracks, due to too many links in
** trslnk.
**
** As a first try, all hits are passed to the PATREC. If this fails three
** more attempts are made where the size of the bins used to find high
** levels of multiplicity in r-phi are increased by factors of 2, 3 and
** finally 4. If this still fails an error message is printed warning
** that NO TPC Tracks will be reconstructed for this event.
**
** The verbosity of LEPTracking is now controlled completely via
** streamlog on the C++ side, and via flags set in tkinit on the F77
** side.
**
** Revision 1.36  2008/06/27 15:32:27  aplin
** corrected conversion of covariance matrix, and made more use of streamlog
**
** Revision 1.35  2008/06/26 09:35:56  aplin
**
** A clean up has been made of the arrays that hold the TE's/Hits for
** track fitting. An effort has been made to ensure that both the arrays
** are big enough to hold the required number of hits, and that the arrays
** are all of the same dimensions between calling routines.
**
** All verbose output, except for the case where the tracking is dropped
** for a complete event, has now been hidden behind debug IF
** Statements. The level of verbosity can be controlled via the variables
** TPCDBG, TKSTDBG, FSFDBG and IDEB  in tkinit.F
**
** Revision 1.34  2008/04/17 14:36:29  aplin
** reduced printoutlevel in f77 and moved cout to debug streamlog_out for C++
**
** Revision 1.33  2008/04/10 21:49:14  aplin
** Corrected covariance matrix definition. Added extra Expert Histos in TPCDigi. Removed VTX and SIT Hits from LEPTracking
**
** Revision 1.32  2008/03/11 15:19:18  engels
** removed spurious dev code (S. Aplin)
**
** Revision 1.31  2008/02/14 15:34:11  aplin
** removed spurious dev code
**
** Revision 1.30  2008/02/14 14:43:30  aplin
** removed spurious dev code
**
** Revision 1.29  2008/02/13 10:23:30  aplin
** fixed mm->cm conversion bug
**
** Revision 1.28  2008/02/11 16:54:51  aplin
** ntroduced smearing which depends on the parameterisation supplied by the LC-TPC group.
** It is parameterised in phi and theta. Not yet in z.
**
** The Covariance matrix corrected so that it is set for XYZ and not r-phi as previously. The corresponding conversion back to r-phi has been set in LEPTracking and FullLDCTracking.
**
** Revision 1.26  2007/09/13 07:39:04  rasp
** Updated version of the LEPTracking processor. For each
** digitized TPC hit spatial resolutions are accessed via
** call to the TrackerHit::getCovMatrix method.
**
** Revision 1.24  2007/09/05 09:47:29  rasp
** Updated version
**
** Revision 1.22  2006/10/17 12:34:19  gaede
** replaced registerProcessorParameter with registerInput/OutputCollection
**
** Revision 1.21  2006/06/28 15:29:04  aplin
** The B-Field is now variable for LEPTracking via the gear xml file. The B-Field is specified in the TPCParameters as follows: <parameter name="tpcBField" type="double"> 4.0  </parameter>
**
** The value is passed internaly to the F77 code via the same function which passes the TPC geometry i.e. gettpcgeom(float* innerrad, float* outerrad, int* npadrows, float* maxdrift, float* tpcpixz, float* ionpoten, float* tpcrpres, float* tpczres, float* tpcbfield). It is set in setmat.F. tpcgeom.F had to be modified as it also uses gettpcgeom, although it does not make use of the B-Field.
**
** Revision 1.22   gaede
** added registerInput/OutputCollection for Marlin -c
** 
** Revision 1.20  2006/05/28 15:22:15  owendt
** changed text for the explanation of a processor parameter
**
** Revision 1.19  2006/04/27 13:07:43  samson
** Fix minor syntax errors to achieve compatibility with gcc4
**
** Revision 1.18  2006/02/09 18:00:41  owendt
** removed cout statements for debugging
**
** Revision 1.17  2006/02/03 15:09:11  owendt
** i) Corrected bug in calculation of weights, relocated brace.
** ii) Weights are now calculated as the percentage of hits that a given MC particle contributes to the reconstructed track's hit collection.
**
** Revision 1.16  2005/12/06 15:26:23  aplin
** corrected erroneous definition of MC Track Relation weight
**
** Revision 1.15  2005/11/03 15:16:14  aplin
** Added the Trackstring creation and the biulding of full Track candiates (TK's) which have passed the Delphi Ambiguity resolver fxambi. The material description of the vtx detector, as for the TPC, is hard coded in setmat. Presently the VTX and SIT resolutions are hard coded in LEPTrackingProcessor. The debug output has been reduced and can be controlled via TKSTDBG etc. in tkinit.F. delsolve contains the delphi ambuguity resolver written in C and is contained in the directory named C. The Tk's are written back into the C++ side in tktrev. The corresponding Tk bank structure analogous to the TE bank structure has been added in tktkbank whilst the access wrapper functions are contained in LEPTracking.
**
** Revision 1.13  2005/08/08 07:09:13  aplin
** Made f77 tracking code use GEAR to define the geomtery of the TPC. LTPDRO now defines the maximum number of rows is used to define the size of arrays, this is limited to 224 due the use of 7 '32 bit' bit registers in trkfnd.F increased, though at present it is not likely that anybody would want more. The number of TPC padrows is defined at run time by NRTPC which should of course not exceed LTPDRO, although this is checked and the programe exits with a verbose error message. A wrapper function gettpcgeom is used to pass the GEAR TPC parameters from C++ to f77. MarlinUtil/include/marlin_tpcgeom.h have MarlinUtil/src/marlin_tpcgeom.cc consequently been removed as they are no longer needed.
**
** Revision 1.12  2005/08/04 12:54:51  aplin
** *** empty log message ***
**
** Revision 1.11  2005/08/03 21:31:09  aplin
** tk*bank structures initialisation move here from BrahmsInitProcessor and BrahmEndProcessor
**
** Revision 1.10  2005/08/03 19:05:24  aplin
** corrected erroneous function declaration of tkmktecpp, by using float * instead of numerous floats and added output collection names as steering parametes
** 
*/
#include "LEPTrackingProcessor.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "marlin/Exceptions.h"
#include <cmath>


#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/MCParticle.h>
#include <EVENT/Track.h>
#include <EVENT/SimTrackerHit.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <IMPL/LCFlagImpl.h>


#include <cfortran.h>


#include"tpchitbank.h"
#include"tkhitbank.h"
#include"tktebank.h"
#include"tktkbank.h"
#include"tkmcbank.h"
//#include"marlin_tpcgeom.h"
#include"constants.h"

// STUFF needed for GEAR
#include <marlin/Global.h>
#include <gear/GEAR.h>
#include <gear/TPCParameters.h>
#include <gear/PadRowLayout2D.h>
#include <gearimpl/FixedPadSizeDiskLayout.h>
#include <gear/BField.h>
//



PROTOCCALLSFFUN0(INT,TKTREV,tktrev)
#define TKTREV() CCALLSFFUN0(TKTREV,tktrev)

PROTOCCALLSFFUN0(INT,CHECKTPCROWS,checktpcrows)
#define CHECKTPCROWS() CCALLSFFUN0(CHECKTPCROWS,checktpcrows)

  // FIXME:SJA: the namespace should be used explicitly
using namespace lcio ;
using namespace marlin ;
using namespace constants ;
using namespace std ; 

int subdetfirsthitindex(string subdet);

FCALLSCFUN1(INT,subdetfirsthitindex,SUBDETFIRSTHITINDEX,subdetfirsthitindex, STRING)

  int numofsubdethits(string subdet);

FCALLSCFUN1(INT,numofsubdethits,NUMOFSUBDETHITS,numofsubdethits, STRING)

  int writetpccpp(float c, int a, int b); 

FCALLSCFUN3(INT,writetpccpp,WRITETPCCPP,writetpccpp, FLOAT, INT, INT)
      
  float readtpchitscpp(int a, int b); 

FCALLSCFUN2(FLOAT,readtpchitscpp,READTPCHITSCPP,readtpchitscpp, INT, INT)

  int writetkhitcpp(float c, int a, int b); 

FCALLSCFUN3(INT,writetkhitcpp,WRITETKHITCPP,writetkhitcpp, FLOAT, INT, INT)

  int ireadtkhitscpp(int a, int b); 

FCALLSCFUN2(INT,ireadtkhitscpp,IREADTKHITSCPP,ireadtkhitscpp, INT, INT)

  float rreadtkhitscpp(int a, int b); 

FCALLSCFUN2(FLOAT,rreadtkhitscpp,RREADTKHITSCPP,rreadtkhitscpp, INT, INT)


  int writetktkcpp(float c, int a, int b); 

FCALLSCFUN3(INT,writetktkcpp,WRITETKTKCPP,writetktkcpp, FLOAT, INT, INT)

  int writetktecpp(float c, int a, int b); 

FCALLSCFUN3(INT,writetktecpp,WRITETKTECPP,writetktecpp, FLOAT, INT, INT)

  float rreadtktkcpp(int a, int b);

FCALLSCFUN2(FLOAT,rreadtktkcpp,RREADTKTKCPP,rreadtktkcpp, INT, INT)

  float rreadtktecpp(int a, int b); 

FCALLSCFUN2(FLOAT,rreadtktecpp,RREADTKTECPP,rreadtktecpp, INT, INT)

  int ireadtktkcpp(int a, int b); 

FCALLSCFUN2(INT,ireadtktkcpp,IREADTKTKCPP,ireadtktkcpp, INT, INT)

  int ireadtktecpp(int a, int b); 

FCALLSCFUN2(INT,ireadtktecpp,IREADTKTECPP,ireadtktecpp, INT, INT)

  int tkmktkcpp(int modid,int subdetbits,int MesrCode,int tracktype, int numtes,int Charge,int unused,int ndf,float chi2,float L,float xstart, float ystart, float zstart, float xend, float yend, float zend, float cord1,float cord2,float cord3,float theta,float phi,float invp,float* cov);

FCALLSCFUN23(INT,tkmktkcpp,TKMKTKCPP,tkmktkcpp, INT , INT ,INT ,INT ,INT ,INT ,INT ,INT, FLOAT, FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT, FLOAT, FLOAT, FLOATV)
  
  int tkmktecpp(int subid,int submod,int unused,int MesrCode,int PnteTE,int Q,int ndf,float chi2,float L,float cord1,float cord2,float cord3,float theta,float phi,float invp,float dedx,float* cov);

FCALLSCFUN17(INT,tkmktecpp,TKMKTECPP,tkmktecpp, INT , INT ,INT ,INT ,INT ,INT ,INT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOAT ,FLOATV)

  int addtetktkcpp(int a , int b);  

FCALLSCFUN2(INT,addtetktkcpp,ADDTETKTKCPP,addtetktkcpp, INT, INT)

  int addhittktecpp(int a, int b); 

FCALLSCFUN2(INT,addhittktecpp,ADDHITTKTECPP,addhittktecpp, INT, INT)

  int readtkitkdatcpp(int a, int b);

FCALLSCFUN2(INT,readtkitkdatcpp,READTKITKDATCPP,readtkitkdatcpp, INT, INT)

  int readtkitedatcpp(int a, int b); 

FCALLSCFUN2(INT,readtkitedatcpp,READTKITEDATCPP,readtkitedatcpp, INT, INT)

  int writetkitkdatcpp(int c, int a, int b);

FCALLSCFUN3(INT,writetkitkdatcpp,WRITETKITKDATCPP,writetkitkdatcpp, INT, INT, INT)

  int writetkitedatcpp(int c, int a, int b);

FCALLSCFUN3(INT,writetkitedatcpp,WRITETKITEDATCPP,writetkitedatcpp, INT, INT, INT)


  // definition of gettpcgeom done here it just sends the geomtertry into to tpcgeom.F
  int gettpcgeom(float* innerrad, float* outerrad, int* npadrows, 
                  float* maxdrift, float* tpcpixz, float* ionpoten, float* tpcrpres, float* tpczres, float* tpcbfield){

  //  try{

  const gear::TPCParameters& gearTPC = Global::GEAR->getTPCParameters() ;
  const gear::PadRowLayout2D& padLayout = gearTPC.getPadLayout() ;
  const gear::DoubleVec & planeExt = padLayout.getPlaneExtent() ;
  
  *innerrad = 0.1 * float( planeExt[0] ) ;
  *outerrad = 0.1 *float( planeExt[1] ) ;
  *npadrows = padLayout.getNRows() ;
  *maxdrift = 0.1 * float( gearTPC.getMaxDriftLength() );
  //  *tpcpixz = 0.1 * float(gearTPC.getDoubleVal("tpcPixZ")) ;
  // FIXME:SJA: should this really be multiplied by 0.1 or has it just got caught up in the other mm->cm convertions 
  *ionpoten = 0.1 * float(gearTPC.getDoubleVal("tpcIonPotential")) ;  
  //  *tpcrpres = 0.1 * float(gearTPC.getDoubleVal("tpcRPhiResConst")) ;  
  //  *tpczres = 0.1 * float(gearTPC.getDoubleVal("tpcZRes")) ;
  //  *tpcbfield = float(gearTPC.getDoubleVal("BField")) ;
  *tpcbfield = float(Global::GEAR->getBField().at( gear::Vector3D( 0., 0., 0.) ).z());

  //  }
//  catch() {return 1} ;

  return 0;
}

FCALLSCFUN9(INT,gettpcgeom,GETTPCGEOM,gettpcgeom, PFLOAT, PFLOAT, PINT, 
            PFLOAT, PFLOAT, PFLOAT, PFLOAT, PFLOAT, PFLOAT )




  // end of cfortran.h definitions

// function to generate a unique key for each [r][phi] bin based on a 64bit bitfield
// the most significant 32 bits are used for r 
unsigned long long make_keyNew( unsigned r, unsigned phi ){

  unsigned long long temp = 0xffffffff & r ;

  temp = temp << 32 ;

  return  ( temp )  |  ( 0xffffffff & phi )  ;
} 


// Map to store the enteries for a 2D(r-phi) projection of the Tracker hits 
typedef std::map< unsigned long long  , std::vector<EVENT::TrackerHit*> >  HitMap ; 

  LEPTrackingProcessor aLEPTrackingProcessor ;

LEPTrackingProcessor::LEPTrackingProcessor() : Processor("LEPTrackingProcessor") {
  
  // modify processor description
  _description = "Produces Track collection from TPC TrackerHit collections using LEP tracking algorithms" ;

  // register steering parameters: name, description, class-variable, default value

  registerInputCollection( LCIO::TRACKERHIT,
                           "TPCTrackerHitCollectionName" , 
                           "Name of the TPC TrackerHit collection"  ,
                           _colNameTPC ,
                           std::string("TPCTrackerHits") ) ;

  registerInputCollection( LCIO::TRACKERHIT,
                           "VTXTrackerHitCollectionName" , 
                           "Name of the VTX TrackerHit collection"  ,
                           _colNameVTX ,
                           std::string("VTXTrackerHits") ) ;
  
  registerInputCollection( LCIO::TRACKERHIT,
                           "SITTrackerHitCollectionName" , 
                           "Name of the SIT TrackerHit collection"  ,
                           _colNameSIT ,
                           std::string("SITTrackerHits") ) ;
  
  registerOutputCollection( LCIO::TRACK,
                            "TPCTrackCollectionName" , 
                            "Name of the TPC Track collection"  ,
                            _colNameTPCTracks ,
                            std::string("TPCTracks") ) ;

//  registerOutputCollection( LCIO::TRACK,
//                            "TrackCollectionName" , 
//                            "Name of the Track collection"  ,
//                            _colNameTracks ,
//                            std::string("Tracks") ) ;
  
  registerOutputCollection( LCIO::LCRELATION,
                            "MCTPCTrackRelCollectionName" , 
                            "Name of the TPC Track MC Relation collection"  ,
                            _colNameMCTPCTracksRel ,
                            std::string("TPCTracksMCP") ) ;
  
  registerOutputCollection( LCIO::LCRELATION,
                            "MCTrackRelCollectionName" , 
                            "Name of the Track MC Relation collection"  ,
                            _colNameMCTracksRel ,
                            std::string("TracksMCP") ) ;

  registerOutputCollection( LCIO::TRACKERHIT,
                            "DroppedCollectionName" , 
                            "Name of the TrackerHit collection NOT used in PATREC"  ,
                            _droppedColName ,
                            std::string("DroppedTPCTrackeHits") ) ;
  
  registerOutputCollection( LCIO::TRACKERHIT,
                            "UsedCollectionName" , 
                            "Name of the TrackerHit collection used in PATREC"  ,
                            _usedColName ,
                            std::string("UsedTPCTrackerHits") ) ;
  
  
  registerProcessorParameter( "BinHeight" , 
                              "Bin Height in pad multiples"  ,
                              _binHeight ,
                              int(1) ) ;

  registerProcessorParameter( "BinWidth" , 
                              "Bin Width in pad multiples"  ,
                              _binWidth ,
                              int(3) ) ;

  registerProcessorParameter( "MultiplicityCut" , 
                              "Cut on the number of hits in r-phi bin"  ,
                              _multiplicityCut ,
                              int(8) ) ;
}


void LEPTrackingProcessor::init() { 

  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;
  
}

void LEPTrackingProcessor::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;
} 

void LEPTrackingProcessor::processEvent( LCEvent * evt ) { 

  static bool firstEvent = true ;
  
  //  f77histos->fill1DHist("testhisto",gsl_ran_gaussian(_random,1.0));
  
  // this gets called for every event 
  // usually the working horse ...
  
//   int skipToEvent = 3 ;
//   if(_nEvt<skipToEvent) {
//     ++_nEvt;
//     return;
//   }

  if(firstEvent==true) streamlog_out(MESSAGE) << "LEPTrackingProcessor called for first event" << endl;

  firstEvent = false ;


  // create bank structure
  TkMCBank = new Tk_MC_Bank;
  TPCHitBank = new TPC_Hit_Bank;  
  TkHitBank = new Tk_Hit_Bank;  
  TkTeBank = new Tk_Te_Bank;  
  TkTkBank = new Tk_Tk_Bank;  


  LCCollection* tpcTHcol = 0 ;

  try{
    tpcTHcol = evt->getCollection( _colNameTPC ) ;
  }
  catch(DataNotAvailableException &e){
  }
  

  LCCollection* vtxTHcol = 0 ;
  try{
    vtxTHcol = evt->getCollection( _colNameVTX ) ;
  }
  catch(DataNotAvailableException &e){
  }
  
  LCCollection* sitTHcol = 0 ;
  try{
    sitTHcol = evt->getCollection( _colNameSIT ) ;
  }
  catch(DataNotAvailableException &e){
  }
  
  const gear::TPCParameters& gearTPC = Global::GEAR->getTPCParameters() ;
  const gear::PadRowLayout2D& padLayout = gearTPC.getPadLayout() ;

  if (padLayout.getNRows()>CHECKTPCROWS()) 
      {
      std::stringstream errorMsg;
      errorMsg << "\nProcessor: LEPTracking \n" <<
        "The number of TPC padrows specified in the GEAR file is " << padLayout.getNRows() << "\n" 
               << "This is larger than the max number of rows that the code can handle." << "\n"
               << "The maximum number is determined by LTPDRO which is currently set to " 
               << CHECKTPCROWS() << "\n"
               << "LTPDRO must be a multiple of 32, and is defined as N32BITREG*32 \n"
               << "Increase N32BITREG in ./src/f77/include/padrow.inc \n"
               << "gmake clean and recompile"
               << "\n" ;
      throw gear::Exception(errorMsg.str());
    }


  if( tpcTHcol != 0 ){
    
    LCCollectionVec* tpcTrackVec = new LCCollectionVec( LCIO::TRACK )  ;
    //LCCollectionVec* TrackVec = new LCCollectionVec( LCIO::TRACK )  ;
    
    // if we want to point back to the hits we need to set the flag
    LCFlagImpl trkFlag(0) ;
    trkFlag.setBit( LCIO::TRBIT_HITS ) ;
    tpcTrackVec->setFlag( trkFlag.getFlag()  ) ;
    //TrackVec->setFlag( trkFlag.getFlag()  ) ;
    
    //LCCollectionVec* lcRelVec = new LCCollectionVec( LCIO::LCRELATION )  ;
    LCCollectionVec* tpclcRelVec = new LCCollectionVec( LCIO::LCRELATION )  ;
    
    LCCollectionVec* droppedCol = new LCCollectionVec( LCIO::TRACKERHIT ) ;
    droppedCol->setSubset() ;     
    LCCollectionVec* usedCol = new LCCollectionVec( LCIO::TRACKERHIT ) ;
    usedCol->setSubset() ; 


    int nTPCHits = tpcTHcol->getNumberOfElements()  ;   
    streamlog_out(DEBUG) << "Number of TPCHit before filtering: " << nTPCHits << endl;
    
    TkHitBank->setFirstHitIndex("TPC"); 
    
    _goodHits.reserve(nTPCHits);

    _goodHits.clear();

    selectTPCHits(tpcTHcol,usedCol);
    
    streamlog_out(DEBUG) << "Number of TPCHit passed to PATREC: " << _goodHits.size() << endl;

    FillTPCHitBanks();

    TkHitBank->setLastHitIndex("TPC"); 
    
    //    cout << "the number of tpc hits sent to brahms = " << TPCHitBank->size() << endl;
    //    CNTPC.ntphits = TPCHitBank->size();
    
    if(TkHitBank->getNumOfSubDetHits("TPC") > 0) {
      int tpcsubid = TkHitBank->getSubdetectorID(TkHitBank->getFirstHitIndex("TPC")) ;
      streamlog_out(DEBUG) << "the first hit for the TPC has id " << tpcsubid << endl ;
    }
    
    int errTKTREV = TKTREV(); 

    if(errTKTREV==911){

      
      streamlog_out(DEBUG) << endl;
      streamlog_out(DEBUG) << "   LEPTrackingProcessor: TKTREV returns:" << errTKTREV << endl;
      streamlog_out(DEBUG) << "   LEPTrackingProcessor: Trying to remove hits alla CurlKiller" << endl;
      streamlog_out(DEBUG) << endl;
      
      for(int i=1;i<4;++i){

        streamlog_out(DEBUG) << "number of TE's = " << TkTeBank->size() << endl ;
        streamlog_out(DEBUG) << "number of TK's = " << TkTkBank->size() << endl ;

        _goodHits.clear();
        TkMCBank->clear();
        TkTeBank->clear();
        TkTkBank->clear();

        delete droppedCol;
        delete usedCol;


        droppedCol = new LCCollectionVec( LCIO::TRACKERHIT ) ;
        droppedCol->setSubset() ;     
        usedCol = new LCCollectionVec( LCIO::TRACKERHIT ) ;
        usedCol->setSubset() ; 


        selectTPCHits(tpcTHcol, usedCol, droppedCol, _binHeight*i,_binWidth*i);
        
        FillTPCHitBanks();
        
        streamlog_out(DEBUG) << "Number of TPCHit after filtering: " << _goodHits.size() << endl;

        if(TkHitBank->getNumOfSubDetHits("TPC") > 0) {
          int tpcsubid = TkHitBank->getSubdetectorID(TkHitBank->getFirstHitIndex("TPC")) ;
          streamlog_out(DEBUG) << "the first hit for the TPC has id " << tpcsubid << endl ;
        }
        
        errTKTREV = TKTREV();  

        if(errTKTREV!=911) {
          break;
        }

        else if(i==3){
          streamlog_out(ERROR) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
          streamlog_out(ERROR) << "   LEPTrackingProcessor: TKTREV returns:" << errTKTREV << endl;
          streamlog_out(ERROR) << "   LEPTrackingProcessor: Removing hits failed to resolve the problem" << endl;          
          streamlog_out(ERROR) << "   LEPTrackingProcessor: This event contains NO TPC TRACKS" << endl;          
          streamlog_out(ERROR) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

          stringstream str;
          str << "_Failure" 
              << "_LEPTracking" ;
          
          stringstream error ;
          error << "Too Many Links" ;

          evt->parameters().setValue(str.str(),error.str());

        }
        else {
          streamlog_out(DEBUG) << "TKTREV return:" << errTKTREV << " trying to remove more hits using bigger bins" << endl;
        }
    
      }
    }


    evt->addCollection( usedCol , _usedColName ) ;
    evt->addCollection( droppedCol , _droppedColName ) ;
   
    streamlog_out(DEBUG) << "TKTREV return:" << errTKTREV << endl;

    streamlog_out(DEBUG) << "number of TE's = " << TkTeBank->size() << endl ;

    streamlog_out(DEBUG) << "number of TK's = " << TkTkBank->size() << endl ;



    for(int te=0; te<TkTeBank->size();te++){


      if( TkTeBank->getSubdetector_ID(te)==500 ) {

        TrackImpl* tpcTrack = new TrackImpl ; 
      
        const double ref_r = 10.*TkTeBank->getCoord1_of_ref_point(te);
        const double ref_phi =TkTeBank->getCoord2_of_ref_point(te)/TkTeBank->getCoord1_of_ref_point(te);
        const double ref_z = 10.*TkTeBank->getCoord3_of_ref_point(te);

        //         cout << "ref_r = " << ref_r << endl;
        //         cout << "ref_phi = " << ref_phi << endl;
      
        // transformation from 1/p to 1/R = consb * (1/p) / sin(theta)
        // consb is given by 1/R = (c*B)/(pt*10^9) where B is in T and pt in GeV  
      
        //        const double bField = gearTPC.getDoubleVal("BField") ;
        const double bField = Global::GEAR->getBField().at( gear::Vector3D( 0., 0., 0.) ).z() ;
        const double consb = (2.99792458* bField )/(10*1000.) ;     // divide by 1000 m->mm

        // tan lambda and curvature remain unchanged as the track is only extrapolated
        // set negative as 1/p is signed with geometric curvature clockwise negative

        const double omega = ( -consb*TkTeBank->getInvp(te) )/ sin( TkTeBank->getTheta(te) ) ;
        const double tanLambda = tan( (twopi/4.) - TkTeBank->getTheta(te) ) ;

        tpcTrack->setOmega( omega ) ;
        tpcTrack->setTanLambda( tanLambda ) ;      

        // computation of D0 and Z0 taken from fkrtpe.F in Brahms
       
        // xref and yref of ref point 
        const double xref = ref_r*cos(ref_phi) ;
        const double yref = ref_r*sin(ref_phi) ;
        const double zref = ref_z ; 
        const double trkRadius = 1. / omega ;


        ////////////////////////////////

        // center of circumference
        const double xc = xref + trkRadius * sin( TkTeBank->getPhi(te) ) ;
        const double yc = yref - trkRadius * cos( TkTeBank->getPhi(te) ) ;
        
        const double xc2 = xc * xc ; 
        const double yc2 = yc * yc ;
        
        const double DCA = ( sqrt( xc2+yc2 ) - fabs(trkRadius) ) ;
        
        double phiOfPCA = atan2 (yc,xc) ;
        
        if (DCA<0.) phiOfPCA = phiOfPCA + twopi/2. ;
        
        if ( phiOfPCA < -twopi/2. ) phiOfPCA = twopi + phiOfPCA ;
        else if ( phiOfPCA > twopi/2. ) phiOfPCA = phiOfPCA = twopi + phiOfPCA ;   
        
        const double x0 = fabs( DCA ) * cos( phiOfPCA ) ;
        const double y0 = fabs( DCA ) * sin( phiOfPCA ) ;
        
        double phi = phiOfPCA + twopi/2. - ( fabs(omega)/omega ) * ( fabs(DCA)/DCA ) * (twopi/4.) ;
        
        if (phi<-twopi/2.) phi =  twopi + phi ;
        else if (phi>twopi/2.)  phi = -twopi + phi ;
        
        const double d0 = y0 * cos( phi )  - x0 * sin( phi ) ;
        
        const double alpha = - omega * ( xref - x0 ) * cos( phi ) - omega * ( yref - y0 ) * sin( phi ) ;
        const double beta = 1.0 - omega * ( xref - x0 ) * sin( phi ) + omega * ( yref - y0 ) * cos( phi ) ;
        
        double dphi = atan2( alpha,beta ) ;
        
        double larc =  - dphi/ omega ;
        
        if (larc < 0. ) {
          if ( dphi < 0.0 ) dphi = dphi + twopi ;
          else dphi = dphi - twopi ;
          larc =  - dphi/ omega ;
        }
        
        double z0 = zref - larc * tanLambda ;

        float refPoint[3] ;
        
        refPoint[0] = x0 ;
        refPoint[1] = y0 ;
        refPoint[2] = z0 ;

        tpcTrack->setPhi( phi ) ;       
        tpcTrack->setD0( d0 ) ;
        tpcTrack->setZ0( z0 ) ;       
        tpcTrack->setReferencePoint( refPoint ) ;

//         std::cout << "calc value of omega = " << omega;
//         std::cout << " calc value of phi = " << phi;
//         std::cout << " calc value of d0 = " << d0;
//         std::cout << " calc value of z0 = " << z0;
//         std::cout << " calc value of tanLambda = " << tanLambda<< std::endl;

        ////////////////////////////////

        tpcTrack->setIsReferencePointPCA(true) ;
        tpcTrack->setChi2(TkTeBank->getChi2(te)) ;
        tpcTrack->setNdf(TkTeBank->getNdf(te)) ;
        tpcTrack->setdEdx(TkTeBank->getDe_dx(te)) ;

        const vector <int> * hits ;
        vector<MCParticle*> mcPointers ;
        vector<int> mcHits ;

        hits = TkTeBank->getHitlist(te) ;

        //        std::cout << "the number of the hits on TE = " << hits->size() << std::endl;

        for(unsigned int tehit=0; tehit<hits->size();tehit++){

          TrackerHit* trkHitTPC = dynamic_cast<TrackerHit*>( tpcTHcol->getElementAt( hits->at(tehit) ) ) ;

          tpcTrack->addHit(trkHitTPC) ;
        
          for(unsigned int j=0; j<trkHitTPC->getRawHits().size(); j++){ 
          
            SimTrackerHit * simTrkHitTPC =dynamic_cast<SimTrackerHit*>(trkHitTPC->getRawHits().at(j)) ;
            MCParticle * mcp = dynamic_cast<MCParticle*>(simTrkHitTPC->getMCParticle()) ; 

            //            if(mcp == NULL) cout << "mc particle pointer = null" << endl ; 
          
            bool found = false;
          
            for(unsigned int k=0; k<mcPointers.size();k++)
              {
                if(mcp==mcPointers[k]){
                  found=true;
                  mcHits[k]++;
                }
              }
            if(!found){
              mcPointers.push_back(mcp);
              mcHits.push_back(1);
            }
          }
        }
      
        for(unsigned int k=0; k<mcPointers.size();k++){

          MCParticle * mcp = mcPointers[k];

          LCRelationImpl* tpclcRel = new LCRelationImpl;
          tpclcRel->setFrom (tpcTrack);
          tpclcRel->setTo (mcp);
          float weight = (float)(mcHits[k])/(float)(tpcTrack->getTrackerHits().size());
          //float weight = (float)(tpcTrack->getTrackerHits().size())/(float)mcHits[k];


          tpclcRel->setWeight(weight);
        

          tpclcRelVec->addElement( tpclcRel );
        }
      
        //FIXME:SJA:  Covariance matrix not included yet needs converting for 1/R and TanLambda
      
        
        tpcTrack->subdetectorHitNumbers().resize(8);
        tpcTrack->subdetectorHitNumbers()[0] = int(0);
        tpcTrack->subdetectorHitNumbers()[1] = int(0);
        tpcTrack->subdetectorHitNumbers()[2] = int(0);
        tpcTrack->subdetectorHitNumbers()[3] = int(hits->size());
        tpcTrack->subdetectorHitNumbers()[4] = int(0);
        tpcTrack->subdetectorHitNumbers()[5] = int(0);
        tpcTrack->subdetectorHitNumbers()[6] = int(0);
        tpcTrack->subdetectorHitNumbers()[7] = int(hits->size());


        tpcTrackVec->addElement( tpcTrack );

      
        //      cout << "TkTeBank->getSubdetector_ID(te) = " << TkTeBank->getSubdetector_ID(te) << endl;
        //      cout << "TkTeBank->getSubmodule(te) = " << TkTeBank->getSubmodule(te) << endl;
        //      cout << "TkTeBank->getUnused(te) = " << TkTeBank->getUnused(te) << endl;
        //      cout << "TkTeBank->getMeasurement_code(te) = " << TkTeBank->getMeasurement_code(te) << endl;
        //      cout << "TkTeBank->getPointer_to_end_of_TE(te) = " << TkTeBank->getPointer_to_end_of_TE(te) << endl;
        //      cout << "TkTeBank->getNdf(te) = " << TkTeBank->getNdf(te) << endl;
        //      cout << "TkTeBank->getChi2(te) = " << TkTeBank->getChi2(te) << endl;
        //      cout << "TkTeBank->getLength(te) = " << TkTeBank->getLength(te) << endl;
        //      cout << "TkTeBank->getCoord1_of_ref_point(te) = " << TkTeBank->getCoord1_of_ref_point(te) << endl;
        //      cout << "TkTeBank->getCoord2_of_ref_point(te) = " << TkTeBank->getCoord2_of_ref_point(te) << endl;
        //      cout << "TkTeBank->getCoord3_of_ref_point(te) = " << TkTeBank->getCoord3_of_ref_point(te) << endl;
        //      cout << "TkTeBank->getTheta(te) = " << TkTeBank->getTheta(te) << endl;
        //      cout << "TkTeBank->getPhi(te) = " << TkTeBank->getPhi(te) << endl;
        //      cout << "TkTeBank->getInvp(te) = " << TkTeBank->getInvp(te) << endl;
        //      cout << "TkTeBank->getDe_dx(te) = " << TkTeBank->getDe_dx(te) << endl;
        //      cout << "TkTeBank->getCovmatrix1(te) = " << TkTeBank->getCovmatrix1(te) << endl;
        //      cout << "TkTeBank->getCovmatrix2(te) = " << TkTeBank->getCovmatrix2(te) << endl;
        //      cout << "TkTeBank->getCovmatrix3(te) = " << TkTeBank->getCovmatrix3(te) << endl;
        //      cout << "TkTeBank->getCovmatrix4(te) = " << TkTeBank->getCovmatrix4(te) << endl;
        //      cout << "TkTeBank->getCovmatrix5(te) = " << TkTeBank->getCovmatrix5(te) << endl;
        //      cout << "TkTeBank->getCovmatrix6(te) = " << TkTeBank->getCovmatrix6(te) << endl;
        //      cout << "TkTeBank->getCovmatrix7(te) = " << TkTeBank->getCovmatrix7(te) << endl;
        //      cout << "TkTeBank->getCovmatrix8(te) = " << TkTeBank->getCovmatrix8(te) << endl;
        //      cout << "TkTeBank->getCovmatrix9(te) = " << TkTeBank->getCovmatrix9(te) << endl;
        //      cout << "TkTeBank->getCovmatrix0(te) = " << TkTeBank->getCovmatrix10(te) << endl;
        //      cout << "TkTeBank->getCovmatrix1(te) = " << TkTeBank->getCovmatrix11(te) << endl;
        //      cout << "TkTeBank->getCovmatrix2(te) = " << TkTeBank->getCovmatrix12(te) << endl;
        //      cout << "TkTeBank->getCovmatrix3(te) = " << TkTeBank->getCovmatrix13(te) << endl;
        //      cout << "TkTeBank->getCovmatrix4(te) = " << TkTeBank->getCovmatrix14(te) << endl;
        //      cout << "TkTeBank->getCovmatrix5(te) = " << TkTeBank->getCovmatrix15(te) << endl;
        //
      }
    }
    // set the parameters to decode the type information in the collection
    // for the time being this has to be done manually
    // in the future we should provide a more convenient mechanism to 
    // decode this sort of meta information

    //     StringVec typeNames ;
    //     IntVec typeValues ;
    //     typeNames.push_back( LCIO::TRACK ) ;
    //     typeValues.push_back( 1 ) ;
    //     tpcTrackVec->parameters().setValues("TrackTypeNames" , typeNames ) ;
    //     tpcTrackVec->parameters().setValues("TrackTypeValues" , typeValues ) ;

    //    evt->addCollection( tpcTrackVec , _colNameTPCTracks) ;
    //    evt->addCollection( lcRelVec , _colNameMCTracksRel) ;


//  //******************************
//  // try here for TK's
//
//    
//    for(int tk=0; tk<TkTkBank->size();tk++){
//
//      TrackImpl* Track = new TrackImpl ; 
//        
//      const double ref_r = 10.*TkTkBank->getCoord1_of_ref_point(tk);
//      const double ref_phi =TkTkBank->getCoord2_of_ref_point(tk)/TkTkBank->getCoord1_of_ref_point(tk);
//      const double ref_z = 10.*TkTkBank->getCoord3_of_ref_point(tk);
//      
//      //         cout << "ref_r = " << ref_r << endl;
//      //         cout << "ref_phi = " << ref_phi << endl;
//      
//      // transformation from 1/p to 1/R = consb * (1/p) / sin(theta)
//      // consb is given by 1/R = (c*B)/(pt*10^9) where B is in T and pt in GeV  
//      
//      //      const double bField = gearTPC.getDoubleVal("BField") ;
//      const double bField = Global::GEAR->getBField().at( gear::Vector3D( 0., 0., 0.) ).z() ;
//      const double consb = (2.99792458* bField )/(10*1000.) ;     // divide by 1000 m->mm
//      
//      // tan lambda and curvature remain unchanged as the track is only extrapolated
//      // set negative as 1/p is signed with geometric curvature clockwise negative
//      
//      const double omega = ( -consb*TkTkBank->getInvp(tk) )/ sin( TkTkBank->getTheta(tk) ) ;
//      const double tanLambda = tan( (twopi/4.) - TkTkBank->getTheta(tk) ) ;
//      
//      Track->setOmega( omega ) ;
//      Track->setTanLambda( tanLambda ) ;      
//      
//      // computation of D0 and Z0 taken from fkrtpe.F in Brahms
//      
//      // xref and yref of ref point 
//      const double xref = ref_r*cos(ref_phi) ;
//      const double yref = ref_r*sin(ref_phi) ;
//      const double zref = ref_z ; 
//      const double trkRadius = 1. / omega ;
//      
//      
//      ////////////////////////////////
//      
//      // center of circumference
//      const double xc = xref + trkRadius * sin( TkTkBank->getPhi(tk) ) ;
//      const double yc = yref - trkRadius * cos( TkTkBank->getPhi(tk) ) ;
//      
//      const double xc2 = xc * xc ; 
//      const double yc2 = yc * yc ;
//      
//      const double DCA = ( sqrt( xc2+yc2 ) - fabs(trkRadius) ) ;
//      
//      double phiOfPCA = atan2 (yc,xc) ;
//      
//      if (DCA<0.) phiOfPCA = phiOfPCA + twopi/2. ;
//      
//      if ( phiOfPCA < -twopi/2. ) phiOfPCA = twopi + phiOfPCA ;
//      else if ( phiOfPCA > twopi/2. ) phiOfPCA = phiOfPCA = twopi + phiOfPCA ;   
//      
//      const double x0 = fabs( DCA ) * cos( phiOfPCA ) ;
//      const double y0 = fabs( DCA ) * sin( phiOfPCA ) ;
//      
//      double phi = phiOfPCA + twopi/2. - ( fabs(omega)/omega ) * ( fabs(DCA)/DCA ) * (twopi/4.) ;
//      
//      if (phi<-twopi/2.) phi =  twopi + phi ;
//      else if (phi>twopi/2.)  phi = -twopi + phi ;
//      
//      const double d0 = y0 * cos( phi )  - x0 * sin( phi ) ;
//      
//      const double alpha = - omega * ( xref - x0 ) * cos( phi ) - omega * ( yref - y0 ) * sin( phi ) ;
//      const double beta = 1.0 - omega * ( xref - x0 ) * sin( phi ) + omega * ( yref - y0 ) * cos( phi ) ;
//      
//      double dphi = atan2( alpha,beta ) ;
//      
//      double larc =  - dphi/ omega ;
//      
//      if (larc < 0. ) {
//        if ( dphi < 0.0 ) dphi = dphi + twopi ;
//        else dphi = dphi - twopi ;
//        larc =  - dphi/ omega ;
//      }
//      
//      double z0 = zref - larc * tanLambda ;
//      
//      float refPoint[3] ;
//      
//      refPoint[0] = x0 ;
//      refPoint[1] = y0 ;
//      refPoint[2] = z0 ;
//      
//      Track->setPhi( phi ) ;       
//      Track->setD0( d0 ) ;
//      Track->setZ0( z0 ) ;       
//      Track->setReferencePoint( refPoint ) ;
//      
//      //                 std::cout << "calc value of omega = " << omega;
//      //                 std::cout << " calc value of phi = " << phi;
//      //                 std::cout << " calc value of d0 = " << d0;
//      //                 std::cout << " calc value of z0 = " << z0;
//      //                 std::cout << " calc value of tanLambda = " << tanLambda<< std::endl;
//      //
//      ////////////////////////////////
//      
//      Track->setIsReferencePointPCA(true) ;
//      Track->setChi2(TkTkBank->getChi2(tk)) ;
//      Track->setNdf(TkTkBank->getNdf(tk)) ;
//      //        Track->setdEdx(TkTkBank->getDe_dx(tk)) ;
//      
//      const vector <int> * hits ;
//      const vector <int> * tes ;
//      vector<MCParticle*> mcPointers ;
//      vector<int> mcHits ;
//      
//      
//      tes = TkTkBank->getTElist(tk) ;
//      
//      //      std::cout << "the number of TE's in TK " << tk << " = " << tes->size() << std::endl;   
//      
//      for(unsigned int tkte=0; tkte<tes->size();tkte++){
//        
//        
//        //        std::cout << "the number of the TE in TK = " << tes->at(tkte) << std::endl;
//        //        std::cout << "the Subdetector number of the TE in TK " << TkTeBank->getSubdetector_ID(tes->at(tkte)) << std::endl;
//        
//        hits = TkTeBank->getHitlist(tes->at(tkte)) ;
//        
//        //        std::cout << "the number of the hits on TE = " << hits->size() << std::endl;
//        
//        //          for(unsigned int tkhit=0; tkhit<hits->size();tkhit++){
//        
//        //           std::cout << "the Subdetector number of the Hit is " << TkHitBank->getSubdetectorID(hits->at(tkhit)) << std::endl; 
//        //          }
//        
//        //        }
//
//        for(unsigned int tkhit=0; tkhit<hits->size();tkhit++){
//          
//          TrackerHit* trkHit ;
//
//          if(TkHitBank->getSubdetectorID(hits->at(tkhit))==500){
//            
//            int tpchitindex = hits->at(tkhit) - TkHitBank->getFirstHitIndex("TPC") ;
//            trkHit = dynamic_cast<TrackerHit*>( tpcTHcol->getElementAt( tpchitindex ) ) ;
//          }
//          
//          if(TkHitBank->getSubdetectorID(hits->at(tkhit))>99
//             && TkHitBank->getSubdetectorID(hits->at(tkhit))<106){
//           
//            int vtxhitindex = hits->at(tkhit) - TkHitBank->getFirstHitIndex("VTX") ;  
//            trkHit = dynamic_cast<TrackerHit*>( vtxTHcol->getElementAt( vtxhitindex ) ) ;
//          }          
//
//          if(TkHitBank->getSubdetectorID(hits->at(tkhit))>399
//             && TkHitBank->getSubdetectorID(hits->at(tkhit))<403){
//           
//            int sithitindex = hits->at(tkhit) - TkHitBank->getFirstHitIndex("SIT") ;  
//            trkHit = dynamic_cast<TrackerHit*>( sitTHcol->getElementAt( sithitindex ) ) ;
//          }
//
//          Track->addHit(trkHit) ;
//          
//          for(unsigned int j=0; j<trkHit->getRawHits().size(); j++){ 
//            
//            SimTrackerHit * simTrkHit =dynamic_cast<SimTrackerHit*>(trkHit->getRawHits().at(j)) ;
//            MCParticle * mcp = dynamic_cast<MCParticle*>(simTrkHit->getMCParticle()) ; 
//
//            //            if(mcp == NULL) cout << "mc particle pointer = null" << endl ; 
//            
//            bool found = false;
//            
//            for(unsigned int k=0; k<mcPointers.size();k++)
//              {
//                if(mcp==mcPointers[k]){
//                  found=true;
//                  mcHits[k]++;
//                }
//              }
//            if(!found){
//              mcPointers.push_back(mcp);
//              mcHits.push_back(1);
//            }
//          }
//        }
//      }
//
//
//    for(unsigned int k=0; k<mcPointers.size();k++){
//        
//        MCParticle * mcp = mcPointers[k];
//        
//        LCRelationImpl* lcRel = new LCRelationImpl;
//        lcRel->setFrom (Track);
//        lcRel->setTo (mcp);
//        float weight = (float)(mcHits[k])/(float)(Track->getTrackerHits().size());
//        //float weight = (float)(Track->getTrackerHits().size())/(float)mcHits[k];
//        
//        // debug
//        /*
//        std::cout << "TkTkBank->size() : " << TkTkBank->size() << " Track : " << tk 
//                  << "  # MCs : " << mcPointers.size() 
//                  << "  actual : " << k << "  # TrackerHits : " 
//                  << Track->getTrackerHits().size();
//        std::cout << "   mcHits[" << k << "] = " << mcHits[k];
//        std::cout << "   LEPTR WEIGHT: " 
//                  << weight << "  mcp-> " << mcp->getPDG() << " energy = " << mcp->getEnergy() << std::endl;
//        */
//        
//        
//        lcRel->setWeight(weight);
//        
//        
//        lcRelVec->addElement( lcRel );
//      }
//      
//
//        
//      //FIXME:SJA:  Covariance matrix not included yet needs converting for 1/R and TanLambda
//      
//      
//      TrackVec->addElement( Track );
//
//    }

    //    // set the parameters to decode the type information in the collection
    //    // for the time being this has to be done manually
    //    // in the future we should provide a more convenient mechanism to 
    //    // decode this sort of meta information
    //
    //    //     StringVec typeNames ;
    //    //     IntVec typeValues ;
    //    //     typeNames.push_back( LCIO::TRACK ) ;
    //    //     typeValues.push_back( 1 ) ;
    //    //     TrackVec->parameters().setValues("TrackTypeNames" , typeNames ) ;
    //    //     TrackVec->parameters().setValues("TrackTypeValues" , typeValues ) ;
    //
    
    evt->addCollection( tpcTrackVec , _colNameTPCTracks) ;
    evt->addCollection( tpclcRelVec , _colNameMCTPCTracksRel) ;
    //    evt->addCollection( TrackVec , _colNameTracks) ;
    //    evt->addCollection( lcRelVec , _colNameMCTracksRel) ;
    

  }
  

  //******************************  

  delete TkMCBank;
  delete TPCHitBank;
  delete TkHitBank;
  delete TkTeBank;
  delete TkTkBank;

  _nEvt ++ ;
  
}



void LEPTrackingProcessor::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void LEPTrackingProcessor::end(){ 

//  std::cout << "LEPTrackingProcessor::end()  " << name() 
//            << " processed " << _nEvt << " events in " << _nRun << " runs "
//            << std::endl ;
//
}

void LEPTrackingProcessor::selectTPCHits(LCCollection* tpcTHcol, LCCollection* usedCol){
  if( tpcTHcol != 0 ){
    
    int n_THits = tpcTHcol->getNumberOfElements()  ;
    for(int i=0; i< n_THits; i++){
      TrackerHit* THit = dynamic_cast<TrackerHit*>( tpcTHcol->getElementAt( i ) ) ;
      usedCol->addElement(THit) ;
      _goodHits.push_back(THit);
    }
  }
}

void LEPTrackingProcessor::selectTPCHits(LCCollection* tpcTHcol, LCCollectionVec* usedCol, LCCollectionVec* droppedCol, int nbinHeight, int nbinWidth){

  if( tpcTHcol != 0 ){
    
    int n_THits = tpcTHcol->getNumberOfElements()  ;
    
    
    const gear::TPCParameters& gearTPC = Global::GEAR->getTPCParameters() ;
    const gear::PadRowLayout2D& padLayout = gearTPC.getPadLayout() ;
    
    const gear::DoubleVec & planeExt = padLayout.getPlaneExtent() ;
    
    double gearRMin = planeExt[0] ;
    double gearRMax = planeExt[1] ;
    
    const gear::Vector2D padCoord = padLayout.getPadCenter(1) ;

    double binHeight = padLayout.getRowHeight(1) * (double) nbinHeight ;
    double binWidth = padLayout.getPadWidth(1) * padCoord[0] * (double) nbinWidth ;
        

    // create bined pad layout
    const gear::FixedPadSizeDiskLayout padsAsBins(gearRMin, gearRMax, binHeight, binWidth) ;                                       

    // create hit map
    HitMap hitMap ; 
    

    for(int i=0; i< n_THits; i++){
      
      TrackerHit* THit = dynamic_cast<TrackerHit*>( tpcTHcol->getElementAt( i ) ) ;
      
      double *pos;
      pos = (double*) THit->getPosition(); 
      
      double rad = sqrt(pos[0]*pos[0]+pos[1]*pos[1]);      
      
      //get phi of current hit
      float phi = atan2(pos[1],pos[0]);
      if (phi<0.) phi=phi+twopi;     
      
      int padIndex = padsAsBins.getNearestPad(rad,phi);
      unsigned int iRowHit = padsAsBins.getRowNumber(padIndex);      
      unsigned int iPhiHit = padsAsBins.getPadNumber(padIndex);
      
      // enter hit into hitMap
      hitMap[  make_keyNew( iRowHit, iPhiHit ) ].push_back(  THit ) ;      

    }

    //loop over hitmap and fill both collections of cut and remaining hits

    for( HitMap::iterator it = hitMap.begin() ;it != hitMap.end() ;  ++it ) {
      
      const std::vector<EVENT::TrackerHit*>& v = it->second ;
      
      if(   v.size() >=  (unsigned) _multiplicityCut ) {

        for( unsigned i = 0 ; i < v.size() ; i++){
          droppedCol->addElement(  v[i] ) ;
        } 

        //                 int color = 0x88ff88 ;
        //                 int layer = 6 ;
//                 int marker = 2 ;
//                 int size = 1 ;
        
        //                MarlinCED::drawObjectsWithPosition( v.begin(), v.end() , marker, size , color, layer) ;  
        
      }

      if(   v.size() < (unsigned) _multiplicityCut  ) {
        
        for( unsigned i = 0 ; i < v.size() ; i++){
          usedCol->addElement(  v[i] ) ;

          _goodHits.push_back(v[i]);
          
        }            
        
//                  int color = 0x88ffff ;
//                  int layer = 7 ;
//                  int marker = 2 ;
//                  int size = 1 ;
        
        //                 MarlinCED::drawObjectsWithPosition( v.begin(), v.end() , marker, size , color, layer) ; 
      }
      
    }   
        
  }

}


void LEPTrackingProcessor::FillTPCHitBanks(){

  TPCHitBank->clear();
  TkHitBank->clear();

  for(int i=0; i< _goodHits.size(); ++i){
      
    TrackerHit* trkHitTPC = _goodHits[i];
    
    double *pos;
    float  de_dx;
    float  time;
    
    //      cellId = 	trkHitTPC->getCellID();
    pos = (double*) trkHitTPC->getPosition(); 
    de_dx = trkHitTPC->getdEdx();
    time = trkHitTPC->getTime();
    
      // convert to cm needed for BRAHMS(GEANT)
    float x = 0.1*pos[0];
    float y = 0.1*pos[1];
    float z = 0.1*pos[2];
    
    // convert de/dx from GeV (LCIO) to number of electrons 
    const gear::TPCParameters& gearTPC = Global::GEAR->getTPCParameters() ;
    double tpcIonisationPotential = gearTPC.getDoubleVal("tpcIonPotential");
    de_dx = de_dx/tpcIonisationPotential;
    
//       double tpcRPhiResConst = gearTPC.getDoubleVal("tpcRPhiResConst");
//       double tpcRPhiResDiff  = gearTPC.getDoubleVal("tpcRPhiResDiff");
//       double aReso = tpcRPhiResConst*tpcRPhiResConst;
//       double driftLenght = gearTPC.getMaxDriftLength() - fabs(pos[2]);
//       if (driftLenght <0) { 
//         driftLenght = 0.10;
//       }
//       double bReso = tpcRPhiResDiff*tpcRPhiResDiff;
//       double tpcRPhiRes = sqrt(aReso + bReso*driftLenght);
//       double tpcZRes = gearTPC.getDoubleVal("tpcZRes");
    

    // Covariance Matrix in LCIO is defined in XYZ convert to R-Phi-Z
    // For no error in r
    
    
    double rSqrd = pos[0]*pos[0] + pos[1]*pos[1];
    double phi = atan2(pos[1],pos[0]); 
    double tpcRPhiRes = sqrt(trkHitTPC->getCovMatrix()[0]+trkHitTPC->getCovMatrix()[2]);
    double tpcZRes = sqrt(trkHitTPC->getCovMatrix()[5]);
    
      //      cout << "row_hits->getY() = " <<  pos[1] << "  row_hits->getY() = " << pos[0] ; 
//      cout << "  phi = " <<  phi ;  
//      cout << "  tpcRPhiRes = " << tpcRPhiRes ;
//      cout << "  cos(phi)*cos(phi)*tpcRPhiRes*tpcRPhiRes = " << trkHitTPC->getCovMatrix()[2] << endl;

    // convert mm to cm 
    tpcRPhiRes = 0.1 * tpcRPhiRes;
    tpcZRes = 0.1 * tpcZRes;
    
    
    // Brahms resolution code for TPC = 3 REF tkhtpc.F
    int icode = 3;
    int subid = 500;
    
    int mctrack = 0;
    

    TkHitBank->add_hit(x,y,z,de_dx,subid,mctrack,0,0,icode,tpcRPhiRes,tpcZRes);
    
    TPCHitBank->add_hit(x,y,z,de_dx,subid,tpcRPhiRes,tpcZRes,mctrack);
    
  }
}

