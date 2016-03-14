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

namespace Seidenader { namespace ObjectInterfaces
{
	// Jim Brown Program code.
	static const unsigned long ProgCode_5066_CreateImageBuffer = 5066;
	static const unsigned long ProgCode_5067_ValidateCurrentTreeData = 5067;

	//Marc Ziegler error base
	static const int Maz_ErrorBase = 10000;
	//SVGuiExtentUpdater
	static const int Err_10001_SetImageExtent_InvalidParameter = Maz_ErrorBase + 1;
	static const int Err_10002_SetImageExtentToParent_InvalidParameter = Maz_ErrorBase + 2;
	static const int Err_10003_SetImageExtentToFit_InvalidParameter = Maz_ErrorBase + 3;
	//SVCommandInspectionExtentUpdater
	static const int Err_10004_SVCommandInspectionExtentUpdater_InvalidMode = Maz_ErrorBase + 4;
	static const int Err_10005_SVCommandInspectionExtentUpdater_ResetAllObjects = Maz_ErrorBase + 5;
	static const int Err_10006_SVCommandInspectionExtentUpdater_RunOnce = Maz_ErrorBase + 6;
	static const int Err_10007_SVCommandInspectionExtentUpdater_InvalidParameter = Maz_ErrorBase + 7;
	//Misc
	static const int Err_10008_ImportInspectionWrongVersion = Maz_ErrorBase + 8;
	static const int Err_10009_LoadOfResourceDllFailed = Maz_ErrorBase + 9;
	static const int Err_10010_TreeParser_InspectionCreateFailed = Maz_ErrorBase + 10;
	//RingBuffer
	static const int Err_10011_RingBuffer_OnValidFailed = Maz_ErrorBase + 11;
	static const int Err_10012_RingBuffer_NoInputImage = Maz_ErrorBase + 12;
	static const int Err_10013_RingBuffer_DepthValueInvalid = Maz_ErrorBase + 13;
	static const int Err_10014_LinkedValueConnectInput_NoInspection = Maz_ErrorBase + 14;
	static const int Err_10015_LinkedValueConnectInput_ConnectFailed = Maz_ErrorBase + 15;
	static const int Err_10016_LinkedValueConnectInput_InvalidValue = Maz_ErrorBase + 16;
	static const int Err_10017_LinkedValueConnectInput_ResetFailed = Maz_ErrorBase + 17;
	static const int Err_10018_RingBuffer_InvalidRingDepth = Maz_ErrorBase + 18;
	static const int Err_10019_RingBuffer_InvalidImageIndex = Maz_ErrorBase + 19;

	static const int Err_10020_InsertTaskObject_InvalidParameter = Maz_ErrorBase + 20;
	static const int Err_10021_InsertTaskObject_CreateObjectFailed = Maz_ErrorBase + 21;
	static const int Err_10022_ConstructTaskObject_Failed = Maz_ErrorBase + 22;
	static const int Err_10023_ConstructTaskObject_InvalidObjectID = Maz_ErrorBase + 23;
	static const int Err_10024_DestroyChildObject_InvalidParameter = Maz_ErrorBase + 24;
	static const int Err_10025_ShouldResetInspection_InvalidParameter = Maz_ErrorBase + 25;
	//GoOnline
	static const int Err_10026_GoOnline_WrongCameraModel = Maz_ErrorBase + 26;
	static const int Err_10027_GoOnline_WrongCameraVender = Maz_ErrorBase + 27;

	static const int Err_10028_LoadOfScintillaDllFailed = Maz_ErrorBase + 28;
	static const int Err_10029_ValueObject_Parameter_WrongSize = Maz_ErrorBase + 29;

