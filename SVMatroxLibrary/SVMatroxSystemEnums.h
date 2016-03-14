//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystemEnums
//* .File Name       : $Workfile:   SVMatroxSystemEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:15:32  $
//******************************************************************************

#ifndef SVMATROXSYSTEMENUMS_H
#define SVMATROXSYSTEMENUMS_H

#include "SVMatroxTypedefs.h"
#include "SVMatroxEnumConvertor.h"

/**
@SVObjectName Matrox System Control Enum

@SVObjectOverview This enum is used to set the system control values.

@SVObjectOperations None

*/

struct SVMatroxSystemControl
{
	enum SVMatroxSystemControlEnum
	{
		SVAllocationOverscan,		// M_ALLOCATION_OVERSCAN  
									// Sets whether image buffers, allocated on the system, are allocated with an overscan region. 
									// The size of the overscan region is specified by M_ALLOCATION_OVERSCAN_SIZE. 
									// M_DEFAULT - Specifies the optimal setting for the system. For a Host system, the default is M_ENABLE. 
									// M_DISABLE - Specifies that image buffers allocated on the system will have no overscan region. 
									// M_ENABLE - Specifies that image buffers are allocated with an overscan region. 

		SVAllocationOverscanSize,	// M_ALLOCATION_OVERSCAN_SIZE 
									// Sets the size of the overscan region, added around all subsequently allocated image buffers (MbufAlloc...()). 
									// The overscan settings of previously allocated image buffers are not changed. 
									// For more information, see the Buffer overscan region section in Chapter 16: Specifying and managing your data buffers. 
									// To enable or disable the allocation of an overscan region, change the setting of M_ALLOCATION_OVERSCAN. 
									// M_DEFAULT - Specifies the default size of the overscan. 
									// Value - Specifies the size of the overscan, in pixels. 
									// The size of the overscan must be at least 16 pixels. 
	 
		SVBusMasterCopyFromHost,	// M_BUS_MASTER_COPY_FROM_HOST 
									// Sets whether a copy operation from a Host (off-board) buffer to an M_ON_BOARD buffer is driven by your imaging board (bus master) or by the Host CPU. 
									// M_DEFAULT - Specifies to use the default value. 
									// M_DISABLE - Specifies that the copy operation is driven by the Host CPU. 
									// M_ENABLE - Specifies that the copy operation is driven by your imaging board. This speeds up the copy and reduces Host intervention. 

		SVBusMasterCopyToHost,		// M_BUS_MASTER_COPY_TO_HOST 
									// Sets whether a copy operation from an M_ON_BOARD buffer to a Host buffer (off-board) is driven by your imaging board (bus master) or by the Host CPU. 
									// M_DEFAULT - Specifies to use the default value. 
									// M_DISABLE is the default. 
									// M_DISABLE - Specifies that the copy operation is driven by the Host CPU. 
									// M_ENABLE - Specifies that the copy operation is driven by your imaging board. 
									// This speeds up the copy and reduces Host intervention. 
									// Note that this does not accelerate calls to MbufGet...() or MbufPut...() functions. 

		SVDefaultPitchByte,			// M_DEFAULT_PITCH_BYTE 
									// Sets the factor that MIL uses to set the pitch, in bytes, of the buffers allocated on the system. 
									// The pitch is the number of units between the beginnings of any two adjacent rows of the buffer's data. 
									// The buffers allocated on the system will have a pitch that is a multiple of this value. 
									// Sets the factor that MIL uses to set the pitch, in bytes, of the buffers allocated on the system.
									// M_DEFAULT - The default factor is 16. 
									// Value - Specifies the factor used to set the pitch, in bytes, of the buffers allocated on the system. 
									// The value specified must be a power of 2, and must be greater than or equal to 16. 

