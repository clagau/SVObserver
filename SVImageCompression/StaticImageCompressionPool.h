// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : StaticImageCompressionPool.h
// * .File Name       : $Workfile:   StaticImageCompressionPool.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:19:42  $
// ******************************************************************************

#pragma once

#include "SVDataManagerLibrary/DataManager.h"
#include "SVCompressionDlg.h"
#include "Lead.h"

#ifdef ICOMPRESSIONPOOLMASTER
#  define ICOMPPOOLEXT
#else
#  define ICOMPPOOLEXT extern
#endif

/*

StaticImageCompressionPool can operate in two modes: 
1 using resources that are allocated for that instance, and
2 using resources that are allocated globally.  

If StaticImageCompressionPool is constructed without parameters, or with a pool size of 0, then the resources will be allocated for the specific instance. If StaticImageCompressionPool is constructed with a pool size of larger than 0, then a global pool will be created and shared amongst instances.

If an instance is using shared resources, then svmpICompressionDlg will equal NULL; otherwise it will reference the instances resources.
 
For now....   Once a pool has been allocated to a specific size, it can not be resized. If an instance is generated with a different size pool, the new size will be ignored, although everything will be instantiated correctly, but with the original pool size. In the mentioned case, an error number will be returned in an error variable was passed in.

This object maintains a parameterized constructor for allocating the appropriate static resources is the resources are unavailiable.  This object maintains the Create, Destroy, and Destroy Instance operators for handling start-up and shut-down of the resource pool.

The Create operator is used to start-up and initialize the static image compression pool if it is not already created.

The Destroy operator is used to shut-down and destroy the static image compression pool if it was created.

The Destroy Instance operator is used to destroy the single image compression instance that was create for for this single image compression operation.

*/
class StaticImageCompressionPool  
{
public:
	
	StaticImageCompressionPool(long alPoolSize = 0, SVCompressionDlg** appParentsDlg = NULL, HRESULT* ahrpResult = NULL);

	virtual ~StaticImageCompressionPool();
	
	HRESULT Create (); // Pool
	
	HRESULT Destroy (); // Pool
	HRESULT DestroyInstance ();
	
	// This attribute holds the access to the data manager that manages which compression resources are available.
	static  SVDataManager*     svmpICompressionPoolDataManager;
	
	// This attribute contains the interface object to perform the compression operations.
	static SVCompressionDlg*   svmpICompressionDlgPool;
	
	// This attribute holds the current manager handle to be used.
	static long                svmlICompressionPoolManagerHdl;

	// This attribute provides access to the processing space for the compression operations.
	static HGLOBAL*            svmghpDIBInputBufferPool;
	// This attribute holds the number of compression resources available.
	static long                svmlICompressionPoolSize;

	// This attribute denotes the state of the compression system.
	static long                svmlIsInitialized;

	// This attribute protects the compression system if it is currently initializing.
	static CMutex              svmIsInitializedLock;
	
	// This attribute holds the current compression interface dialog.
	SVCompressionDlg*          svmpICompressionDlg;

	// This attribute holds the current compression input buffer used for processing.
	HGLOBAL                    svmghDIBInputBuffer;
	
};

