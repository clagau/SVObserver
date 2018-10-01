//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQObject
//* .File Name       : $Workfile:   SVPPQObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.20  $
//* .Check In Date   : $Date:   20 Nov 2014 05:03:12  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVSharedMemoryLibrary/SMRingbuffer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVInfoStructs.h"
#include "Definitions/SVPPQEnums.h"
#include "SVPPQShiftRegister.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVVirtualCamera.h"
#include "TriggerInformation/SVCameraTriggerData.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "SVMonitorList.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes


#pragma region Declarations
const long	StandardPpqLength	= 2;

#pragma endregion Declarations

class SVPPQObject : 
	public SVObjectClass,
	public SVObserverTemplate< SVInspectionCompleteInfoStruct >
{
// Setup Functions

public:
	typedef std::vector< std::pair< _variant_t, bool > > SVVariantBoolVector;

	SVPPQObject( LPCSTR ObjectName );
	SVPPQObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPPQOBJECT );

	virtual ~SVPPQObject();

public:
	void SetNAKMode(SvDef::NakGeneration  NAKMode, int NAKPar);
	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	
	virtual HRESULT ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData ) override;

	bool Create();
	bool Rebuild();
	void Destroy();
	void DetachAll();

	void SetPPQOutputMode( SvDef::SVPPQOutputModeEnum ePPQOutputMode );
	void SetOutputDelay( long lDelayTime );
	void SetResetDelay( long lResetTime );
	void SetPPQLength( long lPPQLength );
	void SetMaintainSourceImages( bool bMaintainImages );
	void SetInspectionTimeout( long lTimeoutMillisec );
	void SetConditionalOutputName( const std::string& conditionName );

	void GetPPQOutputMode( SvDef::SVPPQOutputModeEnum& rePPQOutputMode ) const;
	void GetOutputDelay( long& rlDelayTime ) const;
	void GetResetDelay( long& rlResetTime ) const;
	void GetPPQLength( long& rlPPQLength ) const;
	long GetPPQLength() const;
	void GetMaintainSourceImages( bool& rbMaintainImages ) const;
	void GetInspectionTimeout( long& rlTimeoutMillisec ) const;
	const std::string& GetConditionalOutputName() const;

	bool AttachTrigger( SvTi::SVTriggerObject* pTrigger );
	bool AttachCamera( SVVirtualCamera* pCamera, long lPosition, bool p_AllowMinusOne = false );
	bool AttachInspection( SVInspectionProcess* pInspection );

	bool DetachTrigger( SvTi::SVTriggerObject* pTrigger );
	bool DetachCamera( SVVirtualCamera* pCamera, bool bRemoveDepends = false );
	bool DetachInspection( SVInspectionProcess* pInspection );

	bool AddSharedCamera(  SVVirtualCamera* pCamera );
	void GetInspectionCount( long &lSize ) const;

	size_t GetCameraCount() const;
	void GetCameraList( std::deque< SVVirtualCamera* >& p_rCameras ) const;
	HRESULT GetVirtualCameras( SVVirtualCameraMap& p_rCameras ) const;

	void GetTrigger( SvTi::SVTriggerObject*& ppTrigger );
	bool GetInspection( long lIndex, SVInspectionProcess*& ppInspection ) const;

	//************************************
	// Method:    IsObjectInPPQ
	// Description:  Check if the object is part of this PPQ.
	// Parameter: const SVObjectClass & object The object which should be checked.
	// Returns:   bool True if object is part of the PPQ.
	//************************************
	bool IsObjectInPPQ( const SVObjectClass& object ) const;

	// PPQ position management functions
	bool SetCameraPPQPosition( long lPosition, SVVirtualCamera* pCamera );
	bool GetCameraPPQPosition( long &lPosition, SVVirtualCamera* pCamera ) const;

	
	/// Check and prepare if configuration can go online.
	/// In error cases this method throw Exception.
	void PrepareGoOnline();
	
	/// Go online.
	/// In error cases this method throw Exception.
	void GoOnline();
	bool GoOffline();
	bool IsOnline() const;

	bool IsProductExpired( const SVProductInfoStruct* pProduct ) const;
	HRESULT GetProduct( SVProductInfoStruct& p_rProduct, long lProcessCount ) const;

	bool ReserveNextRunOnceProductInfoStruct( SVProductInfoStruct& p_rsvProduct, SVDataManagerLockTypeEnum p_LockType = SV_PPQ );

	void AddInput( SVIOEntryHostStructPtr pInput );
	bool RemoveInput( SVIOEntryHostStructPtr pInput );
	HRESULT GetInputIOValues( SVVariantBoolVector& rInputValues ) const;
	bool AssignInputs( const SVVariantBoolVector& rInputValues );
	bool RebuildInputList(bool bHasCameraTrigger);
	void GetAvailableInputs( SVIOEntryHostStructPtrVector& p_IOEntries ) const;
	void GetAllInputs( SVIOEntryHostStructPtrVector& p_IOEntries ) const;
	void AddDefaultInputs();
	bool AddToAvailableInputs(SVIOObjectType eType, const std::string& rName );
	SVIOEntryHostStructPtr GetInput( const std::string& rName ) const;

	void AddCameraDataInputs(SVIOEntryHostStructPtrVector& list);
	void RemoveCameraDataInputs(SVIOEntryHostStructPtrVector& list);
	void SetDefaultConditionalOutput();
	bool HasCameraDataInputForConditionalOutput() const;
	bool HasDigitalInputForConditionalOutput() const;

	void AddOutput( SVIOEntryHostStructPtr pOutput );
	bool RemoveOutput( SVIOEntryHostStructPtr pOutput );
	bool WriteOutputs( SVProductInfoStruct* pProduct );
	bool ResetOutputs();
	bool RebuildOutputList();
	void GetAllOutputs( SVIOEntryHostStructPtrVector& p_IOEntries ) const;
	void AddDefaultOutputs();

	//************************************
	/// Writes DataResponse and Camera into the camera response queue

	/// \param pCaller [unused]
	/// \param p_rTriggerInfo [in]
	//************************************
	bool FinishCamera( void *pCaller, SVODataResponseClass* pResponse );

	//************************************
	/// Writes trigger information into SVTriggerInfoQueue m_oTriggerQueue;
	/// Calls ResetOutputs() if in SVPPQTimeDelayMode or SVPPQTimeDelayAndDataCompleteMode

	/// \param pCaller [unused]
	/// \param p_rTriggerInfo [in] the trigger information to be queued
	//************************************
	bool FinishTrigger( void *pCaller, SvTi::SVTriggerInfoStruct& p_rTriggerInfo );

	void DumpDMInfo( LPCTSTR p_szName ) const;

	bool IsProductAlive( long p_ProductCount ) const;

	HRESULT GetNextAvailableIndexes( SVPPQInfoStruct& p_rPPQInfo, SVDataManagerLockTypeEnum p_LockType ) const;

	SVInputObjectList*    m_pInputList;
	SVOutputObjectList*   m_pOutputList;

	SVDWordValueObjectClass m_voOutputState;
	SVLongValueObjectClass m_voTriggerCount;

	void PersistInputs(SvOi::IObjectWriter& rWriter);

	SvTi::SVCameraTriggerData& GetCameraInputData();
	long GetExtraBufferSize() const;

	/// Set or unset Monitor list and activated the shared memory for it.
	/// \param rActiveList [in] The new monitor list.
	/// In error cases (only possible in set case) this method throw Exception
	void SetMonitorList(const ActiveMonitorList& rActiveList);
	bool HasActiveMonitorList() const;
	
	///Set the slotmanager 
	void SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager);
	
	///Get the SlotManager
	SvSml::RingBufferPointer GetSlotmanager();

	//************************************
	/// Insert objects of the children which fit to the attribute filter.
	/// \param objectList [in, out] The list where the object will be added.
	/// \param AttributesAllowedFilter [in] The filter for the attributes.
	//************************************
	void fillChildObjectList(SVObjectPtrDeque& objectList, UINT AttributesAllowedFilter = 0) const;


	virtual DWORD GetObjectColor() const override;

	/// \returns workload information for the most recently completed product 
	ProductWorkloadInformation GetMostRecentWorkLoadInformation(){return m_MostRecentWorkLoadInfo;}

