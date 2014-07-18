//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQObject
//* .File Name       : $Workfile:   SVPPQObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.18  $
//* .Check In Date   : $Date:   17 Jul 2014 20:13:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVMaterialsLibrary/SVMaterials.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVInfoStructs.h"
#include "SVPPQEnums.h"
#include "SVPPQShiftRegister.h"
#include "SVValueObject.h"
#include "SVVirtualCamera.h"
#include "SVCameraTriggerData.h"
#include "BasicValueObjects.h"
#include "SVMonitorList.h"
#pragma endregion Includes

#pragma region Declarations
const TCHAR	PpqLength[]			= _T("Length");
const long	StandardPpqLength	= 2;

class SVInspectionProcess;
#pragma endregion Declarations

class SVPPQObject : 
	public SVObjectClass,
	public SVObserverTemplate< SVInspectionCompleteInfoStruct >
{
// Setup Functions

public:
	typedef std::vector< std::pair< _variant_t, bool > > SVVariantBoolVector;

	SVPPQObject( LPCSTR ObjectName );
	SVPPQObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPPQOBJECT );

	virtual ~SVPPQObject();
private:
	void init();
public:

	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;
	
	virtual HRESULT ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData );

	BOOL Create();
	BOOL Rebuild();
	BOOL Destroy();
	BOOL IsCreated();
	BOOL DetachAll();

	BOOL SetPPQOutputMode( SVPPQOutputModeEnum ePPQOutputMode );
	BOOL SetOutputDelay( long lDelayTime );
	BOOL SetResetDelay( long lResetTime );
	BOOL SetPPQLength( long lPPQLength );
	BOOL SetMaintainSourceImages( bool bMaintainImages );
	BOOL SetInspectionTimeout( long lTimeoutMillisec );
	void SetConditionalOutputName( const SVString& conditionName );

	BOOL GetPPQOutputMode( SVPPQOutputModeEnum& rePPQOutputMode ) const;
	BOOL GetOutputDelay( long& rlDelayTime ) const;
	BOOL GetResetDelay( long& rlResetTime ) const;
	BOOL GetPPQLength( long& rlPPQLength ) const;
	long GetPPQLength() const;
	BOOL GetMaintainSourceImages( bool& rbMaintainImages ) const;
	BOOL GetInspectionTimeout( long& rlTimeoutMillisec ) const;
	const SVString& GetConditionalOutputName() const;

	BOOL AttachTrigger( SVTriggerObject *pTrigger );
	BOOL AttachCamera( SVVirtualCamera *pCamera, long lPosition, bool p_AllowMinusOne = false );
	BOOL AttachInspection( SVInspectionProcess *pInspection );

	BOOL DetachTrigger( SVTriggerObject *pTrigger );
	BOOL DetachCamera( SVVirtualCamera *pCamera, BOOL bRemoveDepends = FALSE );
	BOOL DetachInspection( SVInspectionProcess *pInspection );

	BOOL AddSharedCamera(  SVVirtualCamera *pCamera );
	BOOL GetInspectionCount( long &lSize );

	size_t GetCameraCount() const;
	HRESULT GetCameraList( std::deque< SVVirtualCamera* >& p_rCameras ) const;
	HRESULT GetVirtualCameras( SVVirtualCameraMap& p_rCameras ) const;

	BOOL GetTrigger( SVTriggerObject *&ppTrigger );
	BOOL GetInspection( long lIndex, SVInspectionProcess *&ppInspection );

	HRESULT GetInspections( std::vector< SVInspectionProcess* >& rvecInspections ) const;

	// PPQ position management functions
	BOOL SetCameraPPQPosition( long lPosition, SVVirtualCamera *pCamera );
	BOOL GetCameraPPQPosition( long &lPosition, SVVirtualCamera *pCamera );

	// Runtime Functions
	HRESULT CanGoOnline();
	HRESULT GoOnline();
	BOOL GoOffline();
	BOOL IsOnline() const;

	bool IsProductExpired( const SVProductInfoStruct* pProduct ) const;
	HRESULT GetProduct( SVProductInfoStruct& p_rProduct, long lProcessCount ) const;

	BOOL ReserveNextRunOnceProductInfoStruct( SVProductInfoStruct& p_rsvProduct, SVDataManagerLockTypeEnum p_LockType = SV_PPQ );

	BOOL AddInput( SVIOEntryHostStructPtr pInput );
	BOOL RemoveInput( SVIOEntryHostStructPtr pInput );
	HRESULT GetInputIOValues( SVVariantBoolVector& p_rInputValues ) const;
	BOOL AssignInputs( const SVVariantBoolVector& p_rInputValues );
	BOOL RebuildInputList(bool bHasCameraTrigger);
	BOOL GetAvailableInputs( SVIOEntryHostStructPtrList& p_IOEntries ) const;
	BOOL GetAllInputs( SVIOEntryHostStructPtrList& p_IOEntries ) const;
	BOOL AddDefaultInputs();
	BOOL AddToAvailableInputs(SVIOObjectType eType, CString strName );
	SVIOEntryHostStructPtr GetInput( const SVString& name ) const;

	void AddCameraDataInputs(SVIOEntryHostStructPtrList& list);
	void RemoveCameraDataInputs(SVIOEntryHostStructPtrList& list);
	void SetDefaultConditionalOutput();
	bool HasCameraDataInputForConditionalOutput() const;
	bool HasDigitalInputForConditionalOutput() const;

	BOOL AddOutput( SVIOEntryHostStructPtr pOutput );
	BOOL RemoveOutput( SVIOEntryHostStructPtr pOutput );
	BOOL WriteOutputs( SVProductInfoStruct *pProduct );
	BOOL ResetOutputs();
	BOOL RebuildOutputList();
	BOOL GetAllOutputs( SVIOEntryHostStructPtrList& p_IOEntries ) const;
	BOOL AddDefaultOutputs();

	BOOL FinishCamera( void *pCaller, SVODataResponseClass *pResponse );
	BOOL FinishTrigger( void *pCaller, SVTriggerInfoStruct& p_rTriggerInfo );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void DumpDMInfo( LPCTSTR p_szName ) const;

	bool IsProductAlive( long p_ProductCount ) const;

	HRESULT GetNextAvailableIndexes( SVPPQInfoStruct& p_rPPQInfo, SVDataManagerLockTypeEnum p_LockType ) const;

	SVInputObjectList*    m_pInputList;
	SVOutputObjectList*   m_pOutputList;

	// PLC Connection String
	HRESULT SetPLCName( CString p_rstrName );
	HRESULT GetPLCName( CString& p_rstrName ) const;
	const CString& GetPLCName() const;

	SVDWordValueObjectClass m_voOutputState;
	SVLongValueObjectClass m_voTriggerCount;

	void PersistInputs(SVObjectWriter& rWriter);

	SVCameraTriggerData& GetCameraInputData();
	long GetExtraBufferSize() const;

	HRESULT SetMonitorList(const ActiveMonitorList& rActiveList);
	bool HasActiveMonitorList() const;

