// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConfigurationTags
// * .File Name       : $Workfile:   SVConfigurationTags.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.8  $
// * .Check In Date   : $Date:   12 Feb 2015 13:42:02  $
// ******************************************************************************

#pragma once

namespace  SvXml
{
	constexpr char* CTAG_PPQ_MODE						 = (_T("PPQMode"));
	constexpr char* CTAG_UNIQUE_REFERENCE_Id            = (_T("UniqueReferenceId")); // @WARNING:  Differs only in capitalization from CTAG_UNIQUE_REFERENCE_ID.
	constexpr char* CTAG_UNIQUE_REFERENCE_ID            = (_T("UniqueReferenceID")); // Id
	constexpr char* CTAG_TRIGGER                        = (_T("Trigger"));
	constexpr char* CTAG_CAMERA                         = (_T("Camera"));
	constexpr char* CTAG_TRIGGER_DEVICE                 = (_T("TriggerDevice"));
	constexpr char* CTAG_SOFTWARETRIGGER_PERIOD			= (_T("SoftwareTriggerPeriod"));
	constexpr char* CTAG_START_OBJECT_ID				= (_T("StartObjectID"));
	constexpr char* CTAG_TRIGGER_PER_OBJECT_ID			= (_T("TriggerPerObjectID"));
	constexpr char* CTAG_LIGHT_REFERENCE_ARRAY          = (_T("LightReferenceArray"));
	constexpr char* CTAG_ACQUISITION_DEVICE_FILE_NAME   = (_T("AcquisitionDeviceFileName"));
	constexpr char* CTAG_TYPE                           = (_T("Type"));
	constexpr char* CTAG_BAND_LINK                      = (_T("BandLink"));
	constexpr char* CTAG_COLOR	                      = (_T("Color"));
	constexpr char* CTAG_ACQUISITION_DEVICE             = (_T("AcquisitionDevice"));
	constexpr char* CTAG_SIZE                           = (_T("Size"));
	constexpr char* CTAGF_BAND_X                        = (_T("Band_%d")); // CTAGF indicates formatting is required.
	constexpr char* CTAGF_LIGHTREFERENCE_X              = (_T("LightReference_%d")); // CTAGF indicates formatting is required.
	constexpr char* CTAG_NAME                           = (_T("Name"));
	constexpr char* CTAG_RESOURCE_ID                    = (_T("ResourceID"));
	constexpr char* CTAG_VALUE                          = (_T("Value"));
	constexpr char* CTAG_POSITION                       = (_T("Position"));
	constexpr char* CTAG_PPQ_POSITION                   = (_T("PPQPosition"));
	constexpr char* CTAG_REMOTE_INDEX                   = (_T("RemoteIndex"));
	constexpr char* CTAG_REMOTE_INITIAL_VALUE           = (_T("RemoteInitialValue"));
	constexpr char* CTAG_INPUT                          = (_T("Input"));
	constexpr char* CTAG_ATTRIBUTE                      = (_T("Attribute"));
	constexpr char* CTAG_PPQ_LENGTH                     = (_T("PPQLength"));
	constexpr char* CTAG_PPQ_MAINTAIN_SRC_IMAGE         = (_T("PPQMaintainSrcImage"));
	constexpr char* CTAG_PPQ_INSPECTION_TIMEOUT         = (_T("PPQInspectionTimeout"));
	constexpr char* CTAG_PPQ_OUTPUT_RESET_DELAY_TIME    = (_T("PPQOutputResetDelayTime"));
	constexpr char* CTAG_PPQ_CONDITIONAL_OUTPUT         = (_T("PPQConditonalOutput"));
	constexpr char* CTAG_PPQ_OUTPUT_DELAY_TIME          = (_T("PPQOutputDelayTime"));
	constexpr char* CTAG_PPQ_MAX_PROCESSINGOFFSET_4INTEREST	= (_T("PPQMaxProcessingOffsetGap4Interest"));
	constexpr char* CTAG_INSPECTION_FILE_NAME           = (_T("InspectionFileName"));
	constexpr char* CTAG_INSPECTION_TOOLSET_IMAGE       = (_T("ToolsetImage"));
	constexpr char* CTAG_INSPECTION_NEW_DISABLE_METHOD  = (_T("NewDisableMethod"));
	constexpr char* CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT  = (_T("EnableAuxiliaryExtent"));
	constexpr char* CTAG_INSPECTION                     = (_T("Inspection"));
	constexpr char* CTAG_NUMBER_OF_IO_ENTRIES           = (_T("NumberOfIOEntries"));
	constexpr char* CTAGF_IO_ENTRY_X                    = (_T("IOEntry%d")); // CTAGF indicates formatting is required.
	constexpr char* CTAG_IO_ENTRY_NAME                  = (_T("IOEntryName"));
	constexpr char* CTAG_IS_OUTPUT                      = (_T("IsOutput"));
	constexpr char* CTAG_CHANNEL                        = (_T("Channel"));
	constexpr char* CTAG_IS_FORCED                      = (_T("IsForced"));
	constexpr char* CTAG_FORCED_VALUE                   = (_T("ForcedValue"));
	constexpr char* CTAG_IS_INVERTED                    = (_T("IsInverted"));
	constexpr char* CTAG_IS_COMBINED                    = (_T("IsCombined"));
	constexpr char* CTAG_IS_COMBINED_ACK                = (_T("IsCombinedACK"));
	constexpr char* CTAG_SVIPDOC                        = (_T("SVIPDoc"));
	constexpr char* CTAG_CONDITIONAL_HISTORY            = (_T("ConditionalHistory"));
	constexpr char* CTAG_IPDOC_VIEWS                    = (_T("IPDocViews"));
	constexpr char* CTAG_HEIGHT_RESULT_VIEW             = (_T("HeightResultView"));
	constexpr char* CTAG_WIDTH_TOOLSET_VIEW             = (_T("WidthToolsetView"));
	constexpr char* CTAG_WINDOW_PLACEMENT               = (_T("WindowPlacement"));
	constexpr char* CTAG_TOOLSET_SCRIPT                 = (_T("ToolsetScript"));
	constexpr char* CTAG_VIEWS                          = (_T("Views"));
	constexpr char* CTAG_VIEW_NUMBER                    = (_T("ViewNumber"));
	constexpr char* CTAG_SVTOOLSET_TAB_VIEW_CLASS       = (_T("SVToolSetTabViewClass"));
	constexpr char* CTAG_TOOLSET_VIEW                   = (_T("ToolSetView"));
	constexpr char* CTAG_IMAGE_VIEW                     = (_T("ImageView"));
	constexpr char* CTAG_SCROLL_SIZE                    = (_T("ScrollSize"));
	constexpr char* CTAG_SCROLL_POINT                   = (_T("ScrollPoint"));
	constexpr char* CTAG_CELL_HEIGHT                    = (_T("CellHeight"));
	constexpr char* CTAG_CELL_WIDTH                     = (_T("CellWidth"));
	constexpr char* CTAG_IMAGE_ZOOM_FACTOR              = (_T("ImageZoomFactor"));
	constexpr char* CTAG_IMAGE_ZOOM_FACTOR_EX           = (_T("ImageZoomFactorEx"));
	constexpr char* CTAG_VIEW_INITIALIZED               = (_T("ViewInitialized"));
	constexpr char* CTAG_IMAGE_NAME                     = (_T("ImageName"));
	constexpr char* CTAG_LENGTH                         = (_T("Length"));
	constexpr char* CTAG_FLAGS                          = (_T("Flags"));
	constexpr char* CTAG_SHOW_COMMAND                   = (_T("ShowCommand"));
	constexpr char* CTAG_MINIMUM_POSITION               = (_T("MinimumPosition"));
	constexpr char* CTAG_MAXIMUM_POSITION               = (_T("MaximumPosition"));
	constexpr char* CTAG_NORMAL_POSITION                = (_T("NormalPosition"));
	constexpr char* CTAG_X                              = (_T("X"));
	constexpr char* CTAG_Y                              = (_T("Y"));
	constexpr char* CTAG_LEFT                           = (_T("Left"));
	constexpr char* CTAG_TOP                            = (_T("Top"));
	constexpr char* CTAG_RIGHT                          = (_T("Right"));
	constexpr char* CTAG_BOTTOM                         = (_T("Bottom"));
	constexpr char* CTAG_CX                             = (_T("CX"));
	constexpr char* CTAG_CY                             = (_T("CY"));
	constexpr char* CTAG_ENVIRONMENT                    = (_T("Environment"));
	constexpr char* CTAG_PPQ                            = (_T("PPQ"));
	constexpr char* CTAG_IO                             = (_T("IO"));
	constexpr char* CTAG_VIEWED_INPUTS                  = (_T("ViewedInputs"));
	constexpr char* CTAG_MONITOR_LISTS                  = (_T("MonitorLists"));
	constexpr char* CTAG_PRODUCTVALUES_LIST             = (_T("ProductValuesList"));
	constexpr char* CTAG_PRODUCTIMAGE_LIST              = (_T("ProductImageList"));
	constexpr char* CTAG_REJECTCONDITION_LIST           = (_T("RejectConditionList"));
	constexpr char* CTAG_FAILSTATUS_LIST                = (_T("FailStatusList"));
	constexpr char* CTAG_PPQ_NAME                       = (_T("PPQName"));
	constexpr char* CTAG_REJECT_QUEUE_DEPTH             = (_T("RejectQueueDepth"));
	constexpr char* CTAG_IS_ACTIVATED						= (_T("IsActivated"));
	constexpr char* CTAG_VERSION_NUMBER                 = (_T("VersionNumber"));
	constexpr char* CTAGF_INPUT_X                       = (_T("Input_%d")); // CTAGF indicates formatting is required.
	constexpr char* CTAG_ITEM_NAME                      = (_T("ItemName"));
	constexpr char* CTAG_IO_TYPE                        = (_T("IO_Type"));
	constexpr char* CTAG_DESCRIPTION		              = _T("Description");
	//CTAG_ONLINE_DISPLAY kept for backward compatibility
	constexpr char* CTAG_ONLINE_DISPLAY                 = (_T("Online_Display"));
	constexpr char* CTAG_IMAGE_DISPLAY_UPDATE           = (_T("Image_Display_Update"));
	constexpr char* CTAG_RESULT_DISPLAY_UPDATE          = (_T("Result_Display_Update"));