protected:
	struct SVTriggerQueueElement
	{
		SVTriggerQueueElement();
		SVTriggerQueueElement( const SVTriggerQueueElement& p_rObject );

		virtual ~SVTriggerQueueElement();

		SvTi::SVTriggerInfoStruct m_TriggerInfo;
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

	typedef std::map<std::string, SVObjectReference> SVNameObjectMap;

	typedef std::map<SVGUID, SVNameObjectMap> SVInspectionFilterValueMap; // Inspection Guid to FilterValueMap mapping
	bool m_bActiveMonitorList;
	SvSml::RingBufferPointer m_SlotManager;
	


	typedef std::pair< long, SVInspectionInfoStruct > SVInspectionInfoPair;
	typedef std::pair< long, SVProductInfoRequestStruct > SVProductRequestPair;
	typedef std::vector< SVProductInfoStruct* > SVProductPositionQueue;
	typedef std::deque< long > SVProcessCountDeque;
	typedef std::set< long > SVProcessCountSet;
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

	//************************************
	/// Processes the first element (if available) in the trigger queue and creates a product from it
	/// \param rProcessed [out] false if the trigger queue empty, otherwise true.
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessTrigger( bool& rProcessed );

	//************************************
	/// Extracts the first valid entry in in m_oNotifyInspectionsSet and calls NotifyInspections()
	/// \param rProcessed [out] true if one inspection was successfully notified, otherwise false.
	/// \returns S_OK (or E_FAIL if NotifyInspections failed)
	//************************************
	HRESULT ProcessNotifyInspections( bool& rProcessed );

	//************************************
	/// Empties m_ProcessInspectionsSet and starts all Inspections that were contained in it
	/// \param rProcessed [out] true if an item was removed from m_oCamerasQueue, otherwise false
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessInspections( bool& rProcessed );

	//************************************
	/// Controls the output of completed results that require a delay before they are output.
	/// \param rProcessed [out] true if a SVProductInfoStruct was processed
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessDelayOutputs( bool& rProcessed );

	//************************************
	/// If at least two entries in m_oOutputsResetQueue: removes the head and calls ResetOutputs()
	/// \param rProcessed [out] true if ResetOutputs() was called
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessResetOutputs( bool& rProcessed );

	//************************************
	/// If data valid delay is set then processes when the signals are to be set
	/// \param rProcessed [out] true if Data Valid Delay processed
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessDataValidDelay( bool& rProcessed );

	//************************************
	/// If possible, processes one camera response (either from the pending camera responses or directly from the trigger queue)
	/// \param rProcessed [out] true if a camera response was processed successfully
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessCameraResponses( bool& rProcessed );
	//************************************
	/// If all inspections for a product are done, sets the product to complete
	/// \param rProcessed [out] true m_oTriggerQueue is empty or if a product has been set to complete
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessCompleteInspections( bool& rProcessed );
	//************************************
	/// Possibly used when results are requested via remote
	/// \param rProcessed [out] true if false otherwise
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessProductRequests( bool& rProcessed );

	HRESULT ProcessTimeDelayOutputs( SVProductInfoStruct& p_rProduct );
	HRESULT ProcessTimeDelayAndDataCompleteOutputs( SVProductInfoStruct& p_rProduct, bool& rProcessed );

	SVProductInfoStruct* GetProductInfoStruct(long processCount) const;

	SVProductInfoStruct* IndexPPQ( SvTi::SVTriggerInfoStruct& p_rTriggerInfo );
	void InitializeProduct( SVProductInfoStruct* p_pNewProduct, const SVVariantBoolVector& p_rInputValues );
	bool StartOutputs( SVProductInfoStruct* p_pProduct );
	HRESULT NotifyInspections( long p_Offset );
	HRESULT StartInspection( const SVGUID& p_rInspectionID );

	bool AddResultsToPPQ( long p_PPQIndex );
	bool SetInspectionComplete( long p_PPQIndex );
	bool SetProductComplete( long p_PPQIndex );
	bool SetProductComplete( SVProductInfoStruct& p_rProduct );

	bool SetProductIncomplete( long p_PPQIndex );
	bool SetProductIncomplete( SVProductInfoStruct& p_rProduct );

	bool RecycleProductInfo( SVProductInfoStruct *pProduct );

	void RebuildProductCameraInfoStructs();

	//************************************
	/// Processes a single camera queue event
	/// \param p_rElement [in] is the camera queue element to be looked at
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessCameraResponse( const SVCameraQueueElement& p_rElement );

	HRESULT BuildCameraInfos( SVGuidSVCameraInfoStructMap& p_rCameraInfos ) const;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;

	long m_ProcessingOutputDelay;
	SvTl::SVTimeStamp m_NextOutputDelayTimestamp;

	long m_ProcessingOutputReset;
	SvTl::SVTimeStamp m_NextOutputResetTimestamp;

	long m_ProcessingDataValidDelay;
	SvTl::SVTimeStamp m_NextDataValidDelayTimestamp;

	// Queues for the PPQ's threads to store incoming objects to be processed
	SVTriggerInfoQueue m_oTriggerQueue; ///< A ring buffer containing SVTriggerQueueElement s, i.e. SvTi::SVTriggerInfoStruct s and SVVariantBoolVector s
	SVInspectionInfoQueue m_oInspectionQueue;
	SVProcessCountQueue m_oOutputsDelayQueue;
	SVProcessCountQueue m_oOutputsResetQueue;
	SVProcessCountQueue m_DataValidDelayQueue;
	mutable SVProductRequestQueue m_ProductRequests;

	SVCameraResponseQueue m_CameraResponseQueue;
	SVPendingCameraResponseMap m_PendingCameraResponses;

	SVProcessCountSet m_oNotifyInspectionsSet;
	SVGuidSet m_ProcessInspectionsSet;

	// Pointers to the PPQ's I/O Lists
	SVIOEntryHostStructPtrVector m_AllInputs;
	SVIOEntryHostStructPtrVector m_AllOutputs;
	SVIOEntryHostStructPtrVector m_UsedInputs;
	SVIOEntryHostStructPtrVector m_UsedOutputs;

	SVIOEntryHostStructPtr m_pTriggerToggle;	
	SVIOEntryHostStructPtr m_pOutputToggle;
	SVIOEntryHostStructPtr m_pDataValid;

	// Pointers to the Subsystem objects used by the PPQ
	SvTi::SVTriggerObject*            m_pTrigger;
	SVInspectionProcessVector    m_arInspections;

	// Pointer to the PPQ's buckets
	SVPPQShiftRegister m_ppPPQPositions;

	SVProductInfoStruct*  m_pMasterProductInfos;
	SVProductPointerQueue m_qAvailableProductInfos;

	// DataManager index handles
	SVSmartIndexArrayHandlePtr m_pResultDataCircleBuffer;

	// Value Objects used by the PPQ
	SVBoolValueObjectClass  m_voTriggerToggle;	
	SVBoolValueObjectClass  m_voOutputToggle;
	SVBoolValueObjectClass  m_voACK;
	SVBoolValueObjectClass  m_voNAK;
	SVBoolValueObjectClass  m_voMasterFault;
	SVBoolValueObjectClass  m_voMasterWarning;
	SVBoolValueObjectClass  m_voNotInspected;
	SVBoolValueObjectClass  m_voDataValid;

	SvTi::SVCameraTriggerData m_CameraInputData;

	bool m_TriggerToggle;
	bool m_OutputToggle;

	// Pointers to the Master Lists
	SVCameraInfoMap m_Cameras;

private:
	std::string m_conditionalOutputName; // persist this
	SVGUID m_conditionalOutputValueID; // do not persist this

	ProductWorkloadInformation m_MostRecentWorkLoadInfo; ///< workload information for the most recently completed product

	void AssignCameraToAcquisitionTrigger();
	bool ResolveConditionalOutput();
	bool AlwaysWriteOutputs() const;
	bool EvaluateConditionalOutput(long DataIndex) const;
	void init();

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
		typedef std::map<std::string, SVPPQTrackingElement> SVQueueTrackingMap;
		typedef std::map<std::string, long> SVPPQTrackingMap;

		SVPPQTracking();
		SVPPQTracking( const SVPPQTracking& p_rObject );

		virtual ~SVPPQTracking();

		void clear();

		void IncrementCount( const std::string& p_rName );
		void IncrementCount( const std::string& p_rName, size_t p_Index );
		void IncrementTimeCount( const std::string& p_rName, size_t p_Index );

		size_t m_QueueLength;
		size_t m_TimeLength;

		SVPPQTrackingMap m_Counts;
		SVQueueTrackingMap m_QueueCounts;
		SVQueueTrackingMap m_QueueWriteTimeCounts;
	};

	SVPPQTracking m_PPQTracking;
