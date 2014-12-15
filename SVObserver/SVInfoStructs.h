//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryStruct
//* .File Name       : $Workfile:   SVInfoStructs.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   10 Dec 2014 06:44:10  $
//******************************************************************************

#ifndef INC_SVINFOSTRUCTS_INCLUDED
#define INC_SVINFOSTRUCTS_INCLUDED

#ifndef _DEBUG
	#define DebugBreak() NULL
#endif

#include <map>
#include <vector>
#include <boost/any.hpp>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVIOEntryHostStruct.h"
#include "SVValueObjectReference.h"

enum SVProductInspectedState
{
	PRODUCT_NOT_INSPECTED       = 0x8001,	// 1000 0000 0000 0001
	PRODUCT_NOT_ACKNOWLEDGED    = 0x8002,	// 1000 0000 0000 0010
	PRODUCT_INSPECTION_DISABLED	= 0x8004,	// 1000 0000 0000 0100
	
	PRODUCT_INSPECTION_RUN      = 0x0000,	// 0000 0000 0000 0000
	PRODUCT_INSPECTION_WARNING  = 0x0001,	// 0000 0000 0000 0001
	PRODUCT_INSPECTION_FAILED   = 0x0002,	// 0000 0000 0000 0010
	PRODUCT_INSPECTION_PASSED   = 0x0004,	// 0000 0000 0000 0100

	PRODUCT_INSPECTION_MASK     = 0x0FFF,	// 0000 1111 1111 1111
	PRODUCT_INSPECTION_NOT_RUN  = 0x8000	// 1000 0000 0000 0000
};

class SVInputObjectList;
class SVOutputObjectList;
class SVTriggerObject;
class SVPPQObject;
class SVImageClass;
class SVVirtualCamera;
class SVInspectionProcess;
class SVValueObjectClass;
class SVObjectClass;

struct SVIOEntryStruct 
{
	SVIOEntryStruct();
	SVIOEntryStruct( const SVIOEntryStruct& p_rsvObject );

	virtual ~SVIOEntryStruct();

	bool operator<( const SVIOEntryStruct& p_rsvObject ) const;

	bool empty() const;
	void clear();

	void InitEntry();

	SVIOEntryHostStructPtr m_IOEntryPtr;

	bool m_EntryValid;
	bool m_CombinedValue;

};

typedef std::vector< SVIOEntryStruct > SVIOEntryStructVector;


struct SVInputsInfoStruct 
{
	SVInputsInfoStruct();
	SVInputsInfoStruct( const SVInputsInfoStruct& p_rsvObject );
	virtual ~SVInputsInfoStruct();

	const SVInputsInfoStruct& operator=( const SVInputsInfoStruct& p_rsvObject );

	void Reset();
	void Init();

	SVInputObjectList	*pInputsList;
	SVClock::SVTimeStamp m_BeginProcess;
	SVClock::SVTimeStamp m_EndProcess;
};


struct SVOutputsInfoStruct 
{
	SVOutputsInfoStruct();
	SVOutputsInfoStruct( const SVOutputsInfoStruct& p_rsvObject );
	virtual ~SVOutputsInfoStruct();

	const SVOutputsInfoStruct& operator=( const SVOutputsInfoStruct& p_rsvObject );

	void Reset();
	void Init();

	SVOutputObjectList *pOutputsList;
	long lOutputDelay;
	long lResetDelay;
	SVClock::SVTimeStamp m_EndOutputDelay;
	SVClock::SVTimeStamp m_EndResetDelay;
	SVClock::SVTimeStamp m_BeginProcess;
	SVClock::SVTimeStamp m_EndProcess;
};


struct SVPPQInfoStruct 
{
	SVPPQInfoStruct();
	SVPPQInfoStruct( const SVPPQInfoStruct& p_rsvObject );

	virtual ~SVPPQInfoStruct();

	HRESULT Assign( const SVPPQInfoStruct &p_rsvObject, SVDataManagerLockTypeEnum p_LockType );

	void Reset();
	void InitPPQInfo();
	void ClearIndexes();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	SVPPQObject *pPPQ;

	SVDataManagerHandle m_ResultDataDMIndexHandle;
	SVDataManagerHandle m_ResultImagePublishedDMIndexHandle;

	SVIOEntryStructVector m_InputData;

};


struct SVTriggerInfoStruct 
{
	SVTriggerInfoStruct();
	SVTriggerInfoStruct( const SVTriggerInfoStruct& p_rsvObject );
	virtual ~SVTriggerInfoStruct();

	const SVTriggerInfoStruct& operator=( const SVTriggerInfoStruct& p_rsvObject );

	void Reset();
	void Init();

