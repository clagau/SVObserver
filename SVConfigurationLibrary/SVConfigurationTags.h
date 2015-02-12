// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVConfigurationTags
// * .File Name       : $Workfile:   SVConfigurationTags.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.7  $
// * .Check In Date   : $Date:   07 Jan 2015 15:52:10  $
// ******************************************************************************
// SVConfigurationTags.h

#ifndef _INC_SVCONFIGURATIONTAGS_INCLUDED
#define _INC_SVCONFIGURATIONTAGS_INCLUDED

LPCTSTR const CTAG_DESCRIPTION                    = (_T("Description"));
LPCTSTR const CTAG_HISTORY_LEVEL                  = (_T("HistoryLevel"));
LPCTSTR const CTAG_MODULE_TRIGGER_SOURCE          = (_T("ModuleTriggerSource"));
LPCTSTR const CTAG_INTERNAL_TRIGGER_SOURCE        = (_T("InternalTriggerSource"));
LPCTSTR const CTAG_PPQ_SETTINGS                   = (_T("PPQSettings"));
LPCTSTR const CTAG_PPQ_SETTINGS_ACTIVATED         = (_T("PPQSettingsActivated"));
LPCTSTR const CTAG_PPQ_MODE                       = (_T("PPQMode"));
LPCTSTR const CTAG_OUTPUT_DELAY_TIME              = (_T("OutputDelayTime"));
LPCTSTR const CTAG_CURRENT_DDE_SERVER             = (_T("CurrentDDEServer"));
LPCTSTR const CTAG_CURRENT_DDE_TOPIC              = (_T("CurrentDDETopic"));
LPCTSTR const CTAG_IS_DDE_SERVER_CONNECTED        = (_T("IsDDEServerConnected"));
LPCTSTR const CTAG_IS_IO_LOADED                   = (_T("IsIOLoaded"));
LPCTSTR const CTAG_IO_FILE_NAME                   = (_T("IOFileName"));
LPCTSTR const CTAG_NUMBER_OF_ACQ_DEVICES          = (_T("NumberOfAcqDevices"));
LPCTSTR const CTAGF_ACQ_BOARD_X                   = (_T("AcqBoard%d"));
LPCTSTR const CTAG_UNIQUE_OBJECT_ID               = (_T("UniqueObjectId"));
LPCTSTR const CTAG_NAME_OF_BOARD_TYPE             = (_T("NameOfBoardType"));
LPCTSTR const CTAG_NAME_OF_BOARD_DRIVER           = (_T("NameOfBoardDriver"));
LPCTSTR const CTAG_MAXIMIM_DEVICE_NUMBER          = (_T("MaximumDeviceNumber"));
LPCTSTR const CTAG_DIGITIZER_ARRAY_CLASS          = (_T("DigitizerArrayClass"));
LPCTSTR const CTAG_SIZE_OF_IPDOC_INFO_LIST        = (_T("SizeOfIPDocInfoList"));
LPCTSTR const CTAG_SIZE_OF_DIGITIZER_ARRAY        = (_T("SizeOfDigitizerArray"));
LPCTSTR const CTAG_RED_DIGITIZER                  = (_T("RedDigitizer"));
LPCTSTR const CTAG_GREEN_DIGITIZER                = (_T("GreenDigitizer"));
LPCTSTR const CTAG_BLUE_DIGITIZER                 = (_T("BlueDigitizer"));
LPCTSTR const CTAG_UNIQUE_REFERENCE_Id            = (_T("UniqueReferenceId"));
LPCTSTR const CTAG_UNIQUE_REFERENCE_ID            = (_T("UniqueReferenceID")); // Id
LPCTSTR const CTAG_OBJECT_NAME                    = (_T("ObjectName"));
LPCTSTR const CTAG_TRIGGER                        = (_T("Trigger"));
LPCTSTR const CTAGF_TRIGGER_X                     = (_T("Trigger_%d"));
LPCTSTR const CTAG_CAMERA                         = (_T("Camera"));
LPCTSTR const CTAGF_CAMERA_X                      = (_T("Camera_%d"));
LPCTSTR const CTAG_DIGITIZER_INFO_STRUCT          = (_T("DigitizerInfoStruct"));
LPCTSTR const CTAG_IS_ACQUISITION_TRIGGER_INFORMATION_STORED  = (_T("IsAcquisitionTriggerInformationStored"));
LPCTSTR const CTAG_TRIGGER_DEVICE                 = (_T("TriggerDevice"));
LPCTSTR const CTAG_SOFTWARETRIGGER_DEVICE         = (_T("SoftwareTrigger"));
LPCTSTR const CTAG_SOFTWARETRIGGER_PERIOD         = (_T("SoftwareTriggerPeriod"));
LPCTSTR const CTAG_IS_PQ_INFORMATION_STORED       = (_T("IsPQInformationStored"));
LPCTSTR const CTAG_PQ_OBJECT_CLASS                = (_T("PQObjectClass"));
LPCTSTR const CTAG_ACTIVATE_DIGITIZER             = (_T("ActivateDigitizer"));
LPCTSTR const CTAG_SELECTED_SOURCE                = (_T("SelectedSource"));
LPCTSTR const CTAG_ACQUISITION_SOURCE_LIST        = (_T("AcquisitionSourceList"));
LPCTSTR const CTAG_MAXIMUM_GRAB_CHANNEL           = (_T("MaximumGrabChannel"));
LPCTSTR const CTAG_MAXIMIM_SIMULTANEOUS_GRAB_CHANNEL  = (_T("MaximumSimultaneousGrabChannel"));
LPCTSTR const CTAG_MAXIMUM_MONOCHROME_SOURCE      = (_T("MaximumMonochromeSource"));
LPCTSTR const CTAG_MAXIMUM_RGB_SOURCE             = (_T("MaximumRGBSource"));
LPCTSTR const CTAG_MAXIMUM_BAND_NUMBER_TYPES      = (_T("MaximumBandNumberTypes"));
LPCTSTR const CTAG_SIZE_OF_BAND_NUMBER_TYPE_LONG_ARRAY    = (_T("SizeOfBandNumberTypeLongArray"));
LPCTSTR const CTAG_BAND_NUMBER_TYPE_LONG_ARRAY    = (_T("BandNumberTypeLongArray"));
LPCTSTR const CTAG_SIZE_OF_PPQ_INDEX_HANDLE_OWNER_ARRAY   = (_T("SizeOfPPQIndexHandleOwnerArray"));
LPCTSTR const CTAGF_PPQ_INDEX_HANDLE_OWNER_X      = (_T("PPQIndexHandleOwner%d"));
LPCTSTR const CTAG_DDE_INPUT_ENTRY_LIST           = (_T("DDEInputEntryList"));
LPCTSTR const CTAG_PQ_DIGITAL_INPUT_INFO_LIST     = (_T("PQDigitialInputInfoList"));
LPCTSTR const CTAG_SIZE_OF_ACQUISITION_SOURCE_LIST    = (_T("SizeOfAcquisitionSourceList"));
LPCTSTR const CTAGF_ACQUISITION_SOURCE_X          = (_T("AcquisitonSource%d"));
LPCTSTR const CTAG_IMAGE_INFO_CLASS               = (_T("ImageInfoClass"));
LPCTSTR const CTAG_LIGHT_REFERENCE_ARRAY          = (_T("LightReferenceArray"));
LPCTSTR const CTAG_ACQUISITION_TYPE               = (_T("AcquisitionType"));
LPCTSTR const CTAG_ACQUISITION_TRIGGER_TYPE       = (_T("AcquisitionTriggerType"));
LPCTSTR const CTAG_ACQUISITION_SOURCE_NAME        = (_T("AcquisitionSourceName"));
LPCTSTR const CTAG_ACQUISITION_DEVICE_FILE_NAME   = (_T("AcquisitionDeviceFileName"));
LPCTSTR const CTAG_EXTENT_INFORMATION             = (_T("ExtentInformation"));
LPCTSTR const CTAG_ABSOLUTE_EXTENT_INFORMATION    = (_T("AbsoluteExtentInformation"));
LPCTSTR const CTAG_TYPE                           = (_T("Type"));
LPCTSTR const CTAG_SAVE_LOAD_INDEX                = (_T("SaveLoadIndex"));
LPCTSTR const CTAG_BAND_NUMBER                    = (_T("BandNumber"));
LPCTSTR const CTAG_BAND_LINK                      = (_T("BandLink"));
LPCTSTR const CTAG_ACQUISITION_DEVICE             = (_T("AcquisitionDevice"));
LPCTSTR const CTAG_PIXEL_DEPTH                    = (_T("PixelDepth"));
LPCTSTR const CTAG_ROTATION_ANGLE                 = (_T("RotationAngle"));
LPCTSTR const CTAG_SIZE                           = (_T("Size"));
LPCTSTR const CTAGF_BAND_X                        = (_T("Band_%d"));
LPCTSTR const CTAGF_LIGHTREFERENCE_X              = (_T("LightReference_%d"));
LPCTSTR const CTAG_NAME                           = (_T("Name"));
LPCTSTR const CTAG_RESOURCE_ID                    = (_T("ResourceID"));
LPCTSTR const CTAG_VALUE                          = (_T("Value"));
LPCTSTR const CTAG_OBJECT_LEFT_POSITION           = (_T("ObjectLeftPosition"));
LPCTSTR const CTAG_OBJECT_TOP_POSITION            = (_T("ObjectTopPosition"));
LPCTSTR const CTAG_OBJECT_WIDTH                   = (_T("ObjectWidth"));
LPCTSTR const CTAG_OBJECT_HEIGHT                  = (_T("ObjectHeight"));
LPCTSTR const CTAG_POSITION                       = (_T("Position"));
LPCTSTR const CTAG_INDEX_HANDLE                   = (_T("IndexHandle"));
LPCTSTR const CTAG_PPQ_POSITION                   = (_T("PPQPosition"));
LPCTSTR const CTAG_REMOTE_INDEX                   = (_T("RemoteIndex"));
LPCTSTR const CTAG_REMOTE_INITIAL_VALUE           = (_T("RemoteInitialValue"));
LPCTSTR const CTAG_DATA_SOURCE_TYPE               = (_T("DataSourceType"));
LPCTSTR const CTAG_INITIAL_SIZE                   = (_T("InitialSize"));
LPCTSTR const CTAG_DDE_ATTRIBUTES                 = (_T("DDEAttributes"));
LPCTSTR const CTAG_INPUT                          = (_T("Input"));
LPCTSTR const CTAG_SIZE_OF_DDE_INPUT_ENTRY_LIST   = (_T("SizeOfDDEInputEntryList"));
LPCTSTR const CTAG_ATTRIBUTE                      = (_T("Attribute"));
LPCTSTR const CTAG_DATA_TYPE                      = (_T("DataType"));
LPCTSTR const CTAG_MAXIMUM_DATA_SIZE              = (_T("MaximumDataSize"));
LPCTSTR const CTAG_ENABLE_DIGITAL_OUTPUT          = (_T("EnableDigitalOutput"));
LPCTSTR const CTAG_USE_FOR_MASTER_RESULT          = (_T("UseForMasterResult"));
LPCTSTR const CTAG_WAIT_FOR_DDE_INPUT_COMPLETION  = (_T("WaitForDDEInputCompletion"));
LPCTSTR const CTAG_PRODUCER_UNIQUE_REFERENCE_ID   = (_T("ProducerUniqueReferenceID"));
LPCTSTR const CTAG_DATA_LINK_UNIQUE_REFERENCE_ID  = (_T("DataLinkUniqueReferenceID"));
LPCTSTR const CTAG_DEFAULT_DATA                   = (_T("DefaultData"));
LPCTSTR const CTAG_CURRENT_DATA                   = (_T("CurrentData"));
LPCTSTR const CTAG_ARRAY_DATA                     = (_T("ArrayData"));
LPCTSTR const CTAG_SIZE_OF_PQ_DIGITAL_INPUT_ENTRY_LIST    = (_T("SizeOfPQDigitalInputEntryList"));
LPCTSTR const CTAGF_PQ_DIGITAL_INPUT_ENTRY_X      = (_T("PQDigitalInputEntry%d"));
LPCTSTR const CTAG_DEFINITION_OF_EVENT_SERVER     = (_T("DefinitionOfEventServer"));
LPCTSTR const CTAG_TOGGLE_FLAG                    = (_T("ToggleFlag"));
//LPCTSTR const CTAG_UNIQUE_REFERENCE_ID            = (_T("UniqueReferenceID"));
LPCTSTR const CTAG_PQ_CHECKER_INFORMATION         = (_T("PQCheckerInformation"));
LPCTSTR const CTAG_PPQ_DEFINITION_INFORMATION     = (_T("PPQDefinitionInformation"));
LPCTSTR const CTAG_DDE_CONTROL_DEFINITION_INFORMATION = (_T("DDEControlDefinitionInformation"));
LPCTSTR const CTAG_DATA_VALID_OUTPUT_OBJECT       = (_T("DataValidOutputObject"));
LPCTSTR const CTAG_NOT_INSPECTED_OUTPUT_OBJECT    = (_T("NotInspectedOutputObject"));
LPCTSTR const CTAG_NAK_MISTRIGGER_OUTPUT_OBJECT   = (_T("NAK_Misstrigger_OutputObject"));
LPCTSTR const CTAG_ACK_COMPLEMENT_OF_NAK_OUTPUT_OBJECT    = (_T("ACK_ComplementOfNAK_OutputObject"));
LPCTSTR const CTAG_MF_MASTER_FAULT_OUTPUT_OBJECT  = (_T("MF_MasterFault_OutputObject"));
LPCTSTR const CTAG_MW_MASTER_WARNING_OUTPUT_OBJECT    = (_T("MW_MasterWarning_OutputObject"));
LPCTSTR const CTAG_PPQ_LENGTH                     = (_T("PPQLength"));
LPCTSTR const CTAG_PPQ_MAINTAIN_SRC_IMAGE         = (_T("PPQMaintainSrcImage"));
LPCTSTR const CTAG_PPQ_INSPECTION_TIMEOUT         = (_T("PPQInspectionTimeout"));
LPCTSTR const CTAG_PPQ_OUTPUT_RESET_DELAY_TIME    = (_T("PPQOutputResetDelayTime"));
LPCTSTR const CTAG_PPQ_TRIGGER_COUNT_ID           = (_T("PPQTriggerCountID"));
LPCTSTR const CTAG_PPQ_CONDITIONAL_OUTPUT		  = (_T("PPQConditonalOutput"));
//LPCTSTR const CTAG_PPQ_MODE                       = (_T("PPQMode"));
LPCTSTR const CTAG_PPQ_OUTPUT_DELAY_TIME          = (_T("PPQOutputDelayTime"));
LPCTSTR const CTAG_DDE_OUTPUT_ENTRY_LIST          = (_T("DDEOutputEntryList"));
LPCTSTR const CTAG_OUTPUT                         = (_T("Output"));
LPCTSTR const CTAG_SIZE_OF_DDE_OUTPUT_ENTRY_LIST  = (_T("SizeOfDDEOutputEntryList"));
LPCTSTR const CTAG_NUMBER_OF_VIEWS                = (_T("NumberOfViews"));
LPCTSTR const CTAG_INSPECTION_FILE_NAME           = (_T("InspectionFileName"));
LPCTSTR const CTAG_INSPECTION_TOOLSET_IMAGE       = (_T("ToolsetImage"));
LPCTSTR const CTAG_INSPECTION_NEW_DISABLE_METHOD  = (_T("NewDisableMethod"));
LPCTSTR const CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT  = (_T("EnableAuxiliaryExtent"));
LPCTSTR const CTAG_IPDOC_REQUIREMENT_INFO         = (_T("IPDocRequirementInfo"));
LPCTSTR const CTAG_DEFAULT_SEC_PATH               = (_T("DefaultSECPath"));
LPCTSTR const CTAG_DEFAULT_SEC_TITLE              = (_T("DefaultSECTitle"));
LPCTSTR const CTAG_SIZE_OF_CAMERA_INFO_ARRAY      = (_T("SizeOfCameraInfoArray"));
LPCTSTR const CTAGF_CAMERA_INFO_X                 = (_T("CameraInfo%d"));
LPCTSTR const CTAG_DIGITIZER_NUMBER               = (_T("DigitizerNumber"));
LPCTSTR const CTAG_INSPECTION                     = (_T("Inspection"));
LPCTSTR const CTAG_EXTERNAL_TRIGGER_SERVER        = (_T("ExternalTriggerServer"));
LPCTSTR const CTAG_TIMER_TRIGGER_SERVER           = (_T("TimerTriggerServer"));
LPCTSTR const CTAG_IO_ENTRY_IDENTIFIER            = (_T("IOEntryIdentifer"));
LPCTSTR const CTAG_NUMBER_OF_IO_ENTRIES           = (_T("NumberOfIOEntries"));
LPCTSTR const CTAGF_IO_ENTRY_X                    = (_T("IOEntry%d"));
LPCTSTR const CTAG_NUMBER_OF_INPUT_ENTRIES        = (_T("NumberofInputEntries"));
LPCTSTR const CTAGF_INPUT_ENTRY_X                 = (_T("InputEntry%d"));
LPCTSTR const CTAG_TRIGGER_EVENT_HANDLING         = (_T("TriggerEventHandling"));
LPCTSTR const CTAG_DELAY_TIME                     = (_T("DelayTime"));
LPCTSTR const CTAG_PERIODIC_TIME                  = (_T("PeriodicTime"));
LPCTSTR const CTAG_IO_ENTRY_NAME                  = (_T("IOEntryName"));
LPCTSTR const CTAG_IS_OUTPUT                      = (_T("IsOutput"));
LPCTSTR const CTAG_CHANNEL                        = (_T("Channel"));
LPCTSTR const CTAG_IS_MODULE_IO_LIST_ENTRY        = (_T("IsModuleIOListEntry"));
LPCTSTR const CTAG_IS_FORCED                      = (_T("IsForced"));
LPCTSTR const CTAG_FORCED_VALUE                   = (_T("ForcedValue"));
LPCTSTR const CTAG_IS_INVERTED                    = (_T("IsInverted"));
LPCTSTR const CTAG_DEFAULT_VALUE                  = (_T("DefaultValue"));
LPCTSTR const CTAG_IO_OBJECT_ATTRIBUTE            = (_T("IOObjectAttribute"));
LPCTSTR const CTAG_IS_COMBINED                    = (_T("IsCombined"));
LPCTSTR const CTAG_IS_COMBINED_ACK                = (_T("IsCombinedACK"));
//LPCTSTR const CTAG_OBJECT_NAME                    = (_T("ObjectName"));
LPCTSTR const CTAG_OBJECT_ATTRIBUTE_SET           = (_T("ObjectAttributeSet"));
LPCTSTR const CTAG_SVIPDOC                        = (_T("SVIPDoc"));
LPCTSTR const CTAG_PATCH_VALUE                    = (_T("PatchValue"));
LPCTSTR const CTAG_CONDITIONAL_HISTORY            = (_T("ConditionalHistory"));
LPCTSTR const CTAG_IPDOC_VIEWS                    = (_T("IPDocViews"));
LPCTSTR const CTAG_HEIGHT_RESULT_VIEW             = (_T("HeightResultView"));
LPCTSTR const CTAG_WIDTH_TOOLSET_VIEW             = (_T("WidthToolsetView"));
LPCTSTR const CTAG_WINDOW_PLACEMENT               = (_T("WindowPlacement"));
LPCTSTR const CTAG_OWNER_UNIQUE_REFERENCE_ID      = (_T("OwnerUniqueReferenceID"));
LPCTSTR const CTAG_TOOLSET_SCRIPT                 = (_T("ToolsetScript"));
LPCTSTR const CTAG_VIEW_IDENTIFICATION            = (_T("ViewIdentification"));
LPCTSTR const CTAG_HISTORY_BLOCK                  = (_T("HistoryBlock"));
LPCTSTR const CTAG_VIEWS                          = (_T("Views"));
//LPCTSTR const CTAG_END_OF_VIEWS                   = (_T("End of Views"));
LPCTSTR const CTAG_VIEW_NUMBER                    = (_T("ViewNumber"));
LPCTSTR const CTAG_SVIMAGE_VIEW_SCROLL            = (_T("SVImageViewScroll"));
LPCTSTR const CTAG_SVIMAGE_VIEW_CLASS             = (_T("SVImageViewClass"));
LPCTSTR const CTAG_SVTOOLSET_TAB_VIEW_CLASS       = (_T("SVToolSetTabViewClass"));
LPCTSTR const CTAG_SVRESULTVIEW_CLASS             = (_T("SVResultViewClass"));
LPCTSTR const CTAG_IMAGE_VIEW                     = (_T("ImageView"));
LPCTSTR const CTAG_SCROLL_SIZE                    = (_T("ScrollSize"));
LPCTSTR const CTAG_SCROLL_POINT                   = (_T("ScrollPoint"));
LPCTSTR const CTAG_CELL_HEIGHT                    = (_T("CellHeight"));
LPCTSTR const CTAG_CELL_WIDTH                     = (_T("CellWidth"));
LPCTSTR const CTAG_IMAGE_ZOOM_FACTOR              = (_T("ImageZoomFactor"));
LPCTSTR const CTAG_IMAGE_ZOOM_FACTOR_EX           = (_T("ImageZoomFactorEx"));
LPCTSTR const CTAG_VIEW_INITIALIZED               = (_T("ViewInitialized"));
LPCTSTR const CTAG_IMAGE_NAME                     = (_T("ImageName"));
LPCTSTR const CTAG_FREEZE_SCRIPT                  = (_T("FreezeScript"));
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
LPCTSTR const CTAGF_DIG_X                         = (_T("Dig_%d"));
LPCTSTR const CTAGF_INSPECTION_X                  = (_T("Inspection_%d"));
LPCTSTR const CTAG_PPQ                            = (_T("PPQ"));
LPCTSTR const CTAGF_PPQ_X                         = (_T("PPQ_%d"));
LPCTSTR const CTAG_IO                             = (_T("IO"));
LPCTSTR const CTAG_VIEWED_INPUTS                  = (_T("ViewedInputs"));
LPCTSTR const CTAG_MONITOR_LISTS                  = (_T("MonitorLists"));
LPCTSTR const CTAG_PRODUCTVALUES_LIST             = (_T("ProductValuesList"));
LPCTSTR const CTAG_PRODUCTIMAGE_LIST              = (_T("ProductImageList"));
LPCTSTR const CTAG_REJECTCONDITION_LIST           = (_T("RejectConditionList"));
LPCTSTR const CTAG_FAILSTATUS_LIST                = (_T("FailStatusList"));
LPCTSTR const CTAG_PPQ_NAME			              = (_T("PPQName"));
LPCTSTR const CTAG_REJECT_QUEUE_DEPTH             = (_T("RejectQueueDepth"));
LPCTSTR const CTAG_VERSION_NUMBER                 = (_T("VersionNumber"));
LPCTSTR const CTAG_COLOR_SVIM_MODE_ACTIVE         = (_T("ColorSVIMModeActive"));
LPCTSTR const CTAG_CREATION_DATE                  = (_T("CreationDate"));
LPCTSTR const CTAG_LAST_UPDATE                    = (_T("LastUpdate"));
LPCTSTR const CTAG_AUTHOR                         = (_T("Author"));