protected:
	typedef SVVector< SVInspectionProcess* > SVPPQInspectionProcessVector;

	struct SVTriggerQueueElement
	{
		SVTriggerQueueElement();
		SVTriggerQueueElement( const SVTriggerQueueElement& p_rObject );

		virtual ~SVTriggerQueueElement();

		SVTriggerInfoStruct m_TriggerInfo;
		SVVariantBoolVector m_Inputs;
	};

	struct SVCameraQueueElement
	{
		SVCameraQueueElement();
		SVCameraQueueElement( const SVCameraQueueElement& p_rObject );
		SVCameraQueueElement( SVVirtualCamera* p_pCamera, const SVODataResponseClass& p_rData );

		virtual ~SVCameraQueueElement();

		SVVirtualCamera* m_pCamera;
		SVODataResponseClass m_Data;
	};

	struct SVCameraInfoElement
	{
		SVCameraInfoElement();
		SVCameraInfoElement( const SVCameraInfoElement& p_rObject );

		virtual ~SVCameraInfoElement();

		long m_CameraPPQIndex;

		SVIOEntryHostStructPtr m_IOEntryPtr;
		bool m_ToggleState;

	};

	typedef std::map<SVString, SVGUID> SVFilterElementMap; // Dotted name to Guid mapping
	typedef std::map<SVGUID, SVFilterElementMap> SVInspectionFilterElementMap; // Inspection Guid to FilterElementMap mapping
	struct SVSharedMemoryFilters
	{
		SVSharedMemoryFilters();

		void clear();
		SVInspectionFilterElementMap m_RejectConditionValues;
	};
	SVSharedMemoryFilters m_SharedMemoryItems;
	bool m_bActiveMonitorList;

	typedef std::pair< long, SVInspectionInfoStruct > SVInspectionInfoPair;
	typedef std::pair< long, SVProductInfoRequestStruct > SVProductRequestPair;
	typedef std::vector< SVProductInfoStruct* > SVProductPositionQueue;
	typedef std::deque< long > SVProcessCountDeque;
	typedef std::set< long > SVProcessCountSet;
	typedef std::set< SVGUID > SVInspectionIDSet;
	typedef SVTQueueObject< SVCameraQueueElement > SVCameraResponseQueue;
	typedef SVTQueueObject< SVProductInfoStruct* > SVProductPointerQueue;
	typedef SVRingBuffer< SVTriggerQueueElement > SVTriggerInfoQueue;
	typedef SVTQueueObject< SVProductInfoStruct > SVProductQueue;
	typedef SVTQueueObject< SVInspectionInfoPair > SVInspectionInfoQueue;
	typedef SVTQueueObject< long > SVProcessCountQueue;
	typedef SVTQueueObject< SVProductRequestPair > SVProductRequestQueue;
	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;
	typedef std::map< SVVirtualCamera*, SVCameraInfoElement > SVCameraInfoMap;
	typedef std::map< SVVirtualCamera*, SVCameraQueueElement > SVPendingCameraResponseMap;

	static void CALLBACK OutputTimerCallback( UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2 );
	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	HRESULT MarkProductInspectionsMissingAcquisiton( SVProductInfoStruct& p_rProduct, SVVirtualCamera* p_pCamera );

	void ThreadProcess( bool& p_WaitForEvents );

	HRESULT NotifyProcessTimerOutputs();

	HRESULT ProcessTrigger( bool& p_rProcessed );
	HRESULT ProcessNotifyInspections( bool& p_rProcessed );
	HRESULT ProcessInspections( bool& p_rProcessed );
	HRESULT ProcessDelayOutputs( bool& p_rProcessed );
	HRESULT ProcessResetOutputs( bool& p_rProcessed );
	HRESULT ProcessCameraResponses( bool& p_rProcessed );
	HRESULT ProcessCameraInputs( bool& p_rProcessed );
	HRESULT ProcessCompleteInspections( bool& p_rProcessed );
	HRESULT ProcessProductRequests( bool& p_rProcessed );

	HRESULT ProcessTimeDelayOutputs( SVProductInfoStruct& p_rProduct );
	HRESULT ProcessTimeDelayAndDataCompleteOutputs( SVProductInfoStruct& p_rProduct, bool& p_rProcessed );

	HRESULT GetProductInfoStruct( SVProductInfoStruct*& p_rpProduct, long lProcessCount ) const;
	HRESULT GetProductInfoStruct( SVProductInfoStruct*& p_rpProduct, SVClock::SVTimeStamp p_TimeStamp ) const;
	HRESULT GetProductIndex( long& p_rIndex, long lProcessCount ) const;
	HRESULT GetProductIndex( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp ) const;

	SVProductInfoStruct* IndexPPQ( SVTriggerInfoStruct& p_rTriggerInfo );
	BOOL InitializeProduct( SVProductInfoStruct* p_pNewProduct, const SVVariantBoolVector& p_rInputValues );
	BOOL StartOutputs( SVProductInfoStruct* p_pProduct );
	HRESULT NotifyInspections( long p_Offset );
	HRESULT StartInspection( const SVGUID& p_rInspectionID );

	bool AddResultsToPPQ( long p_PPQIndex );
	bool SetInspectionComplete( long p_PPQIndex );
	bool SetProductComplete( long p_PPQIndex );
	bool SetProductComplete( SVProductInfoStruct& p_rProduct );

	bool SetProductIncomplete( long p_PPQIndex );
	bool SetProductIncomplete( SVProductInfoStruct& p_rProduct );

	BOOL RecycleProductInfo( SVProductInfoStruct *pProduct );

	BOOL RebuildProductInfoStructs();

	HRESULT DisplayGoOnlineError(const CString& sReason, HRESULT hr = S_OK);

	HRESULT ProcessCameraResponse( const SVCameraQueueElement& p_rElement );

	HRESULT BuildCameraInfos( SVStdMapSVVirtualCameraPtrSVCameraInfoStruct& p_rCameraInfos ) const;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;

	long m_ProcessingOutputDelay;
	SVClock::SVTimeStamp m_NextOutputDelayTimestamp;

	long m_ProcessingOutputReset;
	SVClock::SVTimeStamp m_NextOutputResetTimestamp;

	// Queues for the PPQ's threads to store incoming objects to be processed
	SVTriggerInfoQueue m_oTriggerQueue;
	SVProcessCountQueue m_oCamerasQueue;
	SVInspectionInfoQueue m_oInspectionQueue;
	SVProcessCountQueue m_oOutputsDelayQueue;
	SVProcessCountQueue m_oOutputsResetQueue;
	mutable SVProductRequestQueue m_ProductRequests;

	SVCameraResponseQueue m_CameraResponseQueue;
	SVPendingCameraResponseMap m_PendingCameraResponses;

	SVProcessCountSet m_oNotifyInspectionsSet;
	SVInspectionIDSet m_ProcessInspectionsSet;

	// Pointers to the PPQ's I/O Lists
	SVIOEntryHostStructPtrList m_AllInputs;
	SVIOEntryHostStructPtrList m_AllOutputs;
	SVIOEntryHostStructPtrList m_UsedInputs;
	SVIOEntryHostStructPtrList m_UsedOutputs;

	SVIOEntryHostStructPtr m_pTriggerToggle;	
	SVIOEntryHostStructPtr m_pOutputToggle;
	SVIOEntryHostStructPtr m_pDataValid;

	// Pointers to the Subsystem objects used by the PPQ
	SVTriggerObject*            m_pTrigger;
	SVPPQInspectionProcessVector    m_arInspections;

	// Pointer to the PPQ's buckets
	SVPPQShiftRegister m_ppPPQPositions;

	SVProductInfoStruct*  m_pMasterProductInfos;
	SVProductPointerQueue m_qAvailableProductInfos;

	// DataManager index handles
	SVSmartIndexArrayHandlePtr m_pResultDataCircleBuffer;
	SVSmartIndexArrayHandlePtr m_pResultImagePublishedCircleBuffer;

	// Value Objects used by the PPQ
	SVBoolValueObjectClass  m_voTriggerToggle;	
	SVBoolValueObjectClass  m_voOutputToggle;
	SVBoolValueObjectClass  m_voACK;
	SVBoolValueObjectClass  m_voNAK;
	SVBoolValueObjectClass  m_voMasterFault;
	SVBoolValueObjectClass  m_voMasterWarning;
	SVBoolValueObjectClass  m_voNotInspected;
	SVBoolValueObjectClass  m_voDataValid;

	SVCameraTriggerData m_CameraInputData;

	bool m_TriggerToggle;
	bool m_OutputToggle;

	// Pointers to the Master Lists
	SVCameraInfoMap m_Cameras;

	// PLC string Identifier - example : PLC_1
	CString	m_strPLCId;

