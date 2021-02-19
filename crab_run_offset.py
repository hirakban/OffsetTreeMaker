from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'offset_treemaker_MC_UL2016_FlatPU0to75_106X_preVFP_v8'
config.General.workArea = 'crab_projects/MC_UL2016_FlatPU0to75_106X_preVFP_v8'
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_offset.py'
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ["Fall15_25nsV2_MC.db"]
config.JobType.inputFiles = ["pileup_2016.txt", "Summer19UL18_V5_MC", "hotjets-UL16.root"]
config.JobType.outputFiles = ["Offset_MC_UL2016_FlatPU0to75_106X_preVFP_v8.root"]

config.section_("Data")
#config.Data.allowNonValidInputDataset = True
config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL16RECOAPV-FlatPU0to75_106X_mcRun2_asymptotic_preVFP_v8_ext2-v2/AODSIM'
 # '/SingleNeutrino/RunIISummer19UL16RECO-FlatPU0to70_106X_mcRun2_asymptotic_v13-v3/AODSIM'
config.Data.splitting = 'FileBased' #LumiBased 
#config.Data.lumiMask = 'Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt'
config.Data.unitsPerJob = 2
config.Data.outLFNDirBase = '/store/user/hbandyop/l1offsetUL16_result/New_MC_Samples/MC_UL2016_FlatPU0to75_106X_preVFP_v8'
config.Data.publication = False
#config.Data.ignoreLocality = True
#config.Data.publishDataName = 'offset_analysis'

config.section_("Site")
#config.Site.blacklist = ['T1_US_FNAL']
#config.Site.whitelist = ['T2_CH_CERN']
config.Site.storageSite = "T3_US_FNALLPC"

# source /cvmfs/cms.cern.ch/crab3/crab.csh
