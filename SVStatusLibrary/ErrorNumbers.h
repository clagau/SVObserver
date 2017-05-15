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

namespace SvStl
{
	// Jim Brown Program code.
	static const unsigned long ProgCode_5068_ValidateTabData = 5068;

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
	static const int Err_10013_RingBuffer_DepthValueInvalid = Maz_ErrorBase + 13;

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

	static const int Err_10030_AddFriend_InvalidParameter = Maz_ErrorBase + 30;
	static const int Err_10031_AddFriend_Failed = Maz_ErrorBase + 31;
	static const int Err_10032_DestroyFriendObject_InvalidParameter = Maz_ErrorBase + 32;
	static const int Err_10033_DestroyFriendObject_Failed = Maz_ErrorBase + 33;

	//Consistent use of error messages
	static const int Err_10030 = Maz_ErrorBase + 30;

	//AFXMessageBox-replace
	static const int Err_10036 = Maz_ErrorBase + 36;
	static const int Err_10037 = Maz_ErrorBase + 37;
	static const int Err_10038 = Maz_ErrorBase + 38;
	static const int Err_10039 = Maz_ErrorBase + 39;
	static const int Err_10041 = Maz_ErrorBase + 41;
	static const int Err_10042 = Maz_ErrorBase + 42;
	static const int Err_10043 = Maz_ErrorBase + 43;
	static const int Err_10046 = Maz_ErrorBase + 46;
	static const int Err_10047 = Maz_ErrorBase + 47;
	static const int Err_10048 = Maz_ErrorBase + 48;
	static const int Err_10049 = Maz_ErrorBase + 49;
	static const int Err_10050 = Maz_ErrorBase + 50;
	static const int Err_10051 = Maz_ErrorBase + 51;
	static const int Err_10052 = Maz_ErrorBase + 52;
	static const int Err_10054 = Maz_ErrorBase + 54;
	static const int Err_10055 = Maz_ErrorBase + 55;
	static const int Err_10056 = Maz_ErrorBase + 56;
	static const int Err_10057 = Maz_ErrorBase + 57;
	static const int Err_10058 = Maz_ErrorBase + 58;
	static const int Err_10059 = Maz_ErrorBase + 59;
	static const int Err_10060 = Maz_ErrorBase + 60;
	static const int Err_10064 = Maz_ErrorBase + 64;
	static const int Err_10065 = Maz_ErrorBase + 65;
	static const int Err_10066 = Maz_ErrorBase + 66;
	static const int Err_10067 = Maz_ErrorBase + 67;
	static const int Err_10068 = Maz_ErrorBase + 68;
	static const int Err_10069 = Maz_ErrorBase + 69;
	static const int Err_10070 = Maz_ErrorBase + 70;
	static const int Err_10071 = Maz_ErrorBase + 71;
	static const int Err_10072 = Maz_ErrorBase + 72;
	static const int Err_10073 = Maz_ErrorBase + 73;
	static const int Err_10074 = Maz_ErrorBase + 74;
	static const int Err_10075 = Maz_ErrorBase + 75;
	static const int Err_10076 = Maz_ErrorBase + 76;
	static const int Err_10077 = Maz_ErrorBase + 77;
	static const int Err_10078 = Maz_ErrorBase + 78;
	static const int Err_10079 = Maz_ErrorBase + 79;
	static const int Err_10080 = Maz_ErrorBase + 80;
	static const int Err_10081 = Maz_ErrorBase + 81;
	static const int Err_10082 = Maz_ErrorBase + 82;
	static const int Err_10083 = Maz_ErrorBase + 83;
	static const int Err_10084 = Maz_ErrorBase + 84;
	static const int Err_10085 = Maz_ErrorBase + 85;
	static const int Err_10086 = Maz_ErrorBase + 86;
	static const int Err_10087 = Maz_ErrorBase + 87;
	static const int Err_10088 = Maz_ErrorBase + 88;
	static const int Err_10089 = Maz_ErrorBase + 89;
	static const int Err_10090 = Maz_ErrorBase + 90;
	static const int Err_10091 = Maz_ErrorBase + 91;
	static const int Err_10092 = Maz_ErrorBase + 92;
	static const int Err_10093 = Maz_ErrorBase + 93;
	static const int Err_10094 = Maz_ErrorBase + 94;
	static const int Err_10095 = Maz_ErrorBase + 95;
	static const int Err_10096 = Maz_ErrorBase + 96;
	static const int Err_10097 = Maz_ErrorBase + 97;
	static const int Err_10098 = Maz_ErrorBase + 98;
	static const int Err_10099 = Maz_ErrorBase + 99;
	static const int Err_10100 = Maz_ErrorBase + 100;
	static const int Err_10101 = Maz_ErrorBase + 101;
	static const int Err_10102 = Maz_ErrorBase + 102;
	static const int Err_10103 = Maz_ErrorBase + 103;
	static const int Err_10104 = Maz_ErrorBase + 104;
	static const int Err_10105 = Maz_ErrorBase + 105;
	static const int Err_10106 = Maz_ErrorBase + 106;
	static const int Err_10107 = Maz_ErrorBase + 107;
	static const int Err_10108 = Maz_ErrorBase + 108;
	static const int Err_10109 = Maz_ErrorBase + 109;
	static const int Err_10110 = Maz_ErrorBase + 110;
	static const int Err_10111 = Maz_ErrorBase + 111;
	static const int Err_10112 = Maz_ErrorBase + 112;
	static const int Err_10113 = Maz_ErrorBase + 113;
	static const int Err_10114 = Maz_ErrorBase + 114;
	static const int Err_10115 = Maz_ErrorBase + 115;
	static const int Err_10116 = Maz_ErrorBase + 116;
	static const int Err_10117 = Maz_ErrorBase + 117;
	static const int Err_10118 = Maz_ErrorBase + 118;
	static const int Err_10119 = Maz_ErrorBase + 119;
	static const int Err_10120 = Maz_ErrorBase + 120;
	static const int Err_10121 = Maz_ErrorBase + 121;
	static const int Err_10122 = Maz_ErrorBase + 122;
	static const int Err_10123 = Maz_ErrorBase + 123;
	static const int Err_10124 = Maz_ErrorBase + 124;
	static const int Err_10125 = Maz_ErrorBase + 125;
	static const int Err_10126 = Maz_ErrorBase + 126;
	static const int Err_10127 = Maz_ErrorBase + 127;
	static const int Err_10128 = Maz_ErrorBase + 128;
	static const int Err_10129 = Maz_ErrorBase + 129;
	static const int Err_10130 = Maz_ErrorBase + 130;
	static const int Err_10131 = Maz_ErrorBase + 131;
	static const int Err_10132 = Maz_ErrorBase + 132;
	static const int Err_10133 = Maz_ErrorBase + 133;
	static const int Err_10134 = Maz_ErrorBase + 134;
	static const int Err_10135 = Maz_ErrorBase + 135;
	static const int Err_10136 = Maz_ErrorBase + 136;
	static const int Err_10137 = Maz_ErrorBase + 137;
	static const int Err_10138 = Maz_ErrorBase + 138;
	static const int Err_10139 = Maz_ErrorBase + 139;
	static const int Err_10140 = Maz_ErrorBase + 140;
	static const int Err_10169 = Maz_ErrorBase + 169;
	static const int Err_10170 = Maz_ErrorBase + 170;
	static const int Err_10171 = Maz_ErrorBase + 171;
	static const int Err_10172 = Maz_ErrorBase + 172;
	static const int Err_10173 = Maz_ErrorBase + 173;
	static const int Err_10174 = Maz_ErrorBase + 174;
	static const int Err_10175 = Maz_ErrorBase + 175;
	static const int Err_10176 = Maz_ErrorBase + 176;
	static const int Err_10177 = Maz_ErrorBase + 177;
	static const int Err_10178 = Maz_ErrorBase + 178;
	static const int Err_10179 = Maz_ErrorBase + 179;
	static const int Err_10180 = Maz_ErrorBase + 180;
	static const int Err_10181 = Maz_ErrorBase + 181;
	static const int Err_10182 = Maz_ErrorBase + 182;
	static const int Err_10183 = Maz_ErrorBase + 183;
	static const int Err_10184 = Maz_ErrorBase + 184;
	static const int Err_10185 = Maz_ErrorBase + 185;
	static const int Err_10187 = Maz_ErrorBase + 187;
	static const int Err_10188 = Maz_ErrorBase + 188;
	static const int Err_10189 = Maz_ErrorBase + 189;
	static const int Err_10190 = Maz_ErrorBase + 190;
	static const int Err_10191 = Maz_ErrorBase + 191;
	static const int Err_10192 = Maz_ErrorBase + 192;
	static const int Err_10193 = Maz_ErrorBase + 193;
	static const int Err_10194 = Maz_ErrorBase + 194;
	static const int Err_10195 = Maz_ErrorBase + 195;
	static const int Err_10196 = Maz_ErrorBase + 196;
	static const int Err_10197 = Maz_ErrorBase + 197;
	static const int Err_10198 = Maz_ErrorBase + 198;
	static const int Err_10200 = Maz_ErrorBase + 200;
	static const int Err_10201 = Maz_ErrorBase + 201;
	static const int Err_10202 = Maz_ErrorBase + 202;
	static const int Err_10203 = Maz_ErrorBase + 203;
	static const int Err_10204 = Maz_ErrorBase + 204;
	static const int Err_10205 = Maz_ErrorBase + 205;
	static const int Err_10207 = Maz_ErrorBase + 207;
	static const int Err_10208 = Maz_ErrorBase + 208;
	static const int Err_10209 = Maz_ErrorBase + 209;
	static const int Err_10210 = Maz_ErrorBase + 210;
	static const int Err_10211 = Maz_ErrorBase + 211;
	static const int Err_10212 = Maz_ErrorBase + 212;
	static const int Err_10213 = Maz_ErrorBase + 213;
	static const int Err_10214 = Maz_ErrorBase + 214;
	static const int Err_10215 = Maz_ErrorBase + 215;
	static const int Err_10216 = Maz_ErrorBase + 216;
	static const int Err_10217 = Maz_ErrorBase + 217;
	static const int Err_10218 = Maz_ErrorBase + 218;
	static const int Err_10219 = Maz_ErrorBase + 219;
	static const int Err_10220 = Maz_ErrorBase + 220;
	static const int Err_10221 = Maz_ErrorBase + 221;
	static const int Err_10222 = Maz_ErrorBase + 222;
	static const int Err_10223 = Maz_ErrorBase + 223;
	static const int Err_10224 = Maz_ErrorBase + 224;
	static const int Err_10225 = Maz_ErrorBase + 225;
	static const int Err_10226 = Maz_ErrorBase + 226;
	static const int Err_10227 = Maz_ErrorBase + 227;
	static const int Err_10228 = Maz_ErrorBase + 228;
	static const int Err_10229 = Maz_ErrorBase + 229;
	static const int Err_10230 = Maz_ErrorBase + 230;
	static const int Err_10231 = Maz_ErrorBase + 231;
	static const int Err_10232 = Maz_ErrorBase + 232;
	static const int Err_10233 = Maz_ErrorBase + 233;
	static const int Err_10234 = Maz_ErrorBase + 234;
	static const int Err_10235 = Maz_ErrorBase + 235;
	static const int Err_10236 = Maz_ErrorBase + 236;
	static const int Err_10237 = Maz_ErrorBase + 237;
	static const int Err_10238 = Maz_ErrorBase + 238;
	static const int Err_10239 = Maz_ErrorBase + 239;
	static const int Err_10240 = Maz_ErrorBase + 240;
	static const int Err_10241 = Maz_ErrorBase + 241;
	static const int Err_10242 = Maz_ErrorBase + 242;
	static const int Err_10243 = Maz_ErrorBase + 243;
	static const int Err_10244 = Maz_ErrorBase + 244;
	static const int Err_10245 = Maz_ErrorBase + 245;
	static const int Err_10248 = Maz_ErrorBase + 248;
	static const int Err_10249 = Maz_ErrorBase + 249;
	static const int Err_10250 = Maz_ErrorBase + 250;
	static const int Err_10251 = Maz_ErrorBase + 251;
	static const int Err_10252 = Maz_ErrorBase + 252;
	static const int Err_10253 = Maz_ErrorBase + 253;

