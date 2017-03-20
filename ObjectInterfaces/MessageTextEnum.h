//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Enum for the MessageTextID
//******************************************************************************
#pragma once

namespace Seidenader { namespace ObjectInterfaces
{
	enum MessageTextEnum
	{
		Tid_Empty,
		Tid_Default,
		Tid_ErrorClass,
		Tid_XmlParserError,
		Tid_RestoreVariantError,
		Tid_TranslateVariantError,
		Tid_ToVariant_InvalidArgument,

		Tid_XML_UnexpectedType,
		Tid_XML_UnexpectedOnElementData,
		Tid_XML_InvalidPointer,
		Tid_XML_InvalidArrayElement,
		Tid_XML_Error,

		//SVObserver SVDLLToolLoadLibraryClass
		Tid_ToolLoadError_ImageDef,
		Tid_ToolLoadError_NoImageDef,
		Tid_ToolLoadError_BothSetsImagesDef,
		Tid_ToolLoadError_BothImagesDef,
		Tid_ToolLoadError_InFunctionDef,
		Tid_ToolLoadError_SimpleTest,
		Tid_ToolLoadError_SimpleTestException,
		Tid_ToolLoadError_DllStartup,
		Tid_ToolLoadError_DllStartupException,
		Tid_ToolLoadError_GetToolName,
		Tid_ToolLoadError_GetToolNameException,
		Tid_ToolLoadError_GetToolVersion,
		Tid_ToolLoadError_GetToolVersionException,
		Tid_GetProcAddressError_SimpleTest,
		Tid_GetProcAddressError_GetToolName,
		Tid_GetProcAddressError_GetToolVersion,
		Tid_GetProcAddressError_RunTool,
		Tid_GetProcAddressError_Startup,
		Tid_GetProcAddressError_ShutDown,
		Tid_GetProcAddressError_InitializeRun,
		Tid_GetProcAddressError_UninitializeRun,
		Tid_GetProcAddressError_GetInputValueDefinitions,
		Tid_GetProcAddressError_DestroyInputValueDefinitionStructures,
		Tid_GetProcAddressError_DestroyResultValueDefinitionStructures,
		Tid_GetProcAddressError_SetInputValues,
		Tid_GetProcAddressError_GetResultValues,
		Tid_GetProcAddressError_GetMessageString,
		Tid_GetProcAddressError_ValidateValueParameter,
		Tid_GetProcAddressError_GetResultValueDefinitions,
		Tid_GetProcAddressError_GetNumberOfInputImages,
		Tid_GetProcAddressError_GetResultImageDefinitions,
		Tid_GetProcAddressError_DestroyImageDefinitionStructure,
		Tid_ToolLoadError,
		Tid_RunTool_Exception,
		Tid_SVInitializeRun_Exception,

		///Defines for SVConfigurationObjects
		Tid_CreateSFailed,
		Tid_NumberOfIosMissing,
		Tid_IOEntryIsMissing,
		Tid_CreationOfPPQFailed,
		Tid_InputIsMissing,
		Tid_MsgIOTypeIsMissing,
		Tid_MsgIONameIsMissing,
		Tid_MsgPPQPOsitionIsMissing,
		Tid_MsgRemoteIndexIsMissing,

		Tid_ActivatingDiskProtection,
		Tid_DeactivatingDiskProtection,
		Tid_DiskProtectionRemainsActive,
		Tid_DiskProtectionRemainsInactive,
		Tid_CouldNotExecuteFormatString,

		//RangeValue
		Tid_RangeValue_EmptyString,
		Tid_RangeValue_WrongRange,
		Tid_ErrorUnknownEnum,
		Tid_FailHigh,
		Tid_FailLow,
		Tid_WarnHigh,
		Tid_WarnLow,
		Tid_IsLessThan,
		Tid_IsInvalidRef,