	BOOL bValid;
	SVTriggerObject *pTrigger;
	long lTriggerCount;
	bool m_ToggleState;
	SVClock::SVTimeStamp m_BeginProcess;
	SVClock::SVTimeStamp m_ToggleTimeStamp;
	SVClock::SVTimeStamp m_CallbackReceived;
	SVClock::SVTimeStamp m_PushedOutputs;
	SVClock::SVTimeStamp m_PreviousTrigger;
	boost::any m_Data;
};


struct SVCameraInfoStruct 
{
	SVCameraInfoStruct();
	SVCameraInfoStruct( const SVCameraInfoStruct &p_rsvData );
	virtual ~SVCameraInfoStruct();

	const SVCameraInfoStruct &operator=( const SVCameraInfoStruct &p_rsvData );

	HRESULT Assign( const SVCameraInfoStruct& p_rsvObject, SVDataManagerLockTypeEnum p_LockType );
	HRESULT Assign( SVClock::SVTimeStamp p_StartFrameTS, SVClock::SVTimeStamp p_EndFrameTS, const SVDataManagerHandle& p_rIndexHandle, SVDataManagerLockTypeEnum p_LockType );

	void Reset();
	void ClearInfo();
	void ClearCameraInfo();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	long GetIndex() const;

	const SVDataManagerHandle& GetSourceImageDMIndexHandle() const;

	SVVirtualCamera* pCamera;

	SVClock::SVTimeStamp m_StartFrameTimeStamp;
	SVClock::SVTimeStamp m_EndFrameTimeStamp;
	SVClock::SVTimeStamp m_CallbackTimeStamp;

protected:
	void ClearIndexes();

	SVDataManagerHandle m_SourceImageDMIndexHandle;

};

typedef std::vector< SVCameraInfoStruct > SVStdVectorSVCameraInfoStruct;
typedef std::map< SVVirtualCamera*, SVCameraInfoStruct > SVStdMapSVVirtualCameraPtrSVCameraInfoStruct;
typedef std::map< GUID, SVCameraInfoStruct > SVGUIDSVCameraInfoStructMap;


struct SVInspectionInfoStruct 
{
	SVInspectionInfoStruct();
	SVInspectionInfoStruct( const SVInspectionInfoStruct &p_rsvData );
	virtual ~SVInspectionInfoStruct();

	const SVInspectionInfoStruct &operator=( const SVInspectionInfoStruct &p_rsvData );

	HRESULT Assign( const SVInspectionInfoStruct &p_rsvData, SVDataManagerLockTypeEnum p_LockType );

	void Reset();
	void Init();
	void ClearIndexes();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	SVInspectionProcess* pInspection;
	SVProductInspectedState oInspectedState;
	
	SVDataManagerHandle m_ResultImageDMIndexHandle;

	bool m_CanProcess;
	bool m_InProcess;
	bool m_HasBeenQueued;
	
	SVClock::SVTimeStamp m_BeginInspection;
	SVClock::SVTimeStamp m_EndInspection;
	SVClock::SVTimeStamp m_BeginToolset;
	SVClock::SVTimeStamp m_EndToolset;
	SVClock::SVTimeStamp m_CallbackReceived;

	double m_ToolSetEndTime;
	double m_ToolSetAvgTime;
	long m_lastInspectedSlot; // Shared Memory
};

typedef std::map< SVGUID, SVInspectionInfoStruct > SVGUIDSVInspectionInfoStructMap;
typedef std::vector<SVInspectionInfoStruct> SVStdVectorSVInspectionInfoStruct;


struct SVProductInfoStruct 
{
	SVProductInfoStruct();
	SVProductInfoStruct( const SVProductInfoStruct &p_rsvData );
	virtual ~SVProductInfoStruct();

	const SVProductInfoStruct &operator=( const SVProductInfoStruct &p_rsvData );

	HRESULT Assign( const SVProductInfoStruct &p_rsvData, SVDataManagerLockTypeEnum p_LockType );

	bool empty() const;

	void InitProductInfo();
	void Reset();
	void ClearIndexes();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	inline long ProcessCount() const {return oTriggerInfo.lTriggerCount;}
	inline const SVClock::SVTimeStamp& TimeStamp() const {return oTriggerInfo.m_BeginProcess;}

	bool IsAlive() const;

	void DumpIndexInfo( SVString& p_rData );

	HRESULT GetResultDataIndex( SVDataManagerHandle& p_rHandle ) const;
	HRESULT GetResultImageIndex( SVImageIndexStruct& p_rIndex, const SVGUID& p_rInspectionID ) const;

	bool IsProductActive() const;
	void SetProductActive();
	void SetProductComplete();