private:
	void AssignCameraToAcquisitionTrigger();
	bool ResolveConditionalOutput();
	bool AlwaysWriteOutputs() const;
	bool EvaluateConditionalOutput(long dataIndex) const;
	SVString m_conditionalOutputName; // persist this
	SVGUID m_conditionalOutputValueID; // do not persist this

#ifdef EnableTracking
	struct SVPPQTrackingElement
	{
		typedef std::vector< long > SVCountVector;

		SVPPQTrackingElement();
		SVPPQTrackingElement( const SVPPQTrackingElement& p_rObject );

		virtual ~SVPPQTrackingElement();

		void clear();

		void IncrementCount( size_t p_Index, size_t p_VectorSize );

		SVCountVector m_TrackedCounts;

	};

	struct SVPPQTracking
	{
		typedef std::map< SVString, SVPPQTrackingElement > SVQueueTrackingMap;
		typedef std::map< SVString, long > SVPPQTrackingMap;

		SVPPQTracking();
		SVPPQTracking( const SVPPQTracking& p_rObject );

		virtual ~SVPPQTracking();

		void clear();

		void IncrementCount( const SVString& p_rName );
		void IncrementCount( const SVString& p_rName, size_t p_Index );
		void IncrementTimeCount( const SVString& p_rName, size_t p_Index );

		size_t m_QueueLength;
		size_t m_TimeLength;

		SVPPQTrackingMap m_Counts;
		SVQueueTrackingMap m_QueueCounts;
		SVQueueTrackingMap m_QueueWriteTimeCounts;
	};

	SVPPQTracking m_PPQTracking;
