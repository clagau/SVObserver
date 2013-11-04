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

#ifndef STATICIMAGECOMPRESSIONPOOL_H
#define STATICIMAGECOMPRESSIONPOOL_H

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

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageCompression\StaticImageCompressionPool.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:19:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Feb 2010 08:19:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Sep 2009 16:13:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix include issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Jul 2009 12:17:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new transaction id variable used to track the data manager index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jun 2007 13:06:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * This project includes a change to fix a problem with the attachment of the Lead Tools ActiveX to an SVObserver Dialog.  All Lead Tools includes have been moved from the header files to the cpp files and the appropriate externs were generated to get access to thedesired data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Apr 2003 15:38:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Create() to create the Static Pool dialogs rather than being created in the constructor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Feb 2003 17:56:16   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changes to debug Image Compression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Jan 2003 15:15:14   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include reference and removed direct path information and added it to project information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 13:51:02   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/