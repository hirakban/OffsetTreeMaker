from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'OffsetTreeMaker_Run3_DATA_Run2023C-PromptReco-v4'
config.General.workArea = 'crab_projects/Data_with_depth/Run3_DATA_Run2023C-PromptReco-v4'
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_offset.py'
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ["Fall15_25nsV2_MC.db"]
config.JobType.inputFiles = ["pileup_2023.txt", "vetomaps_Summer23Prompt23_RunBC_v1.root"]
config.JobType.outputFiles = ["Offset_Run3_Data_2023_RunC_v4.root"]

config.section_("Data")
#config.Data.allowNonValidInputDataset = True
config.Data.inputDataset = '/ZeroBias/Run2023C-PromptReco-v4/AOD'

# 2022_RunC_rereco: dataset=/ZeroBias/Run2022C-27Jun2023-v1/AOD
# 2022_RunD_rereco: dataset=/ZeroBias/Run2022D-27Jun2023-v1/AOD
# 2022_RunE_rereco: dataset=/ZeroBias/Run2022E-27Jun2023-v1/AOD
# 2022_RunF_promptreco: dataset=/ZeroBias/Run2022F-PromptReco-v1/AOD
# 2022_RunG_promptreco: dataset=/ZeroBias/Run2022G-PromptReco-v1/AOD

# 2023_RunC_promptreco_v1 : dataset=/ZeroBias/Run2023C-PromptReco-v1/AOD
# 2023_RunC_promptreco_v2 : dataset=/ZeroBias/Run2023C-PromptReco-v2/AOD
# 2023_RunC_promptreco_v3 : dataset=/ZeroBias/Run2023C-PromptReco-v3/AOD
# 2023_RunC_promptreco_v4 : dataset=/ZeroBias/Run2023C-PromptReco-v4/AOD

# 2023_RunD_promptreco_v1 : dataset=/ZeroBias/Run2023D-PromptReco-v1/AOD
# 2023_RunD_promptreco_v2 : dataset=/ZeroBias/Run2023D-PromptReco-v2/AOD

config.Data.splitting = 'LumiBased' #FileBased
config.Data.lumiMask = 'Cert_Collisions2023_eraC_367095_368823_Golden.txt'
config.Data.unitsPerJob = 100
config.Data.outLFNDirBase = '/store/user/hbandyop/Run3_L1Offset/Data_with_depth/Run2023C-PromptReco-v4'
config.Data.publication = False
#config.Data.ignoreLocality = True
#config.Data.publishDataName = 'offset_analysis'

config.section_("Site")
#config.Site.blacklist = ['T1_US_FNAL']
#config.Site.whitelist = ['T2_CH_CERN']
config.Site.storageSite = "T3_US_FNALLPC"

# source /cvmfs/cms.cern.ch/crab3/crab.sh