		Tid_ErrorInitMonitorListInSharedMemory,
		Tid_ErrorWriteMonitorListNoSharedMemory,
		Tid_ErrorReleaseProductNoReaderLock,
		Tid_RingBuffer_Depth_Invalid_Value,
		Tid_Error_NoFontFileSpec,
		Tid_Error_FontFileNotExist,
		Tid_Error_FontFileEmpty,
		Tid_SVAcquisitionClass_GetNextIndex,
		Tid_ArchiveTool_PathEmpty,
		Tid_ArchiveTool_CreateFileFailed,
		Tid_ArchiveTool_CreatePathFailed,
		Tid_ArchiveTool_WriteToFileFailed,
		Tid_ArchiveTool_AllocImageFailed,
		Tid_PathFileNotFound,
		Tid_BarCode_UnableToRead,
		Tid_BarCode_UnableToSave,
		Tid_BlobAnalyzer_ResultCreationFailed,
		Tid_CreationFailed,
		Tid_CreationOf2Failed,
		Tid_Command_StartSvimConfig,
		Tid_Command_EndSvimConfig,
		Tid_ErrorMIL,
		Tid_ErrorGettingPPQ,
		Tid_ErrorGettingInputObjectList,
		Tid_ErrorFillingInputs,
		Tid_ErrorGettingPPQCount,
		Tid_ErrorGettingOutputs,
		Tid_Acquisition_SourceInformationFailed,
		Tid_TooManyVariables,
		Tid_EquationParserError,
		Tid_InvalidData,
		Tid_Error_NoResultObject,
		Tid_ImageClass_DestroyError,
		Tid_GetParentImageHandleError,
		Tid_DisplayedImageIsUnavailable,
		Tid_InspThreadGetResultImageIndexError,
		Tid_ErrorNotAllDataItemsFound,
		Tid_ErrorNotAllImageItemsFound,
		Tid_ErrorProcessNotifyLastInspected,
		Tid_Unknown,
		Tid_ErrorLockingInputImageRequests,
		Tid_ErrorLockingInputRequests,
		Tid_ErrorRemovingAllInputRequests,
		Tid_ErrorRemovingAllInputImageRequests,
		Tid_ErrorUnlockingInputImageRequests,
		Tid_ErrorUnlockingInputRequests,
		Tid_IPDoc_NoCameraColorAttached,
		Tid_ErrorGettingTimeStamp,
		Tid_ProcessTrigger,
		Tid_ErrorNotAllRejectConditionItemsFound,
		Tid_ReleaseProduct,
		Tid_ErrorCopyLastInspectedToReject,
		Tid_CommitSharedMemory,
		Tid_SVIODoc_ClosingImpossible,
		Tid_SVIPDoc_ClosingImpossible,
		Tid_SVIPDoc_GoIntoRegTestFailed,
		Tid_ErrorDetachingInput,
		Tid_ColorToolMustBeFirstMessage,
		Tid_LightReference_NotAvailable,
		Tid_LUT_NotAvailable,
		Tid_LUT_ShouldOverwriteAllBands,
		Tid_Error_WrongCameraModel,
		Tid_Error_WrongCameraVendor,
		Tid_CreateBufferFailed,
		Tid_CreateImageChildBuffer_parent,
		Tid_CreateImageChildBuffer_child,
		Tid_CreateImageChildBuffer,
		Tid_MatroxImage_UnableToFindFile,
		Tid_UnKnownFileFormat,
		Tid_FailedToLoadImage,
		Tid_FailedToSaveImage,
		
		//Custom Filter 2
		Tid_DataInvalidKernelSize,
		Tid_DataInvalidKernelWidth,
		Tid_DataInvalidKernelHeight,
		Tid_DataInvalidNormalizationFactor,
		Tid_ExportFailed,
		Tid_ImportFailed,
		Tid_XmlFormatInvalid,
		Tid_VersionMismatch,
		Tid_ImportFailedDataInvalid,

		Tid_True,
		Tid_False,
		Tid_ConditionalValidated,
		Tid_FormulaValidated,
		Tid_Error_InvalidFormula,
		Tid_Error_CreationFilterFailed,
		Tid_Error_ToolPositionError,
		Tid_RingBuffer_Depth_Invalid_ValueString,
		Tid_RingBuffer_ImageIndex_Invalid_ValueString,
		Tid_Error_NoDeviceContext,
		Tid_Lpt_WrongState,
		Tid_Sax_ConInitializeExFailed,
		Tid_UnknowThread,

		//MatroxGigE
		Tid_MatroxGigE_Connect_FindCameraError,
		Tid_MatroxGigE_Connect_GetDigitizerError,
		Tid_MatroxGigE_Connect_AllocError,
		Tid_MatroxGigE_Connect_ReadError,
		Tid_MatroxGigE_Connect_CreateError,
		Tid_MatroxGigE_Connect_ReloadError,
		Tid_MatroxGigE_Connect_StartDigitizerError,
		Tid_MatroxGigE_Disconnect_FindCameraError,
		Tid_MatroxGigE_Disconnect_GetDigitizerError,
		Tid_MatroxGigE_Disconnect,
		Tid_MatroxGigE_Connect,