LPCTSTR const CTAGF_INPUT_X                       = (_T("Input_%d"));
//LPCTSTR const CTAGF_INPUT_X_DIGITAL               = (_T("Input_%d_Digital"));
LPCTSTR const CTAGF_OUTPUT_X                      = (_T("Output_%d"));
LPCTSTR const CTAG_ITEM_NAME                      = (_T("ItemName"));
LPCTSTR const CTAG_IO_TYPE                        = (_T("IO_Type"));
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
LPCTSTR const CTAGF_DEVICE_PARAM_X                = _T("DeviceParam_%d");
LPCTSTR const CTAG_DEVICE_PARAM_TYPE              = _T("DeviceParamType");
LPCTSTR const CTAG_DEVICE_PARAM_VALUE             = _T("DeviceParamValue");
LPCTSTR const CTAG_DEVICE_PARAM_UNITS             = _T("DeviceParamUnits");
LPCTSTR const CTAG_OPTIONS                        = _T("Options");
LPCTSTR const CTAGF_OPTION_X                      = _T("Option_%d");

LPCTSTR const CTAG_VARIABLE_CAMERA_FORMAT         = _T("VariableCameraFormat");

LPCTSTR const CTAG_FILEACQUISITION_MODE					= _T("FileAcquisitionMode");
LPCTSTR const CTAG_FILEACQUISITION_IMAGE_FILENAME		= _T("ImageFilename");
LPCTSTR const CTAG_FILEACQUISITION_IMAGE_DIRECTORYNAME	= _T("ImageDirectory");
LPCTSTR const CTAG_FILEACQUISITION_LOADINGMODE			= _T("LoadingMode");
LPCTSTR const CTAG_FILEACQUISITION_IMAGE_SIZE_EDIT_MODE	= _T("ImageSizeEditMode");
LPCTSTR const CTAG_FILEACQUISITION_FILE_IMAGE_WIDTH		= _T("ImageWidth");
LPCTSTR const CTAG_FILEACQUISITION_FILE_IMAGE_HEIGHT	= _T("ImageHeight");

