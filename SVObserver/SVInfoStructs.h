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
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "InspectionEngine/SVCameraInfoStruct.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObjectLibrary/SVIOEntryHostStruct.h"
#include "SVStatusLibrary/ProductWorkloadInformation.h" 
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerInformation/SVTriggerInfoStruct.h"
#include "TriggerRecordController/ITriggerRecordRW.h"
#pragma endregion Includes

class SVToolClass;
//class LoopTool;
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
class SVPPQObject;
class SVImageClass;
class SVVirtualCamera;
class SVInspectionProcess;
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
	SvTl::SVTimeStamp m_BeginProcess;
	SvTl::SVTimeStamp m_EndProcess;
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
	long lDataValidDelay;
	bool DataValidResult;
	bool OutputToggleResult;
	SvTl::SVTimeStamp m_EndOutputDelay;
	SvTl::SVTimeStamp m_EndResetDelay;
	SvTl::SVTimeStamp m_EndDataValidDelay;
	SvTl::SVTimeStamp m_BeginProcess;
	SvTl::SVTimeStamp m_EndProcess;
};


struct SVPPQInfoStruct 
{
	SVPPQInfoStruct();
	SVPPQInfoStruct( const SVPPQInfoStruct& p_rsvObject );

	virtual ~SVPPQInfoStruct();

	HRESULT Assign( const SVPPQInfoStruct &p_rsvObject );

	void Reset();
	void InitPPQInfo();
	void ClearIndexes();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	SVPPQObject* pPPQ;

	SVDataManagerHandle m_ResultDataDMIndexHandle;

	SVIOEntryStructVector m_InputData;
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

	HRESULT GetNextAvailableIndexes( );

	void setNextTriggerRecord();

	void SetProductComplete();
	
	SVInspectionProcess* pInspection;
	SVProductInspectedState oInspectedState;

	bool m_CanProcess;
	bool m_InProcess;
	bool m_HasBeenQueued;
	
	SvTl::SVTimeStamp m_BeginInspection;
	SvTl::SVTimeStamp m_EndInspection;
	SvTl::SVTimeStamp m_BeginToolset;
	SvTl::SVTimeStamp m_EndToolset;
	SvTl::SVTimeStamp m_CallbackReceived;
	
	int m_inspectionPosInTrc = -1; //position of the inspection in triggerRecordController
	SvTrc::ITriggerRecordRWPtr m_triggerRecordWrite = nullptr;
	SvTrc::ITriggerRecordRPtr  m_triggerRecordComplete = nullptr;

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

	HRESULT Assign( const SVProductInfoStruct &p_rsvData );

	bool empty() const;

	void InitProductInfo();
	void Reset();
	void ClearIndexes();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType );

	inline long ProcessCount() const {return oTriggerInfo.lTriggerCount;}
	inline const SvTl::SVTimeStamp& TimeStamp() const {return oTriggerInfo.m_BeginProcess;}

	bool IsAlive() const;

	void DumpIndexInfo( std::string& p_rData );

	HRESULT GetResultDataIndex( SVDataManagerHandle& p_rHandle ) const;

	bool IsProductActive() const;
	void SetProductActive();
	void SetProductComplete();

	ProductWorkloadInformation GetWorkloadInformation(){return m_WorkloadInfo;}

	std::string m_ProductState;
	BOOL bTriggered;
	BOOL bStartAcquisition;
	BOOL bFinishAcquisition;
	BOOL bDelayExpired;
	BOOL bDataComplete;
	BOOL bStreamed;
	HRESULT hrPPQStatus;

	ProductWorkloadInformation m_WorkloadInfo;

	SvTi::SVTriggerInfoStruct oTriggerInfo;
	SVInputsInfoStruct oInputsInfo;
	SVOutputsInfoStruct oOutputsInfo;
	SVPPQInfoStruct oPPQInfo;

	SVGuidSVCameraInfoStructMap m_svCameraInfos;
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

	bool empty() const;
	void clear();

	SVGUID m_InspectionID;
	SVProductInfoStruct m_ProductInfo;

};

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