		Tid_FileCamera_NoImageFile,
		Tid_FileCamera_LoadFailed,
		Tid_FileAcquisition_FormatError,
		Tid_ObjectBuilder_SetObjectValueError,
		Tid_ErrorUsedPreviouslyForSec,

		//ParseError
		Tid_ParseError_NameMustNotContainKeyword,
		Tid_ParseError_OutOfMemory,
		Tid_ParseError_NameExpressionExpected,
		Tid_ParseError_OwnerNotExist,
		Tid_ParseError_UnexpectedMember,
		Tid_ParseError_CannotReadGuid,
		Tid_ParseError_ExpressionExpected,
		Tid_ParseError_UnknownExpression,
		Tid_ParseError_UnexpectedEndOfCommand,
		Tid_ParseError_CriticalErrorRemovingDependencies,
		Tid_ParseError_UnexpectedEndOfParse,
		Tid_ParseError_UnexpectedEndOfName,
		Tid_ParseError_EmptyToken,
		Tid_ParseError_GuidCannotEvaluated,
		Tid_ParseError_NameOperandExpected,
		Tid_ParseError_OperatorExpected,
		Tid_ParseError_OperandExpected,
		Tid_ParseError_CommandTerminationExpected,
		Tid_ParseError_TooManyExpressions,
		Tid_ParseError_ClassOperandExpected,
		Tid_ParseError_OperatorOperand,
		Tid_ParseError_ObjectOperandExpected,
		Tid_ParseError_UnableToReadInputList,
		Tid_ParseError_CommandTerminationOrObjectMemberExpected,
		Tid_ParseError_NoValidObjectOwner,
		Tid_ParseError_WrongEmbeddedValues,
		Tid_ParseError_CannotConstructObject,
		Tid_ParseError_ObjectRequired,
		Tid_ParseError_ValuesExpected,
		Tid_ParseError_NoValidObject,
		Tid_ParseError_SetObjectMemberFailed,
		Tid_Error_ScriptParseFailed,

		//SVObserver
		Tid_SVObserver_CannotRun_WrongModelNumber,
		Tid_SVObserver_WrongModelNumber,
		Tid_SVObserver_ModelNumberInvalid,
		Tid_SVObserver_AuthorizationFailed_Modification,
		Tid_SVObserver_AuthorizationFailed_Execution,
		Tid_SVObserver_MatroxGigELicenseNotFound,
		Tid_SVObserver_MatroxLicenseNotFound,
		Tid_SVObserver_WrongVersionNumber,
		Tid_SVObserver_ConfigurationLoadFailed,
		Tid_SVObserver_SecurityFileLoadFailed,
		Tid_SVObserver_WrongPathnameEntered,
		Tid_SVObserver_RegisterClassObjectsFailed,
		Tid_SVObserver_RegisterClassObjectsFailed_Question,
		Tid_SVObserver_OleInitFailed,
		Tid_CanGoOnlineFailure_Trigger,
		Tid_CanGoOnlineFailure_Acquisition,
		Tid_CanGoOnlineFailure_InspectionTool,
		Tid_CanGoOnlineFailure_Inspection,
		Tid_CanGoOnlineFailure_ConditionalOutput,
		Tid_GoOnlineFailure_RecycleProduct,
		Tid_GoOnlineFailure_RecycleProductId,
		Tid_GoOnlineFailure_Inspection,
		Tid_GoOnlineFailure_Acquisition,
		Tid_GoOnlineFailure_Trigger,
		Tid_GoOnlineFailure_InternalTrigger,
		Tid_GoOnlineFailure_CreatePPQThread,
		Tid_GoOnlineFailure_CreateTimerThread,
		Tid_GoOnlineFailure_InvalidPointerConfig,
		Tid_GoOnlineFailure_RaidBits,
		Tid_GoOnlineFailure_ModuleReadyOutput,
		Tid_ErrorNotEnoughDiskSpace,
		Tid_GoOnlineFailure_SendCameraParam,
		Tid_AmountOfSystemMemoryText,
		Tid_Version,
		Tid_ConfigLoadTime,
		Tid_UserQuestionCloseConfig,
		Tid_UserQuestionSaveChanges,
		Tid_UnableConfig,
		Tid_Load,
		Tid_Save,
		Tid_GoOnlineTime,

