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
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/any.hpp>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVOCore/SVCameraInfoStruct.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVLibrary/SVIOEntryHostStruct.h"
#include "SVValueObjectLibrary/SVValueObjectReference.h"
#include "SVRunControlLibrary/ProductWorkloadInformation.h" 
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerInformation/SVTriggerInfoStruct.h"
#pragma endregion Includes

class SVToolClass;

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
	long lDataValidDelay;
	bool DataValidResult;
	bool OutputToggleResult;
	SVClock::SVTimeStamp m_EndOutputDelay;
	SVClock::SVTimeStamp m_EndResetDelay;
	SVClock::SVTimeStamp m_EndDataValidDelay;
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

	SVPPQObject* pPPQ;

	SVDataManagerHandle m_ResultDataDMIndexHandle;
	SVDataManagerHandle m_ResultImagePublishedDMIndexHandle;

	SVIOEntryStructVector m_InputData;
};

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

	ProductWorkloadInformation GetWorkloadInformation(){return m_WorkloadInfo;}

	SVString m_ProductState;
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

	SVString m_ObjectName;
	SVImageInfoClass m_ImageInfo;
	SVSmartHandlePointer m_ImageHandlePtr;
	bool m_bUsingCameraName;

private:
	SVInputImageRequestInfoStruct( const SVInputImageRequestInfoStruct& p_rsvObject );

	const SVInputImageRequestInfoStruct& operator=( const SVInputImageRequestInfoStruct& p_rsvObject );

};

typedef SVSharedPtr< SVInputImageRequestInfoStruct > SVInputImageRequestInfoStructPtr;
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
