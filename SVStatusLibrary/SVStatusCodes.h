//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatusCodes
//* .File Name       : $Workfile:   SVStatusCodes.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   10 Dec 2014 12:12:38  $
//******************************************************************************

#ifndef SVSTATUSCODES_H
#define SVSTATUSCODES_H

#include <comdef.h>

#include "SVMessage/SVMessage.h"

#define SV_MAKE_SEVERITY( p_Severity ) \
	( ( ( ( unsigned long ) p_Severity ) << 30 ) & 0xC0000000 )

#define SV_MAKE_FACILITY( p_Facility ) \
	( ( ( ( unsigned long ) p_Facility ) << 16 ) & 0x3FFF0000 )

#define SV_MAKE_CODE( p_Code ) \
	( ( ( unsigned long ) p_Code ) & 0x0000FFFF )

#define SV_MAKE_STATUS( p_Severity, p_Facility, p_Code ) \
	( SV_MAKE_SEVERITY( p_Severity ) | SV_MAKE_FACILITY( p_Facility ) | SV_MAKE_CODE( p_Code ) )

#define SV_SUCCEEDED( p_Status ) SUCCEEDED( p_Status )

#define SV_FAILED( p_Status ) FAILED( p_Status )

#define SV_SEVERITY( p_Status ) \
	( ( ( ( unsigned long ) p_Status ) >> 30 ) & 0x00000003 )

enum SVStatusFacilityEnum
{
	SV_FACILITY_SVNONE = FAC_NONE,
	SV_FACILITY_WIN32 = FACILITY_WIN32,
	SV_FACILITY_SYSTEM = FAC_SYSTEM,
	SV_FACILITY_APPLICATION = FAC_APPLICATION,
	SV_FACILITY_SVTCPIP = FAC_SVTCPIP,
	SV_FACILITY_SVSECURITY = FAC_SVSECURITY,
	SV_FACILITY_SVPLCRSLINX = FAC_SVPLCRSLinx,
	SV_FACILITY_SVPLCCOMMAND = FAC_SVPLCCOMMAND,
	SV_FACILITY_SVPLC = FAC_SVPLC,
	SV_FACILITY_SVPIPES = FAC_SVPIPES,
	SV_FACILITY_SVOBSERVER = FAC_SVOBSERVER,
	SV_FACILITY_SVMACHINEMESSAGE = FAC_SVMACHINEMESSAGE,
	SV_FACILITY_SVLIBRARY = FAC_SVLIBRARY,
	SV_FACILITY_SVLANGUAGEMANAGER = FAC_SVLANGUAGEMANAGER,
	SV_FACILITY_SVIPC = FAC_SVIPC,
	SV_FACILITY_SVINTEK = FAC_SVINTEK,
	SV_FACILITY_SVIMCOMMAND = FAC_SVIMCOMMAND,
	SV_FACILITY_SVIMAGECOMPRESSION = FAC_SVIMAGECOMPRESSION,
	SV_FACILITY_SVFOCUSNT = FAC_SVFOCUSNT,
	SV_FACILITY_SVFOCUSDBMANAGER = FAC_SVFOCUSDBMANAGER,
	SV_FACILITY_SVFOCUSDB = FAC_SVFOCUSDB,
	SV_FACILITY_SVEQUATION = FAC_SVEQUATION,
	SV_FACILITY_SVDATAMANAGER = FAC_SVDATAMANAGER,
	SV_FACILITY_SVCMDCOMSERVER = FAC_SVCMDCOMSERVER,
	SV_FACILITY_SVCMDCOMCLIENT = FAC_SVCMDCOMCLIENT,
	SV_FACILITY_SVBATCHREPORT = FAC_SVBATCHREPORT,
	SV_FACILITY_SVACCESS = FAC_SVACCESS,
	SV_FACILITY_SVTVICLPT = FAC_SVTVICLPT,
	SV_FACILITY_SVOLIBRARY = FAC_SVOLIBRARY,
	SV_FACILITY_SVSYSTEMLIBRARY = FAC_SVSYSTEMLIBRARY,
	SV_FACILITY_SVMATROXLIBRARY = FAC_SVMATROXLIBRARY,
	SV_FACILITY_SVXMLLIBRARY = FAC_SVXMLLIBRARY,
};

enum SVStatusLevelEnum
{
	SV_LEVEL_WARNING       = SEV_WARNING,
	SV_LEVEL_SUCCESS       = SEV_SUCCESS,
	SV_LEVEL_INFORMATIONAL = SEV_INFORMATIONAL,
	SV_LEVEL_FATAL         = SEV_FATAL,
};

// This is a code that would be returned by a function such as 
// Matrox, Intek, ect...These codes are not defined in the message file.
enum SVStatusCodesEnum
{
	SV_SUCCESS_SVPLC_0000 = SVMSG_PLC_NO_ERROR,
	SV_WARNING_SVPLC_0001 = SVMSG_PLC_CONSTRUCTOR_FAILED,
	SV_FATAL_SVPLC_0002 = SVMSG_PLC_FAILED,