		Tid_Camera_UseQuestion,
		Tid_Config_SwitchResetQuestion,
		Tid_Config_SwitchInvalidQuestion,
		Tid_Config_InspectionImportFailed,
		Tid_Error_NoMilHostBuffer,
		Tid_Error_MilImageTypeInvalid,
		Tid_Error_FontNotMfoExt,
		Tid_Error_NoFontConstraintsFileSpec,
		Tid_Error_FontConstraintsRequiredMfoExt,
		Tid_Error_FontConstraintsFileNotExist,
		Tid_Error_FontConstraintsFileEmpty,
		Tid_Error_NoFontControlsFileSpec,
		Tid_Error_FontControlsRequiredMfoExt,
		Tid_Error_FontControlsFileNotExist,
		Tid_Error_FontControlsFileEmpty,
		Tid_ImportInspectionError,
		Tid_Error_InvalidThresholdValue,
		Tid_Pattern_Model2Large,
		Tid_Pattern_DontCare2Small,
		Tid_Pattern_Invalid_ShouldLeave,
		Tid_ErrorGettingInputs,
		Tid_ErrorFindingPPQEntries,
		Tid_InvalidReference,

		Tid_RegressionTest_NoBmpFileSelected,
		Tid_RegressionTest_WrongListSize,
		Tid_RegressionTest_NoFileSelected,
		Tid_RegressionTest_WrongFormat,
		Tid_RegressionTest_NoFilesSelected,
		Tid_RegressionTest_FileNotExist,
		Tid_RemoteOutput_TriggerCountDeleteError,
		Tid_RemoteOutput_DeletingOutput,
		Tid_RemoteOutput_DeletingAllOutput,
		Tid_ResultView_InitFailed,
		Tid_StatTool_ResultFailed,

		Tid_InvalidImagePath,
		Tid_InvalidDrive,
		Tid_InvalidFileName,
		Tid_AP_InvalidFile,
		Tid_Error_you_have_Selected_X_Must_less_then_Y,
		Tid_ArchiveTool_WarningMaxImages,
		Tid_AP_NotEnoughImageMemoryInChangeMode,
		Tid_AP_NotEnoughImageMemoryToSelect,
		Tid_AP_NotEnoughMemoryPleaseDeselectImage,
		Tid_AP_NotEnoughMemoryInChangeMode,
		Tid_AP_NotEnoughMemoryPleaseDeselect,
		Tid_Threshold_OutOfRange,
		Tid_CriticalUnableToConnectTo,
		Tid_UnknownString,
		Tid_Error_DuplicateEmbeddedId,
		Tid_Error_CannotAddInputRequest,
		Tid_Error_CannotAddInputRequestMarker,
		Tid_Error_CannotRunOnce,
		Tid_ErrorIn,

		Tid_Error_NoAnalyzerDetails,
		Tid_Error_AnalyzerCreationFailed,
		Tid_Error_AnalyzerInstantiationFailed,
		Tid_LinearSpecial_OrientationVertical,
		Tid_LinearSpecial_IsRotated,
		Tid_MaskPage_StartOCXFailed,
		Tid_Error_CannotOpenFile,
		Tid_Error_CannotOpenTADlg,
		Tid_UnableStart_Utility,
		Tid_SetClipboardDataFailed,
		Tid_ClipboardDataConverionFailed,
		Tid_ClipboardMemoryFailed,
		Tid_GetClipboardDataFailed,
		Tid_ToolInvalid,
		Tid_Clipboard_VersionMismatch,
		Tid_ColorToolInsertFailed,
		Tid_NonColorToolInsertFailed,
		Tid_RenameError_DuplicateName,
		Tid_ErrorInReset,
		Tid_InValidNewExtents,
		Tid_MilVersion_Error,
		Tid_StatToolInvalidVariable,
		Tid_PatModelSizeErr,
		Tid_PatInvalidFilename,
		Tid_DontCareInvalidFilename,
		Tid_DontCareImage_LoadFailed,
		Tid_PatAllocModelFailed,
		Tid_PatAllocFailed,
		Tid_PatAdditionalCandidatesErr,
		Tid_PatCandidateSpaceErr,
		Tid_PatPreliminaryAcceptanceThresholdErr,
		Tid_Security_GainedAccess,
		Tid_Security_Login,
		Tid_Security_Access,
		Tid_Security_UserNoRights,
		Tid_Security_Access_Granted,
		Tid_Security_Access_Denied,
		Tid_Security_Disabled,
		Tid_OS_Error_Message,

		Tid_Drive_Full,
		Tid_ConditionalValue_Invalid,

