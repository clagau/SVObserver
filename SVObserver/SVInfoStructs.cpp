//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryStruct
//* .File Name       : $Workfile:   SVInfoStructs.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 06:54:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVInfoStructs.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#pragma endregion Includes

void SVInspectionInfoStruct::Init()
{
	//Init only initializes these values because the rest of the values are required for the next process cycle
	m_InspectedState = PRODUCT_NOT_INSPECTED;

	m_CanProcess = false;
	m_InProcess = false;
	m_HasBeenQueued = false;

	m_BeginInspection = 0.0;
	m_EndInspection = 0.0;
	m_BeginToolset = 0.0;
	m_EndToolset = 0.0;

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;

	m_triggerRecordWrite = nullptr;
	m_triggerRecordComplete = nullptr;
	m_lastInspectedSlot = -1;
	m_ObjectID = 0;
	m_bReject = false;
}

void SVInspectionInfoStruct::ClearIndexes()
{
	m_CanProcess = false;
	m_InProcess = false;
}

bool SVInspectionInfoStruct::setNextAvailableTR( )
{
	if( nullptr != m_pInspection )
	{
		setNextTriggerRecord();
	}
	else
	{
		return false;
	}

	return true;
}

void SVInspectionInfoStruct::setNextTriggerRecord(SvTrc::TriggerData triggerData)
{
	m_triggerRecordWrite = SvTrc::getTriggerRecordControllerRWInstance().createTriggerRecordObjectToWrite(m_inspectionPosInTrc);
	if (nullptr != m_triggerRecordWrite)
	{
		m_triggerRecordWrite->setTriggerData(triggerData);
	}
	m_triggerRecordComplete = nullptr;
}

void SVInspectionInfoStruct::setTriggerRecordCompleted()
{
	if (nullptr != m_triggerRecordWrite)
	{
		m_triggerRecordComplete = SvTrc::getTriggerRecordControllerRWInstance().closeWriteAndOpenReadTriggerRecordObject(m_triggerRecordWrite);
	}
}

void SVInspectionInfoStruct::setTriggerRecordIncompleted()
{
	if (nullptr != m_triggerRecordWrite)
	{
		SvTrc::getTriggerRecordControllerRWInstance().closeWriteObjectWithoutUpdateLastTrId(m_triggerRecordWrite);
	}
}

SVProductInfoStruct::SVProductInfoStruct()
: m_ProductActive( 0 )
, m_monitorListSMSlot(-1)
, m_lastPPQPosition {0L}
{
	Reset();
}

SVProductInfoStruct::SVProductInfoStruct( const SVProductInfoStruct &rRhs )
: m_ProductState( rRhs.m_ProductState )
, m_triggered( rRhs.m_triggered )
, m_dataComplete( rRhs.m_dataComplete )
, m_triggerInfo( rRhs.m_triggerInfo )
, m_outputsInfo( rRhs.m_outputsInfo )
, m_pPPQ( rRhs.m_pPPQ )
, m_svCameraInfos( rRhs.m_svCameraInfos )
, m_svInspectionInfos( rRhs.m_svInspectionInfos )
, m_ProductActive( 0 )
, m_monitorListSMSlot(rRhs.m_monitorListSMSlot)
, m_lastPPQPosition{rRhs.m_lastPPQPosition}
{
	for(int i=0; i < SvDef::cMaximumCameras; ++i)
	{
		m_hasCameraImage[i] = rRhs.m_hasCameraImage[i];
	}
	if( rRhs.IsProductActive() )
	{
		SetProductActive();
	}
}

SVProductInfoStruct::~SVProductInfoStruct()
{
	SetProductComplete();
	Reset();
}

