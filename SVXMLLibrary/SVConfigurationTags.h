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
	LPCTSTR const CTAG_PPQ_MODE = (_T("PPQMode"));
	LPCTSTR const CTAG_UNIQUE_REFERENCE_Id            = (_T("UniqueReferenceId")); // @WARNING:  Differs only in capitalization from CTAG_UNIQUE_REFERENCE_ID.
	LPCTSTR const CTAG_UNIQUE_REFERENCE_ID            = (_T("UniqueReferenceID")); // Id
	LPCTSTR const CTAG_TRIGGER                        = (_T("Trigger"));
	LPCTSTR const CTAG_CAMERA                         = (_T("Camera"));
	LPCTSTR const CTAG_TRIGGER_DEVICE                 = (_T("TriggerDevice"));
	LPCTSTR const CTAG_SOFTWARETRIGGER_DEVICE         = (_T("SoftwareTrigger"));
	LPCTSTR const CTAG_SOFTWARETRIGGER_PERIOD         = (_T("SoftwareTriggerPeriod"));
	LPCTSTR const CTAG_LIGHT_REFERENCE_ARRAY          = (_T("LightReferenceArray"));
	LPCTSTR const CTAG_ACQUISITION_DEVICE_FILE_NAME   = (_T("AcquisitionDeviceFileName"));
	LPCTSTR const CTAG_TYPE                           = (_T("Type"));
	LPCTSTR const CTAG_BAND_LINK                      = (_T("BandLink"));
	LPCTSTR const CTAG_COLOR	                      = (_T("Color"));
	LPCTSTR const CTAG_ACQUISITION_DEVICE             = (_T("AcquisitionDevice"));
	LPCTSTR const CTAG_SIZE                           = (_T("Size"));
	LPCTSTR const CTAGF_BAND_X                        = (_T("Band_%d")); // CTAGF indicates formatting is required.
	LPCTSTR const CTAGF_LIGHTREFERENCE_X              = (_T("LightReference_%d")); // CTAGF indicates formatting is required.
	LPCTSTR const CTAG_NAME                           = (_T("Name"));
	LPCTSTR const CTAG_RESOURCE_ID                    = (_T("ResourceID"));
	LPCTSTR const CTAG_VALUE                          = (_T("Value"));
	LPCTSTR const CTAG_POSITION                       = (_T("Position"));
	LPCTSTR const CTAG_PPQ_POSITION                   = (_T("PPQPosition"));
	LPCTSTR const CTAG_REMOTE_INDEX                   = (_T("RemoteIndex"));
	LPCTSTR const CTAG_REMOTE_INITIAL_VALUE           = (_T("RemoteInitialValue"));
	LPCTSTR const CTAG_INPUT                          = (_T("Input"));
	LPCTSTR const CTAG_ATTRIBUTE                      = (_T("Attribute"));
	LPCTSTR const CTAG_PPQ_LENGTH                     = (_T("PPQLength"));
	LPCTSTR const CTAG_PPQ_MAINTAIN_SRC_IMAGE         = (_T("PPQMaintainSrcImage"));
	LPCTSTR const CTAG_PPQ_INSPECTION_TIMEOUT         = (_T("PPQInspectionTimeout"));
	LPCTSTR const CTAG_PPQ_OUTPUT_RESET_DELAY_TIME    = (_T("PPQOutputResetDelayTime"));
	LPCTSTR const CTAG_PPQ_TRIGGER_COUNT_ID           = (_T("PPQTriggerCountID"));
	LPCTSTR const CTAG_PPQ_CONDITIONAL_OUTPUT         = (_T("PPQConditonalOutput"));
	LPCTSTR const CTAG_PPQ_OUTPUT_DELAY_TIME          = (_T("PPQOutputDelayTime"));
	LPCTSTR const CTAG_INSPECTION_FILE_NAME           = (_T("InspectionFileName"));
	LPCTSTR const CTAG_INSPECTION_TOOLSET_IMAGE       = (_T("ToolsetImage"));
	LPCTSTR const CTAG_INSPECTION_NEW_DISABLE_METHOD  = (_T("NewDisableMethod"));
	LPCTSTR const CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT  = (_T("EnableAuxiliaryExtent"));
	LPCTSTR const CTAG_INSPECTION                     = (_T("Inspection"));
	LPCTSTR const CTAG_NUMBER_OF_IO_ENTRIES           = (_T("NumberOfIOEntries"));
	LPCTSTR const CTAGF_IO_ENTRY_X                    = (_T("IOEntry%d")); // CTAGF indicates formatting is required.
	LPCTSTR const CTAG_IO_ENTRY_NAME                  = (_T("IOEntryName"));
	LPCTSTR const CTAG_IS_OUTPUT                      = (_T("IsOutput"));
	LPCTSTR const CTAG_CHANNEL                        = (_T("Channel"));
	LPCTSTR const CTAG_IS_FORCED                      = (_T("IsForced"));
	LPCTSTR const CTAG_FORCED_VALUE                   = (_T("ForcedValue"));
	LPCTSTR const CTAG_IS_INVERTED                    = (_T("IsInverted"));
	LPCTSTR const CTAG_IS_COMBINED                    = (_T("IsCombined"));
	LPCTSTR const CTAG_IS_COMBINED_ACK                = (_T("IsCombinedACK"));
	LPCTSTR const CTAG_SVIPDOC                        = (_T("SVIPDoc"));
	LPCTSTR const CTAG_CONDITIONAL_HISTORY            = (_T("ConditionalHistory"));
	LPCTSTR const CTAG_IPDOC_VIEWS                    = (_T("IPDocViews"));
	LPCTSTR const CTAG_HEIGHT_RESULT_VIEW             = (_T("HeightResultView"));
	LPCTSTR const CTAG_WIDTH_TOOLSET_VIEW             = (_T("WidthToolsetView"));
	LPCTSTR const CTAG_WINDOW_PLACEMENT               = (_T("WindowPlacement"));
	LPCTSTR const CTAG_TOOLSET_SCRIPT                 = (_T("ToolsetScript"));
	LPCTSTR const CTAG_VIEWS                          = (_T("Views"));
	LPCTSTR const CTAG_VIEW_NUMBER                    = (_T("ViewNumber"));
	LPCTSTR const CTAG_SVTOOLSET_TAB_VIEW_CLASS       = (_T("SVToolSetTabViewClass"));
	LPCTSTR const CTAG_TOOLSET_VIEW                   = (_T("ToolSetView"));
	LPCTSTR const CTAG_IMAGE_VIEW                     = (_T("ImageView"));
	LPCTSTR const CTAG_SCROLL_SIZE                    = (_T("ScrollSize"));
	LPCTSTR const CTAG_SCROLL_POINT                   = (_T("ScrollPoint"));
	LPCTSTR const CTAG_CELL_HEIGHT                    = (_T("CellHeight"));
	LPCTSTR const CTAG_CELL_WIDTH                     = (_T("CellWidth"));
	LPCTSTR const CTAG_IMAGE_ZOOM_FACTOR              = (_T("ImageZoomFactor"));
	LPCTSTR const CTAG_IMAGE_ZOOM_FACTOR_EX           = (_T("ImageZoomFactorEx"));
	LPCTSTR const CTAG_VIEW_INITIALIZED               = (_T("ViewInitialized"));
	LPCTSTR const CTAG_IMAGE_NAME                     = (_T("ImageName"));
	LPCTSTR const CTAG_LENGTH                         = (_T("Length"));
	LPCTSTR const CTAG_FLAGS                          = (_T("Flags"));
	LPCTSTR const CTAG_SHOW_COMMAND                   = (_T("ShowCommand"));
	LPCTSTR const CTAG_MINIMUM_POSITION               = (_T("MinimumPosition"));
	LPCTSTR const CTAG_MAXIMUM_POSITION               = (_T("MaximumPosition"));
	LPCTSTR const CTAG_NORMAL_POSITION                = (_T("NormalPosition"));
	LPCTSTR const CTAG_X                              = (_T("X"));
	LPCTSTR const CTAG_Y                              = (_T("Y"));
	LPCTSTR const CTAG_LEFT                           = (_T("Left"));
	LPCTSTR const CTAG_TOP                            = (_T("Top"));
	LPCTSTR const CTAG_RIGHT                          = (_T("Right"));
	LPCTSTR const CTAG_BOTTOM                         = (_T("Bottom"));
	LPCTSTR const CTAG_CX                             = (_T("CX"));
	LPCTSTR const CTAG_CY                             = (_T("CY"));
	LPCTSTR const CTAG_ENVIRONMENT                    = (_T("Environment"));
	LPCTSTR const CTAG_PPQ                            = (_T("PPQ"));
	LPCTSTR const CTAG_IO                             = (_T("IO"));
	LPCTSTR const CTAG_VIEWED_INPUTS                  = (_T("ViewedInputs"));
	LPCTSTR const CTAG_MONITOR_LISTS                  = (_T("MonitorLists"));
	LPCTSTR const CTAG_PRODUCTVALUES_LIST             = (_T("ProductValuesList"));
	LPCTSTR const CTAG_PRODUCTIMAGE_LIST              = (_T("ProductImageList"));
	LPCTSTR const CTAG_REJECTCONDITION_LIST           = (_T("RejectConditionList"));
	LPCTSTR const CTAG_FAILSTATUS_LIST                = (_T("FailStatusList"));
	LPCTSTR const CTAG_PPQ_NAME                       = (_T("PPQName"));
	LPCTSTR const CTAG_REJECT_QUEUE_DEPTH             = (_T("RejectQueueDepth"));
	LPCTSTR const CTAG_IS_ACTIVATED						= (_T("IsActivated"));
	LPCTSTR const CTAG_VERSION_NUMBER                 = (_T("VersionNumber"));
	LPCTSTR const CTAGF_INPUT_X                       = (_T("Input_%d")); // CTAGF indicates formatting is required.
	LPCTSTR const CTAG_ITEM_NAME                      = (_T("ItemName"));
	LPCTSTR const CTAG_IO_TYPE                        = (_T("IO_Type"));
	LPCTSTR const CTAG_DESCRIPTION		              = _T("Description");
	//CTAG_ONLINE_DISPLAY kept for backward compatibility
	LPCTSTR const CTAG_ONLINE_DISPLAY                 = (_T("Online_Display"));
	LPCTSTR const CTAG_IMAGE_DISPLAY_UPDATE           = (_T("Image_Display_Update"));
	LPCTSTR const CTAG_RESULT_DISPLAY_UPDATE          = (_T("Result_Display_Update"));

	LPCTSTR const CTAG_LUT                            = _T("LUT");
	LPCTSTR const CTAG_LUT_TRANSFORM_OPERATION        = _T("LUTTransformOperation");
	LPCTSTR const CTAG_LUT_BAND_DATA                  = _T("LutBandData");
	LPCTSTR const CTAG_LUT_TRANSFORM_PARAMETERS       = _T("LutTransformParameters");

	LPCTSTR const CTAG_CONFIGURATION_TYPE             = _T("ConfigurationType");

	LPCTSTR const CTAG_DEVICE_PARAM_LIST              = _T("DeviceParamList");
	LPCTSTR const CTAGF_DEVICE_PARAM_X                = _T("DeviceParam_%d"); // CTAGF indicates formatting is required.
	LPCTSTR const CTAG_DEVICE_PARAM_TYPE              = _T("DeviceParamType");
	LPCTSTR const CTAG_DEVICE_PARAM_VALUE             = _T("DeviceParamValue");
	LPCTSTR const CTAG_DEVICE_PARAM_UNITS             = _T("DeviceParamUnits");
	LPCTSTR const CTAG_OPTIONS                        = _T("Options");
	LPCTSTR const CTAGF_OPTION_X                      = _T("Option_%d"); // CTAGF indicates formatting is required.

	LPCTSTR const CTAG_VARIABLE_CAMERA_FORMAT         = _T("VariableCameraFormat");
	LPCTSTR const CTAG_CAMERA_ID				      = _T("CameraID");

	LPCTSTR const CTAG_FILEACQUISITION_MODE					= _T("FileAcquisitionMode");
	LPCTSTR const CTAG_FILEACQUISITION_IMAGE_FILENAME		= _T("ImageFilename");
	LPCTSTR const CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME	= _T("ImageDirectory");
	LPCTSTR const CTAG_FILEACQUISITION_LOADINGMODE			= _T("LoadingMode");
	LPCTSTR const CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE	= _T("ImageSizeEditMode");
	LPCTSTR const CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH		= _T("ImageWidth");
	LPCTSTR const CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT	= _T("ImageHeight");

	LPCTSTR const CTAG_MAX_VALUE                      = _T("MaxValue");

	LPCTSTR const CTAG_IODOC                          = _T("IODoc");
	LPCTSTR const CTAG_PPQ_STATE_UNIQUEID             = _T("PpqStateUniqueID");

	LPCTSTR const CTAG_REMOTE_OUTPUT_PARAMETERS       = _T("RemoteOutputParameters");
	LPCTSTR const CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID = _T("RemoteOutputObjectId");
	LPCTSTR const CTAG_REMOTE_OUTPUT_NAME             = _T("RemoteOutputInputName");
	LPCTSTR const CTAG_REMOTE_GROUP_ID                = _T("RemoteOutputGroupID");
	LPCTSTR const CTAG_REMOTE_OUTPUT_PAR_NUM          = _T("RemoteOutputParNumber");
	LPCTSTR const CTAG_REMOTE_OUTPUT_ENTRY            = _T("RemoteOutputEntry");
	LPCTSTR const CTAG_REMOTE_GROUP_PPQ               = _T("RemoteGroupPPQ");
	LPCTSTR const CTAG_REMOTE_OUTPUT_QUEUE_SIZE       = _T("RemoteGroupQueueSize");

	LPCTSTR const CTAG_INSPECTION_PROCESS             = _T("Inspection Process");
	LPCTSTR const CTAG_TOOLSET_SET                    = _T("Tool Set");
	LPCTSTR const CTAG_DEPENDENT_FILES                = _T("Dependent Files");
	LPCTSTR const CTAG_FILENAME                       = _T("Filename");

	LPCTSTR const CTAG_TOOLGROUPINGS                  = _T("ToolGroupings");
	LPCTSTR const CTAG_GROUP                          = _T("Group");
	LPCTSTR const CTAG_STARTGROUP                     = _T("StartGroup");
	LPCTSTR const CTAG_TOOLS                          = _T("Tools");
	LPCTSTR const CTAG_TOOL                           = _T("Tool");
	LPCTSTR const CTAG_ENDGROUP                       = _T("EndGroup");
	LPCTSTR const CTAG_COLLAPSED                      = _T("Collapsed");
	LPCTSTR const CTAG_STARTGROUP_COMMENT             = _T("StartGroupComment");
	LPCTSTR const CTAG_ENDGROUP_COMMENT               = _T("EndGroupComment");

	LPCTSTR const CTAG_THREAD_SETUP                   = _T("Thread Setup");
	LPCTSTR const CTAG_THREAD_NAME                    = _T("Thread Name");
	LPCTSTR const CTAG_THREAD_AFFINITY                = _T("Thread Affinity");
	LPCTSTR const CTAG_THREAD_MGR_ENABLE              = _T("Thread Mgr Enable");

	LPCTSTR const CTAG_VIEWEDVARIABLES                = _T("ViewedVariables");
	LPCTSTR const CTAG_VIEWEDENVARIABLES              = _T("ViewedEnvVariables");
	LPCTSTR const CTAG_VIEWEDTOOLVARIABLES            = _T("ViewedToolVariables");
	LPCTSTR const CTAG_VIEWEDRESULTS				= _T("ViewedResults");
	LPCTSTR const CTAG_VIEWEDPPQINPUTS                = _T("ViewedPPQInputs");
	LPCTSTR const CTAG_COMPLETENAME                   = _T("CompleteName"); 
	LPCTSTR const CTAG_VIEWEDTABLE					= _T("ViewedTable");

	LPCTSTR const CTAG_GLOBAL_CONSTANTS               = _T("GlobalConstants");
	LPCTSTR const CTAG_REGRESSIONTEST = _T("RegressionTest");
	LPCTSTR const CTAG_USE_PLAY_CONDITION = _T("UsePlayCondition");
	LPCTSTR const CTAG_PLAY_CONDITION_EQUATION = _T("PlayConditionEquation");

	LPCTSTR const CTAG_OBJECT_ATTRIBUTES_SET		  = _T("ObjectAttributesSet");

	/// These are tool copy / paste tags
	const TCHAR* const ToolCopyTag = _T( "Tool-Copy" );
	const TCHAR* const ToolsTag = _T( "Tools" );
	const TCHAR* const ToolTypeTag = _T( "ToolType" );
	const TCHAR* const ToolImageTag = _T("ToolImage");
	const TCHAR* const BaseTag = _T( "Base" );
	const TCHAR* const TypeTag = _T( "Type" );
	const TCHAR* const DataTag = _T( "</DATA>" );
} //namespace SvXml
