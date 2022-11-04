// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	constexpr const char* CTAG_PPQ_MODE						 = (_T("PPQMode"));
	constexpr const char* CTAG_UNIQUE_REFERENCE_ID            = (_T("UniqueReferenceID")); // Id
	constexpr const char* CTAG_TRIGGER                        = (_T("Trigger"));
	constexpr const char* CTAG_CAMERA                         = (_T("Camera"));
	constexpr const char* CTAG_TRIGGER_DEVICE                 = (_T("TriggerDevice"));
	constexpr const char* CTAG_SOFTWARETRIGGER_PERIOD			= (_T("SoftwareTriggerPeriod"));
	constexpr const char* CTAG_START_OBJECT_ID				= (_T("StartObjectID"));
	constexpr const char* CTAG_TRIGGER_PER_OBJECT_ID			= (_T("TriggerPerObjectID"));
	constexpr const char* CTAG_OBJECT_ID_COUNT					= (_T("ObjectIDCount"));
	constexpr const char* CTAG_LIGHT_REFERENCE_ARRAY          = (_T("LightReferenceArray"));
	constexpr const char* CTAG_ACQUISITION_DEVICE_FILE_NAME   = (_T("AcquisitionDeviceFileName"));
	constexpr const char* CTAG_TYPE                           = (_T("Type"));
	constexpr const char* CTAG_BAND_LINK                      = (_T("BandLink"));
	constexpr const char* CTAG_COLOR	                      = (_T("Color"));
	constexpr const char* CTAG_ACQUISITION_DEVICE             = (_T("AcquisitionDevice"));
	constexpr const char* CTAG_SIZE                           = (_T("Size"));
	constexpr const char* CTAGF_BAND_X                        = (_T("Band_%d")); // CTAGF indicates formatting is required.
	constexpr const char* CTAGF_LIGHTREFERENCE_X              = (_T("LightReference_%d")); // CTAGF indicates formatting is required.
	constexpr const char* CTAG_NAME                           = (_T("Name"));
	constexpr const char* CTAG_RESOURCE_ID                    = (_T("ResourceID"));
	constexpr const char* CTAG_VALUE                          = (_T("Value"));
	constexpr const char* CTAG_POSITION                       = (_T("Position"));
	constexpr const char* CTAG_PPQ_POSITION                   = (_T("PPQPosition"));
	constexpr const char* CTAG_REMOTE_INITIAL_VALUE           = (_T("RemoteInitialValue"));
	constexpr const char* CTAG_INPUT                          = (_T("Input"));
	constexpr const char* CTAG_ATTRIBUTE                      = (_T("Attribute"));
	constexpr const char* CTAG_PPQ_LENGTH                     = (_T("PPQLength"));
	constexpr const char* CTAG_PPQ_MAINTAIN_SRC_IMAGE         = (_T("PPQMaintainSrcImage"));
	constexpr const char* CTAG_PPQ_INSPECTION_TIMEOUT         = (_T("PPQInspectionTimeout"));
	constexpr const char* CTAG_PPQ_OUTPUT_RESET_DELAY_TIME    = (_T("PPQOutputResetDelayTime"));
	constexpr const char* CTAG_PPQ_CONDITIONAL_OUTPUT         = (_T("PPQConditonalOutput"));
	constexpr const char* CTAG_PPQ_OUTPUT_DELAY_TIME          = (_T("PPQOutputDelayTime"));
	constexpr const char* CTAG_PPQ_MAX_PROCESSINGOFFSET_4INTEREST	= (_T("PPQMaxProcessingOffsetGap4Interest"));
	constexpr const char* CTAG_INSPECTION_FILE_NAME           = (_T("InspectionFileName"));
	constexpr const char* CTAG_INSPECTION_TOOLSET_IMAGE       = (_T("ToolsetImage"));
	constexpr const char* CTAG_INSPECTION_NEW_DISABLE_METHOD  = (_T("NewDisableMethod"));
	constexpr const char* CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT  = (_T("EnableAuxiliaryExtent"));
	constexpr const char* CTAG_INSPECTION                     = (_T("Inspection"));
	constexpr const char* CTAG_NUMBER_OF_IO_ENTRIES           = (_T("NumberOfIOEntries"));
	constexpr const char* CTAGF_IO_ENTRY_X                    = (_T("IOEntry%d")); // CTAGF indicates formatting is required.
	constexpr const char* CTAG_IO_ENTRY_NAME                  = (_T("IOEntryName"));
	constexpr const char* CTAG_IS_OUTPUT                      = (_T("IsOutput"));
	constexpr const char* CTAG_CHANNEL                        = (_T("Channel"));
	constexpr const char* CTAG_IS_FORCED                      = (_T("IsForced"));
	constexpr const char* CTAG_FORCED_VALUE                   = (_T("ForcedValue"));
	constexpr const char* CTAG_IS_INVERTED                    = (_T("IsInverted"));
	constexpr const char* CTAG_IS_COMBINED                    = (_T("IsCombined"));
	constexpr const char* CTAG_IS_COMBINED_ACK                = (_T("IsCombinedACK"));
	constexpr const char* CTAG_SVIPDOC                        = (_T("SVIPDoc"));
	constexpr const char* CTAG_CONDITIONAL_HISTORY            = (_T("ConditionalHistory"));
	constexpr const char* CTAG_IPDOC_VIEWS                    = (_T("IPDocViews"));
	constexpr const char* CTAG_HEIGHT_RESULT_VIEW             = (_T("HeightResultView"));
	constexpr const char* CTAG_WIDTH_TOOLSET_VIEW             = (_T("WidthToolsetView"));
	constexpr const char* CTAG_WINDOW_PLACEMENT               = (_T("WindowPlacement"));
	constexpr const char* CTAG_TOOLSET_SCRIPT                 = (_T("ToolsetScript"));
	constexpr const char* CTAG_VIEWS                          = (_T("Views"));
	constexpr const char* CTAG_VIEW_NUMBER                    = (_T("ViewNumber"));
	constexpr const char* CTAG_SVTOOLSET_TAB_VIEW_CLASS       = (_T("SVToolSetTabViewClass"));
	constexpr const char* CTAG_TOOLSET_VIEW                   = (_T("ToolSetView"));
	constexpr const char* CTAG_IMAGE_VIEW                     = (_T("ImageView"));
	constexpr const char* CTAG_SCROLL_SIZE                    = (_T("ScrollSize"));
	constexpr const char* CTAG_SCROLL_POINT                   = (_T("ScrollPoint"));
	constexpr const char* CTAG_CELL_HEIGHT                    = (_T("CellHeight"));
	constexpr const char* CTAG_CELL_WIDTH                     = (_T("CellWidth"));
	constexpr const char* CTAG_IMAGE_ZOOM_FACTOR              = (_T("ImageZoomFactor"));
	constexpr const char* CTAG_IMAGE_ZOOM_FACTOR_EX           = (_T("ImageZoomFactorEx"));
	constexpr const char* CTAG_VIEW_INITIALIZED               = (_T("ViewInitialized"));
	constexpr const char* CTAG_IMAGE_NAME                     = (_T("ImageName"));
	constexpr const char* CTAG_LENGTH                         = (_T("Length"));
	constexpr const char* CTAG_FLAGS                          = (_T("Flags"));
	constexpr const char* CTAG_SHOW_COMMAND                   = (_T("ShowCommand"));
	constexpr const char* CTAG_MINIMUM_POSITION               = (_T("MinimumPosition"));
	constexpr const char* CTAG_MAXIMUM_POSITION               = (_T("MaximumPosition"));
	constexpr const char* CTAG_NORMAL_POSITION                = (_T("NormalPosition"));
	constexpr const char* CTAG_X                              = (_T("X"));
	constexpr const char* CTAG_Y                              = (_T("Y"));
	constexpr const char* CTAG_LEFT                           = (_T("Left"));
	constexpr const char* CTAG_TOP                            = (_T("Top"));
	constexpr const char* CTAG_RIGHT                          = (_T("Right"));
	constexpr const char* CTAG_BOTTOM                         = (_T("Bottom"));
	constexpr const char* CTAG_CX                             = (_T("CX"));
	constexpr const char* CTAG_CY                             = (_T("CY"));
	constexpr const char* CTAG_ENVIRONMENT                    = (_T("Environment"));
	constexpr const char* CTAG_PPQ                            = (_T("PPQ"));
	constexpr const char* CTAG_IO                             = (_T("IO"));
	constexpr const char* CTAG_VIEWED_INPUTS                  = (_T("ViewedInputs"));
	constexpr const char* CTAG_MONITOR_LISTS                  = (_T("MonitorLists"));
	constexpr const char* CTAG_PRODUCTVALUES_LIST             = (_T("ProductValuesList"));
	constexpr const char* CTAG_PRODUCTIMAGE_LIST              = (_T("ProductImageList"));
	constexpr const char* CTAG_REJECTCONDITION_LIST           = (_T("RejectConditionList"));
	constexpr const char* CTAG_FAILSTATUS_LIST                = (_T("FailStatusList"));
	constexpr const char* CTAG_PPQ_NAME                       = (_T("PPQName"));
	constexpr const char* CTAG_REJECT_QUEUE_DEPTH             = (_T("RejectQueueDepth"));
	constexpr const char* CTAG_IS_ACTIVATED					  = (_T("IsActivated"));
	constexpr const char* CTAG_VERSION_NUMBER                 = (_T("VersionNumber"));
	constexpr const char* CTAGF_INPUT_X                       = (_T("Input_%d")); // CTAGF indicates formatting is required.
	constexpr const char* CTAG_ITEM_NAME                      = (_T("ItemName"));
	constexpr const char* CTAG_IO_TYPE                        = (_T("IO_Type"));
	constexpr const char* CTAG_DESCRIPTION		              = _T("Description");
	//CTAG_ONLINE_DISPLAY kept for backward compatibility
	constexpr const char* CTAG_ONLINE_DISPLAY                 = (_T("Online_Display"));
	constexpr const char* CTAG_IMAGE_DISPLAY_UPDATE           = (_T("Image_Display_Update"));
	constexpr const char* CTAG_RESULT_DISPLAY_UPDATE          = (_T("Result_Display_Update"));

	constexpr const char* CTAG_LUT                            = _T("LUT");
	constexpr const char* CTAG_LUT_TRANSFORM_OPERATION        = _T("LUTTransformOperation");
	constexpr const char* CTAG_LUT_BAND_DATA                  = _T("LutBandData");
	constexpr const char* CTAG_LUT_TRANSFORM_PARAMETERS       = _T("LutTransformParameters");

	constexpr const char* CTAG_CONFIGURATION_TYPE             = _T("ConfigurationType");

	constexpr const char* CTAG_DEVICE_PARAM_LIST              = _T("DeviceParamList");
	constexpr const char* CTAGF_DEVICE_PARAM_X                = _T("DeviceParam_%d"); // CTAGF indicates formatting is required.
	constexpr const char* CTAG_DEVICE_PARAM_TYPE              = _T("DeviceParamType");
	constexpr const char* CTAG_DEVICE_PARAM_VALUE             = _T("DeviceParamValue");
	constexpr const char* CTAG_DEVICE_PARAM_UNITS             = _T("DeviceParamUnits");
	constexpr const char* CTAG_OPTIONS                        = _T("Options");
	constexpr const char* CTAGF_OPTION_X                      = _T("Option_%d"); // CTAGF indicates formatting is required.

	constexpr const char* CTAG_VARIABLE_CAMERA_FORMAT         = _T("VariableCameraFormat");
	constexpr const char* CTAG_CAMERA_ID				      = _T("CameraID");

	constexpr const char* CTAG_FILEACQUISITION_MODE					= _T("FileAcquisitionMode");
	constexpr const char* CTAG_FILEACQUISITION_IMAGE_FILENAME		= _T("ImageFilename");
	constexpr const char* CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME	= _T("ImageDirectory");
	constexpr const char* CTAG_FILEACQUISITION_LOADINGMODE			= _T("LoadingMode");
	constexpr const char* CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE	= _T("ImageSizeEditMode");
	constexpr const char* CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH		= _T("ImageWidth");
	constexpr const char* CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT	= _T("ImageHeight");

	constexpr const char* CTAG_MAX_VALUE                      = _T("MaxValue");

	constexpr const char* CTAG_IODOC                          = _T("IODoc");

	constexpr const char* CTAG_REMOTE_OUTPUT_PARAMETERS       = _T("RemoteOutputParameters");
	constexpr const char* CTAG_REMOTE_OUTPUT_INPUT_OBJECT_ID = _T("RemoteOutputObjectId");
	constexpr const char* CTAG_REMOTE_OUTPUT_NAME             = _T("RemoteOutputInputName");
	constexpr const char* CTAG_REMOTE_GROUP_ID                = _T("RemoteOutputGroupID");
	constexpr const char* CTAG_REMOTE_OUTPUT_PAR_NUM          = _T("RemoteOutputParNumber");
	constexpr const char* CTAG_REMOTE_OUTPUT_ENTRY            = _T("RemoteOutputEntry");
	constexpr const char* CTAG_REMOTE_GROUP_PPQ               = _T("RemoteGroupPPQ");
	constexpr const char* CTAG_REMOTE_OUTPUT_QUEUE_SIZE       = _T("RemoteGroupQueueSize");

	constexpr const char* CTAG_INSPECTION_PROCESS             = _T("Inspection Process");
	constexpr const char* CTAG_TOOLSET_SET                    = _T("Tool Set");
	constexpr const char* CTAG_DEPENDENT_FILES                = _T("Dependent Files");
	constexpr const char* CTAG_FILENAME                       = _T("Filename");

	constexpr const char* CTAG_TOOLGROUPINGS                  = _T("ToolGroupings");
	constexpr const char* CTAG_TOOLCOLLAPSED				  = _T("ToolCollapsed");
	constexpr const char* CTAG_GROUP                          = _T("Group");
	constexpr const char* CTAG_STARTGROUP                     = _T("StartGroup");
	constexpr const char* CTAG_TOOLS                          = _T("Tools");
	constexpr const char* CTAG_TOOL                           = _T("Tool");
	constexpr const char* CTAG_ENDGROUP                       = _T("EndGroup");
	constexpr const char* CTAG_COLLAPSED                      = _T("Collapsed");
	constexpr const char* CTAG_STARTGROUP_COMMENT             = _T("StartGroupComment");
	constexpr const char* CTAG_ENDGROUP_COMMENT               = _T("EndGroupComment");

	constexpr const char* CTAG_NEXT_OBJECT_ID					= _T("Next ObjectId");

	constexpr const char* CTAG_VIEWEDTABLE					= _T("ViewedTable");
	constexpr const char* CTAG_VIEWEDVARIABLES                = _T("ViewedVariables");
	constexpr const char* CTAG_VIEWEDENVARIABLES              = _T("ViewedEnvVariables");
	constexpr const char* CTAG_VIEWEDTOOLVARIABLES            = _T("ViewedToolVariables");
	constexpr const char* CTAG_VIEWEDRESULTS				= _T("ViewedResults");
	constexpr const char* CTAG_VIEWEDPPQINPUTS                = _T("ViewedPPQInputs");
	constexpr const char* CTAG_COMPLETENAME                   = _T("CompleteName"); 

	constexpr const char* CTAG_GLOBAL_CONSTANTS               = _T("GlobalConstants");
	constexpr const char* CTAG_REGRESSIONTEST = _T("RegressionTest");
	constexpr const char* CTAG_USE_PLAY_CONDITION = _T("UsePlayCondition");
	constexpr const char* CTAG_PLAY_CONDITION_EQUATION = _T("PlayConditionEquation");

	constexpr const char* CTAG_ADDITIONAL_CONFIG_FILES = _T("AdditionalConfigFiles");

	constexpr const char* CTAG_AUDDID_DEFAULT_LIST = _T("AuditDefaultList");
	constexpr const char* CTAG_AUDDID_WHITE_LIST = _T("AuditWhiteList");

	constexpr const char* CTAG_PRE_RUN_EXECUTION = _T("PreRunExecution");
	constexpr const char* CTAG_POST_RUN_EXECUTION = _T("PostRunExecution");

	/// These are tool copy / paste tags
	constexpr const char* ToolCopyTag = _T( "Tool-Copy" );
	constexpr const char* ToolsTag = _T( "Tools" );
	constexpr const char* ToolDepthTag = _T("ToolDepth");
	constexpr const char* ClassIdTag = _T("ClassID");
	constexpr const char* FullToolNameTag = _T("FullToolName%d");
	constexpr const char* BaseTag = _T( "Base" );
	constexpr const char* TypeTag = _T( "Type" );
	constexpr const char* DataTag = _T( "</DATA>" );

	constexpr const char* CTAG_CALCULATE_HASH = _T("CalculateHash");
	constexpr const char* CTAG_IGNORE_FILE = _T("IgnoreFile");
	constexpr const char* CTAGF_FILE_X = _T("File_%d");

	constexpr const char* cDigitalType = _T("Digital");
	constexpr const char* cRemoteType = _T("Remote");
} //namespace SvXml