	constexpr char* CTAG_LUT                            = _T("LUT");
	constexpr char* CTAG_LUT_TRANSFORM_OPERATION        = _T("LUTTransformOperation");
	constexpr char* CTAG_LUT_BAND_DATA                  = _T("LutBandData");
	constexpr char* CTAG_LUT_TRANSFORM_PARAMETERS       = _T("LutTransformParameters");

	constexpr char* CTAG_CONFIGURATION_TYPE             = _T("ConfigurationType");

	constexpr char* CTAG_DEVICE_PARAM_LIST              = _T("DeviceParamList");
	constexpr char* CTAGF_DEVICE_PARAM_X                = _T("DeviceParam_%d"); // CTAGF indicates formatting is required.
	constexpr char* CTAG_DEVICE_PARAM_TYPE              = _T("DeviceParamType");
	constexpr char* CTAG_DEVICE_PARAM_VALUE             = _T("DeviceParamValue");
	constexpr char* CTAG_DEVICE_PARAM_UNITS             = _T("DeviceParamUnits");
	constexpr char* CTAG_OPTIONS                        = _T("Options");
	constexpr char* CTAGF_OPTION_X                      = _T("Option_%d"); // CTAGF indicates formatting is required.

	constexpr char* CTAG_VARIABLE_CAMERA_FORMAT         = _T("VariableCameraFormat");
	constexpr char* CTAG_CAMERA_ID				      = _T("CameraID");

