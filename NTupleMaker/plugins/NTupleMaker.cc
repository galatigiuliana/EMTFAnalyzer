// -*- C++ -*-
//=============================================================
// A package to create Ntuples for the EMTF Emulator
// Package:    NTupleMaker
// Class:      NTupleMaker
//
// Written by David Curry
// ============================================================
   
 
// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"


// includes to fetch all reguired data products from the edm::Event
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCTrackCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCStatusDigiCollection.h"

#include "L1Trigger/CSCCommonTrigger/interface/CSCTriggerGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCDigi/interface/RPCDigiL1Link.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

// Sector Receiver LUT class to transform wire/strip numbers to eta/phi observables
#include "L1Trigger/CSCTrackFinder/interface/CSCSectorReceiverLUT.h"
#include "L1Trigger/CSCTrackFinder/interface/CSCTFPtLUT.h"
#include <L1Trigger/CSCTrackFinder/interface/CSCTFSPCoreLogic.h>
#include "CondFormats/L1TObjects/interface/L1MuTriggerScales.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerScalesRcd.h"
#include "CondFormats/L1TObjects/interface/L1MuTriggerPtScale.h"
#include "CondFormats/DataRecord/interface/L1MuTriggerPtScaleRcd.h"

#include "DataFormats/L1CSCTrackFinder/interface/L1CSCStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCTrackCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/CSCTriggerContainer.h"
#include "DataFormats/L1CSCTrackFinder/interface/TrackStub.h"

#include "TMath.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "EMTFAnalyzer/NTupleMaker/interface/DataEvtSummaryHandler.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>

//#include "L1Trigger/L1TMuonEndCap/interface/MuonInternalTrack.h"
//#include "L1Trigger/L1TMuonEndCap/interface/MuonInternalTrackFwd.h"
#include <L1Trigger/CSCTrackFinder/interface/CSCTFSectorProcessor.h>
#include <L1Trigger/CSCTrackFinder/src/CSCTFDTReceiver.h>
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1TMuon/interface/EMTFHitExtra.h"
#include "DataFormats/L1TMuon/interface/EMTFTrackExtra.h"

// === My Functions ====
#include "EMTFAnalyzer/NTupleMaker/interface/fillAllLCTs.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillAllClusts.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillEMTFTracks.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillGenMuons.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillRecoMuons.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillMuonSegments.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillLeg_CSCTFTracks.h"
#include "EMTFAnalyzer/NTupleMaker/interface/fillUnpackedEMTFTracks.h"

// class declaration
using namespace edm;
using namespace reco;
using namespace std;
//using namespace L1TMuon;
using namespace csc;

class CSCTFSectorProcessor;

class NTupleMaker : public edm::EDAnalyzer {
public:
  explicit NTupleMaker(const edm::ParameterSet&);
  ~NTupleMaker();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  enum { nEndcaps = 2, nSectors = 6};

private:

  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  
  bool LS(int lumi_section, int beg_ls, int end_ls);
  int convertRPCsectors(float rpcphi);
   int convertRPCphiBits(float GblPhi, int sector);
  int convertRPCetaBits(float GblEta);
  
  //TSelectionMonitor controlHistos_;
  DataEvtSummaryHandler summaryHandler_;
  
  edm::ESHandle<CSCGeometry> cscGeom;
  
  std::vector<edm::InputTag> moduleLabels;
  std::vector<TString> HLT_name;
  std::vector<int> theBitCorr;
  std::vector<std::string> HLT_triggerObjects;
  HLTConfigProvider hltConfig;
  
  int printLevel;
  bool isMC;
  bool NoTagAndProbe;
  
  // Get the tokem for all input collections
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> cscTPTag_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFHitExtra>> emtfTPTag_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFHitExtra>> emtfTPTagRPC_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFTrackExtra>> emtfTag_token;
  edm::EDGetTokenT<reco::MuonCollection> muons_token;
  edm::EDGetTokenT<std::vector<reco::Vertex>> vertex_token;
  edm::EDGetTokenT<CSCSegmentCollection> cscSegs_token;
  edm::EDGetTokenT<std::vector<reco::GenParticle>> genTag_token;
  edm::EDGetTokenT<std::vector<pair<csc::L1Track,MuonDigiCollection<CSCDetId,CSCCorrelatedLCTDigi>>> > csctfTag_token;
  // edm::EDGetTokenT<L1MuGMTReadoutCollection> leg_gmtTag_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFTrack>> unp_emtfTag_token;


  CSCSectorReceiverLUT* srLUTs_[5][2];
  CSCTFSPCoreLogic* core_;
  
  const L1MuTriggerScales  *scale;
  const L1MuTriggerPtScale *ptScale;