#endif // EnableTracking

	void SetRejectConditionList(const SVMonitorItemList& rRejectCondList);

	BasicValueObjects	m_PpqValues;

	SVPPQOutputModeEnum m_oOutputMode;
	long				m_lOutputDelay;
	long				m_lResetDelay;
	bool				m_bMaintainSourceImages;
	long                m_lInspectionTimeoutMillisec;
	BOOL				m_bCreated;
	BOOL				m_bOnline;
		
	UINT				m_uOutputTimer;

	long m_NAKCount;
};

typedef SVVector< SVPPQObject* > SVPPQObjectArray;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQObject.h_v  $
 * 
 *    Rev 1.18   17 Jul 2014 20:13:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   03 Jul 2014 16:34:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added comments for filter element map defines
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   19 Jun 2014 17:50:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised SetMonitorList method to also accept reject depth.
 * Added HasActiveMonitorList method.
 * Added SetRejectConditionList method.
 * Added SVFilterElementMap struct for reject condition list (Monitor List).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Jun 2014 10:18:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed char to TCHAR for constants.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   15 May 2014 11:56:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   28 Apr 2014 14:24:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   19 Mar 2014 23:17:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Fixed merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Mar 2014 15:32:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed "PPQ Length" to a Basic Value Object for access by Object Manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Mar 2014 09:12:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  891
 * SCR Title:  Remove tracking elements that hinder performance in release mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifdef EnableTracking around tracking code so it can be enabled for testing but not in the normal build.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Mar 2014 18:23:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Made methods const.
 *   Added init() and GetPPQLength() methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Feb 2014 12:00:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Jan 2014 12:50:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  881
 * SCR Title:  Fix Bug where Remote Inputs are missing after adding a new PPQ (e100)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated AddDefaultInputs to include RemoteInputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Oct 2013 11:00:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to remove deprecated code from the 64bit solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Oct 2013 07:12:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Aug 2013 13:34:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * Revised RebuildInputList method to correct an issue with camera inputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Jul 2013 13:28:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   03 Jul 2013 13:41:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AssignCameraToAcquisitionTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:31:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   29 Apr 2013 14:25:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:12:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   16 Apr 2013 15:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:33:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   21 Jan 2013 11:29:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetConditionalOutputName method
 * Added GetConditionalOutputName method
 * Added AddCameraDataInputs method
 * Added RemoveCameraDataInputa method
 * Added SetDefaultConditionalOutput method
 * Added HasCameraDataInputForConditionalOutput method
 * Added HasDigitalInputForConditionalOutput method
 * Added GetCameraDataInput method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   16 Jan 2013 16:43:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated Ouput fucntionality assoicated with the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   01 Oct 2012 10:46:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  793
 * SCR Title:  Fix PPQ Memory Consumption Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed Notify Camera Queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   04 Sep 2012 15:17:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48.1.0   01 Oct 2012 10:37:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  793
 * SCR Title:  Fix PPQ Memory Consumption Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed Notify Camera Queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   25 Jul 2012 14:44:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed processing bug with pended camera images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   02 Jul 2012 17:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   25 Jun 2012 16:55:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to monitor delay problems with shared memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   19 Jun 2012 13:49:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   16 Apr 2012 09:54:08   tBair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  762
 * SCR Title:  Fix Data Valid Signal Latency
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Reset Outputs and Write Outputs to insure the data valid signal is written just before the output toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   12 Oct 2011 16:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated PPQ to allow for the removal but not detachment of a camera from processing.  This will prevent all execution of inspections associated with that camera on that PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   26 Sep 2011 09:13:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Logging for Trigger and Output Toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   16 Sep 2011 16:18:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   14 Jul 2011 08:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   24 May 2011 15:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ Object to use the lockless Ring Buffer for trigger notifications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   19 Apr 2011 16:22:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   18 Apr 2011 10:59:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   11 Apr 2011 18:51:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with logical image to main image connection when going on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   08 Dec 2010 13:25:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   09 Nov 2010 11:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix invalid variable and type names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   05 Nov 2010 11:13:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   28 Oct 2010 14:08:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   26 Feb 2010 12:27:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udpated PPQ to fix lock-up problem with time delay data completion mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   25 Feb 2010 09:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to move the indexing of the PPQ to the PPQ thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   24 Feb 2010 10:46:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use separated object to handle new indexing funtionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   18 Feb 2010 14:28:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add includable logging for the output toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Feb 2010 10:28:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output toggle functionality to operate similar to the trigger toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Jan 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Dec 2009 12:29:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   06 Nov 2009 16:22:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated issues with archiving and image tracking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   22 Oct 2009 19:34:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   12 Oct 2009 16:00:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that was included in the 4.82 branch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   03 Sep 2009 10:38:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   20 Aug 2009 09:27:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated branch code into main line to fix issues with trigger toggles ad NAKs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   14 Aug 2009 14:48:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to merge branch functionality back into main line code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Jul 2009 12:23:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   14 Nov 2008 14:47:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PLC Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2007 14:41:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.5   12 Oct 2009 13:25:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problems with go-online and startup naks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.4   19 Aug 2009 14:21:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with NAKing startup product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.3   18 Aug 2009 09:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with trigger toggle and output delays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.2   14 Aug 2009 08:31:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to reset the outputs first when in time delay modes and to queue the notify inspection instead of notifying the inspection when indexing the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.1   13 Aug 2009 08:22:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved Camera queuing separate from the PPQ indexing, input reading, and output writing.  It will be an independent operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15.1.0   14 Jul 2009 15:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code touse the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   22 Jan 2007 09:32:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods SetInspectionTimeout and GetInspectionTimeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Jan 2006 09:54:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetInspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   04 Oct 2005 08:47:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed new and delete in ReadInputs.  Added member variable m_apInputsVect to handle the temperary inputentries in readInputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   22 Aug 2005 10:27:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   29 Oct 2003 15:14:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CanGoOnline and GoOnline methods to use HRESULTs as return codes to determine why the SVIM will not go on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   27 Aug 2003 10:28:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for error message describing why SVO can't go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 May 2003 15:09:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified code so that some inputs that were being exposed as outputs were removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Apr 2003 11:25:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Apr 2003 14:07:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Apr 2003 17:31:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made modifications to the DDE inputs/outputs. Some objects are now available again and fixed some bugs with existing code. State output and PPQ inputs are again available as DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Apr 2003 13:28:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a second parameter to DetachCamera.  The second parameter is defaulted to FALSE, if true it will go thru and remove all dependencies of the camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jan 2003 18:37:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to support image overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Dec 2002 15:21:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL GetProductInfoStruct(long lProcessCount, SVProductInfoStruct** ppStruct);
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 12:03:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  296
 * SCR Title:  Fix PPQ when multiple inspections and cameras are used
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added m_hCameraThread and m_oCamerasQueue to SVPPQObject class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 11:19:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DetachAll method to SVPPQObject class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:51:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/