LPCTSTR const CTAG_MAX_VALUE                      = _T("MaxValue");

LPCTSTR const CTAG_IODOC                          = _T("IODoc");
LPCTSTR const CTAG_PLC_PARAMETERS                 = _T("PlcParameters");
LPCTSTR const CTAG_PLC_CONNECTION_STRING          = _T("PlcConnection");
LPCTSTR const CTAG_PLC_ENTRY_LIST                 = _T("PlcEntryList");
LPCTSTR const CTAG_PLC_ENTRY                      = _T("PlcEntry");
LPCTSTR const CTAG_PLC_ADDRESS                    = _T("PlcAddress");
LPCTSTR const CTAG_PLC_INPUT_OBJECT_GUID          = _T("PlcObjectId");
LPCTSTR const CTAG_PLC_ID                         = _T("PlcNameId");
LPCTSTR const CTAG_PLC_ELEMENT_SIZE               = _T("PlcElementSize");
LPCTSTR const CTAG_PLC_CONVERT_DATA               = _T("PlcConvertData");
LPCTSTR const CTAG_PLC_DATA_TYPE                  = _T("PlcDataType");
LPCTSTR const CTAG_PLC_CHANNEL_PAR                = _T("PlcChannelParameter");
LPCTSTR const CTAG_PLC_BLOCK_SIZE                 = _T("PlcBlockSize");
LPCTSTR const CTAG_PLC_QUEUE_SIZE                 = _T("PlcQueueSize");
LPCTSTR const CTAG_PLC_HEARTBEAT_ADDR             = _T("PlcHeartBeatAddr");
LPCTSTR const CTAG_PLC_HEARTBEAT_TIME             = _T("PlcHeartBeatTime");
LPCTSTR const CTAG_PLC_STATE_UNIQUEID             = _T("PlcStateUniqueID");
LPCTSTR const CTAG_PLC_BLOCK_START                = _T("PlcBlockStart");
LPCTSTR const CTAG_PLC_BIT                        = _T("PlcBit");
LPCTSTR const CTAG_PLC_SIZE_BITS                  = _T("PlcSizeIsBits");