  // To set the phi and eta values of LCTs
  std::unique_ptr<GeometryTranslator> geom;

  // David Curry unconvered this from somewhere, sometime
  const Double_t ptscaleOld[31] =  { 0,
				     1.5,   2.0,   2.5,   3.0,   3.5,   4.0,
				     4.5,   5.0,   6.0,   7.0,   8.0,  10.0,  12.0,  14.0,
				     16.0,  18.0,  20.0,  25.0,  30.0,  35.0,  40.0,  45.0,
				     50.0,  60.0,  70.0,  80.0,  90.0, 100.0, 120.0, 140.0 };
  
  // From http://www.phys.ufl.edu/~mrcarver/forAD/L1TMuonUpgradedTrackFinder.h
  const float ptscaleMatt[33] = { 
    -1.,   0.0,   1.5,   2.0,   2.5,   3.0,   3.5,   4.0,
    4.5,   5.0,   6.0,   7.0,   8.0,  10.0,  12.0,  14.0,  
    16.0,  18.0,  20.0,  25.0,  30.0,  35.0,  40.0,  45.0, 
    50.0,  60.0,  70.0,  80.0,  90.0, 100.0, 120.0, 140.0, 1.E6 };
  
};

 
NTupleMaker::NTupleMaker(const edm::ParameterSet& iConfig) {
  
  emtfTag_token = consumes<std::vector<l1t::EMTFTrackExtra>>(iConfig.getParameter<edm::InputTag>("emtfTag"));
  emtfTPTag_token = consumes<std::vector<l1t::EMTFHitExtra>>(iConfig.getParameter<edm::InputTag>("emtfTPTag"));
  emtfTPTagRPC_token = consumes<std::vector<l1t::EMTFHitExtra>>(iConfig.getParameter<edm::InputTag>("emtfTPTagRPC"));
  cscTPTag_token =  consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("cscTPTag"));
  genTag_token  =  consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genTag"));
  muons_token   =  consumes<reco::MuonCollection>(iConfig.getParameter<edm::InputTag>("muonsTag"));
  vertex_token   =  consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertexTag"));
  cscSegs_token = consumes<CSCSegmentCollection>(iConfig.getParameter<edm::InputTag>("cscSegTag"));
  csctfTag_token =  consumes<std::vector<pair<csc::L1Track,MuonDigiCollection<CSCDetId,CSCCorrelatedLCTDigi>>>>(iConfig.getParameter<edm::InputTag>("csctfTag"));
  // leg_gmtTag_token = consumes<L1MuGMTReadoutCollection>(iConfig.getParameter<edm::InputTag>("leg_gmtTag"));
  unp_emtfTag_token = consumes<std::vector<l1t::EMTFTrack>>(iConfig.getParameter<edm::InputTag>("unp_emtfTag"));

  printLevel = iConfig.getUntrackedParameter<int>("printLevel",0);

  NoTagAndProbe = iConfig.getUntrackedParameter<bool>("NoTagAndProbe", true);
  
  // Output File
  edm::Service<TFileService> fs;
  summaryHandler_.initTree(  fs->make<TTree>("tree","Event Summary") );
  TFileDirectory baseDir=fs->mkdir(iConfig.getParameter<std::string>("outputDIR"));
 
  // Is Monte Carlo or not?
  isMC = iConfig.getUntrackedParameter<int>("isMC",0);


  bzero(srLUTs_ , sizeof(srLUTs_));
  int sector=1;    // assume SR LUTs are all same for every sector
  bool TMB07=true; // specific TMB firmware
  // Create a pset for SR/PT LUTs: if you do not change the value in the
  // configuration file, it will load the default minitLUTs
  edm::ParameterSet srLUTset;
  srLUTset.addUntrackedParameter<bool>("ReadLUTs", false);
  srLUTset.addUntrackedParameter<bool>("Binary",   false);
  srLUTset.addUntrackedParameter<std::string>("LUTPath", "./");

  // positive endcap
  int endcap = 1;
  for(int station=1,fpga=0; station<=4 && fpga<5; station++)
    {
      if(station==1)
        for(int subSector=0; subSector<2 && fpga<5; subSector++)
          srLUTs_[fpga++][1] = new CSCSectorReceiverLUT(endcap,sector,subSector+1,
                                                        station, srLUTset, TMB07);
      else
        srLUTs_[fpga++][1]   = new CSCSectorReceiverLUT(endcap,  sector,   0,
                                                        station, srLUTset, TMB07);
    }

  // negative endcap
  endcap = 2;
  for(int station=1,fpga=0; station<=4 && fpga<5; station++)
    {
      if(station==1)
        for(int subSector=0; subSector<2 && fpga<5; subSector++)
          srLUTs_[fpga++][0] = new CSCSectorReceiverLUT(endcap,sector,subSector+1,
                                                        station, srLUTset, TMB07);
      else
        srLUTs_[fpga++][0]   = new CSCSectorReceiverLUT(endcap,  sector,   0,
                                                        station, srLUTset, TMB07);
    }
  // -----------------------------------------------------------------------------

  //geom.reset(new GeometryTranslator());

}


