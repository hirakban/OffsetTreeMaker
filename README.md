# Calculating L1Offset scale factor

In this analyzer, L1RC Jet Energy Corrections(JECs) are derived using root tuples.

For more information on L1Offset see Section 4 of https://iopscience.iop.org/article/10.1088/1748-0221/12/02/P02014/pdf .

![JECs_image](https://github.com/garvitaa/OffsetTreeMaker/blob/master/JERC.png)

## Step 0: Setup
Find the CMSSW release for your dataset. eg. CMSSW_11_2_0_pre1. Now, we set up the work area: 

     cmsrel CMSSW_11_2_0_pre1
     cd CMSSW_11_2_0_pre1/src
     cmsenv
     mkdir test
     cd test
     git clone https://github.com/garvitaa/OffsetTreeMaker.git
     cd OffsetTreeMaker
     scram b
     cmsenv
     voms-proxy-init -voms cms
     
## Step 1: Create root tuples with OffsetTreeMaker.cc
Here we are running the framework /plugins/OffsetTreeMaker.cc using the configuration file run_offset.py . The cmsRun command executes the .py file and gives us an output root file named **_Offset_Data.root_** or _**Offset_MC.root**_ depending upon the choice we made in the **isMC** statement. This runs the code interactively.

Another option is to copy a segment of data locally to test the file. This can be done using the code copy_cfg.py or xrdcp command.

### For Data:
1. Extract corresponding pileup JSON from this site: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/    
2. Using the JSONtoASCII.py convert the json file(pileup_latest.txt) to ascii (pileup_20**XX**.txt) and move a copy of 'pileup_20**XX**.txt' to /plugins.
     ```
     > python JSONtoASCII.py pileup_latest.txt >pileup_20**XX**.txt
     > cp pileup_20**XX**.txt ./plugins
     ```
3. Modify plugins/parsePileUpJSON2.h to include the new pileup file.
     ```
     int parsePileUpJSON2(string filename="pileup_20**XX**.txt")
     ```
4. Modify run_offset.py
     ```
     isMC = cms.bool(False)
     ```
     ```
     readFiles.extend( ['/store/data/__*root file from DAS for your dataset*__.root' ] );
     ```
     ```
     process.GlobalTag = GlobalTag( process.GlobalTag, '__*Global Tag for your dataset from DAS*__' )
     ```
     ```
     puFileName = cms.string("pileup_20**XX**.txt"),
     ```  
5. Execute the code.
     ```
     > cmsRun run_offset.py
     ```
     
### For MC:
**Note: Pileup information is not needed to process the MC, the code still requires the file to run.**

Perform the steps 1, 2, 3, 4 and 5 as given above for data with one modification in Step 4.
     ```
     isMC = cms.bool(True)
     ```
     
     
## Step 2: Submitting CRAB jobs.
To run on entire dataset locally is expensive and time consuming, so we submit CRAB jobs.

### For Data:
Modify crab_run_offset.py
1. Include the new pileup file:
     ```
     config.JobType.inputFiles = ["pileup_20**XX**.txt"]
     ```
2. Add the outputFiles depending upon the name of the output root file from run_offset.py
     ```
     config.JobType.outputFiles = ["Offset_Data.root"]
     ```
3. Add the name of dataset we are analysing 
     ```
     config.Data.inputDataset = '/ZeroBias/--*Dataset name from DAS*--/RECO'
     ```
4. To split the task in jobs, we use Lumibased method for data. 
     ```
     config.Data.splitting = 'LumiBased'
     ```
5. Apply lumi-mask to the input dataset analysis. The golden-json file can be found at: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/ Download the certificate into the present working directory.
     ```
     config.Data.lumiMask = 'Cert_--*golden json file*--.txt'
     ```
6. The last modification is the path to the storage site. 
     ```
     config.Data.outLFNDirBase = '/store/user/--*username*--'
     ```
### For MC:
We make 3 modifications to submit CRAB jobs for MC. Perform the steps as given above for data with modifications in step 2, 3, 4, 5.

2. Change the outputFiles depending upon the name of the output root file from run_offset.py
     ```
     config.JobType.outputFiles = ["Offset_MC.root"]
     ```
3. Change the name of dataset we are analysing .
     ```
     config.Data.inputDataset = '/SingleNeutrino/--*Dataset name from DAS*--/AODSIM'
     ```
4. To split the task in jobs, we use FileBased method for MC. 
     ```
     config.Data.splitting = 'FileBased'
     ```
5. Comment out the lumiMask statement.


To modify the environment for running crab job we use the following commands:
```
> cmsenv
> source /cvmfs/cms.cern.ch/crab3/crab.csh
> voms-proxy-init -voms cms
```
Submit CRAB jobs:
```
> crab submit crab_run_offset.py
```
Once CRAB job finishes, add the .root files using the following command:
```
> hadd Total_Data.root Offset_Data*.root
OR
> hadd Total_MC.root Offset_MCa*.root
```

**_NOTE:_** To calculate the integrated luminosity for the datasets use BrilCalc on lxplus. Look at Appendix A for instructions.

## Step 3: Processing the created nTuples using histomaker.cc 
Once we have the output files for both data and MC we can process them using the histomaker.cc

The value 0.4 or 0.8 is the distance parameter(R) for anti-kT jet clustering. 

### For data:
     ```
     nohup histomaker false 0.4(0.8) Total_Data.root 
     ```
### for MC:
We are reweighting MC with respect to the data file : Total_Data.root 
    ```
    nohup histomaker true 0.4(0.8) Total_Data.root  Total_MC.root  (Total_Data.root is the file that you reweight wrt)
    ```
    
## Step4:
    pick a range of pileup to process samples:
    e.g. (10,30)
    
    a) root -l -b -q 'offsetpT.c (10,30,30,30)'
      (pick all and chs)
    b) root -l -b -q 'scalefactor.c ("all",10,30)' and root -l -b -q 'scalefactor.c ("chs",10,30)' 
    c) root -l -b -q 'l1fastjet_adapted.c("all")' and root -l -b -q 'l1fastjet_adapted.c("chs")'
## Step5:
 for stack plots use offsetpT_stack.c and implement using following:
     
     a) ./run_stack.sh <Run Era> <Luminosity*>
