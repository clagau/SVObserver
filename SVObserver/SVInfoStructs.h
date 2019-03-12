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
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/any.hpp>
#include "Definitions/GlobalConst.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVCameraInfoStruct.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#include "SVIOLibrary/SVIOEntryStruct.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "TriggerInformation/SVTriggerInfoStruct.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerRecordController/ITriggerRecordRW.h"
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

typedef std::vector<std::pair<_variant_t, bool>> VariantBoolPairVector;
typedef std::vector<std::pair<GUID, _variant_t>> GuidVariantPairVector;

struct SVInputsInfoStruct 
{
	SVInputsInfoStruct();
	SVInputsInfoStruct( const SVInputsInfoStruct& p_rsvObject );
	virtual ~SVInputsInfoStruct();

	const SVInputsInfoStruct& operator=( const SVInputsInfoStruct& p_rsvObject );

	void Reset();
	void Init();

	VariantBoolPairVector m_Inputs;
};


struct SVOutputsInfoStruct 
{
	SVOutputsInfoStruct();
	SVOutputsInfoStruct( const SVOutputsInfoStruct& p_rsvObject );
	virtual ~SVOutputsInfoStruct();

	const SVOutputsInfoStruct& operator=( const SVOutputsInfoStruct& p_rsvObject );

	void Reset();
	void Init();

	long m_OutputDelay;
	long m_ResetDelay;
	long m_DataValidDelay;
	bool m_DataValidResult;
	bool m_OutputToggleResult;
	bool m_NakResult;
	SvTl::SVTimeStamp m_EndOutputDelay;
	SvTl::SVTimeStamp m_EndResetDelay;
	SvTl::SVTimeStamp m_EndDataValidDelay;
	SvTl::SVTimeStamp m_BeginProcess;
	SvTl::SVTimeStamp m_EndProcess;
	GuidVariantPairVector m_Outputs;
};


struct SVInspectionInfoStruct 
{
	SVInspectionInfoStruct();
	SVInspectionInfoStruct( const SVInspectionInfoStruct &p_rsvData );
	virtual ~SVInspectionInfoStruct();

	const SVInspectionInfoStruct &operator=( const SVInspectionInfoStruct &p_rsvData );

	HRESULT Assign( const SVInspectionInfoStruct &p_rsvData );

	void Reset();
	void Init();
	void ClearIndexes();
	
	bool setNextAvailableTR();

	void setNextTriggerRecord();

	void setTriggerRecordCompleted();

	SVInspectionProcess* pInspection {nullptr};
	SVProductInspectedState oInspectedState {PRODUCT_NOT_INSPECTED};

	bool m_CanProcess {false};
	bool m_InProcess {false};
	bool m_HasBeenQueued {false};
	
	SvTl::SVTimeStamp m_BeginInspection {0};
	SvTl::SVTimeStamp m_EndInspection {0};
	SvTl::SVTimeStamp m_BeginToolset {0};
	SvTl::SVTimeStamp m_EndToolset {0};
	SvTl::SVTimeStamp m_CallbackReceived {0};
	
	int m_inspectionPosInTrc = -1; //position of the inspection in triggerRecordController
	SvTrc::ITriggerRecordRWPtr m_triggerRecordWrite = nullptr;
	SvTrc::ITriggerRecordRPtr  m_triggerRecordComplete = nullptr;

	double m_ToolSetEndTime {0.0};
	double m_ToolSetAvgTime {0.0};
	long m_lastInspectedSlot {-1}; // Shared Memory
};

typedef std::map< SVGUID, SVInspectionInfoStruct > SVGUIDSVInspectionInfoStructMap;


struct SVProductInfoStruct 
{
	SVProductInfoStruct();
	SVProductInfoStruct( const SVProductInfoStruct &rRhs );
	virtual ~SVProductInfoStruct();

	const SVProductInfoStruct &operator=( const SVProductInfoStruct &rRhs );

