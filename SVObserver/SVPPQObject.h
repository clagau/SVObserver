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
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVPPQShiftRegister.h"
#include "Definitions/SVPPQEnums.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVSharedMemoryLibrary/SMRingbuffer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#pragma endregion Includes


#pragma region Declarations
constexpr long	StandardPpqLength	= 2;
constexpr long g_lPPQExtraBufferSize = 50;
constexpr long g_maxPpqLength = 300;

namespace SvOi
{
class IObjectWriter;
}

namespace SvTrig
{
	class SVTriggerObject;
}
class SVInputObjectList;
class SVOutputObjectList;
struct MonitorListAttributeStruct;
#pragma endregion Declarations

constexpr long getMaxPpqLength()
{
	return std::min<long>(g_maxPpqLength, SvOi::ITriggerRecordControllerRW::cMaxTriggerRecords);
};

class SVPPQObject : 
	public SVObjectClass,
	public SVObserverTemplate<std::pair<long, SVInspectionInfoStruct>>
{
	enum PpqOutputEnums
	{
		TriggerToggle,
		OutputToggle,
		ACK,
		NAK,
		MasterFault,
		MasterWarning,
		NotInspected,
		DataValid,
		OutputNr
	};


public:

	explicit SVPPQObject( LPCSTR ObjectName );
	SVPPQObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPPQOBJECT );

	virtual ~SVPPQObject();

