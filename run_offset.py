# PYTHON configuration file for class: OffsetTreeMaker
# Author: C. Harrington
# Date:  19 - January - 2015

import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource", fileNames = readFiles)
readFiles.extend( [
  '/store/mc/RunIISummer19UL18RECO/SingleNeutrino/AODSIM/FlatPU0to70_106X_upgrade2018_realistic_v11_L1v1-v1/40000/FFBAA035-9896-174A-9477-FFBA1DE40542.root'
] );

isMC = cms.bool(True)

if isMC:
  OutputName = "_MC_UL2018"
  eraName = "Autumn18_V19_MC"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '106X_upgrade2018_realistic_v11_L1v1' )

else:
  run = "C"
  OutputName = "_Data_UL2018"+run
  #eraName = "Autumn18_Run"+run+"_V19_DATA"
  #jetType_name = "AK4PFchs" # or "AK4PF"

  #process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  #process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  #process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff" )
  #from Configuration.AlCa.GlobalTag import GlobalTag
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v24' )

  # ZeroBias Trigger
  process.HLTZeroBias =cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths = cms.vstring('HLT_ZeroBias_part*','HLT_ZeroBias_v*'),
    eventSetupPathsKey = cms.string(''),
    andOr = cms.bool(True), #----- True = OR, False = AND between the HLTPaths
    throw = cms.bool(False)
  )

  #Beam Halo
  process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

  #HCAL HBHE
  process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
  process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
  process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResultRun2Tight'),
    reverseDecision = cms.bool(False)
  )

process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip = cms.int32(5),
    RootFileName = cms.string("Offset" + OutputName + ".root"),
    puFileName = cms.string("pileup_2018.txt"),
    jetVetoMapFileName = cms.string("hotjets-UL18.root"),
    mapName = cms.string("h2hot_ul18_plus_hem1516_and_hbp2m1"),
    isMC = isMC,
    writeCands = cms.bool(False),
    trackTag = cms.InputTag("generalTracks"),
    pfTag = cms.InputTag("particleFlow"),
    pvTag = cms.InputTag("offlinePrimaryVertices"),
    muTag = cms.InputTag("addPileupInfo"),
    rhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
    rhoCTag = cms.InputTag("fixedGridRhoFastjetAll"),
    rhoC0Tag = cms.InputTag("fixedGridRhoFastjetCentralNeutral"),
    rhoCCTag = cms.InputTag("fixedGridRhoFastjetCentralChargedPileUp"),
    pfJetTag = cms.InputTag("ak4PFJetsCHS"),
    era = cms.string(eraName),
    jet_type = cms.string(jetType_name),
    doL1L2L3Res = cms.bool(True),
    dojetVetoMap = cms.bool(True)
    #miniAOD
    #trackTag = cms.InputTag("lostTracks"),
    #pfTag = cms.InputTag("packedPFCandidates"),
    #pvTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
    #muTag = cms.InputTag("slimmedAddPileupInfo"),
    #pfJetTag = cms.InputTag("slimmedJets")
)

process.myseq = cms.Sequence( process.pf )

if isMC :
  process.p = cms.Path( process.myseq )
else:
  process.p = cms.Path( process.HLTZeroBias * 
                        process.CSCTightHaloFilter *
                        process.HBHENoiseFilterResultProducer *
                        process.ApplyBaselineHBHENoiseFilter *
                        process.myseq )