#endif // EnableTracking

	void ResetOutputValueObjects(long DataIndex);
	void ReleaseSharedMemory(SVProductInfoStruct& rProduct);
	void CommitSharedMemory( SVProductInfoStruct& rProduct);

	void SetupProductInfoStructs();

	BasicValueObjects	m_PpqValues;
	SvDef::SVPPQOutputModeEnum m_oOutputMode;
	long m_lOutputDelay;
	long m_lResetDelay;
	long m_DataValidDelay;
	bool m_bMaintainSourceImages;
	long m_lInspectionTimeoutMillisec;
	bool m_bOnline;

	UINT m_uOutputTimer;

	SvDef::NakGeneration  m_NAKMode;			//!Different Mode for NAK Behavior Legacy, Bursts,RepairedLegacy,FixedMaximum
	int m_NAKParameter;						//!Additional Parameter for NAK Behavior 	
	long m_NAKCount;
	long m_FirstNAKProcessCount;		///only trigger >= m_FirstNAKProcessCount will be inspected 
	long m_NewNAKCount;					//!Nak count will be set to 0 if no NAK occurs 
	long m_ReducedPPQPosition;			/// min number of inspection that will be checked for startInspection  for nakMode =2 

	SVObjectPtrDeque m_childObjects;

	//This parameter a temporary parameter to speed up the method fillChildObjectList. 
	//They are not a state of the object only a help for the method. To be able to set the method const, this parameter are muteable.
	mutable SVObjectPtrDeque m_childObjectsForFillChildObjectList;  //<This list is saved for the method fillChildObjectList, to reuse it if the same filter (m_AttributesAllowedFilterForFillChildObjectList) is used. 
	mutable UINT m_AttributesAllowedFilterForFillChildObjectList; //<This filter flag was used in the last run of the method fillChildObjectList.
};

typedef std::vector<SVPPQObject*> SVPPQObjectPtrVector;