	SVString m_ProductState;
	BOOL bTriggered;
	BOOL bStartAcquisition;
	BOOL bFinishAcquisition;
	BOOL bDelayExpired;
	BOOL bDataComplete;
	BOOL bStreamed;
	HRESULT hrPPQStatus;

	SVTriggerInfoStruct oTriggerInfo;
	SVInputsInfoStruct oInputsInfo;
	SVOutputsInfoStruct oOutputsInfo;
	SVPPQInfoStruct oPPQInfo;

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct m_svCameraInfos;
	SVGUIDSVInspectionInfoStructMap	m_svInspectionInfos;
	long m_lastInspectedSlot; // Shared Memory

protected:
	long m_ProductActive;
};

struct SVInspectionCompleteInfoStruct
{
	SVInspectionCompleteInfoStruct();
	SVInspectionCompleteInfoStruct( const SVGUID& p_rInspectionID, const SVProductInfoStruct& p_rProductInfo );
	SVInspectionCompleteInfoStruct( const SVInspectionCompleteInfoStruct& p_rObject );

	virtual ~SVInspectionCompleteInfoStruct();

	HRESULT Assign( const SVGUID& p_rInspectionID, const SVProductInfoStruct& p_rProductInfo, SVDataManagerLockTypeEnum p_LockType );
	HRESULT Assign( const SVInspectionCompleteInfoStruct& p_rObject, SVDataManagerLockTypeEnum p_LockType );

	bool empty() const;
	void clear();

	void ClearExtraInspectionIndexes();

	SVGUID m_InspectionID;
	SVProductInfoStruct m_ProductInfo;

};

struct SVInspectionNameUpdate
{
	SVInspectionNameUpdate();
	SVInspectionNameUpdate( const SVString& p_rName );
	SVInspectionNameUpdate( const SVInspectionNameUpdate& p_rObject );

	virtual ~SVInspectionNameUpdate();

	SVString m_InspectionName;

};

struct SVRenameObject
{
	SVRenameObject();
	SVRenameObject( const SVString& p_rOldName, const SVString& p_rNewName, const SVGUID& p_rObjectId );
	SVRenameObject( const SVRenameObject& p_rObject );

	virtual ~SVRenameObject();

	SVString m_OldName;
	SVString m_NewName;
	SVGUID m_ObjectId;

};

struct SVAddTool
{
	SVAddTool();
	SVAddTool( SVToolClass* p_pTool, int p_Index );
	SVAddTool( const SVAddTool& p_rObject );

	virtual ~SVAddTool();

	SVToolClass* m_pTool;
	int m_Index;

};

struct SVDeleteTool
{
	SVDeleteTool();
	SVDeleteTool( SVToolClass* p_pTool );
	SVDeleteTool( const SVDeleteTool& p_rObject );

	virtual ~SVDeleteTool();

	SVToolClass* m_pTool;

};

struct SVRemoveImages
{
	typedef std::set< SVGUID > SVImageIdSet;

	SVRemoveImages();
	SVRemoveImages( const SVImageIdSet& p_rImages );
	SVRemoveImages( const SVRemoveImages& p_rObject );

	virtual ~SVRemoveImages();

	SVImageIdSet m_Images;

};

struct SVRemoveValues
{
	typedef std::set< SVGUID > SVValueIdSet;

	SVRemoveValues();
	SVRemoveValues( const SVValueIdSet& p_rValues );
	SVRemoveValues( const SVRemoveValues& p_rObject );

	virtual ~SVRemoveValues();

	SVValueIdSet m_Values;

};

struct SVRemoveSubjectStruct
{
	SVRemoveSubjectStruct();

	virtual ~SVRemoveSubjectStruct();

};

struct SVInputRequestInfoStruct
{
	SVInputRequestInfoStruct();
	SVInputRequestInfoStruct( const SVValueObjectReference& p_rValueObject, const _variant_t& p_rValue );
	virtual ~SVInputRequestInfoStruct();

	void Reset();
	void Init();

  SVValueObjectReference refValueObject;
	_variant_t m_Value;

private:
	SVInputRequestInfoStruct( const SVInputRequestInfoStruct& p_rsvObject );

	const SVInputRequestInfoStruct& operator=( const SVInputRequestInfoStruct& p_rsvObject );

};

typedef SVSharedPtr< SVInputRequestInfoStruct > SVInputRequestInfoStructPtr;
typedef std::vector< SVInputRequestInfoStructPtr > SVInputRequestInfoStructPtrVector;

struct SVInputImageRequestInfoStruct
{
	SVInputImageRequestInfoStruct();
	virtual ~SVInputImageRequestInfoStruct();

