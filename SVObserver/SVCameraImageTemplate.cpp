//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraImageTemplate
//* .File Name       : $Workfile:   SVCameraImageTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraImageTemplate.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVVirtualCamera.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVUtilityLibrary/SetBits.h"
#pragma endregion Includes

SVCameraImageTemplate::SVCameraImageTemplate( LPCSTR ObjectName )
: SVImageClass( ObjectName )
, mpCamera( nullptr )
, digitizerObjectID()
, m_CurrentIndex()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed( SvDef::SV_PUBLISH_RESULT_IMAGE, SvOi::SetAttributeType::RemoveAttribute );
}

SVCameraImageTemplate::SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID )
: SVImageClass( POwner, StringResourceID )
, mpCamera( nullptr )
, digitizerObjectID()
, m_CurrentIndex()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed( SvDef::SV_PUBLISH_RESULT_IMAGE, SvOi::SetAttributeType::RemoveAttribute );
}

SVCameraImageTemplate::~SVCameraImageTemplate()
{
}

HRESULT SVCameraImageTemplate::UpdateCameraImage( LPCTSTR p_szCameraName )
{
	mpCamera = nullptr;
	digitizerObjectID.clear();

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectCompleteName( p_szCameraName );

	if( nullptr != l_pObject )
	{
		digitizerObjectID = l_pObject->GetUniqueObjectID();
	}

	return RebuildCameraImage();
}

HRESULT SVCameraImageTemplate::UpdateCameraImage( const SVGUID& p_CameraID )
{
	mpCamera = nullptr;
	digitizerObjectID = p_CameraID;

	return RebuildCameraImage();
}

bool SVCameraImageTemplate::DestroyImage()
{
	mpCamera = nullptr;
	digitizerObjectID.clear();
	m_CurrentIndex.clear();

	bool bOk = SVImageClass::DestroyImage();
	
	return bOk;
}

bool SVCameraImageTemplate::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVImageClass::CreateObject(rCreateStructure);

	l_bOk = l_bOk && ( S_OK == RebuildCameraImage() );

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed( SvDef::SV_PUBLISH_RESULT_IMAGE, SvOi::SetAttributeType::RemoveAttribute );
	SetObjectAttributesAllowed( SvDef::SV_DD_IMAGE, SvOi::SetAttributeType::AddAttribute );
	
	m_isCreated = l_bOk;
	
	return l_bOk;	
}

bool SVCameraImageTemplate::SetImageDepth( long lDepth )
{
	long l_lNewDepth = lDepth;

	if ( !( GetCameraBufferArrayPtr().empty() ) )
	{
		l_lNewDepth = static_cast< long >( GetCameraBufferArrayPtr()->size() );
	}
	
  return SVImageClass::SetImageDepth( l_lNewDepth );
}

void SVCameraImageTemplate::ResetImageIndex()
{
	m_CurrentIndex.clear();
}

bool SVCameraImageTemplate::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	RebuildCameraImage();

	if( !( GetCameraBufferArrayPtr().empty() ) )
	{
		m_LastUpdate = std::max( m_LastUpdate, GetCameraBufferArrayPtr()->GetLastResetTimeStamp() );
	}

	return SVImageClass::ResetObject(pErrorMessages);
}

SVVirtualCamera* SVCameraImageTemplate::GetCamera() const
{
	return mpCamera;
}

std::string SVCameraImageTemplate::GetCameraName() const
{
	std::string l_String;

	if( nullptr != mpCamera )
	{
		l_String = mpCamera->GetName();
	}

	return l_String;
}

HRESULT SVCameraImageTemplate::RestoreMainImage( SVInspectionProcess* p_psvInspection )
{
	HRESULT l_svOk = S_OK;

	if( nullptr != p_psvInspection )
	{
		if( nullptr != mpCamera && nullptr != mpCamera->mpsvDevice && nullptr != mpCamera->mpsvDevice->m_LastImage )
		{
			if( 0 <= m_CurrentIndex.m_CameraDMIndexHandle.GetIndex() && nullptr != GetCameraBufferArrayPtr() )
			{
				GetCameraBufferArrayPtr()->CopyValue( m_CurrentIndex.m_CameraDMIndexHandle );
			}

			l_svOk = p_psvInspection->AddInputImageRequest( this, mpCamera->mpsvDevice->m_LastImage );
		}
	}
	else
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

HRESULT SVCameraImageTemplate::GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const
{
	HRESULT hr = S_OK;

	if( scDigitizerIDTag == rValueName )
	{


		rValue = digitizerObjectID.ToVARIANT();

	}
	else
	{
		hr = SVImageClass::GetObjectValue( rValueName, rValue );
	}

	return hr;
}

HRESULT SVCameraImageTemplate::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	bool bOk = false;

	SvCl::SVObjectStdStringArrayClass ClassIDList;

	if ( ( bOk = PDataObject->GetAttributeData( scDigitizerIDTag, ClassIDList ) ) )
	{
		for( int i = 0; i < static_cast<int> (ClassIDList.size()); i++ )
		{
			// convert the guidStr to a Guid
			AfxGetClassIDFromString( ClassIDList[i].c_str(), &( digitizerObjectID.ToGUID() ) );
		}
	}
	else
	{
		hr = SVImageClass::SetObjectValue( PDataObject );

		m_ObjectAttributesAllowed |= SvDef::SV_REMOTELY_SETABLE;

		return hr;
	}

	m_LastUpdate = SvTl::GetTimeStamp();

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

void SVCameraImageTemplate::Persist( SVObjectWriter& rWriter )
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist(rWriter);
	SVImageClass::PersistImageAttributes( rWriter );

	_variant_t value;
	value.SetString(digitizerObjectID.ToString().c_str());
	rWriter.WriteAttribute(scDigitizerIDTag, value);
	value.Clear();

	rWriter.EndElement();
}