const SVProductInfoStruct &SVProductInfoStruct::operator=( const SVProductInfoStruct &rRhs )
{
	if( this != &rRhs )
	{
		SetProductComplete();

		m_ProductState = rRhs.m_ProductState;
		m_triggered = rRhs.m_triggered;

		for (int i = 0; i < SvDef::cMaximumCameras; ++i)
		{
			m_hasCameraImage[i] = rRhs.m_hasCameraImage[i];
		}
		m_dataComplete = rRhs.m_dataComplete;
		m_lastPPQPosition = rRhs.m_lastPPQPosition;

		m_triggerInfo = rRhs.m_triggerInfo;
		m_outputsInfo = rRhs.m_outputsInfo;
		m_pPPQ = rRhs.m_pPPQ;

		m_svCameraInfos = rRhs.m_svCameraInfos;
		m_svInspectionInfos = rRhs.m_svInspectionInfos;
		m_monitorListSMSlot = rRhs.m_monitorListSMSlot;

		if( rRhs.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return *this;
}

HRESULT SVProductInfoStruct::Assign( const SVProductInfoStruct &rData, bool shouldSkipIPInfo)
{
	HRESULT l_Status = S_OK;

	if( this != &rData )
	{
		SetProductComplete();

		m_ProductState = rData.m_ProductState;
		m_triggered = rData.m_triggered;

		for (int i = 0; i < SvDef::cMaximumCameras; ++i)
		{
			m_hasCameraImage[i] = rData.m_hasCameraImage[i];
		}
		m_dataComplete = rData.m_dataComplete;
		m_lastPPQPosition = rData.m_lastPPQPosition;
		
		m_triggerInfo = rData.m_triggerInfo;
		m_outputsInfo = rData.m_outputsInfo;
		m_pPPQ = rData.m_pPPQ;

		m_monitorListSMSlot = rData.m_monitorListSMSlot;

		m_svCameraInfos = rData.m_svCameraInfos;

		m_svInspectionInfos.clear();

		if (!shouldSkipIPInfo)
		{
			SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = rData.m_svInspectionInfos.begin();

			while (l_InspectIter != rData.m_svInspectionInfos.end())
			{
				m_svInspectionInfos[l_InspectIter->first] = l_InspectIter->second;

				++l_InspectIter;
			}
		}

		if( rData.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return l_Status;
}

bool SVProductInfoStruct::empty() const
{
	return !m_triggered;
}

void SVProductInfoStruct::InitProductInfo()
{
	SetProductComplete();

	m_ProductState.clear();
	for (int i = 0; i < SvDef::cMaximumCameras; ++i)
	{
		m_hasCameraImage[i] = false;
	}
	m_triggered			= false;
	m_dataComplete		= false;
	m_monitorListSMSlot = -1;
	m_outputsInfo = std::move(SVOutputsInfoStruct{});
	m_triggerInfo = std::move(SvTi::SVTriggerInfoStruct{});
	
	for(auto& rCamera : m_svCameraInfos)
	{
		rCamera.second.ClearInfo();
	}

	for(auto& rInspection : m_svInspectionInfos)
	{
		rInspection.second.Init();
	}
}// end Init

void SVProductInfoStruct::Reset()
{
	SetProductComplete();

	m_ProductState.clear();
	for (int i = 0; i < SvDef::cMaximumCameras; ++i)
	{
		m_hasCameraImage[i] = false;
	}
	m_triggered			= false;
	m_dataComplete		= false;
	m_monitorListSMSlot = -1;
	m_outputsInfo = std::move(SVOutputsInfoStruct {});
	m_pPPQ = nullptr;
	m_triggerInfo = std::move(SvTi::SVTriggerInfoStruct {});
	
	for(auto& rCamera : m_svCameraInfos)
	{
		rCamera.second = std::move(SvIe::SVCameraInfoStruct{});
	}

	for(auto& rInspection : m_svInspectionInfos)
	{
		rInspection.second = std::move(SVInspectionInfoStruct{});
	}
}// end Reset

void SVProductInfoStruct::ClearIndexes()
{
	for(auto& rCamera : m_svCameraInfos)
	{
		rCamera.second.ClearCameraInfo();
	}

	for(auto& rInspection : m_svInspectionInfos)
	{
		rInspection.second.ClearIndexes();
	}
}

bool SVProductInfoStruct::setNextAvailableCameraImage( )
{
	for( auto& rCameraData : m_svCameraInfos )
	{
		SvTrc::IImagePtr pImage = rCameraData.second.GetNextImage();
		if( nullptr == pImage )
		{
			return false;
		}
	}

	return true;
}

void SVProductInfoStruct::DumpIndexInfo( std::string& rData )
{
	rData = SvUl::Format( _T( "TriggerCount=%ld-DataComplete=%s" ),	 ProcessCount(),  m_dataComplete ? _T( "T" ) : _T( "F" ));

	for(const auto& rCamera : m_svCameraInfos)
	{
		if(GUID_NULL != rCamera.first)
		{
			std::string CameraName( _T( "(null)" ) );

			SvOi::IObjectClass* pCamera =  SvOi::getObject( rCamera.first );
			if( nullptr != pCamera )
			{
				CameraName = pCamera->GetName();
			}

			SvTrc::IImagePtr pImage = rCamera.second.getImage();
			rData += SvUl::Format( _T( " : %s-Index=%ld" ), CameraName.c_str(), (nullptr != pImage) ? pImage->getBufferPos() : -1);
		}
	}

	for(const auto& rInspection : m_svInspectionInfos)
	{
		std::string l_Temp = SvUl::Format( _T( " : %s-State=0x%x" ), 
			( nullptr != rInspection.second.m_pInspection ) ? rInspection.second.m_pInspection->GetName() : _T( "(null)" ), 
			rInspection.second.m_InspectedState );

		rData += l_Temp;
	}
}

bool SVProductInfoStruct::IsProductActive() const
{
	return ( 0 < m_ProductActive );
}

void SVProductInfoStruct::SetProductActive()
{
	::InterlockedIncrement( &m_ProductActive );

	SVObjectManagerClass::Instance().IncrementProductIndicator();
}

void SVProductInfoStruct::SetProductComplete()
{
	if( 0 < m_ProductActive )
	{
		// Set ObjectManager product active to 0, then set local 
		// SVProductInfoStruct product active to 0.
		SVObjectManagerClass::Instance().AdjustProductIndicator( -m_ProductActive );

		::InterlockedExchange( &m_ProductActive, 0 );
	}

	for(auto& rIpInfoPair: m_svInspectionInfos)
	{
		rIpInfoPair.second.m_CanProcess = false;
		rIpInfoPair.second.m_InProcess = false;
	}
}

void SVProductInfoStruct::setInspectionTriggerRecordComplete(const SVGUID& rIPGuid)
{
	if (GUID_NULL != rIPGuid)
	{
		auto iter = m_svInspectionInfos.find(rIPGuid);
		if (m_svInspectionInfos.end() != iter)
		{
			iter->second.setTriggerRecordCompleted();
		}
	}
	else
	{
		for (auto& rIPInfoPair : m_svInspectionInfos)
		{
			rIPInfoPair.second.setTriggerRecordIncompleted();
		}
	}
}

SVProductInfoStruct moveInspectionToNewProduct(SVProductInfoStruct& sourceProduct, const SVGUID& rIPGuid)
{
	SVProductInfoStruct newProduct;
	newProduct.Assign(sourceProduct, true);
	auto ipIter = sourceProduct.m_svInspectionInfos.find(rIPGuid);
	if (sourceProduct.m_svInspectionInfos.end() != ipIter)
	{
		newProduct.m_svInspectionInfos[ipIter->first] = ipIter->second;
		ipIter->second.m_triggerRecordWrite = nullptr;
	}
	return newProduct;
}

SVInspectionNameUpdate::SVInspectionNameUpdate()
: m_InspectionName()
{
}

SVInspectionNameUpdate::SVInspectionNameUpdate( const std::string& p_rName )
: m_InspectionName( p_rName )
{
}

SVInspectionNameUpdate::SVInspectionNameUpdate( const SVInspectionNameUpdate& p_rObject )
: m_InspectionName( p_rObject.m_InspectionName )
{
}

SVInspectionNameUpdate::~SVInspectionNameUpdate()
{
}

SVRemoveImages::SVRemoveImages()
: m_Images()
{
}

SVRemoveImages::SVRemoveImages( const SVGuidSet& p_rImages )
: m_Images( p_rImages )
{
}

SVRemoveImages::SVRemoveImages( const SVRemoveImages& p_rObject )
: m_Images( p_rObject.m_Images )
{
}

SVRemoveImages::~SVRemoveImages()
{
}

SVRemoveValues::SVRemoveValues()
: m_Values()
{
}

SVRemoveValues::SVRemoveValues( const SVGuidSet& p_rValues )
: m_Values( p_rValues )
{
}

SVRemoveValues::SVRemoveValues( const SVRemoveValues& p_rObject )
: m_Values( p_rObject.m_Values )
{
}

SVRemoveValues::~SVRemoveValues()
{
}

SVRemoveSubjectStruct::SVRemoveSubjectStruct()
{
}

SVRemoveSubjectStruct::~SVRemoveSubjectStruct()
{
}

SVInputRequestInfoStruct::SVInputRequestInfoStruct()
: m_ValueObjectRef(), m_Value()
{
	Reset();
}

SVInputRequestInfoStruct::SVInputRequestInfoStruct( const SVObjectReference& rValueObject, const _variant_t& rValue )
: m_ValueObjectRef( rValueObject ), m_Value( rValue )
{
}

SVInputRequestInfoStruct::~SVInputRequestInfoStruct()
{
	Reset();
}

void SVInputRequestInfoStruct::Reset()
{
	m_ValueObjectRef = SVObjectReference();
	m_Value.Clear();
}// end Reset

void SVInputRequestInfoStruct::Init()
{
}// end Init

SVInputImageRequestInfoStruct::SVInputImageRequestInfoStruct()
: m_ObjectName()
, m_ImageInfo()
, m_ImageHandlePtr()
, m_bUsingCameraName( false )
{
}

SVInputImageRequestInfoStruct::~SVInputImageRequestInfoStruct()
{
	Reset();
}

void SVInputImageRequestInfoStruct::Reset()
{
	m_ObjectName.clear();
}// end Reset

void SVInputImageRequestInfoStruct::Init()
{
	m_ObjectName.clear();
}// end Init

SVOutputRequestInfoStruct::SVOutputRequestInfoStruct()
{
	Reset();
}

SVOutputRequestInfoStruct::~SVOutputRequestInfoStruct()
{
	Reset();
}

void SVOutputRequestInfoStruct::Reset()
{
	m_ProductInfo.Reset();

	pCaller		 = nullptr;
	pOwner		 = nullptr;
	pCallback	 = nullptr;
}// end Reset

void SVOutputRequestInfoStruct::Init()
{
	m_ProductInfo.Reset();

	pCaller		 = nullptr;
	pOwner		 = nullptr;
	pCallback	 = nullptr;
}// end Init

SVProductInfoRequestStruct::SVProductInfoRequestStruct()
: m_pProduct( nullptr ), m_WaitHandle( nullptr )
{
	m_WaitHandle = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( SVProductInfoStruct* p_pProduct )
: m_pProduct( p_pProduct ), m_WaitHandle( nullptr )
{
	m_WaitHandle = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( const SVProductInfoRequestStruct& p_rObject )
: m_pProduct( p_rObject.m_pProduct ), m_WaitHandle( nullptr )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVProductInfoRequestStruct::~SVProductInfoRequestStruct()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = nullptr;
	}
}

const SVProductInfoRequestStruct& SVProductInfoRequestStruct::operator=( const SVProductInfoRequestStruct& p_rObject )
{
	if( this != &p_rObject )
	{
		m_pProduct = p_rObject.m_pProduct;

		p_rObject.GetWaitHandle( m_WaitHandle );
	}

	return *this;
}

bool SVProductInfoRequestStruct::empty() const
{
	return ( nullptr == m_pProduct );
}

void SVProductInfoRequestStruct::clear()
{
	if( nullptr != m_pProduct )
	{
		m_pProduct = nullptr;
	}

	if( nullptr != m_WaitHandle )
	{
		::ResetEvent( m_WaitHandle );
	}
}

HRESULT SVProductInfoRequestStruct::NotifyRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		::SetEvent( m_WaitHandle );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVProductInfoRequestStruct::WaitForRequest() const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
	{
		if( ::ResetEvent( m_WaitHandle ) )
		{
			if( ::WaitForSingleObject( m_WaitHandle, INFINITE ) != WAIT_OBJECT_0 )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVProductInfoRequestStruct::GetWaitHandle( HANDLE& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rHandle )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = nullptr;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

