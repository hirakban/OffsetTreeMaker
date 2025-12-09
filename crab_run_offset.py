from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
#config.General.requestName = 'OffsetTreeMaker_Data_Run2025C-promptreco-v2'      #change
#config.General.workArea = 'crab_projects/Data/Run2025C-promptreco-v2'       #change
config.General.requestName = 'OffsetTreeMaker_MC_Run3Winter25Reco'      #change
config.General.workArea = 'crab_projects/MC/Run3Winter25Reco'       #change
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_offset.py'
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ["Fall15_25nsV2_MC.db"]
config.JobType.inputFiles = ["pileup_391658_398427_1023_2025.txt", "jetveto2025CDEF_V3M.root"]                 #change
config.JobType.outputFiles = ["Offset_MC_Run3Winter25Reco-new.root"]                                   #change
#config.JobType.outputFiles = ["Offset_Run3_Data_2025_RunC-v2.root"]                                   #change

config.section_("Data")
config.Data.allowNonValidInputDataset = True
config.Data.inputDataset = '/SingleNeutrino_E-10_gun/Run3Winter25Reco-142X_mcRun3_2025_realistic_v9-v3/AODSIM'     #change

#MC datasets
# Summer23: dataset=/SingleNeutrino_E-10_gun/Run3Summer23DR-FlatPU0to100_130X_mcRun3_2023_realistic_v15_ext1-v4/AODSIM
# Summer23: dataset=/SingleNeutrino_E-10_gun/Run3Summer23DR-FlatPU0to100_130X_mcRun3_2023_realistic_v15_ext1-v6/AODSIM
# Summer23BPix: dataset=/SingleNeutrino_E-10_gun/Run3Summer23BPixDR-FlatPU0to100_130X_mcRun3_2023_realistic_postBPix_v6_ext1-v3/AODSIM
# Winter24: dataset=/SingleNeutrino_E-10-gun/Run3Winter24Reco-133X_mcRun3_2024_realistic_v10-v2/AODSIM
# Summer24DRPremix: dataset=/SingleNeutrino_Par-E-10_gun/RunIII2024Summer24DRPremix-140X_mcRun3_2024_realistic_v26-v2/AODSIM
# Winter25Reco: dataset=/SingleNeutrino_E-10_gun/Run3Winter25Reco-142X_mcRun3_2025_realistic_v9-v2/AODSIM
# Winter25Reco: dataset=/SingleNeutrino_E-10_gun/Run3Winter25Reco-142X_mcRun3_2025_realistic_v9-v3/AODSIM

#ZeroBias data
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

# 2024_RunA_promptreco_v1 : dataset=/ZeroBias/Run2024A-PromptReco-v1/AOD
# 2024_RunB_promptreco_v1 : dataset=/ZeroBias/Run2024B-PromptReco-v1/AOD
# 2024_RunC_promptreco_v1 : dataset=/ZeroBias/Run2024C-PromptReco-v1/AOD
# 2024_RunD_promptreco_v1 : dataset=/ZeroBias/Run2024D-PromptReco-v1/AOD
# 2024_RunE_promptreco_v1 : dataset=/ZeroBias/Run2024E-PromptReco-v1/AOD
# 2024_RunE_promptreco_v2 : dataset=/ZeroBias/Run2024E-PromptReco-v2/AOD
# 2024_RunF_promptreco_v1 : dataset=/ZeroBias/Run2024F-PromptReco-v1/AOD
# 2024_RunG_promptreco_v1 : dataset=/ZeroBias/Run2024G-PromptReco-v1/AOD
# 2024_RunH_promptreco_v1 : dataset=/ZeroBias/Run2024H-PromptReco-v1/AOD
# 2024_RunI_promptreco_v1 : dataset=/ZeroBias/Run2024I-PromptReco-v1/AOD
# 2024_RunI_promptreco_v2 : dataset=/ZeroBias/Run2024I-PromptReco-v2/AOD

# 2024_RunC_2024CDEReprocessing : dataset=/ZeroBias/Run2024C-2024CDEReprocessing-v1/AOD
# 2024_RunD_2024CDEReprocessing : dataset=/ZeroBias/Run2024D-2024CDEReprocessing-v1/AOD
# 2024_RunE_2024CDEReprocessing : dataset=/ZeroBias/Run2024E-2024CDEReprocessing-v1/AOD

# 2025_RunB_promptreco_v1 : dataset=/ZeroBias/Run2025B-PromptReco-v1/AOD
# 2025_RunC_promptreco_v1 : dataset=/ZeroBias/Run2025C-PromptReco-v1/AOD
# 2025_RunC_promptreco_v2 : dataset=/ZeroBias/Run2025C-PromptReco-v2/AOD
# 2025_RunD_promptreco_v1 : dataset=/ZeroBias/Run2025D-PromptReco-v1/AOD
# 2025_RunE_promptreco_v1 : dataset=/ZeroBias/Run2025E-PromptReco-v1/AOD
# 2025_RunF_promptreco_v1 : dataset=/ZeroBias/Run2025F-PromptReco-v1/AOD
# 2025_RunF_promptreco_v2 : dataset=/ZeroBias/Run2025F-PromptReco-v2/AOD
# 2025_RunG_promptreco_v1 : dataset=/ZeroBias/Run2025G-PromptReco-v1/AOD

config.Data.splitting = 'FileBased' #FileBased #LumiBased
#config.Data.lumiMask = 'Cert_Collisions2025_391658_397778_Golden_plus_397779_398310_ml.json'                            #change
config.Data.unitsPerJob = 50
#config.Data.outLFNDirBase = '/store/user/hbandyop/Run3_L1Offset/Data_with_depth/Run2025C-promptreco-v2-trial2'     #change
config.Data.outLFNDirBase = '/store/user/hbandyop/Run3_L1Offset/MC_New/Run3Winter25Reco-new'             #change
config.Data.publication = False
#config.Data.ignoreLocality = True
#config.Data.publishDataName = 'offset_analysis'

config.section_("Site")
#config.Site.blacklist = ['T1_US_FNAL']
#config.Site.whitelist = ['T2_FR_IPHC']
config.Site.storageSite = "T3_US_FNALLPC"

# source /cvmfs/cms.cern.ch/crab3/crab.sh