LPCTSTR const CTAG_REMOTE_OUTPUT_PARAMETERS       = _T("RemoteOutputParameters");
LPCTSTR const CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID = _T("RemoteOutputObjectId");
LPCTSTR const CTAG_REMOTE_OUTPUT_NAME             = _T("RemoteOutputInputName");
LPCTSTR const CTAG_REMOTE_GROUP_ID                = _T("RemoteOutputGroupID");
LPCTSTR const CTAG_REMOTE_OUTPUT_PAR_NUM          = _T("RemoteOutputParNumber");
LPCTSTR const CTAG_REMOTE_OUTPUT_ENTRY            = _T("RemoteOutputEntry");
LPCTSTR const CTAG_REMOTE_GROUP_PPQ               = _T("RemoteGroupPPQ");
LPCTSTR const CTAG_REMOTE_OUTPUT_QUEUE_SIZE       = _T("RemoteGroupQueueSize");

LPCTSTR const CTAG_INSPECTION_PROCESS			  = _T("Inspection Process");
LPCTSTR const CTAG_TOOLSET_SET					  = _T("Tool Set");
LPCTSTR const CTAG_DEPENDENT_FILES				  = _T("Dependent Files");
LPCTSTR const CTAG_FILENAME						  = _T("Filename");

LPCTSTR const CTAG_TOOLGROUPINGS				= _T("ToolGroupings");
LPCTSTR const CTAG_GROUP						= _T("Group");
LPCTSTR const CTAG_STARTGROUP					= _T("StartGroup");
LPCTSTR const CTAG_TOOLS						= _T("Tools");
LPCTSTR const CTAG_TOOL							= _T("Tool");
LPCTSTR const CTAG_ENDGROUP						= _T("EndGroup");
LPCTSTR const CTAG_COLLAPSED					= _T("Collapsed");
LPCTSTR const CTAG_STARTGROUP_COMMENT			= _T("StartGroupComment");
LPCTSTR const CTAG_ENDGROUP_COMMENT				= _T("EndGroupComment");