NTupleMaker::~NTupleMaker()
{


}


//
// member functions
//



// ------------ method called for each event  ------------
void NTupleMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  



  if(printLevel>0) cout << "\n\n =================================== NEW EVENT ===================================== " << endl;


  bool tagAndProbeExist = false;

  
  //geom->checkAndUpdateGeometry(iSetup);

  // Get the CSC Geometry
  iSetup.get<MuonGeometryRecord>().get(cscGeom);

  summaryHandler_.initStruct();

  DataEvtSummary_t &ev = summaryHandler_.getEvent();


  //event header
  ev.run    = iEvent.id().run();
  ev.lumi   = iEvent.luminosityBlock();
  ev.event  = iEvent.id().event();
  
  // Legacy pT look up tables
  // Initialize CSCTF pT LUTs
  ESHandle< L1MuTriggerScales > scales;
  iSetup.get< L1MuTriggerScalesRcd >().get(scales);
  scale = scales.product();


  ESHandle< L1MuTriggerPtScale > ptscales;
  iSetup.get< L1MuTriggerPtScaleRcd >().get(ptscales);
  ptScale = ptscales.product();


  //ptLUTs_ = new CSCTFPtLUT(ptLUTset, scale, ptScale);
  // Standard Pt LUTs
  edm::ParameterSet ptLUTset;
  ptLUTset.addParameter<bool>("ReadPtLUT", false);
  ptLUTset.addParameter<bool>("isBinary",  false);
  CSCTFPtLUT ptLUT(ptLUTset, scale, ptScale);

  // =========================================================================================================

  if (printLevel > 0 )
    cout << "\n===================== FILLING All LCTs ========================\n"
         <<   "============================================================\n" << endl;
  

  edm::Handle<std::vector<l1t::EMTFHitExtra>> lcts;
  iEvent.getByToken(emtfTPTag_token, lcts);	 
  
  edm::Handle<std::vector<l1t::EMTFHitExtra>> RPC_lcts;
  iEvent.getByToken(emtfTPTagRPC_token, RPC_lcts);
  
  edm::Handle<CSCCorrelatedLCTDigiCollection> csctf_lcts;
  iEvent.getByToken(cscTPTag_token, csctf_lcts);

  // // Fill correlated_lcts from EMTF hits, rather than CSCTF(?) - AWB 15.06.16
  // CSCCorrelatedLCTDigiCollection correlated_lcts;
  // for( auto Lct = lcts->cbegin(); Lct < lcts->cend(); Lct++)
  //   if (Lct->Neighbor() == 0) correlated_lcts.insertDigi( (*Lct).CSC_DetId(), (*Lct).CSC_LCTDigi() );


  if ( lcts.isValid() ) {
    
    // Access the global phi/eta from here
    //std::vector<L1TMuon::TriggerPrimitive> LCT_collection;

    /*
    auto chamber = lcts->begin();
    auto chend  = lcts->end();
    for( ; chamber != chend; ++chamber ) {
      auto digi = (*chamber).second.first;
      edm::Handle<std::vector<l1t::EMTFTrackExtra> > tracks;      auto dend = (*chamber).second.second;
      for( ; digi != dend; ++digi ) {
	TriggerPrimitive trigPrimTemp = TriggerPrimitive((*chamber).first,*digi);
	//trigPrimTemp.setCMSGlobalPhi( geom->calculateGlobalPhi(trigPrimTemp) );
        //trigPrimTemp.setCMSGlobalEta( geom->calculateGlobalEta(trigPrimTemp) );
        LCT_collection.push_back(trigPrimTemp);
      }
    }
    */
    
    //fillAllLCTs(ev, LCT_collection, printLevel);
    fillAllLCTs(ev, lcts, printLevel);
  }
  
  else cout << "\t----->Invalid CSC LCT collection... skipping it\n";
  
  // RPCs
  if (printLevel > 0 )
    cout << "\n===================== FILLING All RPC LCTs ========================\n"
         <<   "============================================================\n" << endl;
  
  if ( RPC_lcts.isValid() ) 
    
    fillAllClusts(ev, RPC_lcts, printLevel);
  
  else cout << "\t----->Invalid RPC LCT collection... skipping it\n";

  // =========================================================================================================
  


  if (printLevel > 0 )
    
    cout << "\n===================== FILLING All EMTF Emulator Tracks ========================\n"
         <<   "=======================================================================\n" << endl;
  
  edm::Handle<std::vector<l1t::EMTFTrackExtra> > tracks;
  iEvent.getByToken(emtfTag_token, tracks);
  
  if ( tracks.isValid() )
    fillEMTFTracks(ev, tracks, printLevel);
  
  else cout << "\t----->Invalid EMTF Track collection... skipping it\n";
  


  // =========================================================================================================

  
  if (printLevel > 0 )

    cout << "\n===================== FILLING All EMTF Unpacked Tracks ========================\n"
         <<   "=======================================================================\n" << endl;

  edm::Handle<std::vector<l1t::EMTFTrack> > unpack_tracks;
  iEvent.getByToken(unp_emtfTag_token, unpack_tracks);

  if ( unpack_tracks.isValid() )
    fillUnpackedEMTFTracks(ev, unpack_tracks, printLevel);

  else cout << "\t----->Invalid EMTF Unpacked Track collection... skipping it\n";


 

  if (printLevel > 0 )
    cout << "\n===================== FILLING All Legacy CSCTF Tracks ========================\n"
         <<   "=======================================================================\n" << endl;

  // Following Thomas Reis' code in: 
  // https://github.com/thomreis/cmssw/blob/l1t-tsg-v2-patch1_bmtf-fix_uGMT-ntuple_cancelByQualOnly/L1Trigger/L1TNtuples/src/L1AnalysisGMT.cc
  
  // // Don't have legacy GMT info in 2016 data - AWB 02.06.16
  
  // // edm::Handle<L1MuGMTReadoutCollection> gmtReadoutCollection;
  // // iEvent.getByToken(leg_gmtTag_token, gmtReadoutCollection);
  
  
  // std::vector<L1MuGMTReadoutRecord> gmt_records = gmtReadoutCollection->getRecords();
  // std::vector<L1MuGMTReadoutRecord>::const_iterator iReadRec;
  // for(iReadRec = gmt_records.begin(); iReadRec != gmt_records.end(); iReadRec++) {
    
  //   std::vector<L1MuRegionalCand>::const_iterator iCSC;
  //   std::vector<L1MuRegionalCand> cscCands = iReadRec->getCSCCands();

  //   for(iCSC = cscCands.begin(); iCSC != cscCands.end(); iCSC++) {

  //   if ( ev.numCsctfTrks >= 3 || (*iCSC).empty() ) continue;

  //     ev.csctf_trkBx         -> push_back( (*iCSC).bx() );
  //     ev.csctf_trkPt         -> push_back( (*iCSC).ptValue() );
  //     ev.csctf_trkEta        -> push_back( (*iCSC).etaValue() );
  //     ev.csctf_trkPhi        -> push_back( (*iCSC).phiValue() );
  //     ev.csctf_trkQual       -> push_back( (*iCSC).quality() );
  //     ev.csctf_trkCharge     -> push_back( (*iCSC).chargeValid() ? (*iCSC).chargeValue() : 0 ); 
  //     ev.numCsctfTrks ++;
  //   } // End for(iCSC = cscCands.begin(); iCSC != cscCands.end(); iCSC++) 

  //   std::vector<L1MuGMTExtendedCand>::const_iterator iGMT;
  //   std::vector<L1MuGMTExtendedCand> gmtCands = iReadRec->getGMTCands();

  //   for(iGMT = gmtCands.begin(); iGMT != gmtCands.end(); iGMT++) {

  //     if ( ev.numGmtTrks >= 3 || (*iGMT).empty() ) continue;

  //     // Some of the central repository code has a rather different interpretation of the quality
  //     // https://github.com/cms-l1t-offline/cmssw/blob/l1t-muon-pass2-CMSSW_8_0_0_pre5/DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h
  //     // Quality codes:
  //     // 0 .. no muon 
  //     // 1 .. beam halo muon (CSC)
  //     // 2 .. very low quality level 1 (e.g. ignore in single and di-muon trigger)
  //     // 3 .. very low quality level 2 (e.g. ignore in single muon trigger use in di-muon trigger)
  //     // 4 .. very low quality level 3 (e.g. ignore in di-muon trigger, use in single-muon trigger)
  //     // 5 .. unmatched RPC 
  //     // 6 .. unmatched DT or CSC
  //     // 7 .. matched DT-RPC or CSC-RPC

  //     ev.gt_trkBx         -> push_back((*iGMT).bx());
  //     ev.gt_trkEta         -> push_back((*iGMT).etaValue());
  //     ev.gt_trkPhi         -> push_back((*iGMT).phiValue());
  //     ev.gt_trkPt         -> push_back((*iGMT).ptValue());
  //     ev.gt_trkQual       -> push_back((*iGMT).quality());
  //     ev.gt_trkDetector   -> push_back((*iGMT).detector());
  //     ev.numGtTrks ++;
      
  //     // Tracks only from endcap
  //     if ( ev.numGmtTrks >= 3 || (*iGMT).detector() < 4 ) continue;

  //     ev.gmt_trkBx      -> push_back((*iGMT).bx());
  //     ev.gmt_trkEta        -> push_back((*iGMT).etaValue());
  //     ev.gmt_trkPhi        -> push_back((*iGMT).phiValue()); 
  //     ev.gmt_trkPt         -> push_back((*iGMT).ptValue());
  //     ev.gmt_trkCharge     -> push_back( (*iGMT).charge_valid() ? (*iGMT).charge() : 0 );
  //     ev.gmt_trkQual       -> push_back((*iGMT).quality());
  //     ev.gmt_trkDetector   -> push_back((*iGMT).detector()); // 1=rpc, 2=dtbx, 4=csc, 3=rpc+dtbx, 5=rpc+csc
  //     ev.numGmtTrks ++;

  //   } // End for(iGMT = gmtCands.begin(); iGMT != gmtCands.end(); iGMT++)

  // } // End for(iReadRec = gmt_records.begin(); iReadRec != gmt_records.end(); iReadRec++)

  
  
  edm::Handle<vector<pair<csc::L1Track,MuonDigiCollection<CSCDetId,CSCCorrelatedLCTDigi> > >> leg_tracks;
  iEvent.getByToken(csctfTag_token, leg_tracks);

  if ( leg_tracks.isValid() && csctf_lcts.isValid() ) {

    int nTrks = 0;
    for(std::vector<std::pair<csc::L1Track,MuonDigiCollection<CSCDetId,CSCCorrelatedLCTDigi>>>::const_iterator lt = leg_tracks->begin();lt != leg_tracks->end();lt++){

      if (nTrks > MAXTRK-1) break;
      
      float eta = 0.9 + 0.05*(lt->first.eta_packed()) + 0.025;
      unsigned sector = lt->first.sector();
      float phi = (0.05217*lt->first.localPhi()) + (sector-1)*1.1 + 0.0218;//*(3.14159265359/180)
      if(phi > 3.14159) phi -= 6.28318;
      
      unsigned pti = 0, quality = 0;
      lt->first.decodeRank(lt->first.rank(),pti,quality);//
      float ptOld = ptscaleOld[pti+1];
      float ptMatt = ptscaleMatt[pti+1];
      float ptGmt = ptscaleMatt[pti];
      int qualA = quality;
      
      // PtAddress gives an handle on other parameters
      ptadd thePtAddress(lt->first.ptLUTAddress());
      
      //Pt needs some more workaround since it is not in the unpacked data
      ptdat thePtData  = ptLUT.Pt(thePtAddress);
      
      int pt_bit = -999;
      
      // front or rear bit?
      int qualB = -99;
      if (thePtAddress.track_fr) {
	pt_bit = thePtData.front_rank&0x1f;
	qualB = (thePtData.front_rank >> 5) & 0x3;
	//csctf_.trChargeValid.push_back(thePtData.charge_valid_front);
      } else {
	pt_bit = thePtData.rear_rank&0x1f;
	qualB = (thePtData.rear_rank>>5) & 0x3;
	//csctf_.trChargeValid.push_back(thePtData.charge_valid_rear);
      }

      // convert the Pt in human readable values (GeV/c)
      float pt = ptScale->getPtScale()->getLowEdge(pt_bit);
      
      // int qual = lt->first.cscMode();
      int qual = lt->first.mode();

      // For EMTF mode definition
      int mode = 0;
      if(lt->first.me1ID())
	mode |= 8;
      if(lt->first.me2ID())
	mode |= 4;
      if(lt->first.me3ID())
	mode |= 2;
      if(lt->first.me4ID())
	mode |= 1;
      
      int modeA = 0;
      if(lt->first.me1ID() > 0)
	modeA |= 8;
      if(lt->first.me2ID() > 0)
	modeA |= 4;
      if(lt->first.me3ID() > 0)
	modeA |= 2;
      if(lt->first.me4ID() > 0)
	modeA |= 1;
      
      int modeB = 0;

      if (printLevel > 0) {
	cout << "\n Legacy Track # " << nTrks << endl;
	cout << "============" << endl;
	cout << " Track Pt   : " << pt << endl;
	cout << " Track Eta  : " << eta << endl;
	cout << " Track Phi  : " << phi << endl;
	cout << " Track Mode : " << mode << endl;
      }
      
      ev.leg_trkPt   -> push_back(pt);
      ev.leg_trkPtOld   -> push_back(ptOld);
      ev.leg_trkPtMatt   -> push_back(ptMatt);
      ev.leg_trkPtGmt   -> push_back(ptGmt);
      ev.leg_trkEta  -> push_back(eta);
      ev.leg_trkPhi  -> push_back(phi);
      ev.leg_trkMode -> push_back(mode);
      ev.leg_trkModeA -> push_back(modeA);
      ev.leg_trkQual -> push_back(qual);
      ev.leg_trkQualA -> push_back(qualA);
      ev.leg_trkQualB -> push_back(qualB);
      ev.leg_trkBx -> push_back(lt->first.BX());


      // For each trk, get the list of its LCTs
      CSCCorrelatedLCTDigiCollection LCTs = lt -> second;

      std::vector<L1TMuon::TriggerPrimitive> LCT_collection;
      
      auto chamber = LCTs.begin();
      auto chend  = LCTs.end();
      for( ; chamber != chend; ++chamber ) {
      	auto digi = (*chamber).second.first;
      	auto dend = (*chamber).second.second;
      	for( ; digi != dend; ++digi ) {
      	  TriggerPrimitive trigPrimTemp = TriggerPrimitive((*chamber).first,*digi);
      	  // trigPrimTemp.setCMSGlobalPhi( geom->calculateGlobalPhi(trigPrimTemp) );
      	  // trigPrimTemp.setCMSGlobalEta( geom->calculateGlobalEta(trigPrimTemp) );
      	  LCT_collection.push_back(trigPrimTemp);
      	}
      }
      
      int LctTrkId_ = 0; // count number of lcts in event
      int lct_bx_beg = -99;
      int lct_bx_end = 99;
      
      auto Lct = LCT_collection.cbegin();
      auto Lctend = LCT_collection.cend();
      for( ; Lct != Lctend; Lct++) {

      	if (LctTrkId_ > MAXTRKHITS-1) break;
	
      	if(Lct->subsystem() != 1) continue;
	
      	if (printLevel>1) cout << "\n==== Legacy LCT CSC " << LctTrkId_ << endl;
	
      	CSCDetId id                = Lct->detId<CSCDetId>();
      	auto lct_station           = id.station();
      	auto lct_endcap            = id.endcap();
      	auto lct_chamber           = id.chamber();
      	int lct_bx                 = Lct->getCSCData().bx - 6; // Offset so center BX is at 0
      	if (lct_bx > lct_bx_beg) lct_bx_beg = lct_bx;
      	if (lct_bx < lct_bx_end) lct_bx_end = lct_bx;
      	int lct_ring               = id.ring();
      	int lct_sector             = CSCTriggerNumbering::triggerSectorFromLabels(id);
      	int lct_subSector          = CSCTriggerNumbering::triggerSubSectorFromLabels(id);
      	int lct_bx0                = Lct->getCSCData().bx0;
      	uint16_t lct_cscID         = Lct->getCSCData().cscID;
      	uint16_t lct_strip         = Lct->getCSCData().strip;
      	//uint16_t lct_pattern       = Lct->getCSCData().pattern;
      	uint16_t lct_bend          = Lct->getCSCData().bend;
      	uint16_t lct_quality       = Lct->getCSCData().quality;
      	uint16_t lct_keywire       = Lct->getCSCData().keywire;
	
      	double lct_phi             = Lct->getCMSGlobalPhi();
      	double lct_eta             = Lct->getCMSGlobalEta();
	
      	if(id.station() == 1)
      	  modeB |= 8;
      	if(id.station() == 2)
      	  modeB |= 4;
      	if(id.station() == 3)
      	  modeB |= 2;
      	if(id.station() == 4)
      	  modeB |= 1;
      
      	if ( printLevel > 0 ) {
      	  cout << "\n======\n";
      	  cout <<"lctEndcap       = " << lct_endcap << endl;
      	  cout <<"lctSector       = " << lct_sector<< endl;
      	  cout <<"lctSubSector    = " << lct_subSector << endl;
      	  cout <<"lctStation      = " << lct_station << endl;
      	  cout <<"lctRing         = " << lct_ring << endl;
      	  cout <<"lctChamber      = " << lct_chamber << endl;
      	  cout <<"lctTriggerCSCID = " << lct_cscID << endl;
      	  cout <<"lctBx           = " << lct_bx << endl;
      	  cout <<"lctBx0          = " << lct_bx0 << endl;
      	  cout <<"lctKeyWire      = " << lct_keywire << endl;
      	  cout <<"lctStrip        = " << lct_strip << endl;
      	  cout <<"lctBend         = " << lct_bend << endl;
      	  cout <<"lctQuality      = " << lct_quality << endl;
      	  cout <<"lct_globphi     = " << lct_phi << endl;
      	  cout <<"lct_globeta     = " << lct_eta << endl;
      	}
	
	
      	// Do not FIll array over their given size!!
      	if (nTrks > MAXTRK-1) {
      	  if (printLevel > 1) cout << "-----> nTrks is greater than MAXTRK-1.  Skipping this Track..." << endl;
      	  continue;
      	}
	
      	if (LctTrkId_ > MAXTRKHITS-1) {
      	  if (printLevel > 1)cout << "-----> LctTrkId_ is greater than MAXTRKHITS-1.  Skipping this Track..." << endl;
      	  continue;
      	}
	
	
      	ev.leg_trkLct_endcap[nTrks][LctTrkId_] = lct_endcap;
	
      	// sector (end 1: 1->6, end 2: 7 -> 12)
      	//if ( lct_endcap == 1)
      	ev.leg_trkLct_sector[nTrks][LctTrkId_] = lct_sector;
      	//else
      	//        lctSector[nTrk][LctTrkId_] = 6+lct_sector;
	
      	ev.leg_trkLct_station[nTrks][LctTrkId_] = lct_station;
	
      	ev.leg_trkLct_ring[nTrks][LctTrkId_] = lct_ring;
	
      	ev.leg_trkLct_chamber[nTrks][LctTrkId_] = lct_chamber;
	
      	ev.leg_trkLct_cscId[nTrks][LctTrkId_] = lct_cscID;
	
      	ev.leg_trkLct_wire[nTrks][LctTrkId_] = lct_keywire;
	
      	ev.leg_trkLct_strip[nTrks][LctTrkId_] = lct_strip;
	
      	ev.leg_trkLct_globPhi[nTrks][LctTrkId_] = lct_phi;
	
      	ev.leg_trkLct_eta[nTrks][LctTrkId_] = lct_eta;

      	if (lct_bx > 3) {
      	  std::cout << "Why is the legacy lct_bx = " << lct_bx << " ? Setting to -999." << std::endl;
      	  lct_bx = -999;
      	}
	
      	ev.leg_trkLct_bx[nTrks][LctTrkId_] = lct_bx;
	
      	LctTrkId_++;
	
      } // end track LCT loop


      
      ev.leg_trkModeB -> push_back(modeB);
      ev.leg_trkBxBeg -> push_back(lct_bx_beg);
      ev.leg_trkBxEnd -> push_back(lct_bx_end);
      ev.numLegTrkLCTs -> push_back(LctTrkId_);
      
      nTrks++;
      
    } // end legacy track loop
    
    ev.numLegTrks = nTrks;

    //fillLeg_CSCTFTracks(ev, leg_tracks, printLevel, srLUTs_, scale, ptScale);

  }
  else cout << "\t----->Invalid Legacy Track collection... skipping it\n";

  // =========================================================================================================
  

  
  

  if (isMC) {
    
    if (printLevel > 0 )
      cout << "\n===================== FILLING Gen Muons ========================\n"
	   <<   "================================================================\n" << endl;
    
    edm::Handle< vector<reco::GenParticle> > genParticles;
    iEvent.getByToken(genTag_token, genParticles);
    
    if ( genParticles.isValid() )
      fillGenMuons(ev, genParticles, printLevel);
    
    else cout << "\t----->Invalid Track collection... skipping it\n";
  }
  
  
  if (!isMC) {
    
    if(printLevel>0) 
      cout << "\n===================== FILLING RECO Muons ========================\n"
	   <<   "================================================================\n" << endl;
    
    
    edm::Handle<reco::MuonCollection> muons;
    iEvent.getByToken(muons_token, muons);
    edm::Handle<std::vector<reco::Vertex>> vertices;
    iEvent.getByToken(vertex_token, vertices);
    
    //edm::Handle<reco::BeamSpot> beamSpot;
    //iEvent.getByLabel("offlineBeamSpot", beamSpot);
    
    if ( muons.isValid() )
      tagAndProbeExist = fillRecoMuons(ev, muons, vertices, printLevel);
    
    // else cout << "\t----->Invalid RECO Muon collection... skipping it\n";
    
    
    
    if(printLevel>0)
      cout << "\n===================== FILLING RECO Muons Segments ========================\n"
           <<   "================================================================\n" << endl;
    
    edm::Handle<CSCSegmentCollection> cscSegments;
    iEvent.getByToken(cscSegs_token, cscSegments);
    
    
    // if ( cscSegments.isValid() && correlated_lcts.isValid())
    if ( cscSegments.isValid() && lcts.isValid() )
      fillSegmentsMuons(ev, muons, cscSegments, cscGeom, lcts, printLevel);
    // leaving out csc tracks for now.  Add back in later
    
    // else cout << "\t----->Invalid RECO Muon SEGMENT collection... skipping it\n";

  }
  
  // ============================================================================



								       
  // =========================================================================
  // End Event methods

  // // Fill 4518, run 259626, 589 colliding bunches, pileup 17 - 18, 4x 430 Hz
  // "259626": [[83, 111], [113, 167], [169, 437]]
  // // Fill 4525, run 259721,     517 colliding bunches, pileup 21 - 24, 4x 230 Hz
  // "259721": [[73, 99], [102, 408]]
  // // Fill 4569, run 260627 recorded 11 hours of data at 3.8 Tesla
  // "260627": [[97, 611], [613, 757], [760, 788], [791, 1051], [1054, 1530], [1533, 1845]]

  // Fill the tree
  if (NoTagAndProbe || tagAndProbeExist)
    // if ( ( iEvent.id().run() == 259626 && ( LS(ev.lumi, 83, 111) || LS(ev.lumi, 113, 167) || LS(ev.lumi, 169, 437) ) ) ||
    // 	 ( iEvent.id().run() == 259721 && ( LS(ev.lumi, 73, 99) || LS(ev.lumi, 102, 408) ) ) ||
    // 	 ( iEvent.id().run() == 260627 && ( LS(ev.lumi, 97, 611) || LS(ev.lumi, 613, 757) || LS(ev.lumi, 760, 788) || LS(ev.lumi, 791, 1051) || LS(ev.lumi, 1054, 1530) || LS(ev.lumi, 1533, 1845) ) ) )
    summaryHandler_.fillTree();
  
  // Clear all objects from memory
  summaryHandler_.resetStruct();

  
    
} // end analyze



