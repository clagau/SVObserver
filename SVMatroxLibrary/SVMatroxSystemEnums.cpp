//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemEnums
//* .File Name       : $Workfile:   SVMatroxSystemEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:27:40  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxSystemEnums.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <mil.h>

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
SVMatroxSystemInquire::SVMatroxSystemInquireEnumMap SVMatroxSystemInquire::m_convertor = boost::assign::map_list_of<>
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
SVMatroxSystemHook::SVMatroxSystemHookEnumMap SVMatroxSystemHook::m_convertor = boost::assign::map_list_of<>
(SVMatroxSystemHook::SVCameraPresent,				M_CAMERA_PRESENT);

// Assign mappings for Hook Info Types
SVMatroxSystemHookInfo::SVMatroxSystemHookInfoEnumMap SVMatroxSystemHookInfo::m_convertor = boost::assign::map_list_of<>
(SVMatroxSystemHookInfo::SVCameraNumber,			M_NUMBER)
(SVMatroxSystemHookInfo::SVCameraPresent,			M_CAMERA_PRESENT)
(SVMatroxSystemHookInfo::SVGigeMacAddress,			M_GC_MAC_ADDRESS);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxSystemEnums.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:27:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2012 14:06:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:35:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