	constexpr char* CTAG_FILEACQUISITION_MODE					= _T("FileAcquisitionMode");
	constexpr char* CTAG_FILEACQUISITION_IMAGE_FILENAME		= _T("ImageFilename");
	constexpr char* CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME	= _T("ImageDirectory");
	constexpr char* CTAG_FILEACQUISITION_LOADINGMODE			= _T("LoadingMode");
	constexpr char* CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE	= _T("ImageSizeEditMode");
	constexpr char* CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH		= _T("ImageWidth");
	constexpr char* CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT	= _T("ImageHeight");

	constexpr char* CTAG_MAX_VALUE                      = _T("MaxValue");

	constexpr char* CTAG_IODOC                          = _T("IODoc");

	constexpr char* CTAG_REMOTE_OUTPUT_PARAMETERS       = _T("RemoteOutputParameters");
	constexpr char* CTAG_REMOTE_OUTPUT_INPUT_OBJECT_ID = _T("RemoteOutputObjectId");
	constexpr char* CTAG_REMOTE_OUTPUT_NAME             = _T("RemoteOutputInputName");
	constexpr char* CTAG_REMOTE_GROUP_ID                = _T("RemoteOutputGroupID");
	constexpr char* CTAG_REMOTE_OUTPUT_PAR_NUM          = _T("RemoteOutputParNumber");
	constexpr char* CTAG_REMOTE_OUTPUT_ENTRY            = _T("RemoteOutputEntry");
	constexpr char* CTAG_REMOTE_GROUP_PPQ               = _T("RemoteGroupPPQ");
	constexpr char* CTAG_REMOTE_OUTPUT_QUEUE_SIZE       = _T("RemoteGroupQueueSize");