	/// Assigned the data from the other instance to this.
	/// \param p_rsvData [in] Source data.
	/// \param shouldSkipIPInfo [in] If true, the IPInfo will not be assigned and the map will be empty.
	/// \returns HRESULT
	HRESULT Assign( const SVProductInfoStruct &p_rsvData, bool shouldSkipIPInfo = false );

	bool empty() const;

	void InitProductInfo();
	void Reset();

	void ClearIndexes();

	bool setNextAvailableCameraImage( );

	inline long ProcessCount() const {return oTriggerInfo.lTriggerCount;}
	inline const SvTl::SVTimeStamp& TimeStamp() const {return oTriggerInfo.m_BeginProcess;}

	void DumpIndexInfo( std::string& p_rData );

	bool IsProductActive() const;
	void SetProductActive();
	void SetProductComplete();
	
	/// Set the TriggerRecord from write to readOnly for the required IP.
	/// \param rIPGuid [in] Guid of the IP. If GUID_NULL then for inspection will done the action.
	void setInspectionTriggerRecordComplete(const SVGUID& rIPGuid);

	mutable std::string m_ProductState;
	bool bTriggered;
	bool bhasCameraImage[SvDef::cMaximumCameras];
	bool bDataComplete;

	SvTi::SVTriggerInfoStruct oTriggerInfo;
	SVInputsInfoStruct oInputsInfo;
	SVOutputsInfoStruct oOutputsInfo;
	SVPPQObject* m_pPPQ = nullptr;

	SvIe::SVGuidSVCameraInfoStructMap m_svCameraInfos;
	SVGUIDSVInspectionInfoStructMap	m_svInspectionInfos;
	long m_lastInspectedSlot; // Shared Memory

protected:
	long m_ProductActive;
};

/// This function copy a productInfo to another except from the IP-Info. It will only one IPInfo copied and the triggerRecordWriter will be moved.
/// \param sourceProduct [in] Source data.
/// \param rIPGuid [in] GUID of the inspection 
/// \returns SVProductInfoStruct
SVProductInfoStruct moveInspectionToNewProduct(SVProductInfoStruct& sourceProduct, const SVGUID& rIPGuid);

struct SVInspectionNameUpdate
{
	SVInspectionNameUpdate();
	SVInspectionNameUpdate( const std::string& p_rName );
	SVInspectionNameUpdate( const SVInspectionNameUpdate& p_rObject );

	virtual ~SVInspectionNameUpdate();

	std::string m_InspectionName;

};

struct SVRemoveImages
{
	SVRemoveImages();
	SVRemoveImages( const SVGuidSet& p_rImages );
	SVRemoveImages( const SVRemoveImages& p_rObject );

	virtual ~SVRemoveImages();

	SVGuidSet m_Images;

};

struct SVRemoveValues
{
	SVRemoveValues();
	SVRemoveValues( const SVGuidSet& p_rValues );
	SVRemoveValues( const SVRemoveValues& p_rObject );

	virtual ~SVRemoveValues();

	SVGuidSet m_Values;

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
	virtual ~SVInputRequestInfoStruct();

	void Reset();
	void Init();

	SVObjectReference m_ValueObjectRef;
	_variant_t m_Value;

private:
	SVInputRequestInfoStruct( const SVInputRequestInfoStruct& rObject );

	const SVInputRequestInfoStruct& operator=( const SVInputRequestInfoStruct& rObject );

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
	SVImageInfoClass m_ImageInfo;
	SvOi::SVImageBufferHandlePtr m_ImageHandlePtr;
	bool m_bUsingCameraName;

private:
	SVInputImageRequestInfoStruct( const SVInputImageRequestInfoStruct& p_rsvObject );

	const SVInputImageRequestInfoStruct& operator=( const SVInputImageRequestInfoStruct& p_rsvObject );

};

typedef std::shared_ptr< SVInputImageRequestInfoStruct > SVInputImageRequestInfoStructPtr;
typedef std::vector< SVInputImageRequestInfoStructPtr > SVInputImageRequestInfoStructPtrVector;

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
