//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageObjectClass
//* .File Name       : $Workfile:   SVImageObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Oct 2013 14:31:00  $
//******************************************************************************

#ifndef SVIMAGEOBJECTCLASS_H
#define SVIMAGEOBJECTCLASS_H

#include <set>
#include <map>
#include <vector>
#include "SVContainerLibrary/SVVector.h"
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVImageObjectClass
{
public:
	typedef SVSharedPtr< SVImageObjectClass > SVImageObjectParentPtr;

	SVImageObjectClass();
	virtual ~SVImageObjectClass();

	bool empty() const;
	void clear();

	unsigned long size() const;
	void resize( unsigned long p_NewSize );

	SVImageObjectParentPtr GetParentImageObject() const;
	void SetParentImageObject( SVImageObjectParentPtr p_ParentPtr );

	const SVImageInfoClass& GetImageInfo() const;
	void SetImageInfo( const SVImageInfoClass& p_rImageInfo );

	const SVClock::SVTimeStamp& GetLastResetTimeStamp() const;
	
	HRESULT ResetObject();
	HRESULT UpdateTimeStamp();

	BOOL Clear( long lIndex, unsigned long ulValue = 0 );
	BOOL Clear( unsigned long ulValue = 0 );
	BOOL ClearAll( unsigned long ulValue = 0 );

	BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const;
	BOOL GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const;

	BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle );
	BOOL SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType );
	
	BOOL CopyValue( const SVDataManagerHandle& p_FromDMIndexHandle, const SVDataManagerHandle& p_ToDMIndexHandle );
	BOOL CopyValue( const SVDataManagerHandle& p_ToDMIndexHandle );

	BOOL GetImageHandle( long lIndex, SVSmartHandlePointer& p_rValuePtr ) const;
	BOOL GetImageHandle( SVSmartHandlePointer& p_rValuePtr ) const;

	HRESULT LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent );
	HRESULT LoadImage( LPCTSTR p_szFileName, SVDataManagerHandle& p_rToDMIndexHandle, bool p_bRestore=false );

	HRESULT CopyToHandle( SVSmartHandlePointer& p_rHandle );
	HRESULT CopyToBSTR( BSTR &p_rbstrData );
	HRESULT CopyFromBSTR( BSTR p_Image );

	//************************************
	//! Get the Extension for the Image
	//! \param pFileName [in]
	//! \param rExtent [out]
	//! \returns HRESULT S_OK if no error occurs
	//************************************
	static HRESULT GetImageExtentFromFile( LPCTSTR pFileName, SVImageExtentClass& rExtent );
protected:
	struct SVImageObjectElement
	{
		SVImageObjectElement();
		SVImageObjectElement( size_t p_MasterIndex, SVImageObjectParentPtr p_ParentPtr, SVSmartHandlePointer p_ImageHandle );

		size_t m_MasterIndex;
		SVImageObjectParentPtr m_ParentPtr;
		SVSmartHandlePointer m_ImageHandle;

	private:
		// Do not implement
		SVImageObjectElement( const SVImageObjectElement& p_rObject );

		// Do not implement
		const SVImageObjectElement& operator=( const SVImageObjectElement& p_rObject );

	};

	typedef SVSharedPtr< SVImageObjectElement > SVImageObjectElementPtr;
	typedef std::vector< SVImageObjectElementPtr > SVImageHandlePtrVector;

	typedef std::set< long > SVImageIndexSet;
	typedef std::map< long, SVImageObjectElementPtr > SVImageHandleMap;

	BOOL CreateBufferArrays();
	BOOL DestroyBufferArrays();

	BOOL CreateImageBuffer( SVImageInfoClass &rInfo, long p_Index, SVImageObjectElementPtr& p_Handle );
	BOOL CreateImageChildBuffer( const SVImageInfoClass &rParentInfo, SVSmartHandlePointer pParentHandle, 
	                             SVImageInfoClass &rChildInfo, long p_Index, SVImageObjectElementPtr& p_Handle );
	BOOL DestroyImageBuffer( SVImageObjectElementPtr& p_Handle );

	BOOL GetArrayImageHandle( long lIndex, SVSmartHandlePointer& rHandle ) const;

	BOOL CreateImageHandleArray( long lSize );
	BOOL DestroyImageHandleArray();

	HRESULT GetMasterIndex( size_t p_Index, size_t& p_rMasterIndex ) const;
	BOOL LockIndex( long lIndex, long lHandleIndex );
	BOOL LockIndex( long lIndex );
	BOOL UnlockIndex( long lIndex );
	
	HRESULT AddImageHandleToPool( long p_Index );

	BOOL DestroyLocal();
	
	SVClock::SVTimeStamp m_LastUpdate;
	SVClock::SVTimeStamp m_LastReset;

	long m_HandleCount;

	SVDataManagerHandle	m_CurrentDMIndexHandle;

	SVImageInfoClass m_ImageInfo;
	SVImageObjectParentPtr m_ParentImagePtr;

	SVImageHandleMap m_MasterImageHandles;

	SVImageIndexSet m_ImageIndexPool;
	SVImageHandlePtrVector m_ImageHandleArray;
	
private:
	// Do not implement
	SVImageObjectClass( const SVImageObjectClass& p_rObject );

	// Do not implement
	const SVImageObjectClass& operator=( const SVImageObjectClass& p_rObject );
	
};

typedef SVSharedPtr< SVImageObjectClass > SVImageObjectClassPtr;
typedef SVVector< SVImageObjectClassPtr > SVImageObjectClassPtrVector;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageObjectClass.h_v  $
 * 
 *    Rev 1.4   01 Oct 2013 14:31:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Jul 2013 17:55:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include to std set
 * Added include to std vector
 * Added include to std map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:16:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   29 Apr 2013 14:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Apr 2013 08:59:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added LoadImageFullSize.  Modified LoadImage to have an additional parameter to restore and defaulted to false.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:53:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   11 Jan 2013 15:00:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated buffer checking and upating functionality to allow the extent data to be updated without rebuilding all of the image buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Mar 2011 07:20:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   11 Mar 2011 14:23:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   25 Feb 2011 12:11:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   18 Feb 2011 16:30:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Feb 2011 14:45:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Feb 2011 11:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Jan 2011 11:20:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Nov 2010 16:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Nov 2010 13:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Feb 2010 10:33:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image object class classes to handle coping information to and from BSTRs and handles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Dec 2009 11:00:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Sep 2009 10:07:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Jul 2009 13:40:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Feb 2005 10:41:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image object with new load file method to fix buffer leaks and disconnection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 14:27:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Feb 2005 14:29:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Aug 2003 13:00:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 10:18:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:05:06   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New image buffer handling and management class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