		SVModifiedBufferHookMode,	// M_MODIFIED_BUFFER_HOOK_MODE 
									// Sets whether to run user-defined functions hooked to a buffer modification on separate threads, up to the number of CPUs present in the computer. 
									// This is particularly useful when functions are hooked using MdigProcess(). 
									// M_DEFAULT Same as M_SINGLE_THREAD.
									// M_SINGLE_THREAD - Specifies that only one thread should be created and that all buffer modifications are run on the same thread. 
									// This is the default value.
									// M_MULTI_THREAD - Specifies to run user-defined functions hooked to a buffer modification on separate threads, up to the number of CPUs present in the computer. 
									// Processing is dispatched by the operating system to available CPUs, depending on their workload. MIL cannot guarantee the processing order of any frame on such a computer, because they are executed concurrently. 
									// Instead, use Microsoft Windows functions to synchronize the threads. See below for combination values. 
/*
		SVProcessingSystem,			// M_PROCESSING_SYSTEM 
									// Sets the system on which to process allocated buffers. 
									// This control type forces the processing of buffers, allocated on the system specified by the SystemId parameter, to be performed by the system specified by the control value. 
									// Note, even when you force processing to be performed by a specific system, some operations might not execute successfully if the specific system does not completely support the requested operation. 
									// This can occur even if processing compensation is enabled. 
									// M_DEFAULT  - Re-establishes the default processing system selected by MIL at system allocation. 
									// M_DEFAULT_HOST - Forces the processing of buffers to be performed by the default Host system. 
									// MIL System identifier - Forces the processing of buffers to be performed by the specified system. 
*/
		SVThreadMode,				// M_THREAD_MODE 
									// Sets the thread execution mode for all threads allocated on the particular system. 
									// M_DEFAULT - Specifies that the thread's execution mode will be returned either to its default value or to the value set using MthrControl() with M_THREAD_MODE. 
									// M_SYNCHRONOUS - Specifies that the execution of a MIL function sent to the processor of a system must be completed (execution terminated) before returning control to the Host. 

		SVTimeout,					// M_TIMEOUT 
									// Sets the maximum amount of time, in secs, for the Host to wait for a synchronous function to return before generating a time-out error. 
									// M_DEFAULT  Specifies the default value. The default value is 20 seconds.
									// Value - Specifies any amount of time, in secs. 
	};

	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVMatroxSystemControlEnum, long> SVMatroxSystemControlEnumMap;
	static SVMatroxSystemControlEnumMap m_convertor;
};

/**
@SVObjectName Matrox System Inquire Enum

@SVObjectOverview This enum is used to get the system control values.

@SVObjectOperations None

*/

struct SVMatroxSystemInquire
{
	enum SVMatroxSystemInquireEnum
	{
		SVAcceleratorPresent,		// M_ACCELERATOR_PRESENT
									// Returns whether the processing accelerator is present.
									// Return values: M_NO  The processing accelerator is not present.  
									// M_YES  The processing accelerator is present.  

		SVAllocationOverscan,		// M_ALLOCATION_OVERSCAN
									// Returns whether image buffers, allocated on the system, will be allocated with an overscan region.
									// Return values: M_DISABLE, M_ENABLE
									// Please see MsysControl() with M_ALLOCATION_OVERSCAN. Note that M_DEFAULT is not returned. (details)

		SVAllocationOverscanSize,	// M_ALLOCATION_OVERSCAN_SIZE
									// Returns the size of the overscan region used when allocating image buffers, in pixels.
									// Return values: 
									// Please see MsysControl() with M_ALLOCATION_OVERSCAN_SIZE. Note that M_DEFAULT is not returned. (details)

		SVAsynchronousCallSupport,	// M_ASYNCHRONOUS_CALL_SUPPORT 
									// Returns whether the system supports asynchronous function execution or not.
									// Return values: M_NO  The system does not support asynchronous execution.  
									// M_YES  The system supports asynchronous execution.  

		SVBoardRevision,			// M_BOARD_REVISION 
									// Returns the board revision number. 

		SVBoardType,				// M_BOARD_TYPE 
									// Returns the type of system board. 

		SVCurrentThreadID,			// M_CURENT_THREAD_ID 
									// Returns the identifier of the current system thread. This identifier can be used with the thread module. 
									// Data type: MIL_ID

		SVDcfSupported,				// M_DCF_SUPPORTED 
									// Returns whether the system supports downloadable digitizer configuration format (DCF) files. 

		SVDefaultPitchByte,			// M_DEFAULT_PITCH_BYTE
									// Returns the factor MIL uses to set the pitch, in bytes, of the buffers allocated on the system. 

		SVDigitizerNum,				// M_DIGITIZER_NUM 
									// Returns the total number of possible independent acquisition paths on the system.
									// Return values: 0 <= Value  The number independent acquisition paths available.  

		SVInitFlag,					// M_INIT_FLAG
									// Returns the system initialization flag.
									// Return values: M_COMPLETE, M_DEFAULT, M_PARTIAL
									// see the InitFlag parameter of MsysAlloc() for details.

		SVLocation,					// M_LOCATION 
									// Returns the location of the specified system. 
									// Return values: M_LOCAL  Local system M_REMOTE  Remote system.  

		SVMemoryFree,				// M_MEMORY_FREE 
									// Returns the total amount of free on-board processing memory, in bytes. 
									// Note that this memory might not be contiguous. 
									// Note that, if the specified system is a JPEG2000 image compression/decompression accelerator, the amount of free on-board processing memory on the JPEG2000 module is returned. Otherwise, the amount of free grab memory will be returned. 

