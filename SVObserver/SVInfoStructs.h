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

#pragma once

#ifndef _DEBUG
	#define DebugBreak() ; // Do Nothing
#endif

#pragma region Includes
#include "Definitions/GlobalConst.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVCameraInfoStruct.h"
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "Triggering/SVTriggerInfoStruct.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#pragma endregion Includes

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

class SVPPQObject;
class SVInspectionProcess;

typedef std::vector<std::pair<uint32_t, _variant_t>> ObjectIdVariantPairVector;

struct SVOutputsInfoStruct 
{
	SVOutputsInfoStruct() = default;
	SVOutputsInfoStruct(const SVOutputsInfoStruct& p_rsvObject) = default;
	virtual ~SVOutputsInfoStruct() = default;

	SVOutputsInfoStruct& operator=(const SVOutputsInfoStruct& rRhs) = default;
	SVOutputsInfoStruct& operator=(SVOutputsInfoStruct&& rRhs) = default;

	bool m_DataValidResult {false};
	bool m_OutputToggleResult {false};
	bool m_NakResult {true};
	double m_EndOutputDelay {0.0};
	double m_EndResetDelay {0.0};
	double m_EndDataValidDelay {0.0};
	double m_BeginProcess {0.0};
	double m_EndProcess {0.0};
	ObjectIdVariantPairVector m_Outputs;
};


struct SVInspectionInfoStruct 
{
	SVInspectionInfoStruct() = default;
	SVInspectionInfoStruct( const SVInspectionInfoStruct& rRhs) = default;
	virtual ~SVInspectionInfoStruct() = default;

	SVInspectionInfoStruct& operator=(const SVInspectionInfoStruct& rRhs) = default;
	SVInspectionInfoStruct& operator=(SVInspectionInfoStruct&& rRhs) = default;

	void Init();

	void ClearIndexes();
	void clearTRCs();
	
	bool setNextAvailableTR();

	void setNextTriggerRecord(SvOi::TriggerData triggerData = SvOi::TriggerData());

	void setTriggerRecordCompleted();
	void setTriggerRecordIncompleted();

	SVInspectionProcess* m_pInspection {nullptr};
	SVProductInspectedState m_InspectedState {PRODUCT_NOT_INSPECTED};

	mutable bool m_CanProcess {false};
	mutable bool m_InProcess {false};
	bool m_HasBeenQueued {false};
	
	double m_BeginInspection {0.0};
	double m_EndInspection {0.0};
	double m_BeginToolset {0.0};
	double m_EndToolset {0.0};
	
	int m_inspectionPosInTrc = -1; //position of the inspection in triggerRecordController
	SvOi::ITriggerRecordRWPtr m_triggerRecordWrite = nullptr;
	SvOi::ITriggerRecordRPtr  m_triggerRecordComplete = nullptr;

	double m_ToolSetEndTime {0.0};
	double m_ToolSetAvgTime {0.0};
	long m_lastInspectedSlot {-1}; // Shared Memory
	DWORD m_ObjectID{0};
	bool m_bReject = false;
};

typedef std::map< uint32_t, SVInspectionInfoStruct > ObjectIdSVInspectionInfoStructMap;


struct SVProductInfoStruct 
{
	SVProductInfoStruct();
	SVProductInfoStruct( const SVProductInfoStruct &rRhs );
	virtual ~SVProductInfoStruct() = default;

	SVProductInfoStruct &operator=( const SVProductInfoStruct &rRhs );

	/// Assigned the data from the other instance to this.
	/// \param p_rsvData [in] Source data.
	/// \param shouldSkipIPInfo [in] If true, the IPInfo will not be assigned and the map will be empty.
	/// \returns HRESULT
	HRESULT Assign( const SVProductInfoStruct &p_rsvData, bool shouldSkipIPInfo = false );

	SVProductInfoStruct copyInspectionToNewProduct(uint32_t iPId);

	bool empty() const;

	void InitProductInfo();
	void Reset();

	void ClearIndexes();
	void clearTRCs();

	bool setNextAvailableCameraImage( );

	inline long ProcessCount() const {return m_triggerInfo.lTriggerCount;}
	inline const double& TimeStamp() const {return m_triggerInfo.m_triggerTimeStamp;}

