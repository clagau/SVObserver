//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ErrorNumbers
//* .File Name       : $Workfile:   ErrorNumbers.h  $
//* .Description     : This is the central location to define error numbers
//*                    for the SVObserver project.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   23 Feb 2015 03:37:16  $
//******************************************************************************

#pragma once

//Marc Ziegler error base
static const int Maz_ErrorBase = 2000;
//SVGuiExtentUpdater
static const int Err_SetImageExtent_InvalidParameter_2001 = Maz_ErrorBase + 1;
static const int Err_SetImageExtentToParent_InvalidParameter_2002 = Maz_ErrorBase + 2;
static const int Err_SetImageExtentToFit_InvalidParameter_2003 = Maz_ErrorBase + 3;
//SVCommandInspectionExtentUpdater
static const int Err_SVCommandInspectionExtentUpdater_InvalidMode_2004 = Maz_ErrorBase + 4;
static const int Err_SVCommandInspectionExtentUpdater_ResetAllObjects_2005 = Maz_ErrorBase + 5;
static const int Err_SVCommandInspectionExtentUpdater_RunOnce_2006 = Maz_ErrorBase + 6;
static const int Err_SVCommandInspectionExtentUpdater_InvalidParameter_2007 = Maz_ErrorBase + 7;
//Misc
static const int Err_ImportInspectionWrongVersion_2008 = Maz_ErrorBase + 8;
static const int Err_LoadOfResourceDllFailed_2009 = Maz_ErrorBase + 9;
static const int Err_TreeParser_InspectionCreateFailed_2010 = Maz_ErrorBase + 10;

//Marcus Eichengrün error base
static const int Mec_ErrorBase = 16000;
//RangeHelper
static const int Err_16001 = Mec_ErrorBase + 1;
static const int Err_16002 = Mec_ErrorBase + 2;
static const int Err_16003 = Mec_ErrorBase + 3;
static const int Err_16004 = Mec_ErrorBase + 4;
static const int Err_16005 = Mec_ErrorBase + 5;
static const int Err_16006 = Mec_ErrorBase + 6;
static const int Err_16007 = Mec_ErrorBase + 7;
static const int Err_16008 = Mec_ErrorBase + 8;
static const int Err_16009 = Mec_ErrorBase + 9;
static const int Err_16010 = Mec_ErrorBase + 10;
static const int Err_16011 = Mec_ErrorBase + 11;
static const int Err_16012 = Mec_ErrorBase + 12;
static const int Err_16013 = Mec_ErrorBase + 13;
static const int Err_16014 = Mec_ErrorBase + 14;
static const int Err_16015 = Mec_ErrorBase + 15;
static const int Err_16016 = Mec_ErrorBase + 16;
static const int Err_16017 = Mec_ErrorBase + 17;
static const int Err_16018 = Mec_ErrorBase + 18;
static const int Err_16019 = Mec_ErrorBase + 19;
static const int Err_16020 = Mec_ErrorBase + 20;
static const int Err_16021 = Mec_ErrorBase + 21;
static const int Err_16022 = Mec_ErrorBase + 22;
static const int Err_16023 = Mec_ErrorBase + 23;
static const int Err_16024 = Mec_ErrorBase + 24;
static const int Err_16025 = Mec_ErrorBase + 25;
static const int Err_16026 = Mec_ErrorBase + 26;
////Errornumbers for Adjust ToolsSize 
static const int FirstErrorAdjustTool  = Mec_ErrorBase + 27;
static const int Err_16027_InvalidOwner = Mec_ErrorBase + 27;
static const int Err_16028_InvalidMode = Mec_ErrorBase + 28;
static const int Err_16029_InvalidSelection = Mec_ErrorBase + 29;
static const int Err_16030_InvalidValuePointer = Mec_ErrorBase + 30;
static const int Err_16031_InvalidSize = Mec_ErrorBase + 31;
static const int Err_16032_InvalidSize = Mec_ErrorBase + 32;
static const int Err_16033_SetExtensionFailed = Mec_ErrorBase + 33;
static const int Err_16034_InvalidMode = Mec_ErrorBase + 34;
static const int Err_16035_InvalidMode = Mec_ErrorBase + 35;
static const int Err_16036_InvalidMode = Mec_ErrorBase + 36;
static const int Err_16037_InvalidSelection = Mec_ErrorBase + 37;

static const int LastErrorAdjustTool  = Mec_ErrorBase + 37;


