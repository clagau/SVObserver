//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemEnums
//* .File Name       : $Workfile:   SVMatroxSystemEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:30  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <mil.h>
#include "SVMatroxSystemEnums.h"

// Assign mappings for Control Types
SVMatroxSystemControl::SVMatroxSystemControlEnumMap SVMatroxSystemControl::m_convertor = boost::assign::map_list_of<>
(SVMatroxSystemControl::SVAllocationOverscanSize,	M_ALLOCATION_OVERSCAN_SIZE)
(SVMatroxSystemControl::SVBusMasterCopyFromHost,	M_BUS_MASTER_COPY_FROM_HOST)
(SVMatroxSystemControl::SVBusMasterCopyToHost,		M_BUS_MASTER_COPY_TO_HOST)
(SVMatroxSystemControl::SVDefaultPitchByte,			M_DEFAULT_PITCH_BYTE)
(SVMatroxSystemControl::SVModifiedBufferHookMode,	M_MODIFIED_BUFFER_HOOK_MODE)
//(SVMatroxSystemControl::SVProcessingSystem,			M_PROCESSING_SYSTEM)
(SVMatroxSystemControl::SVThreadMode,				M_THREAD_MODE)
(SVMatroxSystemControl::SVTimeout,					M_TIMEOUT);

// Assign mappings for Inquire Types
// BRW - Types for map were implicit for in 32-bit, but must be explicitly specified for 64-bit.
SVMatroxSystemInquire::SVMatroxSystemInquireEnumMap SVMatroxSystemInquire::m_convertor = boost::assign::map_list_of< SVMatroxSystemInquireEnum, SVMatroxInt >
(SVMatroxSystemInquire::SVAcceleratorPresent,		M_ACCELERATOR_PRESENT)
(SVMatroxSystemInquire::SVAllocationOverscanSize,	M_ALLOCATION_OVERSCAN_SIZE)
(SVMatroxSystemInquire::SVAsynchronousCallSupport,	M_ASYNCHRONOUS_CALL_SUPPORT)
(SVMatroxSystemInquire::SVBoardRevision,			M_BOARD_REVISION)
(SVMatroxSystemInquire::SVBoardType,				M_BOARD_TYPE)
(SVMatroxSystemInquire::SVDcfSupported,				M_DCF_SUPPORTED)
(SVMatroxSystemInquire::SVDefaultPitchByte,			M_DEFAULT_PITCH_BYTE)
(SVMatroxSystemInquire::SVDigitizerNum,				M_DIGITIZER_NUM)
(SVMatroxSystemInquire::SVInitFlag,					M_INIT_FLAG)
(SVMatroxSystemInquire::SVLocation,					M_LOCATION)
(SVMatroxSystemInquire::SVMemoryFree,				M_MEMORY_FREE)
(SVMatroxSystemInquire::SVMemorySize,				M_MEMORY_SIZE)
(SVMatroxSystemInquire::SVModifiedBufferHookMode,	M_MODIFIED_BUFFER_HOOK_MODE)
(SVMatroxSystemInquire::SVNativeID,					M_NATIVE_ID)
(SVMatroxSystemInquire::SVNativeThreadID,			M_NATIVE_THREAD_ID)
(SVMatroxSystemInquire::SVBoardNumber,				M_NUMBER)
(SVMatroxSystemInquire::SVOwnerApplication,			M_OWNER_APPLICATION) 
//(SVMatroxSystemInquire::SVProcessingSystem,			M_PROCESSING_SYSTEM)
(SVMatroxSystemInquire::SVProcessingSystemType,		M_PROCESSING_SYSTEM_TYPE)
(SVMatroxSystemInquire::SVProcessorNum,				M_PROCESSOR_NUM)
(SVMatroxSystemInquire::SVSerialNumber,				M_SERIAL_NUMBER)
(SVMatroxSystemInquire::SVSerialNumberSize,			M_SERIAL_NUMBER_SIZE)
(SVMatroxSystemInquire::SVSharedMemoryFree,			M_SHARED_MEMORY_FREE)
(SVMatroxSystemInquire::SVSystemDescriptor,			M_SYSTEM_DESCRIPTOR)
(SVMatroxSystemInquire::SVSystemDescriptorSize,		M_SYSTEM_DESCRIPTOR_SIZE)
(SVMatroxSystemInquire::SVSystemType,				M_SYSTEM_TYPE)
(SVMatroxSystemInquire::SVThreadMode,				M_THREAD_MODE)
(SVMatroxSystemInquire::SVTimeout,					M_TIMEOUT);

// Assign mappings for Hook Types
// BRW - Types for map were implicit for in 32-bit, but must be explicitly specified for 64-bit.
SVMatroxSystemHook::SVMatroxSystemHookEnumMap SVMatroxSystemHook::m_convertor = boost::assign::map_list_of< SVMatroxSystemHookEnum, SVMatroxInt >
(SVMatroxSystemHook::SVCameraPresent,				M_CAMERA_PRESENT);

// Assign mappings for Hook Info Types
// BRW - Types for map were implicit for in 32-bit, but must be explicitly specified for 64-bit.
SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnumMap SVMatroxSystemHookInfo::m_convertor = boost::assign::map_list_of< SVMatroxSystemHookInfoEnum, SVMatroxInt >
(SVMatroxSystemHookInfo::SVCameraNumber,			M_NUMBER)
(SVMatroxSystemHookInfo::SVCameraPresent,			M_CAMERA_PRESENT)
(SVMatroxSystemHookInfo::SVGigeMacAddress,			M_GC_MAC_ADDRESS);