public:
	typedef std::pair< long, SVInspectionInfoStruct > SVInspectionInfoPair;
	typedef std::pair< long, SVProductInfoRequestStruct > SVProductRequestPair;
	typedef std::vector< SVProductInfoStruct* > SVProductPositionQueue;
	typedef std::deque< long > SVProcessCountDeque;
	typedef std::set< long > SVProcessCountSet;

	void SetNAKMode(SvDef::NakGeneration  NAKMode, int NAKPar);
	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	
	virtual HRESULT ObserverUpdate( const SVInspectionInfoPair& rData) override;

	bool Create();
	bool Rebuild();
	void Destroy();
	void DetachAll();

	void SetPPQOutputMode( SvDef::SVPPQOutputModeEnum ePPQOutputMode );
	void SetOutputDelay( long lDelayTime );
	void SetResetDelay( long lResetTime );
	void SetPPQLength( long lPPQLength );
	void SetMaintainSourceImages( bool bMaintainImages );
	void setMaxProcessingOffset4Interest(int maxProcessingOffset4Interest) { m_maxProcessingOffset4Interest = maxProcessingOffset4Interest; };
	void SetInspectionTimeout( long lTimeoutMillisec );
	void SetConditionalOutputName( const std::string& conditionName );

	SvDef::SVPPQOutputModeEnum getPPQOutputMode() const { return m_outputMode; }
	long getOutputDelay() const { return m_outputDelay; }
	long getResetDelay() const { return m_resetDelay; }
	long getPPQLength() const;
	bool getMaintainSourceImages() const { return m_maintainSourceImages; }
	int getMaxProcessingOffset4Interest() const { return m_maxProcessingOffset4Interest; }
	long getInspectionTimeout() const { return m_inspectionTimeoutMillisec; }
	const std::string& GetConditionalOutputName() const;

	bool AttachTrigger( SvTrig::SVTriggerObject* pTrigger );
	bool AttachCamera(SvIe::SVVirtualCamera* pCamera, long lPosition, bool p_AllowMinusOne = false);
	bool AttachInspection( SVInspectionProcess* pInspection );

	bool DetachTrigger( SvTrig::SVTriggerObject* pTrigger );
	bool DetachCamera(SvIe::SVVirtualCamera* pCamera, bool bRemoveDepends = false);
	bool DetachInspection( SVInspectionProcess* pInspection );

	bool AddSharedCamera(SvIe::SVVirtualCamera* pCamera);
	void GetInspectionCount( long &lSize ) const;

	size_t GetCameraCount() const;
	SvIe::SVVirtualCameraPtrVector GetVirtualCameras(bool sortAndMakeUnique = false) const;

	SvTrig::SVTriggerObject* GetTrigger() { return m_pTrigger; }
	bool GetInspection( long lIndex, SVInspectionProcess*& ppInspection ) const;

	//************************************
	// Method:    IsObjectInPPQ
	// Description:  Check if the object is part of this PPQ.
	// Parameter: const SVObjectClass & object The object which should be checked.
	// Returns:   bool True if object is part of the PPQ.
	//************************************
	bool IsObjectInPPQ( const SVObjectClass& object ) const;

	// PPQ position management functions
	bool SetCameraPPQPosition( long lPosition, SvIe::SVVirtualCamera* pCamera );
	bool GetCameraPPQPosition( long &lPosition, const SvIe::SVVirtualCamera* const pCamera ) const;

	
	/// Check and prepare if configuration can go online.
	/// In error cases this method throw Exception.
	void PrepareGoOnline();
	
	/// Go online.
	/// In error cases this method throw Exception.
	void GoOnline();
	bool GoOffline();
	bool IsOnline() const;
	void setOnline() { m_bOnline = true; }

	HRESULT GetProduct( SVProductInfoStruct& p_rProduct, long lProcessCount ) const;

	SVProductInfoStruct getProductReadyForRunOnce(uint32_t ipId  );

	void AddInput( SVIOEntryHostStructPtr pInput );
	bool RemoveInput( SVIOEntryHostStructPtr pInput );
	HRESULT GetInputIOValues(std::vector<_variant_t>& rInputValues) const;
	bool RebuildInputList();
	const SVIOEntryHostStructPtrVector& GetUsedInputs() const {return m_UsedInputs;}
	const SVIOEntryHostStructPtrVector& GetAllInputs() const {return m_AllInputs;}
	void AddDefaultInputs();
	bool AddToAvailableInputs(SVIOObjectType eType, const std::string& rName );
	SVIOEntryHostStructPtr GetInput( const std::string& rName ) const;

	void AddOutput( SVIOEntryHostStructPtr pOutput );
	bool RemoveOutput( SVIOEntryHostStructPtr pOutput );
	bool WriteOutputs(SVProductInfoStruct* pProduct);
	bool ResetOutputs();
	bool RebuildOutputList();
	void GetAllOutputs( SVIOEntryHostStructPtrVector& p_IOEntries ) const;
	void AddDefaultOutputs();

	void __stdcall cameraCallback(ULONG_PTR pCaller, CameraInfo&& cameraInfo);

	void __stdcall triggerCallback(SvTrig::SVTriggerInfoStruct&& triggerInfo);

	bool IsProductAlive( long p_ProductCount ) const;

	SVInputObjectList*    m_pInputList;
	SVOutputObjectList*   m_pOutputList;

	SvVol::BasicValueObjectPtr getPpqVaraible(LPCTSTR Name) { return m_PpqValues.getValueObject(Name); }
	void PersistInputs(SvOi::IObjectWriter& rWriter);

	/// Set or unset Monitor list and activated the shared memory for it.
	/// \param rActiveList [in] The new monitor list.
	/// In error cases (only possible in set case) this method throw Exception
	void SetMonitorList(const MonitorListAttributeStruct& rActiveList);
	bool HasActiveMonitorList() const;
	
	///Set the slotmanager 
	void SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager);
	
	///Get the SlotManager
	SvSml::RingBufferPointer GetSlotmanager();

	virtual DWORD GetObjectColor() const override;

	bool setRejectDepth(long depth, SvStl::MessageContainerVector *pErrorMessages =nullptr);

	void setPreTriggerTimeWindow(double preTriggerTimeWindow) { m_PPQPositions.setPreTriggerTimeWindow(preTriggerTimeWindow); }

	const SVIOEntryHostStructPtrVector& getUsedOutputs() const { return m_UsedOutputs; }