	constexpr char* CTAG_INSPECTION_PROCESS             = _T("Inspection Process");
	constexpr char* CTAG_TOOLSET_SET                    = _T("Tool Set");
	constexpr char* CTAG_DEPENDENT_FILES                = _T("Dependent Files");
	constexpr char* CTAG_FILENAME                       = _T("Filename");

	constexpr char* CTAG_TOOLGROUPINGS                  = _T("ToolGroupings");
	constexpr char* CTAG_GROUP                          = _T("Group");
	constexpr char* CTAG_STARTGROUP                     = _T("StartGroup");
	constexpr char* CTAG_TOOLS                          = _T("Tools");
	constexpr char* CTAG_TOOL                           = _T("Tool");
	constexpr char* CTAG_ENDGROUP                       = _T("EndGroup");
	constexpr char* CTAG_COLLAPSED                      = _T("Collapsed");
	constexpr char* CTAG_STARTGROUP_COMMENT             = _T("StartGroupComment");
	constexpr char* CTAG_ENDGROUP_COMMENT               = _T("EndGroupComment");

	constexpr char* CTAG_THREAD_SETUP                   = _T("Thread Setup");
	constexpr char* CTAG_THREAD_NAME                    = _T("Thread Name");
	constexpr char* CTAG_THREAD_AFFINITY                = _T("Thread Affinity");
	constexpr char* CTAG_THREAD_MGR_ENABLE              = _T("Thread Mgr Enable");

	constexpr char* CTAG_NEXT_OBJECT_ID					= _T("Next ObjectId");

	constexpr char* CTAG_VIEWEDVARIABLES                = _T("ViewedVariables");
	constexpr char* CTAG_VIEWEDENVARIABLES              = _T("ViewedEnvVariables");
	constexpr char* CTAG_VIEWEDTOOLVARIABLES            = _T("ViewedToolVariables");
	constexpr char* CTAG_VIEWEDRESULTS				= _T("ViewedResults");
	constexpr char* CTAG_VIEWEDPPQINPUTS                = _T("ViewedPPQInputs");
	constexpr char* CTAG_COMPLETENAME                   = _T("CompleteName"); 
	constexpr char* CTAG_VIEWEDTABLE					= _T("ViewedTable");

	constexpr char* CTAG_GLOBAL_CONSTANTS               = _T("GlobalConstants");
	constexpr char* CTAG_REGRESSIONTEST = _T("RegressionTest");
	constexpr char* CTAG_USE_PLAY_CONDITION = _T("UsePlayCondition");
	constexpr char* CTAG_PLAY_CONDITION_EQUATION = _T("PlayConditionEquation");

	constexpr char* CTAG_OBJECT_ATTRIBUTES_SET = _T("ObjectAttributesSet");

	constexpr char* CTAG_ADDITIONAL_CONFIG_FILES = _T("AdditionalConfigFiles");

	constexpr char* CTAG_AUDDID_DEFAULT_LIST = _T("AuditDefaultList");
	constexpr char* CTAG_AUDDID_WHITE_LIST = _T("AuditWhiteList");

	constexpr char* CTAG_PRE_RUN_EXECUTION = _T("PreRunExecution");
	constexpr char* CTAG_POST_RUN_EXECUTION = _T("PostRunExecution");

	/// These are tool copy / paste tags
	constexpr char* ToolCopyTag = _T( "Tool-Copy" );
	constexpr char* ToolsTag = _T( "Tools" );
	constexpr char* ToolTypeTag = _T( "ToolType" );
	constexpr char* InputImageTag = _T("InputImage%d");
	constexpr char* FullToolNameTag = _T("FullToolName");
	constexpr char* InspectionNameTag = _T("InspectionName");
	constexpr char* BaseTag = _T( "Base" );
	constexpr char* TypeTag = _T( "Type" );
	constexpr char* DataTag = _T( "</DATA>" );


	constexpr char* CTAG_CALCULATE_HASH = _T("CalculateHash");
	constexpr char* CTAG_IGNORE_FILE = _T("IgnoreFile");
	constexpr char* CTAGF_FILE_X = _T("File_%d");
} //namespace SvXml
