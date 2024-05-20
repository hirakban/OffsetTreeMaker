from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")                        
config.General.requestName = 'OffsetTreeMaker_Run3_MC_Run3Summer22DR-FlatPU0to100-Full'
config.General.workArea = 'crab_projects/MC_with_depth/Run3Summer22DR-FlatPU0to100-Full'
config.General.transferLogs = True
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_offset.py'
#config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2022.txt","Vetomap_Summer22_23Sep2023_RunCD_v1.root"]   
config.JobType.outputFiles = ["Offset_MC_Run3Summer22.root"]        

config.section_("Data")
#config.Data.allowNonValidInputDataset = True                        
config.Data.inputDataset = '/SingleNeutrino_E-10_gun/Run3Summer22DR-FlatPU0to100_124X_mcRun3_2022_realistic_v12_ext1-v2/AODSIM'

config.Data.splitting = 'FileBased' # LumiBased
#config.Data.lumiMask = 'Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt'
config.Data.unitsPerJob = 10
config.Data.outLFNDirBase = '/store/user/hbandyop/Run3_L1Offset/MC_New/Summer22'               
config.Data.publication = False
config.Data.ignoreLocality = True
#config.Data.publishDataName = 'offset_analysis'

config.section_("Site")
#config.Site.blacklist = ['T1_US_FNAL']
#config.Site.whitelist = ['T2_CH_CERN']
config.Site.whitelist = ["T2_*","T3_*"]
config.Site.storageSite = "T3_US_FNALLPC"

# source /cvmfs/cms.cern.ch/crab3/crab.sh