protected:

	struct SVCameraQueueElement
	{
		SVCameraQueueElement() = default;
		SVCameraQueueElement( const SVCameraQueueElement& rRhs) = default;
		SVCameraQueueElement(SvIe::SVVirtualCamera* pCamera, const CameraInfo& p_rData );
		SVCameraQueueElement& operator=(const SVCameraQueueElement&) = default;

		virtual ~SVCameraQueueElement() = default;

		SvIe::SVVirtualCamera* m_pCamera{ nullptr };
		CameraInfo m_Data {};
	};

	struct SVCameraInfoElement
	{
		SVCameraInfoElement() = default;
		SVCameraInfoElement( const SVCameraInfoElement& p_rObject ) = default;
		SVCameraInfoElement& operator=(const SVCameraInfoElement&) = default;

		virtual ~SVCameraInfoElement() = default;

		long m_CameraPPQIndex = -1;

		SVIOEntryHostStructPtr m_IOEntryPtr {};
		bool m_ToggleState = true;

	};

	bool m_bActiveMonitorList;
	long m_rejectCount = 0;
	SvSml::RingBufferPointer m_SlotManager;
	
	typedef SVTQueueObject< SVCameraQueueElement > SVCameraResponseQueue;
	typedef SVTQueueObject< SVProductInfoStruct* > SVProductPointerQueue;
	typedef SVRingBuffer<SvTrig::SVTriggerInfoStruct> SVTriggerInfoQueue;
	typedef SVTQueueObject< SVProductInfoStruct > SVProductQueue;
	typedef SVTQueueObject< SVInspectionInfoPair > SVInspectionInfoQueue;
	typedef SVTQueueObject< long > SVProcessCountQueue;
	typedef SVTQueueObject< SVProductRequestPair > SVProductRequestQueue;
	typedef std::map<SvIe::SVVirtualCamera*, SVCameraInfoElement> SVCameraInfoMap;
	typedef std::map<SvIe::SVVirtualCamera*, SVCameraQueueElement> SVPendingCameraResponseMap;

	static void CALLBACK OutputTimerCallback( UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2 );
	static void CALLBACK APCThreadProcess(ULONG_PTR pParam);

	HRESULT MarkProductInspectionsMissingAcquisiton( SVProductInfoStruct& rProduct, SvIe::SVVirtualCamera* pCamera );

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
	/// \return true if an item was removed from m_oCamerasQueue, otherwise false
	//************************************
	bool processInspections( );

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

	HRESULT ProcessOutputs(SVProductInfoStruct& rProduct);
	HRESULT ProcessTimeDelayAndDataCompleteOutputs(SVProductInfoStruct& rProduct, bool& rProcessed);

	SVProductInfoStruct* GetProductInfoStruct(long processCount) const;

	SVProductInfoStruct* IndexPPQ(SvTrig::SVTriggerInfoStruct&& rTriggerInfo);
	void InitializeProduct( SVProductInfoStruct* pNewProduct);
	void StartOutputs( SVProductInfoStruct* p_pProduct );
	HRESULT NotifyInspections( long p_Offset );
	HRESULT StartInspection(uint32_t inspectionID );

	void AddResultsToPPQ(SVProductInfoStruct& rProduct);
	bool SetInspectionComplete(SVProductInfoStruct& rProduct, uint32_t inspId);

	bool SetProductComplete(long p_PPQIndex);
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

	HRESULT BuildCameraInfos(SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos) const;

	mutable SVAsyncProcedure m_AsyncProcedure;

	std::atomic<double> m_NextOutputDelayTimestamp;

	std::atomic<double> m_NextOutputResetTimestamp;

	std::atomic<double> m_NextDataValidDelayTimestamp;

	// Queues for the PPQ's threads to store incoming objects to be processed
	SVTriggerInfoQueue m_oTriggerQueue; ///< A ring buffer containing SVTriggerQueueElement s, i.e. SvTrig::SVTriggerInfoStruct s and SVVariantBoolVector s
	SVInspectionInfoQueue m_oInspectionQueue;
	SVProcessCountQueue m_oOutputsDelayQueue;
	SVProcessCountQueue m_oOutputsResetQueue;
	SVProcessCountQueue m_DataValidDelayQueue;
	mutable SVProductRequestQueue m_ProductRequests;

	SVCameraResponseQueue m_CameraResponseQueue;
	SVPendingCameraResponseMap m_PendingCameraResponses;

	SVProcessCountSet m_oNotifyInspectionsSet;
	std::set<uint32_t> m_ProcessInspectionsSet;

	// Pointers to the PPQ's I/O Lists
	SVIOEntryHostStructPtrVector m_AllInputs;
	SVIOEntryHostStructPtrVector m_AllOutputs;
	SVIOEntryHostStructPtrVector m_UsedInputs;
	SVIOEntryHostStructPtrVector m_UsedOutputs;

	SVIOEntryHostStructPtr m_pTriggerToggle;	
	SVIOEntryHostStructPtr m_pOutputToggle;
	SVIOEntryHostStructPtr m_pDataValid;

	// Pointers to the Subsystem objects used by the PPQ
	SvTrig::SVTriggerObject*       m_pTrigger;
	SVInspectionProcessVector    m_arInspections;

	// Pointer to the PPQ's buckets
	SVPPQShiftRegister m_PPQPositions;

	SVProductInfoStruct*  m_pMasterProductInfos;
	SVProductPointerQueue m_qAvailableProductInfos;

	// Value Objects used by the PPQ
	SvVol::SVBoolValueObjectClass  m_PpqOutputs[PpqOutputEnums::OutputNr];

	bool m_TriggerToggle;
	bool m_OutputToggle;

	// Pointers to the Master Lists
	SVCameraInfoMap m_Cameras;