// Ben Walter
static const int BRW_ErrorBase = 17000;
static const int Err_17000_SVConfigurationPrint_PrintPPQBarSection_ErrorGettingPPQ = BRW_ErrorBase;
static const int Err_17001_SVConfigurationPrint_PrintModuleIO_ErrorGettingInputObjectList = BRW_ErrorBase + 1;
static const int Err_17002_SVConfigurationPrint_PrintModuleIO_ErrorFillingInputs = BRW_ErrorBase + 2;
static const int Err_17003_SVConfigurationPrint_PrintResultIO_ErrorGettingPPQCount = BRW_ErrorBase + 3;
static const int Err_17004_SVConfigurationPrint_PrintResultIO_ErrorGettingPPQ = BRW_ErrorBase + 4;
static const int Err_17005_SVConfigurationPrint_PrintResultIO_ErrorGettingOutputs = BRW_ErrorBase + 5;
static const int Err_17006_SVDiscreteInputsView_OnUpdate_ErrorGettingInputObjectList = BRW_ErrorBase + 6;
static const int Err_17007_SVDiscreteInputsView_OnUpdate_ErrorFillingInputs = BRW_ErrorBase + 7;
static const int Err_17008_SVDiscreteInputsView_OnUpdate_ErrorGettingInputObjectList = BRW_ErrorBase + 8;
static const int Err_17009_SVDiscreteInputsView_OnUpdate_ErrorFillingInputs = BRW_ErrorBase + 9;
static const int Err_17010_SVDiscreteOutputsView_OnUpdate_ErrorGettingPPQCount = BRW_ErrorBase + 10;
static const int Err_17011_SVDiscreteOutputsView_OnUpdate_ErrorGettingPPQ = BRW_ErrorBase + 11;
static const int Err_17012_SVDiscreteOutputsView_OnUpdate_ErrorGettingOutputs = BRW_ErrorBase + 12;
static const int Err_17013_SVInspectionProcess_AddInputRequest_ErrorLockingInputRequests = BRW_ErrorBase + 13;
static const int Err_17014_SVInspectionProcess_AddInputRequest_ErrorUnlockingInputRequests = BRW_ErrorBase + 14;
static const int Err_17015_SVInspectionProcess_AddInputRequest_ErrorUnlockingInputRequests = BRW_ErrorBase + 15;
static const int Err_17016_SVInspectionProcess_AddInputImageRequest_ErrorLockingInputImageRequests = BRW_ErrorBase + 16;
static const int Err_17017_SVInspectionProcess_AddInputImageRequest_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 17;
static const int Err_17018_SVInspectionProcess_AddInputImageRequest_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 18;
static const int Err_17019_SVInspectionProcess_RemoveAllInputRequests_ErrorLockingInputRequests = BRW_ErrorBase + 19;
static const int Err_17020_SVInspectionProcess_RemoveAllInputRequests_ErrorRemovingAllInputRequests = BRW_ErrorBase + 20;
static const int Err_17021_SVInspectionProcess_RemoveAllInputRequests_ErrorUnlockingInputRequests = BRW_ErrorBase + 21;
static const int Err_17022_SVInspectionProcess_RemoveAllInputRequests_ErrorLockingInputImageRequests = BRW_ErrorBase + 22;
static const int Err_17023_SVInspectionProcess_RemoveAllInputRequests_ErrorRemovingAllInputImageRequests = BRW_ErrorBase + 23;
static const int Err_17024_SVInspectionProcess_RemoveAllInputRequests_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 24;
static const int Err_17025_SVInspectionProcess_ProcessInputRequests_ErrorLockingInputRequests = BRW_ErrorBase + 25;
static const int Err_17026_ProcessInputRequests_ErrorUnlockingInputRequests = BRW_ErrorBase + 26;
static const int Err_17027_SVInspectionProcess_RunInspection_ErrorGettingTimeStamp = BRW_ErrorBase + 27;
static const int Err_17028_SVInspectionProcess_RunInspection_ErrorGettingTimeStamp = BRW_ErrorBase + 28;
static const int Err_17029_SVIOAdjustDialogClass_OnInitDialog_ErrorGettingPPQCount = BRW_ErrorBase + 29;
static const int Err_17030_SVIOAdjustDialogClass_OnInitDialog_ErrorGettingPPQ = BRW_ErrorBase + 30;
static const int Err_17031_SVIOAdjustDialogClass_OnInitDialog_ErrorGettingOutputs = BRW_ErrorBase + 31;
static const int Err_17032_SVIODoc_OnExtrasEditRemoteInputs_ErrorFillingInputs = BRW_ErrorBase + 32;
static const int Err_17033_SVIODoc_OnExtrasEditRemoteInputs_ErrorDetachingInput = BRW_ErrorBase + 33;
static const int Err_17034_SVPLCOutputEditDialog_OnInitDialog_ErrorGettingPPQCount = BRW_ErrorBase + 34;
static const int Err_17035_SVPLCOutputEditDialog_OnInitDialog_ErrorGettingPPQ = BRW_ErrorBase + 35;
static const int Err_17036_SVPLCOutputEditDialog_OnInitDialog_ErrorGettingOutputs = BRW_ErrorBase + 36;
static const int Err_17037_SVPLCOutputsView_OnUpdate_ErrorGettingPPQCount = BRW_ErrorBase + 37;
static const int Err_17038_SVPLCOutputsView_OnUpdate_ErrorGettingPPQ = BRW_ErrorBase + 38;
static const int Err_17039_SVPLCOutputsView_OnUpdate_ErrorGettingPLCData = BRW_ErrorBase + 39;
static const int Err_17040_SVPLCOutputsView_OnLButtonDblClk_ErrorGettingPPQCount = BRW_ErrorBase + 40;
static const int Err_17041_SVPPQEntryDialogDigInPageClass_OnInitDialog_ErrorGettingInputs = BRW_ErrorBase + 41;
static const int Err_17042_SVPPQEntryDialogRemotePageClass_OnInitDialog_ErrorGettingInputs = BRW_ErrorBase + 42;
static const int Err_17043_SVPublishListClass_Refresh_ErrorFindingPPQEntries = BRW_ErrorBase + 43;
static const int Err_17044_SVRemoteInputsView_OnUpdate_ErrorGettingInputObjectList = BRW_ErrorBase + 44;
static const int Err_17045_SVRemoteInputsView_OnUpdate_ErrorFillingInputs = BRW_ErrorBase + 45;
static const int Err_17046_SVRemoteInputsView_OnLButtonDblClk_ErrorGettingInputObjectList = BRW_ErrorBase + 46;
static const int Err_17047_SVRemoteInputsView_OnLButtonDblClk_ErrorFillingInputs = BRW_ErrorBase + 47;
static const int Err_17048_SVRemoteOutputEditDialog_OnInitDialog_ErrorGettingPPQCount = BRW_ErrorBase + 48;
static const int Err_17049_SVRemoteOutputEditDialog_OnInitDialog_ErrorGettingPPQ = BRW_ErrorBase + 49;
static const int Err_17050_SVRemoteOutputEditDialog_OnInitDialog_ErrorGettingOutputs = BRW_ErrorBase + 50;
static const int Err_17051_SVRemoteOutputsView_OnUpdate_ErrorGettingPPQCount = BRW_ErrorBase + 51;
static const int Err_17052_SVRemoteOutputsView_OnLButtonDblClk_ErrorGettingPPQCount = BRW_ErrorBase + 52;
static const int Err_17053_Shift_Tool_Source_Image_Extents_Disabled = BRW_ErrorBase + 53;