	SV_WARNING_SVPLCRSLINX_0001 = SVMSG_PLC_INVALID_TRANSACTION_ID_USED_TO_OPEN_CONNECTION,
	SV_WARNING_SVPLCRSLINX_0002 = SVMSG_PLC_INVALID_STATION_NBR_USED_TO_OPEN_CONNECTION,
	SV_WARNING_SVPLCRSLINX_0003 = SVMSG_PLC_REPLY_TIMEOUT,

	SV_SUCCESS_SVBATCHREPORT_0000 = SVMSG_BR_NO_ERROR,

	SV_SUCCESS_SVFOCUSNT_0000 = SVMSG_SVF_NO_ERROR,
	SV_INFORMATION_SVFOCUSNT_0001 = SVMSG_SVF_APPLICATION_STARTED,
	SV_INFORMATION_SVFOCUSNT_0002 = SVMSG_SVF_APPLICATION_ENDED,
	SV_WARNING_SVFOCUSNT_0003 = SVMSG_SVF_END_OF_FILE,
	SV_WARNING_SVFOCUSNT_0004 = SVMSG_SVF_NO_ITEM_SELECTED,
	SV_WARNING_SVFOCUSNT_0005 = SVMSG_SVF_CREATOR_DATA_MISSING,
	SV_WARNING_SVFOCUSNT_0006 = SVMSG_SVF_DESCRIPTION_DATA_MISSING,
	SV_WARNING_SVFOCUSNT_0007 = SVMSG_SVF_NAME_DATA_MISSING,
	SV_WARNING_SVFOCUSNT_0008 = SVMSG_SVF_GROUP_ALREADY_EXISTS,
	SV_WARNING_SVFOCUSNT_0009 = SVMSG_SVF_PRODUCT_ALREADY_EXISTS,
	SV_WARNING_SVFOCUSNT_000A = SVMSG_SVF_PRODUCT_NOT_FOUND,
	SV_FATAL_SVFOCUSNT_000B = SVMSG_SVF_UNHANDLED_EXCEPTION,
	SV_FATAL_SVFOCUSNT_000C = SVMSG_SVF_FAILED_TO_INITIALIZE_PLC,
	SV_FATAL_SVFOCUSNT_000D = SVMSG_SVF_FAILED_TO_INITIALIZE_BATCH_HISTORY,
	SV_WARNING_SVFOCUSNT_000E = SVMSG_SVF_ACCESS_DENIED,
	SV_WARNING_SVFOCUSNT_000F = SVMSG_SVF_MUST_BE_UNIQUE,
	SV_WARNING_SVFOCUSNT_0010 = SVMSG_SVF_NO_RESET_ADDRESS,
	SV_WARNING_SVFOCUSNT_0011 = SVMSG_SVF_REMOTE_MONITOR_FAILED,
	SV_WARNING_SVFOCUSNT_0012 = SVMSG_SVF_MISSING_REMOTE_COMMAND_PARAMETERS,
	SV_WARNING_SVFOCUSNT_0013 = SVMSG_SVF_INVALID_COMMAND_RECEIVED,
	SV_SUCCESS_SVFOCUSNT_0014 = SVMSG_SVF_REMOTE_ACTIVE,
	SV_SUCCESS_SVFOCUSNT_0015 = SVMSG_SVF_REMOTE_DISABLED,
	SV_WARNING_SVFOCUSNT_0016 = SVMSG_SVF_COULD_NOT_SET_SCREEN_NBR_IN_PLC,
	SV_WARNING_SVFOCUSNT_0017 = SVMSG_SVF_COULD_NOT_GET_SCREEN_NBR_FROM_PLC,
	SV_WARNING_SVFOCUSNT_0018 = SVMSG_SVF_RUNREFRESH_THREAD_ERROR,
	SV_WARNING_SVFOCUSNT_0019 = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_MACHINE_MESSAGE,
	SV_WARNING_SVFOCUSNT_001A = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP1_TOTALS,
	SV_WARNING_SVFOCUSNT_001B = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP2_TOTALS,
	SV_WARNING_SVFOCUSNT_001C = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP1_DEFECTS,
	SV_WARNING_SVFOCUSNT_001D = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP2_DEFECTS,
	SV_WARNING_SVFOCUSNT_001E = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_VALUES,
	SV_WARNING_SVFOCUSNT_001F = SVMSG_SVF_MISSING_EQUIPMENT_ENTRY,
	SV_WARNING_SVFOCUSNT_0020 = SVMSG_SVF_NO_PRODUCT_LOADED,
	SV_WARNING_SVFOCUSNT_0021 = SVMSG_SVF_NO_PRODUCT_SELECTED,