	//Steve Jones error base
	static const int Sej_ErrorBase = 15000;
	static const int Err_15001 = Sej_ErrorBase + 1;
	static const int Err_15002 = Sej_ErrorBase + 2;
	static const int Err_15003 = Sej_ErrorBase + 3;
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
	// Obsolete Item Check
	static const int Err_15038_AnalogCamera = Sej_ErrorBase + 38;
	static const int Err_15039_1394Camera = Sej_ErrorBase + 39;
	static const int Err_15040_GageTool = Sej_ErrorBase + 40;
	static const int Err_15041_ProfileTool = Sej_ErrorBase + 41;
	static const int Err_15042_FastOCR = Sej_ErrorBase + 42;
	static const int Err_15043_UnknownProductType = Sej_ErrorBase + 43;
	// Shared memory
	static const int Err_15044_InitMonitorListInSharedMemory = Sej_ErrorBase + 44;
	// Filename Manager 
	static const int Err_15044_FileNameManagerAdd = Sej_ErrorBase + 45;

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
	static const int Err_16043 = Mec_ErrorBase + 43;
	static const int Err_16044_CreateInputList  = Mec_ErrorBase + 44;
	static const int Err_16045_MissingTag = Mec_ErrorBase + 45;
	static const int Err_16046_LOAD_IO_FAILED =  Mec_ErrorBase + 46;
	static const int Err_16047_CreateOutputList =  Mec_ErrorBase + 47;
	static const int Err_16048_ErrrorCreatePPQObject  =  Mec_ErrorBase + 48;
	static const int Err_16049_ErrorMissingInpuTag  =  Mec_ErrorBase + 49;
	static const int Err_16050_ConstructObjectInspection  =  Mec_ErrorBase + 50;
	static const int Err_16051_ErrorMissingIOTypeTag =  Mec_ErrorBase + 51;
	static const int Err_16052_ErrorMissingItemNameTag =  Mec_ErrorBase + 52;
	static const int Err_16053_ErrorMissingIOTypeTag =  Mec_ErrorBase + 53; 
	static const int Err_16054_ErrorMissingPPQPOsitionTag =  Mec_ErrorBase + 54; 
	static const int Err_16055_ErrorMissingRemoteIndexTag =  Mec_ErrorBase + 55;
	static const int Err_16056_ErrorMissingPPQPOsitionTag =  Mec_ErrorBase + 56;
	static const int Err_16057_ErrorMissingItemNameTag =  Mec_ErrorBase + 57;
	static const int Err_16058_ErrorMissingRemoteIndexTag =  Mec_ErrorBase + 58;
	static const int Err_16059_ErrorMissingItemNameTag   =  Mec_ErrorBase + 59;
	static const int Err_16060_IOEntryIsMissing=  Mec_ErrorBase + 60;
	static const int Err_16061_EnviromentTagIsMissing =  Mec_ErrorBase + 61;
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
	///Avoid crash
	static const int 	Err_16077_EndlessLoop =  Mec_ErrorBase + 77;
	static const int 	Err_16078_InvalidHandle =  Mec_ErrorBase + 78;
	///Remove DOM 
	static const int 	Err_16079_ConfigurationTooOld =  Mec_ErrorBase + 79;
	///Replace SVerror 
	static const int 	Err_16080  =  Mec_ErrorBase + 80;
	static const int 	Err_16081  =  Mec_ErrorBase + 81;
	static const int 	Err_16082  =  Mec_ErrorBase + 82;
	static const int 	Err_16083  =  Mec_ErrorBase + 83;
	static const int 	Err_16084  =  Mec_ErrorBase + 84;
	static const int 	Err_16085  =  Mec_ErrorBase + 85;
	static const int 	Err_16086  =  Mec_ErrorBase + 86;
	static const int 	Err_16087  =  Mec_ErrorBase + 87;
	static const int 	Err_16088  =  Mec_ErrorBase + 88;
	static const int 	Err_16089  =  Mec_ErrorBase + 89;
	static const int 	Err_16090  =  Mec_ErrorBase + 90;
	static const int 	Err_16091  =  Mec_ErrorBase + 91;
	static const int 	Err_16092  =  Mec_ErrorBase + 92;
	static const int 	Err_16093  =  Mec_ErrorBase + 93;
	static const int 	Err_16094  =  Mec_ErrorBase + 94;
	static const int 	Err_16095  =  Mec_ErrorBase + 95;
	static const int 	Err_16096  =  Mec_ErrorBase + 96;
	static const int 	Err_16097  =  Mec_ErrorBase + 97;
	static const int 	Err_16098  =  Mec_ErrorBase + 98;
	static const int 	Err_16099  =  Mec_ErrorBase + 99;
	static const int 	Err_16100  =  Mec_ErrorBase + 100;
	static const int 	Err_16101  =  Mec_ErrorBase + 101;
	static const int 	Err_16102  =  Mec_ErrorBase + 102;
	static const int 	Err_16103  =  Mec_ErrorBase + 103;
	static const int 	Err_16104  =  Mec_ErrorBase + 104;
	static const int 	Err_16105  =  Mec_ErrorBase + 105;
	static const int 	Err_16106  =  Mec_ErrorBase + 106;
	static const int 	Err_16107  =  Mec_ErrorBase + 107;
	static const int 	Err_16108  =  Mec_ErrorBase + 108;
	static const int 	Err_16109  =  Mec_ErrorBase + 109;
	static const int 	Err_16110  =  Mec_ErrorBase + 110;
	static const int 	Err_16111  =  Mec_ErrorBase + 111;
	static const int 	Err_16112  =  Mec_ErrorBase + 112;
	static const int 	Err_16113  =  Mec_ErrorBase + 113;
	static const int 	Err_16114  =  Mec_ErrorBase + 114;
	static const int 	Err_16115  =  Mec_ErrorBase + 115;
	static const int 	Err_16116  =  Mec_ErrorBase + 116;
	static const int 	Err_16117  =  Mec_ErrorBase + 117;
	static const int 	Err_16118  =  Mec_ErrorBase + 118;
	static const int 	Err_16119  =  Mec_ErrorBase + 119;
	static const int 	Err_16120  =  Mec_ErrorBase + 120;
	static const int 	Err_16121  =  Mec_ErrorBase + 121;
	static const int 	Err_16122  =  Mec_ErrorBase + 122;
	static const int 	Err_16123  =  Mec_ErrorBase + 123;
	static const int 	Err_16124  =  Mec_ErrorBase + 124;
	static const int 	Err_16125  =  Mec_ErrorBase + 125;
	static const int 	Err_16126  =  Mec_ErrorBase + 126;
	static const int 	Err_16127  =  Mec_ErrorBase + 127;
	static const int 	Err_16128  =  Mec_ErrorBase + 128;
	static const int 	Err_16129  =  Mec_ErrorBase + 129;
	static const int 	Err_16130  =  Mec_ErrorBase + 130;
	static const int 	Err_16131  =  Mec_ErrorBase + 131;
	static const int 	Err_16132  =  Mec_ErrorBase + 132;
	static const int 	Err_16133  =  Mec_ErrorBase + 133;
	static const int 	Err_16134  =  Mec_ErrorBase + 134;
	static const int 	Err_16135  =  Mec_ErrorBase + 135;
	static const int 	Err_16136  =  Mec_ErrorBase + 136;
	static const int 	Err_16137  =  Mec_ErrorBase + 137;
	static const int 	Err_16138  =  Mec_ErrorBase + 138;
	static const int 	Err_16139  =  Mec_ErrorBase + 139;
	static const int 	Err_16140  =  Mec_ErrorBase + 140;
	static const int 	Err_16141  =  Mec_ErrorBase + 141;
	static const int 	Err_16142  =  Mec_ErrorBase + 142;
	static const int 	Err_16143  =  Mec_ErrorBase + 143;
	static const int 	Err_16144  =  Mec_ErrorBase + 144;
	static const int 	Err_16145  =  Mec_ErrorBase + 145;
	static const int 	Err_16148  =  Mec_ErrorBase + 148;
	static const int 	Err_16149  =  Mec_ErrorBase + 149;	
	static const int 	Err_16150  =  Mec_ErrorBase + 150;
	static const int 	Err_16151  =  Mec_ErrorBase + 151;
	static const int 	Err_16152  =  Mec_ErrorBase + 152;
	static const int 	Err_16153  =  Mec_ErrorBase + 153;
	static const int 	Err_16154  =  Mec_ErrorBase + 154;
	static const int 	Err_16155  =  Mec_ErrorBase + 155;
	static const int 	Err_16156  =  Mec_ErrorBase + 156;
	static const int 	Err_16157  =  Mec_ErrorBase + 157;
	static const int 	Err_16159  =  Mec_ErrorBase + 159;	
	static const int 	Err_16160  =  Mec_ErrorBase + 160;
	static const int 	Err_16161  =  Mec_ErrorBase + 161;
	static const int 	Err_16162  =  Mec_ErrorBase + 162;
	static const int 	Err_16163  =  Mec_ErrorBase + 163;
	static const int 	Err_16164  =  Mec_ErrorBase + 164;
	static const int 	Err_16165  =  Mec_ErrorBase + 165;
	static const int 	Err_16166  =  Mec_ErrorBase + 166;
	static const int 	Err_16167  =  Mec_ErrorBase + 167;
	static const int 	Err_16168  =  Mec_ErrorBase + 168;
	static const int 	Err_16169  =  Mec_ErrorBase + 169;	
	static const int 	Err_16170  =  Mec_ErrorBase + 170;
	static const int 	Err_16171  =  Mec_ErrorBase + 171;
	static const int 	Err_16172  =  Mec_ErrorBase + 172;
	static const int 	Err_16173  =  Mec_ErrorBase + 173;
	static const int 	Err_16174  =  Mec_ErrorBase + 174;
	static const int 	Err_16175  =  Mec_ErrorBase + 175;
	static const int 	Err_16176  =  Mec_ErrorBase + 176;
	static const int 	Err_16177  =  Mec_ErrorBase + 177;
	static const int 	Err_16178  =  Mec_ErrorBase + 178;
	static const int 	Err_16179  =  Mec_ErrorBase + 179;	
	static const int 	Err_16180  =  Mec_ErrorBase + 180;
	static const int 	Err_16181  =  Mec_ErrorBase + 181;
	static const int 	Err_16182  =  Mec_ErrorBase + 182;
	static const int 	Err_16183  =  Mec_ErrorBase + 183;
	static const int 	Err_16184  =  Mec_ErrorBase + 184;
	static const int 	Err_16185  =  Mec_ErrorBase + 185;
	static const int 	Err_16186  =  Mec_ErrorBase + 186;
	static const int 	Err_16187  =  Mec_ErrorBase + 187;
	static const int 	Err_16188  =  Mec_ErrorBase + 188;
	static const int 	Err_16189  =  Mec_ErrorBase + 189;	
	static const int 	Err_16190  =  Mec_ErrorBase + 190;
	static const int 	Err_16191  =  Mec_ErrorBase + 191;
	static const int 	Err_16192  =  Mec_ErrorBase + 192;
	static const int 	Err_16193  =  Mec_ErrorBase + 193;
	static const int 	Err_16194  =  Mec_ErrorBase + 194;
	static const int 	Err_16195  =  Mec_ErrorBase + 195;
	static const int 	Err_16196  =  Mec_ErrorBase + 196;
	static const int 	Err_16197  =  Mec_ErrorBase + 197;
	static const int 	Err_16198  =  Mec_ErrorBase + 198;
	static const int 	Err_16199  =  Mec_ErrorBase + 199;	