//Guido Ramseier error base
static const int Gra_ErrorBase = 25000;
static const int Err_25000_SetClipboardData = Gra_ErrorBase + 0;
static const int Err_25001_ClipboardMemory = Gra_ErrorBase + 1;
static const int Err_25002_ToolInvalid = Gra_ErrorBase + 2;
static const int Err_25003_GetClipboardData = Gra_ErrorBase + 3;
static const int Err_25004_ClipboardDataConversion = Gra_ErrorBase + 4;
static const int Err_25005_VersionMismatch = Gra_ErrorBase + 5;
static const int Err_25006_ColorToolInsert = Gra_ErrorBase + 6;
static const int Err_25007_NonColorToolInsert = Gra_ErrorBase + 7;
static const int Err_25008_ClipboardDataConversion = Gra_ErrorBase + 8;
static const int Err_25009_ClipboardDataConversion = Gra_ErrorBase + 9;
static const int Err_25010_ClipboardDataConversion = Gra_ErrorBase + 10;
static const int Err_25011_ClipboardDataConversion = Gra_ErrorBase + 11;
static const int Err_25012_StatTool_Test = Gra_ErrorBase + 12;
static const int Err_25013_NoMatroxDongle = Gra_ErrorBase + 13;

//Robert Yoho error base
static const int Rpy_ErrorBase = 45000;
static const int Err_45000 = Rpy_ErrorBase + 0;
static const int Memory_Log_45001 = Rpy_ErrorBase + 1;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ErrorNumbers.h_v  $
 * 
 *    Rev 1.6   23 Feb 2015 03:37:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Matrox error number description type int
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Feb 2015 02:47:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added no Matrox Dongle exception error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Feb 2015 02:57:36   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  983
 * SCR Title:  Linear Tool - Crash while continuous resizing tool SVO-392
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add error numbers from MZA
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Jan 2015 08:23:34   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Add new Errornumbers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jan 2015 16:40:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed type to int to avoid "unsigned" compiler warnings.
 *   Added new Mec error numbers to make return values unique.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2015 10:34:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Combined unique error numbers
 * changed from define to static const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2014 13:53:40   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */