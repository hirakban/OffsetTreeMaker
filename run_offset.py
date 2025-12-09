# PYTHON configuration file for class: OffsetTreeMaker
# Author: H. Bandyopadhyay
# Date:  12 - June - 2022

import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource", fileNames = readFiles
                  , inputCommands=cms.untracked.vstring('keep *',
                  'drop floatBXVector_gtStage2Digis_CICADAScore_RECO')
                 )

readFiles.extend( [
  '/store/data/Run2025D/ZeroBias/AOD/PromptReco-v1/000/394/391/00000/0a06e3f3-5730-481c-808b-65d045cdff27.root'
#   '/store/mc/Run3Winter25Reco/SingleNeutrino_E-10_gun/AODSIM/142X_mcRun3_2025_realistic_v9-v2/120000/01bdbd7c-4a4c-45e6-a625-243df1901612.root'
] );

isMC = cms.bool(True)

if isMC:
  OutputName = "_MC_Run3Winter25Reco-new"           # change
#  eraName = "Summer20UL18_V2_MC"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
#  process.GlobalTag = GlobalTag( process.GlobalTag, '140X_mcRun3_2024_realistic_v26' )
  process.GlobalTag = GlobalTag( process.GlobalTag, '150X_mcRun3_2025_realistic_v6' )

else:
  run = "2025_RunC-v2"
  OutputName = "_Run3_Data_"+run        # change

#  eraName = "Winter22Run3"+"_RunD"+"_V2_DATA"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )

  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '150X_dataRun3_Prompt_v1' )       # change
#  process.GlobalTag = GlobalTag( process.GlobalTag, '140X_dataRun3_v20' )

  # ZeroBias Trigger
  process.HLTZeroBias =cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    #HLTPaths = cms.vstring('HLT_ZeroBias_part*','HLT_ZeroBias_v*'),
    HLTPaths = cms.vstring('HLT_ZeroBias_v*'),
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
    numSkip = cms.int32(7),                                        # change num, pufile (parsepileup also), vetomap
    RootFileName = cms.string("Offset" + OutputName + ".root"),
#    puFileName = cms.string("pileup_2025_0520.txt"),
    puFileName = cms.string("pileup_391658_398427_1023_2025.txt"),
    jetVetoMapFileName = cms.string("jetveto2025CDEF_V3M.root"),       
#    pubxFileName = cms.string("lumi-per-bx_Run2024I-part1.root"),
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