// ------------ method called once each job just before starting event loop  ------------
void
NTupleMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
NTupleMaker::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
/*
void
NTupleMaker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void
NTupleMaker::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void
NTupleMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
NTupleMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

 // Mask certain lumisections
bool NTupleMaker::LS(int lumi_section, int beg_ls, int end_ls) {
  if ( lumi_section >= beg_ls && lumi_section <= end_ls)
    return true;
  else
    return false;
}

// Cluster sector must be determined manually.
// Sometimes the rpc sector and the cluster sector are not the same
int
NTupleMaker::convertRPCsectors(float rpcphi)
{
    if (     rpcphi >= 15.*M_PI/180. && rpcphi < 75.*M_PI/180.)
        return 1;
    else if (rpcphi >= 75.*M_PI/180. && rpcphi < 135.*M_PI/180.)
        return 2;
    else if (rpcphi >= 135.*M_PI/180. && rpcphi < 195.*M_PI/180.)
        return 3;
    else if (rpcphi >= 195.*M_PI/180. && rpcphi < 255.*M_PI/180.)
        return 4;
    else if (rpcphi >= 255.*M_PI/180. && rpcphi < 315.*M_PI/180.)
        return 5;
    else if (rpcphi >= 315.*M_PI/180. || rpcphi < 15.*M_PI/180.)
        return 6;
    else
        return -1;
}


// convert RPC global phi to CSC LUT phi Bits, only for RPC
int
NTupleMaker::convertRPCphiBits(float GblPhi, int sector)
{
    float phiBit = -999;
    if (sector == 1)
        phiBit = (GblPhi - 0.243) / 1.0835;
    else if (sector == 2)
        phiBit = (GblPhi - 1.2914) / 1.0835;
    else if (sector == 3) {
        if (GblPhi > 0) phiBit = (GblPhi - 2.338) / 1.0835;
        else {
            float sector_distance = abs(GblPhi + 3.1416) + (3.1416 - 2.338);
            phiBit = sector_distance / 1.0835;
        }
    } else if (sector == 4)
        phiBit = (GblPhi + 2.898) / 1.0835;
    else if (sector == 5)
        phiBit = (GblPhi + 1.8507) / 1.0835;
    else if (sector == 6) {
        if (GblPhi < 0) phiBit = (GblPhi + 0.803) / 1.0835;
        else {
            float sector_distance = GblPhi + 0.803;
            phiBit = sector_distance / 1.0835;
        }
    }

    phiBit = phiBit*4096;

    return static_cast<int>(phiBit);
}


// convert global Eta to Eta Bit for pT assignment, for both CSC and RPC
int
NTupleMaker::convertRPCetaBits(float GblEta)
{
    double theEtaBinning = (CSCTFConstants::maxEta - CSCTFConstants::minEta)/(CSCTFConstants::etaBins);
    int theEta_ = (GblEta-CSCTFConstants::minEta)/theEtaBinning;
    //theEta_*theEtaBinning + CSCTFConstants::minEta
    return theEta_;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
NTupleMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(NTupleMaker);