		Tid_TableColumnName_NotUnique,
		Tid_TableColumnName_Empty,
		Tid_TableColumn_AddingFailed,
		Tid_TableColumn_RemovingFailed,
		Tid_TableColumn_RenamingFailed,
		Tid_TableColumn_TooManyEquation,
		Tid_TableObject_CreateFailed,
		Tid_TableObject_Nullptr,
		Tid_TableObject_columnValueMapInvalid,
		Tid_TableObject_columnValue_NoFreeGUID,
		Tid_TableObject_createColumnValueObjectFailed,
		Tid_TableObject_MaxRowWrongValue,
		Tid_TableCopy_Nullptr,
		Tid_UnknownCommitError,
		Tid_UnknownCommandError,

		Tid_MoreThanPercentUsed,
		Tid_ConnectTableSourceFailed,
		Tid_ConnectFailed,
		Tid_ConnectInputFailed,
		Tid_NoValidTableConnected,
		Tid_Error_CreationAnalyzerFailed,
		Tid_Error_SetTableAnalyzerData,
		Tid_NoValidColumnConnected,
		Tid_ExcludeHighMustBeHigher,
		Tid_LinkedValue_ValidateStringFailed,
		Tid_ValueObject_ValidateStringFailed,
		Tid_SetEmbeddedValueFailed,
		Tid_ValidateValue_InvalidElementInVariantArray,
		Tid_ValidateValue_ArraySizeInvalid,
		Tid_UnexpectedError,

		Tid_FailedtoStart,
		Tid_SocketInvalid,
		Tid_InvalidCommand,
		Tid_CannotOpenReader,
		Tid_InvalidArguments,
		Tid_InvalidMonitorlist,
		Tid_RejectNotFound,
		Tid_ProductNotFound,
		Tid_CreateChildBufferFailed,
		Tid_CreateImageHandleArrayFailed,
		Tid_NoSourceImage,
		Tid_MilBarCodeInitFailed,
		Tid_BarCodeMatchStringLoadFailed,
		Tid_UpdateOutputImageExtentsFailed,
		Tid_InitExternalTaskFailed,
		Tid_InitImageFailed,
		Tid_UpdateTransformDataFailed,
		Tid_UpdateLineExtentDataFailed,
		Tid_UpdateLinearToolDataFailed,
		Tid_UpdateBufferFailed,
		Tid_UpdateTranslationFailed,
		Tid_InvalidModuleReadyPointer,
		Tid_InvalidRaidBitPointer,
		Tid_IoController_RebuildOutpuListFailed,
		Tid_GetPixelDepthFailed,
		Tid_GetImageExtentFailed,
		Tid_SetImageExtentFailed,
		Tid_GetEdgeAFailed,
		Tid_GetEdgeBFailed,
		Tid_Pattern_ResetImageFileFailed,
		Tid_Pattern_SetCenterFailed,
		Tid_Pattern_GetModelImageNameFailed,
		Tid_Pattern_SetSearchParamFailed,
		Tid_CreateLutFailed,
		Tid_StatToolTestFailed,
		Tid_RebuildFailed,
		Tid_InvalidOwner,
		Tid_WrongType,
		Tid_InvalidLinkedObjectOrSame,
		Tid_WrongInspection,
		Tid_NoShapeHelper,
		Tid_NoCameraToMainImage,
		Tid_NotImageTypePhysical,
		Tid_NotCreated,
		Tid_ToolsetNotCreated,
		Tid_SetImageHandleIndexFailed,
		Tid_CopyImagesFailed,
		Tid_RunFilterFailed,
		Tid_RunWarpFailed,
		Tid_RunRotateFailed,
		Tid_RunArithmeticFailed,
		Tid_LockingFailed,
		Tid_ExternalTask_CheckToRunFailed,
		Tid_ExternalTask_UnknownException,
		Tid_BinarizeFailed,
		Tid_SetValueFailed,
		Tid_RunImagePolarFailed,
		Tid_UnknownException,
		Tid_EmptyValueList,
		Tid_InvalidRange,
		Tid_RunLinearEdgeFailed,
		Tid_ProcessCommandQueueFailed,
		Tid_ArchiveTool_InitFlagFalse,
		Tid_RecalcLUTFailed,
		Tid_RunLutFailed,
		Tid_StringTooLarge,
		Tid_ErrorMcodeGetResult,
	};
} /* namespace ObjectInterfaces */ } /* namespace Seidenader */

namespace SvOi = Seidenader::ObjectInterfaces;