	void DumpIndexInfo( std::string& p_rData );

	bool IsProductActive() const;
	void SetProductActive();
	void SetProductComplete() const;
	
	/// Set the TriggerRecord from write to readOnly for the required IP.
	/// \param iPId [in] id of the IP. If SvDef::InvalidObjectId then for inspection will done the action.
	void setInspectionTriggerRecordComplete(uint32_t iPId);

	mutable std::string m_ProductState;
	bool m_triggered;
	bool m_hasCameraImage[SvDef::cMaximumCameras];
	bool m_dataComplete;
	long m_lastPPQPosition;

	SvTrig::SVTriggerInfoStruct m_triggerInfo;
	SVOutputsInfoStruct m_outputsInfo;
	SVPPQObject* m_pPPQ = nullptr;

	SvIe::SVObjectIdSVCameraInfoStructMap m_svCameraInfos;
	ObjectIdSVInspectionInfoStructMap	m_svInspectionInfos;
	long m_monitorListSMSlot; // Shared Memory

protected:
	mutable std::atomic_bool m_ProductActive;
};

struct SVInspectionNameUpdate
{
	SVInspectionNameUpdate();
	explicit SVInspectionNameUpdate( const std::string& p_rName );
	explicit SVInspectionNameUpdate( const SVInspectionNameUpdate& p_rObject );

	virtual ~SVInspectionNameUpdate();

	std::string m_InspectionName;
};

struct SVRemoveImages
{
	SVRemoveImages();
	explicit SVRemoveImages( const std::set<uint32_t>& p_rImages );
	explicit SVRemoveImages( const SVRemoveImages& p_rObject );

	virtual ~SVRemoveImages();

	std::set<uint32_t> m_Images;

};

struct SVRemoveValues
{
	SVRemoveValues();
	explicit SVRemoveValues( const std::set<uint32_t>& p_rValues );
	explicit SVRemoveValues( const SVRemoveValues& p_rObject );

	virtual ~SVRemoveValues();

	std::set<uint32_t> m_Values;

};

struct SVRemoveSubjectStruct
{
	SVRemoveSubjectStruct();

	virtual ~SVRemoveSubjectStruct();

};

struct SVInputRequestInfoStruct
{
	SVInputRequestInfoStruct();
	SVInputRequestInfoStruct( const SVObjectReference& rValueObject, const _variant_t& rValue );
	SVInputRequestInfoStruct(const SVInputRequestInfoStruct& rObject) = delete;
	SVInputRequestInfoStruct& operator=(const SVInputRequestInfoStruct& rObject) = delete;
	~SVInputRequestInfoStruct() = default;

	void Reset();
	void Init();

	SVObjectReference m_ValueObjectRef;
	_variant_t m_Value;
};

typedef std::shared_ptr< SVInputRequestInfoStruct > SVInputRequestInfoStructPtr;
typedef std::vector< SVInputRequestInfoStructPtr > SVInputRequestInfoStructPtrVector;

struct SVInputImageRequestInfoStruct
{
	SVInputImageRequestInfoStruct();
	virtual ~SVInputImageRequestInfoStruct();

	void Reset();
	void Init();

	std::string m_ObjectName;
	uint32_t m_toolId = SvDef::InvalidObjectId;
	SVImageInfoClass m_ImageInfo;
	SvOi::SVImageBufferHandlePtr m_ImageHandlePtr;
	bool m_bUsingCameraName;
	bool m_bChangingToolInput = false;

private:
	SVInputImageRequestInfoStruct( const SVInputImageRequestInfoStruct& p_rsvObject );

	const SVInputImageRequestInfoStruct& operator=( const SVInputImageRequestInfoStruct& p_rsvObject );

};

typedef std::shared_ptr< SVInputImageRequestInfoStruct > SVInputImageRequestInfoStructPtr;
typedef std::vector< SVInputImageRequestInfoStructPtr > SVInputImageRequestInfoStructPtrVector;

struct SVProductInfoRequestStruct
{
	SVProductInfoRequestStruct();
	explicit SVProductInfoRequestStruct( SVProductInfoStruct* p_pProduct );
	explicit SVProductInfoRequestStruct( const SVProductInfoRequestStruct& p_rObject );
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