	//Steve Jones error base
	static const int Sej_ErrorBase = 15000;
	static const int Err_15000 = Sej_ErrorBase + 0;
	static const int Err_15001 = Sej_ErrorBase + 1;
	static const int Err_15002 = Sej_ErrorBase + 2;
	static const int Err_15003 = Sej_ErrorBase + 3;
	static const int Err_15004 = Sej_ErrorBase + 4;
	static const int Err_15005 = Sej_ErrorBase + 5;
	static const int Err_15006 = Sej_ErrorBase + 6;
	static const int Err_15007 = Sej_ErrorBase + 7;
	static const int Err_15008 = Sej_ErrorBase + 8;
	static const int Err_15009 = Sej_ErrorBase + 9;
	static const int Err_15010 = Sej_ErrorBase + 10;
	static const int Err_15011 = Sej_ErrorBase + 11;
	static const int Err_15012 = Sej_ErrorBase + 12;
	static const int Err_15013 = Sej_ErrorBase + 13;
	static const int Err_15014 = Sej_ErrorBase + 14;
	static const int Err_15015 = Sej_ErrorBase + 15;
	static const int Err_15016 = Sej_ErrorBase + 16;
	static const int Err_15017 = Sej_ErrorBase + 17;
	static const int Err_15018 = Sej_ErrorBase + 18;
	static const int Err_15019 = Sej_ErrorBase + 19;
	static const int Err_15020 = Sej_ErrorBase + 20;
	static const int Err_15021 = Sej_ErrorBase + 21;
	static const int Err_15022 = Sej_ErrorBase + 22;
	static const int Err_15023 = Sej_ErrorBase + 23;
	static const int Err_15024 = Sej_ErrorBase + 24;
	static const int Err_15025 = Sej_ErrorBase + 25;
	static const int Err_15026 = Sej_ErrorBase + 26;
	static const int Err_15027 = Sej_ErrorBase + 27;
	static const int Err_15028 = Sej_ErrorBase + 28;
	static const int Err_15029 = Sej_ErrorBase + 29;
	static const int Err_15030 = Sej_ErrorBase + 30;
	static const int Err_15031 = Sej_ErrorBase + 31;
	static const int Err_15032 = Sej_ErrorBase + 32;
	static const int Err_15033 = Sej_ErrorBase + 33;
	static const int Err_15034 = Sej_ErrorBase + 34;
	static const int Err_15035 = Sej_ErrorBase + 35;
	static const int Err_15036 = Sej_ErrorBase + 36;
	static const int Err_15037 = Sej_ErrorBase + 37;


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
	static const int Err_16038_EmptyFormula = Mec_ErrorBase + 38;
	static const int Err_16039_NegativePosition = Mec_ErrorBase + 39;
	static const int Err_16040_ToLargePosition = Mec_ErrorBase + 40;
	static const int LastErrorAdjustTool  = Mec_ErrorBase + 40;
	///Errornumbers load Configuration 
	static const int Err_16041_ErrroDestroyingConfig  = Mec_ErrorBase + 41;
	static const int Err_16042_ConvertColorToStandardProduct  = Mec_ErrorBase + 42;
	static const int  Err_16043 = Mec_ErrorBase + 43;
	static const int  Err_16044_CreateInputList  = Mec_ErrorBase + 44;
	static const int Err_16045_MissingTag = Mec_ErrorBase + 45;
	static const int  Err_16046_LOAD_IO_FAILED =  Mec_ErrorBase + 46;
	static const int  Err_16047_CreateOutputList =  Mec_ErrorBase + 47;
	static const int Err_16048_ErrrorCreatePPQObject  =  Mec_ErrorBase + 48;
	static const int Err_16049_ErrorMissingInpuTag  =  Mec_ErrorBase + 49;
	static const int Err_16050_ConstructObjectInspection  =  Mec_ErrorBase + 50;
	static const int Err_16051_ErrorMissingIOTypeTag =  Mec_ErrorBase + 51;
	static const int 	Err_16052_ErrorMissingItemNameTag =  Mec_ErrorBase + 52;
	static const int	Err_16053_ErrorMissingIOTypeTag =  Mec_ErrorBase + 53; 
	static const int	Err_16054_ErrorMissingPPQPOsitionTag =  Mec_ErrorBase + 54; 
	static const int Err_16055_ErrorMissingRemoteIndexTag =  Mec_ErrorBase + 55;
	static const int 	Err_16056_ErrorMissingPPQPOsitionTag =  Mec_ErrorBase + 56;
	static const int 	Err_16057_ErrorMissingItemNameTag =  Mec_ErrorBase + 57;
	static const int 	Err_16058_ErrorMissingRemoteIndexTag =  Mec_ErrorBase + 58;
	static const int	Err_16059_ErrorMissingItemNameTag   =  Mec_ErrorBase + 59;
	static const int 	Err_16060_IOEntryIsMissing=  Mec_ErrorBase + 60;
	static const int 	Err_16061_EnviromentTagIsMissing =  Mec_ErrorBase + 61;
	///New Errornumbers SAX PARSER
	static const int 	Err_16062_TODO =  Mec_ErrorBase + 62;
	static const int 	Err_16063_SAXPARSER =  Mec_ErrorBase + 63;
	static const int 	Err_16064_RESTOREVARIANT =  Mec_ErrorBase + 64;
	static const int 	Err_16065_TRANSLATEVARIANT =  Mec_ErrorBase + 65;
	static const int 	Err_16066_TOVARIANT =  Mec_ErrorBase + 66;
	static const int 	Err_16067_OnStartElement =  Mec_ErrorBase + 67;
	static const int 	Err_16068_OnElementData =  Mec_ErrorBase + 68;
	static const int 	Err_16069_INVALIDPOINTER =  Mec_ErrorBase + 69;
	static const int 	Err_16070_INVALIDPOINTER =  Mec_ErrorBase + 70;
	static const int 	Err_16071_INVALIDPOINTER =  Mec_ErrorBase + 71;
	static const int 	Err_16072_INVALID_ARRAYELEMENT =  Mec_ErrorBase + 72;
	static const int 	Err_16073_COINITIALIZE_ =  Mec_ErrorBase + 73;
	static const int 	Err_16074_InvalidArrayElement =  Mec_ErrorBase + 74;
	///New Errornumbers Archive_Tool
	static const int 	Err_16075_ImageNrToBig =  Mec_ErrorBase + 75;
	///Errornumbers load Configuration 
	static const int 	Err_16076_CouldNotLoadInspection =  Mec_ErrorBase + 76;
	
	
	// Ben Walter
	static const int BRW_ErrorBase = 17000;
	static const int Err_17000_ErrorGettingPPQ = BRW_ErrorBase;
	static const int Err_17001_ErrorGettingInputObjectList = BRW_ErrorBase + 1;
	static const int Err_17002_ErrorFillingInputs = BRW_ErrorBase + 2;
	static const int Err_17003_ErrorGettingPPQCount = BRW_ErrorBase + 3;
	static const int Err_17004_ErrorGettingPPQ = BRW_ErrorBase + 4;
	static const int Err_17005_ErrorGettingOutputs = BRW_ErrorBase + 5;
	static const int Err_17006_ErrorGettingInputObjectList = BRW_ErrorBase + 6;
	static const int Err_17007_ErrorFillingInputs = BRW_ErrorBase + 7;
	static const int Err_17008_ErrorGettingInputObjectList = BRW_ErrorBase + 8;
	static const int Err_17009_ErrorFillingInputs = BRW_ErrorBase + 9;
	static const int Err_17010_ErrorGettingPPQCount = BRW_ErrorBase + 10;
	static const int Err_17011_ErrorGettingPPQ = BRW_ErrorBase + 11;
	static const int Err_17012_ErrorGettingOutputs = BRW_ErrorBase + 12;
	static const int Err_17013_ErrorLockingInputRequests = BRW_ErrorBase + 13;
	static const int Err_17014_ErrorUnlockingInputRequests = BRW_ErrorBase + 14;
	static const int Err_17015_ErrorUnlockingInputRequests = BRW_ErrorBase + 15;
	static const int Err_17016_ErrorLockingInputImageRequests = BRW_ErrorBase + 16;
	static const int Err_17017_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 17;
	static const int Err_17018_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 18;
	static const int Err_17019_ErrorLockingInputRequests = BRW_ErrorBase + 19;
	static const int Err_17020_ErrorRemovingAllInputRequests = BRW_ErrorBase + 20;
	static const int Err_17021_ErrorUnlockingInputRequests = BRW_ErrorBase + 21;
	static const int Err_17022_ErrorLockingInputImageRequests = BRW_ErrorBase + 22;
	static const int Err_17023_ErrorRemovingAllInputImageRequests = BRW_ErrorBase + 23;
	static const int Err_17024_ErrorUnlockingInputImageRequests = BRW_ErrorBase + 24;
	static const int Err_17025_ErrorLockingInputRequests = BRW_ErrorBase + 25;
	static const int Err_17026_ErrorUnlockingInputRequests = BRW_ErrorBase + 26;
	static const int Err_17027_ErrorGettingTimeStamp = BRW_ErrorBase + 27;
	static const int Err_17028_ErrorGettingTimeStamp = BRW_ErrorBase + 28;
	static const int Err_17029_ErrorGettingPPQCount = BRW_ErrorBase + 29;
	static const int Err_17030_ErrorGettingPPQ = BRW_ErrorBase + 30;
	static const int Err_17031_ErrorGettingOutputs = BRW_ErrorBase + 31;
	static const int Err_17032_ErrorFillingInputs = BRW_ErrorBase + 32;
	static const int Err_17033_ErrorDetachingInput = BRW_ErrorBase + 33;
	static const int Err_17034_ErrorGettingPPQCount = BRW_ErrorBase + 34;
	static const int Err_17035_ErrorGettingPPQ = BRW_ErrorBase + 35;
	static const int Err_17036_ErrorGettingOutputs = BRW_ErrorBase + 36;
	static const int Err_17037_ErrorGettingPPQCount = BRW_ErrorBase + 37;
	static const int Err_17038_ErrorGettingPPQ = BRW_ErrorBase + 38;
	static const int Err_17040_ErrorGettingPPQCount = BRW_ErrorBase + 40;
	static const int Err_17041_ErrorGettingInputs = BRW_ErrorBase + 41;
	static const int Err_17042_ErrorGettingInputs = BRW_ErrorBase + 42;
	static const int Err_17043_ErrorFindingPPQEntries = BRW_ErrorBase + 43;
	static const int Err_17044_ErrorGettingInputObjectList = BRW_ErrorBase + 44;
	static const int Err_17045_ErrorFillingInputs = BRW_ErrorBase + 45;
	static const int Err_17046_ErrorGettingInputObjectList = BRW_ErrorBase + 46;
	static const int Err_17047_ErrorFillingInputs = BRW_ErrorBase + 47;
	static const int Err_17048_ErrorGettingPPQCount = BRW_ErrorBase + 48;
	static const int Err_17049_ErrorGettingPPQ = BRW_ErrorBase + 49;
	static const int Err_17050_ErrorGettingOutputs = BRW_ErrorBase + 50;
	static const int Err_17051_ErrorGettingPPQCount = BRW_ErrorBase + 51;
	static const int Err_17052_ErrorGettingPPQCount = BRW_ErrorBase + 52;
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
	static const int Err_25014_GlobalConstantNumber = Gra_ErrorBase + 14;
	static const int Err_25015_GlobalNameAlreadyUsed = Gra_ErrorBase + 15;
	static const int Err_25016_GlobalNameInvalid = Gra_ErrorBase + 16;
	static const int Err_25017_RootChildCreate = Gra_ErrorBase + 17;
	static const int Err_25018_Json_GetConfig = Gra_ErrorBase + 18;
	static const int Err_25019_Json_PutConfig = Gra_ErrorBase + 19;
	static const int Err_25020_MatroxLibraryNone = Gra_ErrorBase + 20;
	static const int Err_25021_MatroxLibraryFatal = Gra_ErrorBase + 21;
	static const int Err_25022_EventThread = Gra_ErrorBase + 22;
	static const int Err_25023_EventThread = Gra_ErrorBase + 23;
	static const int Err_25024_EventThread = Gra_ErrorBase + 24;
	static const int Err_25025_EventThread = Gra_ErrorBase + 25;
	static const int Err_25026_EventThread = Gra_ErrorBase + 26;
	static const int Err_25027_EventThread = Gra_ErrorBase + 27;
	static const int Err_25028_EventThread = Gra_ErrorBase + 28;
	static const int Err_25029_EventThread = Gra_ErrorBase + 29;
	static const int Err_25030_Thread = Gra_ErrorBase + 30;
	static const int Err_25031_Thread = Gra_ErrorBase + 31;
	static const int Err_25032_Thread = Gra_ErrorBase + 32;
	static const int Err_25033_Thread = Gra_ErrorBase + 33;
	static const int Err_25034_Thread = Gra_ErrorBase + 34;
	static const int Err_25035_Thread = Gra_ErrorBase + 35;
	static const int Err_25036_Thread = Gra_ErrorBase + 36;
	static const int Err_25037_AsyncProcedure = Gra_ErrorBase + 37;
	static const int Err_25038_AsyncProcedure = Gra_ErrorBase + 38;
	static const int Err_25039_AsyncProcedure = Gra_ErrorBase + 39;
	static const int Err_25040_AsyncProcedure = Gra_ErrorBase + 40;
	static const int Err_25041_AsyncProcedure = Gra_ErrorBase + 41;
	static const int Err_25042_SetImageHandleIndex = Gra_ErrorBase + 42;
	static const int Err_25043_InvalidSourceIndex = Gra_ErrorBase + 43;
	static const int Err_25044_CameraNotFound = Gra_ErrorBase + 44;
	static const int Err_25045_InvalidCamera = Gra_ErrorBase + 45;
	static const int Err_25046_InvalidImage = Gra_ErrorBase + 46;
	static const int Err_25047_LoadFileFailed = Gra_ErrorBase + 47;
	static const int Err_25048_NoMatroxService = Gra_ErrorBase + 48;

	//Arvid Breitenbach error base
	static const int Abc_ErrorBase = 30000;
	static const int Err_30000_NullImageOnSave = Abc_ErrorBase + 0;

	//Robert Yoho error base
	static const int Rpy_ErrorBase = 45000;
	static const int Err_45000 = Rpy_ErrorBase + 0;
	static const int Memory_Log_45001 = Rpy_ErrorBase + 1;

} /* namespace ObjectInterfaces */ } /* namespace Seidenader */

namespace SvOi = Seidenader::ObjectInterfaces;