		SVMemorySize,				// M_MEMORY_SIZE 
									// Returns the amount of on-board processing memory, in Mbytes. 

		SVModifiedBufferHookMode,	// M_MODIFIED_BUFFER_HOOK_MODE 
									// Returns whether to run user-defined functions hooked to a buffer modification on separate threads, up to the number of CPUs present in the computer.
									// Return values: M_MULTI_THREAD, M_SINGLE_THREAD
									// Please see MsysControl() with M_MODIFIED_BUFFER_HOOK_MODE. Note that M_DEFAULT is not returned. (details)

		SVNativeID,					// M_NATIVE_ID 
									// Returns the identifier of the Native Library device that corresponds to the given MIL system. 

		SVNativeThreadID,			// M_NATIVE_THREAD_ID 
									// Returns the Native Library identifier of the thread currently used by MIL. 

		SVBoardNumber,				// M_NUMBER 
									// Returns the board number of the system. 
									// Return values: M_DEFAULT, M_DEVn
									// see the SystemNum parameter of MsysAlloc() for details.

		SVOwnerApplication,			// M_OWNER_APPLICATION  
									// Returns the MIL identifier of the application on which the system has been allocated. 
									// Data type: MIL_ID

		SVProcessingSystem,			// M_PROCESSING_SYSTEM  
									// Returns the identifier of the system on which to process allocated buffers. 
									// Data type: MIL_ID

		SVProcessingSystemType,		// M_PROCESSING_SYSTEM_TYPE  
									// Returns the type of the system (on-board or Host) used to process allocated buffers. 

		SVProcessorNum,				// M_PROCESSOR_NUM 
									// Returns the number of processors (CPUs) available on the allocated Matrox imaging board. 
									// Return values: 0 <= Value  The number of processors available.  

		SVSerialNumber,				// M_SERIAL_NUMBER 
									// Returns the serial number of the Matrox Imaging board, as a string.
									// Data type: array of type MIL_TEXT_CHAR
									// Array size: To determine the size of the array required to store the serial number, use MsysInquire() with M_SERIAL_NUMBER_SIZE. 

		SVSerialNumberSize,			// M_SERIAL_NUMBER_SIZE 
									// Returns the length of the string returned by M_SERIAL_NUMBER.

		SVSharedMemoryFree,			// M_SHARED_MEMORY_FREE 
									// Returns the total amount of free on-board shared memory, in bytes.

		SVSystemDescriptor,			// M_SYSTEM_DESCRIPTOR  
									// Returns the system descriptor set using MsysAlloc(). 
									// Data type: array of type MIL_TEXT_CHAR
									// Array size: To determine the size of the array required to store the system descriptor, use MsysInquire() with M_SYSTEM_DESCRIPTOR_SIZE.

		SVSystemDescriptorSize,		// M_SYSTEM_DESCRIPTOR_SIZE 
									// Returns the length of the string returned by M_SYSTEM_DESCRIPTOR.

 		SVSystemType,				// M_SYSTEM_TYPE 
									// Returns the type of system allocated. 

		SVThreadMode,				// M_THREAD_MODE 
									// Returns the thread execution mode for all threads allocated on the system. 
									// Return values: M_DEFAULT, M_SYNCHRONOUS

		SVTimeout,					// M_TIMEOUT 
									// Returns the maximum amount of time, in secs, for the Host to wait for a synchronous function to return before generating a time-out error
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor< SVMatroxSystemInquireEnum, SVMatroxInt > SVMatroxSystemInquireEnumMap;
	static SVMatroxSystemInquireEnumMap m_convertor;
};

/**
@SVObjectName Matrox System Hook Enum

@SVObjectOverview This enum is used to get the system hook values.

@SVObjectOperations None

*/
struct SVMatroxSystemHook
{
	enum SVMatroxSystemHookEnum 
	{
		SVCameraPresent
	};

	typedef SVMatroxEnumConvertor< SVMatroxSystemHookEnum, SVMatroxInt > SVMatroxSystemHookEnumMap;
	static SVMatroxSystemHookEnumMap m_convertor;
};

/**
@SVObjectName Matrox System Hook Info Enum

@SVObjectOverview This enum is used to get the system hook values.

@SVObjectOperations None

*/
struct SVMatroxSystemHookInfo
{
	enum SVMatroxSystemHookInfoEnum 
	{
		SVCameraNumber,
		SVCameraPresent,
		SVGigeMacAddress
	};

	typedef SVMatroxEnumConvertor< SVMatroxSystemHookInfoEnum, SVMatroxInt > SVMatroxSystemHookInfoEnumMap;
	static SVMatroxSystemHookInfoEnumMap m_convertor;
};

#endif

