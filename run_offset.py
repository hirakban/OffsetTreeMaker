# PYTHON configuration file for class: OffsetTreeMaker
# Author: C. Harrington
# Date:  19 - January - 2015

import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource", fileNames = readFiles)
readFiles.extend( [
  '/store/mc/Run3Summer23DR/SingleNeutrino_E-10_gun/AODSIM/FlatPU0to100_130X_mcRun3_2023_realistic_v15_ext1-v3/2550000/2fd4ab3f-2b10-45ca-a323-ac67ed3983d6.root'
] );

isMC = cms.bool(True)

if isMC:
  OutputName = "_MC_Run3Summer23"           
#  eraName = "Summer20UL18_V2_MC"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '132X_mcRun3_2023_realistic_v5' )


else:
  run = "D"
  OutputName = "_Run2_Data_UL2018"+run

#  eraName = "Winter22Run3"+"_RunD"+"_V2_DATA"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v26' )

  # ZeroBias Trigger
  process.HLTZeroBias =cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths = cms.vstring('HLT_ZeroBias_part*','HLT_ZeroBias_v*'),
    #HLTPaths = cms.vstring('HLT_ZeroBias_v*'),
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
    numSkip = cms.int32(3),                                        
    RootFileName = cms.string("Offset" + OutputName + ".root"),
    puFileName = cms.string("pileup_2023_new.txt"),
    jetVetoMapFileName = cms.string("Vetomap_Summer23Prompt23_RunC_v1.root"),       
    mapName2 = cms.string("jetvetomap_all"),   
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
    #era = cms.string(eraName),
    jet_type = cms.string(jetType_name),
    doL1L2L3Res = cms.bool(False),
    dojetVetoMap = cms.bool(True),
    writeEnergyDeposition = cms.bool(True)
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