	SV_SUCCESS_SVFOCUSDB_0000 = SVMSG_DB_NO_ERROR,
	SV_WARNING_SVFOCUSDB_0001 = SVMSG_DB_USED_ENGLISH,
	SV_WARNING_SVFOCUSDB_0002 = SVMSG_DB_USED_ANY,
	SV_FATAL_SVFOCUSDB_0003 = SVMSG_DB_OPEN_FAILED,
	SV_FATAL_SVFOCUSDB_0004 = SVMSG_DB_FETCH_FAILED,
	SV_FATAL_SVFOCUSDB_0005 = SVMSG_DB_INSERT_FAILED,
	SV_FATAL_SVFOCUSDB_0006 = SVMSG_DB_NOT_FOUND,
	SV_WARNING_SVFOCUSDB_0007 = SVMSG_DB_MULTI_LINGUAL_STRING_NOT_FOUND,
	SV_FATAL_SVFOCUSDB_0008 = SVMSG_DB_UPDATE_FAILED,
	SV_FATAL_SVFOCUSDB_0009 = SVMSG_DB_COMMIT_FAILED,
	SV_FATAL_SVFOCUSDB_000A = SVMSG_DB_EXECUTE_SQL_FAILED,
	SV_WARNING_SVFOCUSDB_000B = SVMSG_DB_MULTI_LINGUAL_DELETE_FAILED,
	SV_WARNING_SVFOCUSDB_000C = SVMSG_DB_FUNCTION_NOT_IMPLEMENTED,
	SV_WARNING_SVFOCUSDB_000D = SVMSG_DB_ODBC_ERROR,
	SV_WARNING_SVFOCUSDB_000E = SVMSG_DB_INVALID_PRODUCT_CODE,
	SV_WARNING_SVFOCUSDB_000F = SVMSG_DB_PRODUCT_GROUP_NOT_FOUND,
	SV_WARNING_SVFOCUSDB_0010 = SVMSG_DB_PRODUCT_NOT_FOUND,
	SV_WARNING_SVFOCUSDB_0011 = SVMSG_DB_MUST_BE_UNIQUE,
	SV_WARNING_SVFOCUSDB_0012 = SVMSG_DB_NO_OPEN_BATCH_EXISTS,

	SV_SUCCESS_SVFOCUSDBMANAGER_0000 = SVMSG_DBM_NO_ERROR,