	void Reset();
	void Init();

	CString m_strObjectName;
	SVImageInfoClass m_ImageInfo;
	SVSmartHandlePointer m_ImageHandlePtr;
	bool m_bUsingCameraName;

private:
	SVInputImageRequestInfoStruct( const SVInputImageRequestInfoStruct& p_rsvObject );

	const SVInputImageRequestInfoStruct& operator=( const SVInputImageRequestInfoStruct& p_rsvObject );

};

typedef SVSharedPtr< SVInputImageRequestInfoStruct > SVInputImageRequestInfoStructPtr;
typedef std::vector< SVInputImageRequestInfoStructPtr > SVInputImageRequestInfoStructPtrVector;

typedef HRESULT (CALLBACK *LPSVFINISHPROC)(void*,void*,void*);

struct SVOutputRequestInfoStruct
{
	SVOutputRequestInfoStruct();
	virtual ~SVOutputRequestInfoStruct();

	void Reset();
	void Init();

	SVProductInfoStruct m_ProductInfo;
	void *pCaller;
	void *pOwner;
	LPSVFINISHPROC pCallback;
};

typedef SVVector< SVOutputRequestInfoStruct*, SVOutputRequestInfoStruct* > SVOutputRequestInfoStructArray;

struct SVProductInfoRequestStruct
{
	SVProductInfoRequestStruct();
	SVProductInfoRequestStruct( SVProductInfoStruct* p_pProduct );
	SVProductInfoRequestStruct( const SVProductInfoRequestStruct& p_rObject );
	virtual ~SVProductInfoRequestStruct();

	const SVProductInfoRequestStruct& operator=( const SVProductInfoRequestStruct& p_rObject );

	bool empty() const;
	void clear();

	HRESULT NotifyRequestComplete() const;
	HRESULT WaitForRequest() const;

  SVProductInfoStruct* m_pProduct;

private:
	HRESULT GetWaitHandle( HANDLE& p_rHandle ) const;

	HANDLE m_WaitHandle;

};

#endif /* _INC_SVINFOSTRUCTS_INCLUDED */
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInfoStructs.h_v  $
 * 
 *    Rev 1.4   10 Dec 2014 06:44:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added flag to InspectionInfoStruct to prevent additional queuing in Inspection::StartProcess from the PPQ::StartInspection. New Flag m_HasBeenQueued.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Aug 2014 18:02:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added m_lastInspectedSlot to SVInspectionInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 12:16:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   02 May 2013 09:52:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated variable name for the trigger toggle state variable from trigger toggle to toggle state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   29 Apr 2013 14:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:59:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   18 Feb 2013 13:21:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection input info structs from using a CString as the data element to a variant.  Updated all methods that call the inspection interface from CString to variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   21 Jan 2013 10:52:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVTriggerInfoStruct to have a member variable m_data of type boost::any
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   11 Oct 2012 08:53:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new start process state to the inspection info struct and update the variable when the inspection is queued and whe the product gets reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   10 Jul 2012 12:15:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added operator= to class/structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   02 Jul 2012 16:58:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   19 Jun 2012 12:50:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   16 Sep 2011 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   14 Jul 2011 16:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   14 Jul 2011 08:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   05 Jul 2011 14:45:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   18 Apr 2011 10:15:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   10 Feb 2011 14:55:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   15 Dec 2010 10:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   08 Dec 2010 12:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   09 Nov 2010 11:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix invalid variable and type names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   08 Nov 2010 14:40:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   16 Feb 2010 10:16:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Undated Trigger Information structure to include the trigger toggle state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Jan 2010 15:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Dec 2009 11:07:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Sep 2009 10:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Aug 2009 08:41:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with trigger toggle and output delays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   14 Jul 2009 14:00:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Nov 2005 08:30:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added published result image variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Aug 2005 10:16:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Aug 2005 12:32:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Jul 2005 11:29:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added m_bUsingCameraName to use when looking up for main image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Jun 2005 14:54:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use SVValueObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Nov 2004 15:07:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added hrPPQStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   25 Sep 2003 14:34:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Sep 2003 15:28:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PRODUCT_INSPECTION_RUN
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 Sep 2003 11:16:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed enum from PRODUCT_INSPECTION_ERROR to PRODUCT_INSPECTION_NOT_RUN.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 13:02:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jun 2003 15:18:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 10:25:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Mar 2003 13:27:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new struct - SVInputImageRequestInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Dec 2002 19:59:28   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified SVInputRequestInfoStruct to use a CString instead of a char*
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Dec 2002 15:19:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added
 * inline long SVProductInfoStruct::ProcessCount()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:46:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