LPCTSTR const CTAG_THREAD_SETUP					= _T("Thread Setup");
LPCTSTR const CTAG_THREAD_NAME					= _T("Thread Name");
LPCTSTR const CTAG_THREAD_AFFINITY				= _T("Thread Affinity");
LPCTSTR const CTAG_THREAD_MGR_ENABLE			= _T("Thread Mgr Enable");

LPCTSTR const CTAG_VIEWEDVARIABLES		= _T("ViewedVariables");
LPCTSTR const CTAG_VIEWEDENVARIABLES	= _T("ViewedEnvVariables");
LPCTSTR const CTAG_VIEWEDTOOLVARIABLES	= _T("ViewedToolVariables");
LPCTSTR const CTAG_VIEWEDPPQINPUTS		= _T("ViewedPPQInputs");
LPCTSTR const CTAG_COMPLETENAME			= _T("CompleteName"); 

#endif /* _INC_SVCONFIGURATIONTAGS_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVConfigurationLibrary\SVConfigurationTags.h_v  $
 * 
 *    Rev 1.7   07 Jan 2015 15:52:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added new tags for saving Result View variables in configuration file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Dec 2014 06:54:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Thread Manager Enable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Dec 2014 12:11:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in Tags for SVThreadManager
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 2014 12:01:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  921
 * SCR Title:  Add more complete zoom functionality. (runpage)
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added CTAG_IMAGE_ZOOM_FACTOR_EX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2014 16:05:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added tags for Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2014 06:18:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed dead method XmlizeTag
 * Added CTAG_IMAGE_DISPLAY_UPDATE and CTAG_RESULT_DISPLAY_UPDATE tags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2014 16:45:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   New Tags for loading/saving Remote Monitor List
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:39:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jan 2013 15:29:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added tag for PPQ Conditional Output
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   06 Sep 2012 09:51:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed CTAG_INSPECTION_DATA constant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   04 Sep 2012 13:31:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved hard coded flags used in the configuration loading and saving operations to this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   30 Jan 2012 10:35:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new configuration string names for new output streaming subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   21 Oct 2010 10:30:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration element for persisted remote input value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   14 Nov 2008 13:19:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new tags for PLC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   03 Oct 2008 17:27:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed CTAG_SOFTWARETRIGGER_FREQ to CTAG_SOFTWARETRIGGER_PERIOD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Sep 2008 13:08:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added tags for FileAcquistion Configuratrion properties
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Sep 2008 11:15:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Configuration Tags for the Software Trigger parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Jan 2007 09:28:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new tag for the Inspection Timeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Jan 2006 14:53:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CTAG_CONDITIONAL_HISTORY
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Oct 2005 09:48:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Tag for Enabling auxiliary extents for the Inspection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Dec 2003 11:56:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added DEVICE_PARAM_UNITS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   25 Sep 2003 14:22:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new configuration flags to support combined digital outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Sep 2003 07:45:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CTAG_MAX_VALUE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Aug 2003 11:34:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new tag for inspection's disable method to save with the configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Aug 2003 11:32:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new tag for inspection's disable method to save with the configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Jul 2003 08:57:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added CTAG_OPTIONS, CTAGF_OPTION_X, and CTAG_VARIABLE_CAMERA_FORMAT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 Jul 2003 09:50:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added DeviceParams tags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Mar 2003 07:03:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  336
 * SCR Title:  Change Light Reference and LUT dialogs on RGB mono system
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added CTAG_CONFIGURATION_TYPE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Feb 2003 13:24:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new tag for PPQ Maintain Source Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jan 2003 13:25:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new configuration tag for RemoteInput index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Jan 2003 16:11:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added LUT tags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Nov 2002 16:19:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  303
 * SCR Title:  Unable to save a configuration to a UNC path
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed #define to LPCTSTR cont variables to help in debugging data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