	static const int 	Err_16200  =  Mec_ErrorBase + 200;
	static const int 	Err_16201  =  Mec_ErrorBase + 201;
	static const int 	Err_16202  =  Mec_ErrorBase + 202;
	static const int 	Err_16203  =  Mec_ErrorBase + 203;
	static const int 	Err_16204  =  Mec_ErrorBase + 204;
	static const int 	Err_16205  =  Mec_ErrorBase + 205;
	static const int 	Err_16206  =  Mec_ErrorBase + 206;
	static const int 	Err_16207  =  Mec_ErrorBase + 207;
	static const int 	Err_16208  =  Mec_ErrorBase + 208;
	static const int 	Err_16209  =  Mec_ErrorBase + 209;	

	static const int 	Err_16210  =  Mec_ErrorBase + 210;
	static const int 	Err_16211  =  Mec_ErrorBase + 211;
	static const int 	Err_16212  =  Mec_ErrorBase + 212;
	static const int 	Err_16213  =  Mec_ErrorBase + 213;
	static const int 	Err_16214  =  Mec_ErrorBase + 214;
	static const int 	Err_16215  =  Mec_ErrorBase + 215;
	static const int 	Err_16216  =  Mec_ErrorBase + 216;
	static const int 	Err_16217  =  Mec_ErrorBase + 217;
	static const int 	Err_16218  =  Mec_ErrorBase + 218;
	static const int 	Err_16219  =  Mec_ErrorBase + 219;	

