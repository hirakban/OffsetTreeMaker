# Calculating L1Offset scale factor

In this analyzer, L1RC Jet Energy Corrections(JECs) are derived using root tuples. The documentaion of all available files and their purpose can be found in [Appendix B](https://github.com/garvitaa/OffsetTreeMaker#appendix-b).

For more information on L1Offset see Section 4 of https://iopscience.iop.org/article/10.1088/1748-0221/12/02/P02014/pdf .

![JECs_image](https://github.com/garvitaa/OffsetTreeMaker/blob/master/JERC.png)

## Step 0: Setup
Find the CMSSW release for your dataset. eg. CMSSW_10_6_2. Now, we set up the work area.

```console
cmsrel CMSSW_10_6_2
cd CMSSW_10_6_2/src
cmsenv
mkdir test
cd test
git clone https://github.com/garvitaa/OffsetTreeMaker.git
cd OffsetTreeMaker
scram b
cmsenv
voms-proxy-init -voms cms
```

## Step 1: Create root tuples with OffsetTreeMaker.cc
Here we are running the framework /plugins/OffsetTreeMaker.cc using the configuration file run_offset.py . The cmsRun command executes the .py file and gives us an output root file named **_Offset_Data.root_** or _**Offset_MC.root**_ depending upon the choice we made in the **isMC** statement. This runs the code interactively.

Another option is to copy a segment of data locally to test the file. This can be done using the code copy_cfg.py or xrdcp command.

### For Data:
1. Extract corresponding pileup JSON from this site: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/

**OR** on lxplus at: /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/   

2. Using the JSONtoASCII.py convert the json file(pileup_latest.txt) to ascii (pileup_20**XX**.txt) and move a copy of 'pileup_20**XX**.txt' to /plugins.
     ```console
     python JSONtoASCII.py pileup_latest.txt >pileup_20**XX**.txt
     cp pileup_20**XX**.txt ./plugins
     ```
3. Modify plugins/parsePileUpJSON2.h to include the new pileup file.
     ```C
     int parsePileUpJSON2(string filename="pileup_20**XX**.txt")
     ```
4. Modify the following lines in run_offset.py
     ```python
     isMC = cms.bool(False)
     ```
     Add the address to a root file from the dataset. 
     ```python
     readFiles.extend( ['/store/data/__*root file from DAS for your dataset*__.root' ] );
     ```
     Depending on the dataset download the latest JECs into the current folder and specify the era accordingly. For example, If we were going to process UL18 to calculate JECs, we can use the JECs for 2018 RECO dataset. 
     To find and download the appropriate JECs go to: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC.
     
     We also need to select the '_jetType_name_' depending on the type of jet's you want to apply Jet Energy corrections to.
     ```python
     run = "A"
     OutputName = "_Data_UL2018"+run
     eraName = "Summer19UL18_Run"+run+"_V2_SimpleL1_DATA"
     jetType_name = "AK4PFchs" # or "AK4PF"
     ```
     Add the global tag for the corresponding dataset from DAS.
     ```python
     process.GlobalTag = GlobalTag( process.GlobalTag, '__*Global Tag for your dataset from DAS*__' )
     ```
     Change the value of numSkip to another prime number depending on the number of events you want. Typically we need atmost 1.5 million events for each era.
     ```python
     numSkip = cms.int32(***),
     ```
     Change the pileup file name. 
     ```python
     puFileName = cms.string("pileup_20**XX**.txt"),
     ```  
5. Execute the code.
     ```console
     cmsRun run_offset.py
     ```
     
### For MC:
**Note: Pileup information is not needed to process the MC, the code still requires the file to run.**

Perform the steps 1, 2, 3, 4 and 5 as given above for data with two modifications in Step 4.

```python
isMC = cms.bool(True)
```
and
```python
OutputName = "_MC_UL2018"
eraName = "Summer19UL18_V2_SimpleL1_MC"
jetType_name = "AK4PFchs" # or "AK4PF"
```
     
## Step 2: Submitting CRAB jobs.
To run on entire dataset locally is expensive and time consuming, so we submit CRAB jobs.

### For Data:
Modify crab_run_offset.py
1. Include the new pileup file and JECs for Data:
     ```python
     config.JobType.inputFiles = ["pileup_20**XX**.txt", "Summer19UL18_Run**XX**_V2_SimpleL1_DATA"]
     ```
2. Add the outputFiles depending upon the name of the output root file from run_offset.py
     ```python
     config.JobType.outputFiles = ["Offset_Data.root"]
     ```
3. Add the name of dataset we are analysing 
     ```python
     config.Data.inputDataset = '/ZeroBias/--*Dataset name from DAS*--/RECO'
     ```
4. To split the task in jobs, we use Lumibased method for data. 
     ```python
     config.Data.splitting = 'LumiBased'
     ```
5. Apply lumi-mask to the input dataset analysis. The golden-json file can be found at: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/ Download the certificate into the present working directory.
     ```python
     config.Data.lumiMask = 'Cert_--*golden json file*--.txt'
     ```
6. The last modification is the path to the storage site. 
     ```python
     config.Data.outLFNDirBase = '/store/user/--*username*--'
     ```
### For MC:
We make modifications to submit CRAB jobs for MC. 

1. Include the new pileup file and the JECs for MC:
     ```python
     config.JobType.inputFiles = ["pileup_20**XX**.txt", "Summer19UL18_Run**XX**_V2_SimpleL1_MC"]
     ```
2. Change the outputFiles depending upon the name of the output root file from run_offset.py
     ```python
     config.JobType.outputFiles = ["Offset_MC.root"]
     ```
3. Change the name of dataset we are analysing .
     ```python
     config.Data.inputDataset = '/SingleNeutrino/--*Dataset name from DAS*--/AODSIM'
     ```
4. To split the task in jobs, we use FileBased method for MC. 
     ```python
     config.Data.splitting = 'FileBased'
     ```
5. Comment out the lumiMask statement.

6. The last modification is the path to the storage site. 
     ```python
     config.Data.outLFNDirBase = '/store/user/--*username*--/--*MCstorage*--'
     ```

To modify the environment for running crab job we use the following commands:
```console
cmsenv
source /cvmfs/cms.cern.ch/crab3/crab.csh
voms-proxy-init -voms cms
```
Submit CRAB jobs:
```console
crab submit crab_run_offset.py
```
Once CRAB job finishes, add the .root files using the following command:
```console
hadd Total_Data.root Offset_Data*.root
OR
hadd Total_MC.root Offset_MC*.root
```

**_NOTE:_** To calculate the integrated luminosity for the datasets use BrilCalc on lxplus. Look at [Appendix A](https://github.com/garvitaa/OffsetTreeMaker/blob/master/README.md#appendix-a---calculating-luminosity-using-brilcalc) for instructions.

## Step 3: Processing the created nTuples using histomaker.cc 
Once we have the output files for both data and MC we can process them using the histomaker.cc

The value 0.4 or 0.8 is the distance parameter(R) for anti-kT jet clustering. 

### For Data:

```console
nohup histomaker false 0.4(0.8) Total_Data.root
```

### for MC:
We are reweighting MC with respect to the data file : Total_Data.root

```console
nohup histomaker true 0.4(0.8) Total_Data.root  Total_MC.root  (Total_Data.root is the file that you reweight wrt)
```
    
## Step 4: Create histograms, profiles, stack plots
Use ./produce_plots.sh to create plots. Make changes to _**produce_plots.sh**_ as needed.

We will have to run this shell script twice:

- once with a random nPU range (n1, n2, topX, topY) and,
- second time with the nPU range (n1, n2, topX, topY) extracted from the nPU hist produced in the previous run.

```console
./produce_plots.sh <Run> <Luminosity>
```
## Step 5: Produce L1RC and Scalefactor .txt files
Use the same nPU range (n1, n2, topX, topY) as in Step 4 and make changes to _**produce_scalefactors.sh**_ as needed.

```console
./produce_scalefactors.sh <Run> <Luminosity>
```

## Step 6: Produce L1FastJet .txt files
Once the MCTruth information is available we can produce the L1FastJet files for 3 parametrisations:
1. Simple
2. SemiSimple
3. Complex

Make the changes to _**l1fastjet_adapted2020.c**_ as needed. For running this you'll need L1FastJet MC file. 
This script also produces scalefactor vs. eta plots for each era with uncertainties.

```console
root -l -b -q 'l1fastjet_adapted.c("chs")' 
OR 
root -l -b -q 'l1fastjet_adapted.c("all")'
```

# Appendix A - Calculating Luminosity using BrilCalc
To calculate the integrated luminosity for the datasets use BrilCalc on lxplus. Get the normtag at:

https://twiki.cern.ch/twiki/bin/view/CMSPublic/LumiPublicResults#Technical_details

Get CRAB report.
```console
crab report crab_project/<path to the folder containing the results folder>
```
     
Rename and copy the processeddLumis.json to lxplus.
```console
mv processedLumis.json <new name>.json
```
     
On lxplus:

```console
export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.1.7/bin:$PATH
pip uninstall -y brilws
pip install --install-option="--prefix=$HOME/.local" brilws
brilcalc lumi -b "STABLE BEAMS" --byls --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/<normtag_file> -u /fb -i <new name>.json
```
     
# Appendix B
List of all files in this folder along with the purpose of each.
File/Folder | Purpose
---------|------------
bin| Contains histomaker.cc and its build file
Adding other files| Under development

# Appendix C
Here is a record of the dataset processed and their location on lpc.
Dataset | Location of root Tupes | Location of histomaker files
---------|------------|------------
2016 - ZeroBias| /eos/uscms/store/user/gagarwal/offset/L1Offset2018/Data/2016/Data_Run**X**.root where **X** = B, C, D, E, F, G, H | Can't locate
2016 - SingleNeutrino| /eos/uscms/store/user/gagarwal/offset/L1Offset2018/Data/2016/RunIISummer16_MC_Tree.root | Can't locate
2017| Under development | Under development
2018| Under development | Under development