private:
	std::string m_conditionalOutputName; // persist this
	uint32_t m_conditionalOutputValueID; // do not persist this

	bool ResolveConditionalOutput();
	bool AlwaysWriteOutputs() const;
	bool EvaluateConditionalOutput() const;
	void init();

#ifdef EnableTracking
	struct SVPPQTrackingElement
	{
		typedef std::vector< long > SVCountVector;

		SVPPQTrackingElement() = default;
		SVPPQTrackingElement( const SVPPQTrackingElement& p_rObject ) = default;
		SVPPQTrackingElement& operator=(const SVPPQTrackingElement&) = default;

		virtual ~SVPPQTrackingElement();

		void clear();

		void IncrementCount( size_t p_Index, size_t p_VectorSize );

		SVCountVector m_TrackedCounts {};
	};

	struct SVPPQTracking
	{
		typedef std::map<std::string, SVPPQTrackingElement> SVQueueTrackingMap;
		typedef std::map<std::string, long> SVPPQTrackingMap;

		SVPPQTracking() = default;
		SVPPQTracking( const SVPPQTracking& p_rObject ) = default;
		SVPPQTracking& operator=(const SVPPQTracking&) = default;

		virtual ~SVPPQTracking();

		void clear();

		void IncrementCount( const std::string& p_rName );
		void IncrementCount( const std::string& p_rName, size_t p_Index );

		size_t m_QueueLength = 0;
		size_t m_TimeLength = 0;

		SVPPQTrackingMap m_Counts {};
		SVQueueTrackingMap m_QueueCounts {};
		SVQueueTrackingMap m_QueueWriteTimeCounts {};
	};

	SVPPQTracking m_PPQTracking;
#endif // EnableTracking

	void ResetOutputValueObjects();
	void ReleaseSharedMemory(SVProductInfoStruct& rProduct);
	void CommitSharedMemory( SVProductInfoStruct& rProduct);

	bool SetupProductInfoStructs();

	bool setInspections2TRC();

	void setTRofInterest(const SVProductInfoStruct& rProduct, bool isInterest);
	void setTR2StoreForInterestMap(uint32_t inspId, SVProductInfoStruct &rProduct);
	void calcUseProcessingOffset4InterestFlag();
	long getNeededRecords() const;

	SvVol::BasicValueObjects	m_PpqValues;
	SvDef::SVPPQOutputModeEnum m_outputMode;
	long m_outputDelay;
	long m_resetDelay;
	long m_DataValidDelay;
	bool m_maintainSourceImages;
	long m_inspectionTimeoutMillisec;
	bool m_bOnline;

	UINT m_uOutputTimer;

	SvDef::NakGeneration  m_NAKMode;			//!Different Mode for NAK Behavior Legacy, Bursts,RepairedLegacy,FixedMaximum
	int m_NAKParameter;						//!Additional Parameter for NAK Behavior 	
	long m_NAKCount;
	long m_FirstNAKProcessCount;		///only trigger >= m_FirstNAKProcessCount will be inspected 
	long m_NewNAKCount;					//!Nak count will be set to 0 if no NAK occurs 
	long m_ReducedPPQPosition;			/// min number of inspection that will be checked for startInspection  for nakMode =2
	std::atomic_long m_lastPPQPosition {0L};		/// This is the PPQ position of the last SetInspectionComplete call
	int m_maxProcessingOffset4Interest {0};  
	bool m_useProcessingOffset4Interest {false};	///Flag if processing offset will used. It should only used if m_maxProcessingOffset4Interest >2 and <PPQLength, RejectCondition set and at least two Inspections in this PPQ.

	using IpInfoDeque = std::deque<SVInspectionInfoStruct>;
	std::unordered_map<uint32_t, IpInfoDeque> m_storeForInterestMap;

	SVObjectPtrVector m_childObjects;
};

typedef std::vector<SVPPQObject*> SVPPQObjectPtrVector;

