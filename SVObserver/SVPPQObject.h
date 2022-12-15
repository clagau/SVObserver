//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVPPQShiftRegister.h"
#include "Definitions/SVPPQEnums.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVOLibrary/SVQueueObject.h"
#include "Triggering/TriggerDevice.h"
#include "SVSystemLibrary/SVThread.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#pragma endregion Includes


#pragma region Declarations
constexpr long cStandardPpqLength = 2;
constexpr long cStandardOutputDelay = 100;
constexpr long cMinReducedPpqPos = 2;
constexpr long cTriggerQueueSize = 10;
constexpr long cPPQExtraBufferSize = 50;
constexpr long cMaxPpqLength = 300;

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
	return std::min<long>(cMaxPpqLength, SvOi::ITriggerRecordControllerRW::cMaxTriggerRecords);
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

	using PpqProcessFunction = std::function<void()>;

	enum PpqFunction
	{
		Trigger,
		DataValidDelay,
		ResetOutputDelay,
		DelayOutputs,
		CameraResponses,
		NotifyInspection,
		CompleteInspections,
		ProductRequests,
		Size
	};

public:

	explicit SVPPQObject( LPCSTR ObjectName );
	SVPPQObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPPQOBJECT );

	virtual ~SVPPQObject();

public:
	typedef std::pair< long, SVInspectionInfoStruct > SVInspectionInfoPair;
	typedef std::pair< long, SVProductInfoRequestStruct > SVProductRequestPair;
	typedef std::vector< SVProductInfoStruct* > SVProductPositionQueue;

	static void SetTimerResolution(bool timerResolution) { m_timerResolution = timerResolution; }
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
	long GetInspectionCount() const;

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
	void PrepareGoOnline(bool setSoftwareTrigger);
	
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
	SVIOEntryHostStructPtr GetInput( const std::string& rName ) const;

	bool RemoveOutput( SVIOEntryHostStructPtr pOutput );
	bool WriteOutputs(SVProductInfoStruct* pProduct);
	bool ResetOutputs();
	bool RebuildOutputList();
	void AddPpqResults();
	long getOutputCount();

	void __stdcall cameraCallback(ULONG_PTR pCaller, const CameraInfo& rCameraInfo);

	void __stdcall triggerCallback(SvTrig::SVTriggerInfoStruct&& triggerInfo);

	bool IsProductAlive( long p_ProductCount ) const;

	SVInputObjectList*    m_pInputList {nullptr};
	SVOutputObjectList*   m_pOutputList {nullptr};

	SvVol::BasicValueObjectPtr getPpqVaraible(LPCTSTR Name) { return m_PpqValues.getValueObject(Name); }
	void PersistInputs(SvOi::IObjectWriter& rWriter);

	/// Set or unset Monitor list and activated the shared memory for it.
	/// \param rActiveList [in] The new monitor list.
	/// In error cases (only possible in set case) this method throw Exception
	void SetMonitorList(const MonitorListAttributeStruct& rActiveList);
	bool HasActiveMonitorList() const;
	
	virtual DWORD GetObjectColor() const override;

	bool setRejectDepth(long depth, SvStl::MessageContainerVector *pErrorMessages =nullptr);

	void setTriggerTimeWindow(double preTriggerTimeWindow, double postTriggerTimeWindow) { m_PPQPositions.setTriggerTimeWindow(preTriggerTimeWindow, postTriggerTimeWindow); }

	const SVIOEntryHostStructPtrVector& getUsedOutputs() const { return m_UsedOutputs; }

	void NotifyProcessTimerOutputs(double timestamp);

	bool requiresTimer() const;
	HRESULT setModuleReady(bool set);

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

	bool m_bActiveMonitorList {false};
	long m_rejectCount = 0;
	
	typedef SVTQueueObject< SVCameraQueueElement > SVCameraResponseQueue;
	typedef SVTQueueObject< SVProductInfoStruct* > SVProductPointerQueue;
	typedef SVRingBuffer<SvTrig::SVTriggerInfoStruct> SVTriggerInfoQueue;
	typedef SVTQueueObject< SVProductInfoStruct > SVProductQueue;
	typedef SVTQueueObject< SVInspectionInfoPair > SVInspectionInfoQueue;
	typedef SVTQueueObject< long > SVProcessCountQueue;
	typedef SVTQueueObject< SVProductRequestPair > SVProductRequestQueue;
	typedef std::map<SvIe::SVVirtualCamera*, SVCameraInfoElement> SVCameraInfoMap;
	typedef std::map<SvIe::SVVirtualCamera*, SVCameraQueueElement> SVPendingCameraResponseMap;

	static void __stdcall ProcessCallback(ULONG_PTR pCaller);

	HRESULT MarkProductInspectionsMissingAcquisiton( SVProductInfoStruct& rProduct, SvIe::SVVirtualCamera* pCamera );

	void ProcessTrigger();
	void ProcessNotifyInspections();
	void ProcessDelayOutputs();
	void ProcessResetOutputs();
	void ProcessDataValidDelay();
	void ProcessCameraResponses();
	void ProcessCompleteInspections();
	void ProcessProductRequests();

	void ProcessOutputs(SVProductInfoStruct& rProduct);
	void ProcessTimeDelayAndDataCompleteOutputs(SVProductInfoStruct& rProduct);

	SVProductInfoStruct* GetProductInfoStruct(long processCount) const;

	SVProductInfoStruct* IndexPPQ(SvTrig::SVTriggerInfoStruct&& rTriggerInfo);
	void InitializeProduct( SVProductInfoStruct* pNewProduct);
	void StartOutputs( SVProductInfoStruct* p_pProduct );
	bool StartInspection(uint32_t inspectionID );

	void AddResultsToPPQ(SVProductInfoStruct& rProduct);
	bool SetInspectionComplete(SVProductInfoStruct& rProduct, uint32_t inspId);

	bool SetProductComplete( SVProductInfoStruct& rProduct );

	bool SetProductIncomplete( long p_PPQIndex );
	bool SetProductIncomplete( SVProductInfoStruct& rProduct );

	bool RecycleProductInfo( SVProductInfoStruct *pProduct );
	
	void RebuildProductCameraInfoStructs();


	//************************************
	/// Processes a single camera queue event
	/// \param p_rElement [in] is the camera queue element to be looked at
	/// \returns S_OK on success, otherwise E_FAIL
	//************************************
	HRESULT ProcessCameraResponse( const SVCameraQueueElement& p_rElement );

	HRESULT BuildCameraInfos(SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos) const;

	mutable SvSyl::SVThread m_processThread {};

	std::atomic<double> m_NextOutputDelayTimestamp {0.0};
	std::atomic<double> m_NextOutputResetTimestamp {0.0};
	std::atomic<double> m_NextDataValidDelayTimestamp {0.0};

	// Queues for the PPQ's threads to store incoming objects to be processed
	///< A ring buffer containing SVTriggerQueueElement s, i.e. SvTrig::SVTriggerInfoStruct s and SVVariantBoolVector s
	SVTriggerInfoQueue m_oTriggerQueue {cTriggerQueueSize};
	SVInspectionInfoQueue m_oInspectionQueue;
	SVProcessCountQueue m_oOutputsDelayQueue;
	SVProcessCountQueue m_oOutputsResetQueue;
	SVProcessCountQueue m_DataValidDelayQueue;
	mutable SVProductRequestQueue m_ProductRequests;

	SVCameraResponseQueue m_CameraResponseQueue;
	SVPendingCameraResponseMap m_PendingCameraResponses;

	std::vector<long> m_notifyInspectionList;

	// Pointers to the PPQ's I/O Lists
	SVIOEntryHostStructPtrVector m_UsedInputs;
	SVIOEntryHostStructPtrVector m_UsedOutputs;

	SVIOEntryHostStructPtr m_pTriggerToggle;	
	SVIOEntryHostStructPtr m_pOutputToggle;
	SVIOEntryHostStructPtr m_pDataValid;

	// Pointers to the Subsystem objects used by the PPQ
	SvTrig::SVTriggerObject* m_pTrigger {nullptr};
	SVInspectionProcessVector    m_arInspections;

	// Pointer to the PPQ's buckets
	SVPPQShiftRegister m_PPQPositions;

	SVProductInfoStruct*  m_pMasterProductInfos {nullptr};
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

	void ResetOutputValueObjects();

	bool SetupProductInfoStructs();

	bool setInspections2TRC();

	void setTRofInterest(const SVProductInfoStruct& rProduct, bool isInterest);
	void setTR2StoreForInterestMap(uint32_t inspId, SVProductInfoStruct &rProduct);
	void calcUseProcessingOffset4InterestFlag();
	long getNeededRecords() const;

	void checkNakReason(CantProcessEnum cantProcessReason);

	void setPreviousNAK(const SVProductInfoStruct& rCurrentProduct, SVProductInfoStruct* pNextProduct) const;
	void checkTriggerIndex(const SVProductInfoStruct& rCurrentProduct, SVProductInfoStruct* pNextProduct) const;
	void setOutputResults(uint32_t inspectedID, std::vector<bool>& rOutputResult) const;

	void AttachAcqToTriggers(bool setSoftwareTrigger);

	PpqCameraCallBack m_cameraCallback;
	SvTrig::PpqTriggerCallBack m_triggerCallback;
	SvVol::BasicValueObjects	m_PpqValues;
	SvDef::SVPPQOutputModeEnum m_outputMode {SvDef::SVPPQNextTriggerMode};
	std::array <PpqProcessFunction, PpqFunction::Size> m_processFunctions;
	long m_outputDelay {cStandardOutputDelay};
	long m_resetDelay {0};
	long m_DataValidDelay;
	bool m_maintainSourceImages {false};
	long m_inspectionTimeoutMillisec {0};
	bool m_bOnline {false};

	UINT m_uOutputTimer {0};

	SvDef::NakGeneration  m_NAKMode {SvDef::Bursts};//!Different Mode for NAK Behavior Legacy, Bursts,RepairedLegacy,FixedMaximum
	int m_NAKParameter {SvDef::DefaultNakParameter}; //!Additional Parameter for NAK Behavior
	long m_NAKCount {0};
	long m_FirstNAKProcessCount {0};	///only trigger >= m_FirstNAKProcessCount will be inspected 
	long m_NewNAKCount {0};				//!Nak count will be set to 0 if no NAK occurs 
	long m_ReducedPPQPosition {cMinReducedPpqPos};	/// min number of inspection that will be checked for startInspection  for nakMode =2
	std::atomic_long m_lastPPQPosition {0L};		/// This is the PPQ position of the last SetInspectionComplete call
	int m_maxProcessingOffset4Interest {0};  
	bool m_useProcessingOffset4Interest {false};	///Flag if processing offset will used. It should only used if m_maxProcessingOffset4Interest >2 and <PPQLength, RejectCondition set and at least two Inspections in this PPQ.

	using IpInfoDeque = std::deque<SVInspectionInfoStruct>;
	std::unordered_map<uint32_t, IpInfoDeque> m_storeForInterestMap;

	SVObjectPtrVector m_childObjects;

	SvVol::BasicValueObjectPtr m_spTiggercount{ nullptr };
	SvVol::BasicValueObjectPtr m_spNotCompleteCount{ nullptr };
	SvVol::BasicValueObjectPtr m_spMissingImageCount{ nullptr };
	mutable SvVol::BasicValueObjectPtr m_spPpqLength{ nullptr };

	int m_MissingImageCount{ 0 };
	int m_NotCompleteCount{ 0 };
	
	static bool m_timerResolution;
};

typedef std::vector<SVPPQObject*> SVPPQObjectPtrVector;