GUID SVCameraImageTemplate::GetDigitizerID()
{
	return digitizerObjectID.ToGUID();
}

SVImageIndexStruct SVCameraImageTemplate::GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap )
{
	SVImageIndexStruct svIndex;

	if ( nullptr != mpCamera )
	{
		 mpCamera->GetSourceImageIndex( &svIndex.m_CameraDMIndexHandle, rGuidCameraMap );
	}

	return svIndex;
}

HRESULT SVCameraImageTemplate::DisconnectBuffers()
{
	return S_OK;
}

HRESULT SVCameraImageTemplate::ReconnectBuffers()
{
	HRESULT hr = S_OK;

	hr = RebuildCameraImage();

	if( S_OK == hr )
	{
		mpCamera->GetImageInfo( &m_ImageInfo );

		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SvTl::GetTimeStamp();

		if ( !ResetObject() )
		{
			hr = S_FALSE;
		}
	}

	return hr;
}

bool SVCameraImageTemplate::GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const
{
	rsvIndex = m_CurrentIndex;

	return true;
}

HRESULT SVCameraImageTemplate::GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const
{
	HRESULT l_Status( S_OK );

	l_Status = p_rHandle.Assign( rIndex.m_CameraDMIndexHandle, rIndex.m_CameraDMIndexHandle.GetLockType() );

	return l_Status;
}

SVImageObjectClassPtr SVCameraImageTemplate::GetCameraBufferArrayPtr() const
{
	SVImageObjectClassPtr l_ArrayPtr;

	if ( nullptr != mpCamera && nullptr != mpCamera->mpsvDevice )
	{
		l_ArrayPtr = mpCamera->mpsvDevice->GetCircleBuffer();
	}

	return l_ArrayPtr;
}

HRESULT SVCameraImageTemplate::RegisterAsSubObject()
{
	return SVImageClass::RegisterAsSubObject();
}

HRESULT SVCameraImageTemplate::UnregisterAsSubObject()
{
	return SVImageClass::UnregisterAsSubObject();
}

HRESULT SVCameraImageTemplate::RebuildCameraImage()
{
	HRESULT l_Status = S_OK;

	SVVirtualCamera* pCamera( nullptr );
	SVObjectClass* pOwner = GetOwner();

	if( !( digitizerObjectID.empty() ) )
	{
		pCamera = dynamic_cast< SVVirtualCamera* >( SVObjectManagerClass::Instance().GetObject( digitizerObjectID.ToGUID() ) );
	}

	if( ( nullptr == pCamera ) || ( nullptr != pOwner && pOwner->GetObjectType() == SvDef::SVToolSetObjectType ) )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		if( nullptr != pInspection )
		{
			SVVirtualCamera* pTempCamera = pInspection->GetFirstCamera();

			if( nullptr != pTempCamera )
			{
				pCamera = pTempCamera;
			}
		}
	}

	if( nullptr != pCamera )
	{
		if( mpCamera != pCamera )
		{
			SVImageObjectClassPtr l_ArrayPtr;

			if( !pCamera->mpsvDevice.empty() )
			{
				l_ArrayPtr = pCamera->mpsvDevice->GetCircleBuffer();
			}

			if( !( l_ArrayPtr.empty() ) && ( 0 < l_ArrayPtr->size() ) )
			{
				SVImageInfoClass CameraImageInfo;

				pCamera->GetImageInfo( &CameraImageInfo );

				if( CreateBuffers( CameraImageInfo, l_ArrayPtr ) )
				{
					mpCamera = pCamera;
					digitizerObjectID = mpCamera->GetUniqueObjectID();
				}
			}

			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

			if( nullptr != pInspection )
			{
				pInspection->AddSharedCamera( mpCamera );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