	SV_SUCCESS_SVIMCOMMAND_0000 = SVMSG_SVIMCMD_NO_ERROR,
	SV_FATAL_SVIMCOMMAND_0001 = SVMSG_SVIMCMD_DOWNLOAD_FAILED,
	SV_FATAL_SVIMCOMMAND_0002 = SVMSG_SVIMCMD_GO_ONLINE_FAILED,
	SV_FATAL_SVIMCOMMAND_0003 = SVMSG_SVIMCMD_OUT_OF_MEMORY,
	SV_FATAL_SVIMCOMMAND_0004 = SVMSG_SVIMCMD_INVALID_SERVER_RESPONSE,
	SV_WARNING_SVIMCOMMAND_0005 = SVMSG_SVIMCMD_REQUEST_REJECTED,
	SV_WARNING_SVIMCOMMAND_0006 = SVMSG_SVIMCMD_FILE_IO_ERROR,
	SV_SUCCESS_SVIMCOMMAND_0007 = SVMSG_SVIMCMD_ONLINE,
	SV_SUCCESS_SVIMCOMMAND_0008 = SVMSG_SVIMCMD_LOADING,
	SV_SUCCESS_SVIMCOMMAND_0009 = SVMSG_SVIMCMD_LOADED,
	SV_SUCCESS_SVIMCOMMAND_000A = SVMSG_SVIMCMD_ONLINE_PENDING,
	SV_SUCCESS_SVIMCOMMAND_000B = SVMSG_SVIMCMD_RUNNING_TEST,
	SV_SUCCESS_SVIMCOMMAND_000C = SVMSG_SVIMCMD_RUNNING,
	SV_SUCCESS_SVIMCOMMAND_000D = SVMSG_SVIMCMD_REGRESSION_TEST,
	SV_SUCCESS_SVIMCOMMAND_000E = SVMSG_SVIMCMD_SETUP_MODE,
	SV_SUCCESS_SVIMCOMMAND_000F = SVMSG_SVIMCMD_SAVING_CONFIG,
	SV_SUCCESS_SVIMCOMMAND_0010 = SVMSG_SVIMCMD_STOPPING,
	SV_FATAL_SVIMCOMMAND_0011 = SVMSG_STREAM_ALREADY_REGISTERED_BY_ANOTHER_CONTROL,
	SV_FATAL_SVIMCOMMAND_0012 = SVMSG_STREAM_ALREADY_REGISTERED_BY_THIS_CONTROL,
	SV_FATAL_SVIMCOMMAND_0013 = SVMSG_STREAM_NOT_REGISTERED,
	SV_FATAL_SVIMCOMMAND_0014 = SVMSG_SERVER_UNABLE_TO_CONNECT_TO_CONTROL,
	SV_FATAL_SVIMCOMMAND_0015 = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS,
	SV_INFORMATION_SVIMCOMMAND_0016 = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST,
	SV_INFORMATION_SVIMCOMMAND_0017 = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_NOT_AN_INPUT,
	SV_INFORMATION_SVIMCOMMAND_0018 = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST,
	SV_FATAL_SVIMCOMMAND_0019 = SVMSG_TOO_MANY_REQUESTED_ITEMS,
	SV_INFORMATION_SVIMCOMMAND_001A = SVMSG_REQUESTED_LIST_IS_EMPTY,
	SV_FATAL_SVIMCOMMAND_001B = SVMSG_PRODUCT_NO_LONGER_AVAILABLE,
	SV_INFORMATION_SVIMCOMMAND_001C = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE,
	SV_FATAL_SVIMCOMMAND_001D = SVMSG_CONFIGURATION_NOT_LOADED,
	SV_WARNING_SVIMCOMMAND_001E = SVMSG_OBJECT_NOT_PROCESSED,
	SV_FATAL_SVIMCOMMAND_001F = SVMSG_UNSUPPORTED_COMPRESSION_FORMAT,
	SV_FATAL_SVIMCOMMAND_0021 = SVMSG_INDEX_NOT_FOUND,
	SV_INFORMATION_SVIMCOMMAND_0022 = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE,
	SV_INFORMATION_SVIMCOMMAND_0023 = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY,
	SV_FATAL_SVIMCOMMAND_0024 = SVMSG_INCORRECT_ARRAY_DATA_TYPE,
	SV_FATAL_SVIMCOMMAND_0025 = SVMSG_INVALID_IMAGE_SOURCE,
	SV_INFORMATION_SVIMCOMMAND_0026 = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED,
	SV_INFORMATION_SVIMCOMMAND_0027 = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE,
	SV_WARNING_SVIMCOMMAND_0028 = SVMSG_INCORRECT_CHECKSUM,
	SV_FATAL_SVIMCOMMAND_0029 = SVMSG_MEMORY_ERROR_IN_STREAMING_DATA,
	SV_INFORMATION_SVIMCOMMAND_002A = SVMSG_OBJECT_ALREADY_SET_IN_THIS_LIST,
	SV_FATAL_SVIMCOMMAND_002B = SVMSG_43_FONT_STR_LEN_GREATER_THAN_MAX,
	SV_FATAL_SVIMCOMMAND_002C = SVMSG_44_RUNONCE_ONLINE,
	SV_WARNING_SVIMCOMMAND_002D = SVMSG_45_CALIBRATEFONT_ERROR,
	SV_WARNING_SVIMCOMMAND_002E = SVMSG_46_LOADFONT_ERROR,
	SV_WARNING_SVIMCOMMAND_002F = SVMSG_47_SAVEFONT_ERROR,
	SV_WARNING_SVIMCOMMAND_0030 = SVMSG_48_READSTRING_ERROR,
	SV_WARNING_SVIMCOMMAND_0031 = SVMSG_49_VERIFYSTRING_ERROR,
	SV_FATAL_SVIMCOMMAND_0032 = SVMSG_50_MODE_CHANGING_ERROR,
	SV_FATAL_SVIMCOMMAND_0033 = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR,
	SV_FATAL_SVIMCOMMAND_0034 = SVMSG_52_MODE_GUI_IN_USE_ERROR,
	SV_INFORMATION_SVIMCOMMAND_0035 = SVMSG_53_SVIM_NOT_IN_RUN_MODE,
	SV_INFORMATION_SVIMCOMMAND_0036 = SVMSG_54_SVIM_BUSY,
	SV_WARNING_SVIMCOMMAND_0037 = SVMSG_NO_FONT_FOUND,
	SV_WARNING_SVIMCOMMAND_0038 = SVMSG_FONT_IMAGE_FAIL,
	SV_WARNING_SVIMCOMMAND_0039 = SVMSG_FONT_COPY_FAIL,
	SV_FATAL_SVIMCOMMAND_003A = SVMSG_FONT_COPY_THREW_EXCEPTION,
	SV_WARNING_SVIMCOMMAND_003B = SVMSG_FONT_INVALID,
	SV_WARNING_SVIMCOMMAND_003C = SVMSG_INVALID_VARIANT_PARAMETER,
	SV_WARNING_SVIMCOMMAND_003D = SVMSG_FAILED_TO_REMOVE_FONT,
	SV_WARNING_SVIMCOMMAND_003E = SVMSG_FAILED_TO_ADD_FONT,
	SV_FATAL_SVIMCOMMAND_0801 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0801 ),
	SV_FATAL_SVIMCOMMAND_0802 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0802 ),
	SV_FATAL_SVIMCOMMAND_0803 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0803 ),
	SV_FATAL_SVIMCOMMAND_0804 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0804 ),
	SV_FATAL_SVIMCOMMAND_0805 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0805 ),
	SV_FATAL_SVIMCOMMAND_0806 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVIMCOMMAND, 0x0806 ),

	SV_SUCCESS_SVIPC_0000 = SVMSG_IPC_NO_ERROR,

	SV_SUCCESS_SVLANGUAGEMANAGER_0000 = SVMSG_LM_NO_ERROR,

	SV_SUCCESS_SVLIBRARY_0000 = SVMSG_LIB_NO_ERROR,
	SV_FATAL_SVLIBRARY_0001 = SVMSG_LIB_LISTCTRL_ACTION_FAILED,
	SV_FATAL_SVLIBRARY_0002 = SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED,
	SV_FATAL_SVLIBRARY_0003 = SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED,
	SV_WARNING_SVLIBRARY_0004 = SVMSG_LIB_SET_REGISTRY_VALUE_FAILED,
	SV_WARNING_SVLIBRARY_0005 = SVMSG_LIB_REGISTRY_EXPORT_FAILED,
	SV_WARNING_SVLIBRARY_0006 = SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED,
	SV_WARNING_SVLIBRARY_0007 = SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED,
	SV_WARNING_SVLIBRARY_0008 = SVMSG_LIB_REGISTRY_IMPORT_FAILED,
	SV_WARNING_SVLIBRARY_0009 = SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE,
	SV_WARNING_SVLIBRARY_000A = SVMSG_LIB_REGISTRY_IMPORT_EXPECTED_KEY,
	SV_FATAL_SVLIBRARY_000B = SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR,
	SV_FATAL_SVLIBRARY_000C = SVMSG_LIB_PACKFILE_IO_ERROR,

	SV_SUCCESS_SVPIPES_0000 = SVMSG_PIPES_NO_ERROR,
	SV_FATAL_SVPIPES_0001 = SVMSG_PIPES_OPEN_FAILED,
	SV_FATAL_SVPIPES_0002 = SVMSG_PIPES_READ_FAILED,
	SV_FATAL_SVPIPES_0003 = SVMSG_PIPES_WRITE_FAILED,
	SV_FATAL_SVPIPES_0004 = SVMSG_PIPES_IO_ERROR,
	SV_WARNING_SVPIPES_0005 = SVMSG_PIPES_END_OF_FILE,
	SV_WARNING_SVPIPES_0006 = SVMSG_PIPES_REQUIRED_DATA_MISSING,

	SV_SUCCESS_SVPLCCOMMAND_0000 = SVMSG_PLCCMD_NO_ERROR,
	SV_FATAL_SVPLCCOMMAND_0001 = SVMSG_PLCCMD_SET_VALUE_FAILED,
	SV_WARNING_SVPLCCOMMAND_0002 = SVMSG_PLC_ERROR,
	SV_FATAL_SVPLCCOMMAND_0003 = SVMSG_PLCCMD_GET_VALUE_FAILED,
	SV_FATAL_SVPLCCOMMAND_0004 = SVMSG_PLC_APC_DATA_BAD,

	SV_SUCCESS_SVTCPIP_0000 = SVMSG_TCPIP_NO_ERROR,

	SV_SUCCESS_SVOBSERVER_0000 = SVMSG_SVO_NO_ERROR,
	SV_WARNING_SVOBSERVER_0001 = SVMSG_SVO_THREAD_ERROR,
	SV_WARNING_SVOBSERVER_000F = SVMSG_SVO_15_LOAD_CONFIG_WARNING,
	SV_FATAL_SVOBSERVER_0010 = SVMSG_SVO_16_LOAD_CONFIG_ERROR,
	SV_FATAL_SVOBSERVER_0011 = SVMSG_SVO_17_NO_MEMORY_ERROR,
	SV_FATAL_SVOBSERVER_0012 = SVMSG_SVO_18_OUT_OF_NP_MEMORY,
	SV_FATAL_SVOBSERVER_0013 = SVMSG_SVO_19_PROBLEM_ALLOCATING_DIGITIZER,
	SV_FATAL_SVOBSERVER_0014 = SVMSG_SVO_20_INCORRECT_CAMERA_FILE,
	SV_INFORMATION_SVOBSERVER_0019 = SVMSG_SVO_25_SVOBSERVER_STARTED,
	SV_INFORMATION_SVOBSERVER_001A = SVMSG_SVO_26_SVOBSERVER_STOPPED,
	SV_INFORMATION_SVOBSERVER_001B = SVMSG_SVO_27_SVOBSERVER_GO_ONLINE,
	SV_INFORMATION_SVOBSERVER_001C = SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE,
	SV_INFORMATION_SVOBSERVER_001D = SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED,
	SV_WARNING_SVOBSERVER_001E = SVMSG_SVO_30_EXCEPTION_IN_MIL,
	SV_FATAL_SVOBSERVER_001F = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL,
	SV_FATAL_SVOBSERVER_0020 = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY,
	SV_WARNING_SVOBSERVER_0021 = SVMSG_SVO_33_OBJECT_INDEX_INVALID,
	SV_WARNING_SVOBSERVER_0022 = SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE,
	SV_FATAL_SVOBSERVER_0023 = SVMSG_SVO_35_LEGACY_ERROR_TRAP,
	SV_FATAL_SVOBSERVER_0024 = SVMSG_SVO_36_MIL_ERROR,
	SV_FATAL_SVOBSERVER_0025 = SVMSG_SVO_37_INVALID_RUN_STATUS,
	SV_FATAL_SVOBSERVER_0026 = SVMSG_SVO_38_INPUT_REQUEST_FAILED,
	SV_FATAL_SVOBSERVER_0027 = SVMSG_SVO_39_IMAGE_REQUEST_FAILED,
	SV_INFORMATION_SVOBSERVER_0028 = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED,
	SV_FATAL_SVOBSERVER_0028 = SVMSG_SVO_40_UPDATE_MAINIMAGE_FAILED,
	SV_INFORMATION_SVOBSERVER_0029 = SVMSG_SVO_41_ERROR_MIMBINARIZE,
	SV_INFORMATION_SVOBSERVER_002A = SVMSG_SVO_42_CURRENT_INDEX,
	SV_INFORMATION_SVOBSERVER_002B = SVMSG_SVO_43_GENERAL,
	SV_FATAL_SVOBSERVER_10D7 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOLIBRARY, 0x10D7 ),
	SV_FATAL_SVOBSERVER_10D8 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOLIBRARY, 0x10D8 ),
	SV_FATAL_SVOBSERVER_10E0 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOLIBRARY, 0x10E0 ),
	SV_SUCCESS_SVOBSERVER_2000_HAS_EXTENTS = SV_MAKE_STATUS( SV_LEVEL_SUCCESS, SV_FACILITY_SVOBSERVER, 0x2000 ),
	SV_FATAL_SVOBSERVER_2001_SET_IMAGE_HANDLE_INDEX = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2001 ),
	SV_FATAL_SVOBSERVER_2002_INVALID_SOURCE_INDEX = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2002 ),
	SV_FATAL_SVOBSERVER_2003_CAMERA_NOT_FOUND = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2003 ),
	SV_FATAL_SVOBSERVER_2004_INVALID_CAMERA = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2004 ),
	SV_FATAL_SVOBSERVER_2005_INVALID_IMAGE = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2005 ),
	SV_FATAL_SVOBSERVER_2006_DUPLICATE_DISCRETE_OUTPUT = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOBSERVER, 0x2006 ),

	SV_INFORMATION_SVSECURITY_0000 = SVMSG_SVS_ACCESS_GRANTED_1,
	SV_INFORMATION_SVSECURITY_0001 = SVMSG_SVS_ACCESS_GRANTED,
	SV_INFORMATION_SVSECURITY_0002 = SVMSG_SVS_ACCESS_GRANTED1,
	SV_INFORMATION_SVSECURITY_0003 = SVMSG_SVS_ACCESS_DENIED,
	SV_INFORMATION_SVSECURITY_0004 = SVMSG_SVS_ACCESS_LOGGED_OUT,

	SV_INFORMATION_SVACCESS_0000 = SECURITY_POINT_ACCESS_POINTS,
	SV_INFORMATION_SVACCESS_0001 = SECURITY_POINT_FILE_MENU,
	SV_INFORMATION_SVACCESS_0002 = SECURITY_POINT_ROOT,
	SV_INFORMATION_SVACCESS_0003 = SECURITY_POINT_FILE_MENU_NEW,
	SV_INFORMATION_SVACCESS_0004 = SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION,
	SV_INFORMATION_SVACCESS_0005 = SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION,
	SV_INFORMATION_SVACCESS_0006 = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS,
	SV_INFORMATION_SVACCESS_0007 = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION,
	SV_INFORMATION_SVACCESS_0008 = SECURITY_POINT_FILE_MENU_PRINT,
	SV_INFORMATION_SVACCESS_0009 = SECURITY_POINT_FILE_MENU_PRINT_SETUP,
	SV_INFORMATION_SVACCESS_000A = SECURITY_POINT_FILE_MENU_SAVE_IMAGE,
	SV_INFORMATION_SVACCESS_000B = SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS,
	SV_INFORMATION_SVACCESS_000C = SECURITY_POINT_FILE_MENU_EXIT,
	SV_INFORMATION_SVACCESS_000D = SECURITY_POINT_VIEW_MENU,
	SV_INFORMATION_SVACCESS_000E = SECURITY_POINT_VIEW_MENU_PPQ_BAR,
	SV_INFORMATION_SVACCESS_000F = SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE,
	SV_INFORMATION_SVACCESS_0010 = SECURITY_POINT_MODE_MENU,
	SV_INFORMATION_SVACCESS_0011 = SECURITY_POINT_MODE_MENU_RUN,
	SV_INFORMATION_SVACCESS_0012 = SECURITY_POINT_MODE_MENU_STOP,
	SV_INFORMATION_SVACCESS_0013 = SECURITY_POINT_MODE_MENU_EDIT_TOOLSET,
	SV_INFORMATION_SVACCESS_0015 = SECURITY_POINT_MODE_MENU_TEST,
	SV_INFORMATION_SVACCESS_0016 = SECURITY_POINT_MODE_MENU_REGRESSION_TEST,
	SV_INFORMATION_SVACCESS_0017 = SECURITY_POINT_EXTRAS_MENU,
	SV_INFORMATION_SVACCESS_0018 = SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON,
	SV_INFORMATION_SVACCESS_0019 = SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER,
	SV_INFORMATION_SVACCESS_001A = SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS,
	SV_INFORMATION_SVACCESS_001B = SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP,
	SV_INFORMATION_SVACCESS_001C = SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION,
	SV_INFORMATION_SVACCESS_001D = SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN,
	SV_INFORMATION_SVACCESS_001E = SECURITY_POINT_UNRESTRICTED_FILE_ACCESS,
	SV_INFORMATION_SVACCESS_001F = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT,
	SV_INFORMATION_SVACCESS_0020 = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL,
	SV_INFORMATION_SVACCESS_0021 = SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,
	SV_INFORMATION_SVACCESS_0022 = SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE,

	SV_SUCCESS_SVMACHINEMESSAGE_0000 = SVMSG_SVMM_BASE_MASK,

	SV_SUCCESS_CMDCOMCTRL_0000 = SVMSG_CMDCOMSRV_NO_ERROR,
	SV_WARNING_CMDCOMCTRL_0001 = SVMSG_CMDCOMSRV_ERROR,
	SV_WARNING_CMDCOMCTRL_0002 = SVMSG_CMDCOMSRV_XML_ERROR,
	SV_WARNING_CMDCOMCTRL_0003 = SVMSG_CMDCOMSRV_MEMORY_ERROR,
	SV_WARNING_CMDCOMCTRL_0004 = SVMSG_CMDCOMSRV_PACKEDFILE_ERROR,
	SV_WARNING_CMDCOMCTRL_0005 = SVMSG_CMDCOMSRV_FILE_ERROR,
	SV_WARNING_CMDCOMCTRL_0006 = SVMSG_IMAGE_NOT_LOCKED,
	SV_WARNING_CMDCOMCTRL_0007 = SVMSG_IMAGE_NOT_LOCKED,
	SV_WARNING_CMDCOMCTRL_0008 = SVMSG_NO_PPQ_FOUND,
	SV_WARNING_CMDCOMCTRL_0009 = SVMSG_CMDCOMCTRL_ERROR,
	SV_WARNING_CMDCOMCTRL_000A = SVMSG_CMDCOMCTRL_XML_ERROR,
	SV_WARNING_CMDCOMCTRL_000B = SVMSG_CMDCOMCTRL_MEMORY_ERROR,
	SV_WARNING_CMDCOMCTRL_000C = SVMSG_CMDCOMCTRL_FILE_ERROR,
	SV_WARNING_CMDCOMCTRL_000D = SVMSG_CMDCOMCTRL_PROFESSIONAL_LICENSE_ONLY,

	SV_SUCCESS_SVDATAMANAGER_05DB = SVDM_NO_ERROR,
	SV_WARNING_SVDATAMANAGER_05DC = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR,
	SV_WARNING_SVDATAMANAGER_05DD = SVDM_1501INVALIDLOCKTYPE_ERROR,
	SV_WARNING_SVDATAMANAGER_05DE = SVDM_1502NO_INDEXESAVAILABLE_ERROR,
	SV_WARNING_SVDATAMANAGER_05DF = SVDM_1503INVALIDBUFFERINDEX_ERROR,
	SV_WARNING_SVDATAMANAGER_05E0 = SVDM_1504BUFFERINDEXNOTINUSE_ERROR,
	SV_WARNING_SVDATAMANAGER_05E1 = SVDM_1505NOLOCKSTORELEASE_ERROR,
	SV_INFORMATION_SVDATAMANAGER_05E2 = SVDM_1506FINALLOCKTYPERELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05E3 = SVDM_1507FINALINDEXRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_05E4 = SVDM_1508INDEXESNOTFREED_FATAL,
	SV_FATAL_SVDATAMANAGER_05E5 = SVDM_1509NOALLOCATE_FATAL,
	SV_FATAL_SVDATAMANAGER_05E6 = SVDM_1510MAXIMUMDEPTH_ERROR,
	SV_FATAL_SVDATAMANAGER_05E7 = SVDM_1511NOALLOCATE_FATAL,
	SV_FATAL_SVDATAMANAGER_05E8 = SVDM_1512COULDNOTLOCKDATAMANAGER_FATAL,
	SV_FATAL_SVDATAMANAGER_05E9 = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL,
	SV_FATAL_SVDATAMANAGER_05EA = SVDM_1514NOTUSED_FATAL,
	SV_FATAL_SVDATAMANAGER_05EB = SVDM_1515COULDNOTLOCKMANAGEDARRAY_FATAL,
	SV_FATAL_SVDATAMANAGER_05EC = SVDM_1516COULDNOTLOCKMANAGEDARRAY_FATAL,
	SV_FATAL_SVDATAMANAGER_05ED = SVDM_1517COULDNOTLOCKMANAGEDARRAY_FATAL,
	SV_FATAL_SVDATAMANAGER_05EE = SVDM_1518COULDNOTLOCKDATAMANAGER_FATAL,
	SV_FATAL_SVDATAMANAGER_05EF = SVDM_1519ACQUISITIONNOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_05F0 = SVDM_1520INSPECTIONNOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_05F1 = SVDM_1521PPQNOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_05F2 = SVDM_1522ARCHIVENOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_05F3 = SVDM_1523DCOMNOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_05F4 = SVDM_1524OTHERNOLOCKTORELEASE_FATAL,
	SV_INFORMATION_SVDATAMANAGER_05F5 = SVDM_1525FINALACQUISITIONRELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05F6 = SVDM_1526FINALINSPECTIONRELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05F7 = SVDM_1527FINALPPQRELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05F8 = SVDM_1528FINALARCHIVERELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05F9 = SVDM_1529FINALDCOMRELEASE_INFORMATION,
	SV_INFORMATION_SVDATAMANAGER_05FA = SVDM_1530FINALOTHERRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_05FB = SVDM_1531MANAGEDINDEXARRAY_FATAL,
	SV_INFORMATION_SVDATAMANAGER_05FC = SVDM_1532FINALDCOMRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_05FD = SVDM_1533LASTINSPECTIONNOLOCKTORELEASE_FATAL,
	SV_INFORMATION_SVDATAMANAGER_05FE = SVDM_1534FINALLASTINSPECTIONRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_05FF = SVDM_1535COULDNOTLOCKMANAGEDARRAY_FATAL,
	SV_FATAL_SVDATAMANAGER_0600 = SVDM_1536DISPLAY_NOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_0601 = SVDM_1537DISPLAY_FINALRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_0602 = SVDM_1538LASTINSPECTED_NOLOCKTORELEASE_FATAL,
	SV_FATAL_SVDATAMANAGER_0603 = SVDM_1539LASTINSPECTED_FINALRELEASE_INFORMATION,
	SV_FATAL_SVDATAMANAGER_0604 = SVDM_1540LOCK_INVALIDTRANSACTIONID_FATAL,
	SV_FATAL_SVDATAMANAGER_0605 = SVDM_1541RELEASE_INVALIDTRANSACTIONID_FATAL,

	SV_SUCCESS_SVINTEK_0000 = SVMSG_SVINTEK_NO_ERROR,

	SV_FATAL_SVIMAGECOMPRESSION_0064 = SVMSG_SVIMAGECOMPRESSION_NO_STATIC_POOL,

	SV_FATAL_SVEQUATION_0064 = SVMSG_TOO_MANY_VARIABLES,

	SV_FATAL_SVFILEACQUISITION_0076 = SVMSG_IMAGE_FORMAT_ERROR,
	SV_FATAL_SVFILEACQUISITION_0077 = SVMSG_IMAGE_LOAD_ERROR,

	SV_SUCCESS_SVMATROXGIGE_0000 = SVMSG_SVMATROXGIGE_NO_ERROR,

	SV_WARNING_SVTVICLPT_0001 = SVMSG_INVALID_LINE_STATE,

	SV_FATAL_SVOLIBRARY_0001 = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVOLIBRARY, 0x0001 ),

	SV_FATAL_SVSYSTEMLIBRARY_0001 = SVMSG_QUEUE_USER_APC_ERROR,
	SV_FATAL_SVSYSTEMLIBRARY_0002 = SVMSG_THREAD_CREATION_ERROR,
	SV_FATAL_SVSYSTEMLIBRARY_0003 = SVMSG_THREAD_EXIT_ERROR,

	SV_SUCCESS_SVMATROXLIBRARY_0000 = SVMSG_SVMATROXLIBRARY_NO_ERROR,
	SV_FATAL_SVMATROXLIBRARY_0000 = SVMSG_SVMATROXLIBRARY_UNKNOWN_FATAL_ERROR,

	SV_10000_XML_LIB_INVALID_REF_COUNT = SV_MAKE_STATUS( SV_LEVEL_FATAL, SV_FACILITY_SVXMLLIBRARY, 10000 ),

};

#endif // SVSTATUSCODES_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\SVStatusCodes.h_v  $
 * 
 *    Rev 1.2   10 Dec 2014 12:12:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed status code for Mode_Menu_Edit_Move_Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Jul 2014 17:12:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Split the Security access from Image Online Display to Image Display Update and Result Display Update
 * Enum changed SVStatusCodesEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:44:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Oct 2012 09:42:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed error in SV_SEVERITY macro.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Sep 2012 18:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Status information for XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Aug 2012 15:47:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new status code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Jun 2012 17:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Extra error codes were added to track additional states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Mar 2011 11:38:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new status code for object that has extents but are not changeable via the GUI.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Oct 2010 15:20:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  705
 * SCR Title:  Change File Open Attribute in Command Methods Put and Get
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated status codes to remove invalid status codes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Oct 2010 14:36:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  705
 * SCR Title:  Change File Open Attribute in Command Methods Put and Get
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new status code used for file not found condition and unable to create path condition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2010 08:16:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jul 2010 09:14:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated message list to include new message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 13:32:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved file to this library to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