	static const int 	Err_16220_SaveImageBuffer   =  Mec_ErrorBase + 220;	

	static const int Err_16221_FailToCreateEvent   =  Mec_ErrorBase + 221;	
	static const int Err_16222_FailToCreateEvent   =  Mec_ErrorBase + 222;	
	///Replace REWORK SHARED MEMORY
	static const int Err_16223  =  Mec_ErrorBase + 223;	
	static const int Err_16224  =  Mec_ErrorBase + 224;	
	static const int Err_16225  =  Mec_ErrorBase + 225;	
	
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
	static const int Err_25013_NoMatroxDongle = Gra_ErrorBase + 13;
	static const int Err_25014_GlobalConstantNumber = Gra_ErrorBase + 14;
	static const int Err_25015_GlobalNameAlreadyUsed = Gra_ErrorBase + 15;
	static const int Err_25016_GlobalNameInvalid = Gra_ErrorBase + 16;
	static const int Err_25017_RootChildCreate = Gra_ErrorBase + 17;
	static const int Err_25018_Json_GetConfig = Gra_ErrorBase + 18;
	static const int Err_25019_Json_PutConfig = Gra_ErrorBase + 19;
	static const int Err_25020_MatroxLibraryNone = Gra_ErrorBase + 20;
	static const int Err_25021_MatroxLibraryFatal = Gra_ErrorBase + 21;
	static const int Err_25022_NonPagedMemoryFull = Gra_ErrorBase + 22;
	static const int Err_25023_NonPagedMemoryLow = Gra_ErrorBase + 23;
	static const int Err_25030_Thread = Gra_ErrorBase + 30;
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
	static const int Err_25049_DottedName = Gra_ErrorBase + 49;

	//Arvid Breitenbach error base
	static const int Abc_ErrorBase = 30000;
	static const int Err_30000_NullImageOnSave = Abc_ErrorBase + 0;

	//Robert Yoho error base
	static const int Rpy_ErrorBase = 45000;
	static const int Err_45000 = Rpy_ErrorBase + 0;
	static const int Memory_Log_45001 = Rpy_ErrorBase + 1;

} //namespace SvStl

