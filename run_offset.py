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
# '/store/data/Run2022C/ZeroBias/AOD/27Jun2023-v1/2820000/0138650f-11ec-401a-bb88-20c87aa0619c.root'
# '/store/data/Run2022D/ZeroBias/AOD/27Jun2023-v1/2830000/b944540d-d254-48f2-87b2-d2b19062736f.root' 
# '/store/data/Run2022E/ZeroBias/AOD/27Jun2023-v1/2810000/7c703acf-2195-4823-a33a-7dc912c32609.root'
# '/store/data/Run2022F/ZeroBias/AOD/PromptReco-v1/000/360/393/00000/67126efe-5df8-4231-affc-9b6174e79547.root'
# '/store/data/Run2022G/ZeroBias/AOD/PromptReco-v1/000/362/439/00000/41d9f7f2-d0a7-44f3-8b25-6a3a1c859bec.root'
 '/store/data/Run2023C/ZeroBias/AOD/PromptReco-v1/000/367/095/00000/376d252a-3cd0-4919-a629-d074be68fc05.root'
# '/store/data/Run2023D/ZeroBias/AOD/PromptReco-v1/000/369/869/00000/a3832f72-9dbc-4b16-8ade-68e02ecc0797.root'
] );

isMC = cms.bool(False)

if isMC:
  OutputName = "_MC_Summer20UL18"
  eraName = "Summer20UL18_V2_MC"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '106X_upgrade2018_realistic_v16_L1v1' )
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_mcRun2_asymptotic_preVFP_v8' )


else:
  run = "2023_RunC"
  OutputName = "_Run3_Data_"+run+"_v4"
#  eraName = "Winter22Run3"+"_RunD"+"_V2_DATA"
  jetType_name = "AK4PFchs" # or "AK4PF"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '130X_dataRun3_Prompt_v3' )

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
    numSkip = cms.int32(23),
    RootFileName = cms.string("Offset" + OutputName + ".root"),
    puFileName = cms.string("pileup_2023.txt"),
    jetVetoMapFileName = cms.string("vetomaps_Summer23Prompt23_RunBC_v1.root"),
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
