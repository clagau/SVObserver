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

#include "stdafx.h"
#include "SVCameraImageTemplate.h"

#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVVirtualCamera.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"

SVCameraImageTemplate::SVCameraImageTemplate( LPCSTR ObjectName )
: SVImageClass( ObjectName )
, mpCamera( NULL )
, digitizerObjectID()
, m_CurrentIndex()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetBits( ObjectAttributesAllowedRef(), SV_PUBLISH_RESULT_IMAGE, false );
}

SVCameraImageTemplate::SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID )
: SVImageClass( POwner, StringResourceID )
, mpCamera( NULL )
, digitizerObjectID()
, m_CurrentIndex()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageObjectType;

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetBits( ObjectAttributesAllowedRef(), SV_PUBLISH_RESULT_IMAGE, false );
}

SVCameraImageTemplate::~SVCameraImageTemplate()
{
}

HRESULT SVCameraImageTemplate::UpdateCameraImage( LPCTSTR p_szCameraName )
{
	mpCamera = NULL;
	digitizerObjectID.clear();

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectCompleteName( p_szCameraName );

	if( l_pObject != NULL )
	{
		digitizerObjectID = l_pObject->GetUniqueObjectID();
	}

	return RebuildCameraImage();
}

HRESULT SVCameraImageTemplate::UpdateCameraImage( const SVGUID& p_CameraID )
{
	mpCamera = NULL;
	digitizerObjectID = p_CameraID;

	return RebuildCameraImage();
}

BOOL SVCameraImageTemplate::DestroyImage()
{
	BOOL bOk = TRUE;
	
	mpCamera = NULL;
	digitizerObjectID.clear();
	m_CurrentIndex.clear();

	bOk = SVImageClass::DestroyImage();
	
	return bOk;
}

BOOL SVCameraImageTemplate::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	BOOL l_bOk = SVImageClass::CreateObject(PCreateStruct);

	l_bOk = l_bOk && ( RebuildCameraImage() == S_OK );

	// SVMainImageClass is not a result image.
	// We need to remove the PUBLISH attribute.
	SetBits( ObjectAttributesAllowedRef(), SV_PUBLISH_RESULT_IMAGE, false );
	SetBits( ObjectAttributesAllowedRef(), SV_CH_IMAGE, true );
	SetBits( ObjectAttributesAllowedRef(), SV_DD_IMAGE, true );
	
	isCreated = l_bOk;
	
	return l_bOk;	
}

BOOL SVCameraImageTemplate::SetImageDepth( long lDepth )
{
	long l_lNewDepth = lDepth;

	if ( !( GetCameraBufferArrayPtr().empty() ) )
	{
		l_lNewDepth = static_cast< long >( GetCameraBufferArrayPtr()->size() );
	}
	
  return SVImageClass::SetImageDepth( l_lNewDepth );
}

BOOL SVCameraImageTemplate::ResetImageIndex()
{
	BOOL l_Status = true;

	m_CurrentIndex.clear();

	return l_Status;
}

HRESULT SVCameraImageTemplate::ResetObject()
{
	HRESULT l_Status = RebuildCameraImage();

	if( !( GetCameraBufferArrayPtr().empty() ) )
	{
		m_LastUpdate = std::max( m_LastUpdate, GetCameraBufferArrayPtr()->GetLastResetTimeStamp() );
	}

	l_Status = SVImageClass::ResetObject();

	return l_Status;
}

SVVirtualCamera* SVCameraImageTemplate::GetCamera() const
{
	return mpCamera;
}

SVString SVCameraImageTemplate::GetCameraName() const
{
	SVString l_String;

	if( mpCamera != NULL )
	{
		l_String = mpCamera->GetName();
	}

	return l_String;
}

HRESULT SVCameraImageTemplate::RestoreMainImage( SVInspectionProcess* p_psvInspection )
{
	HRESULT l_svOk = S_OK;

	if( p_psvInspection != NULL )
	{
		if( mpCamera != NULL && mpCamera->mpsvDevice != NULL && mpCamera->mpsvDevice->m_LastImage != NULL )
		{
			if( 0 <= m_CurrentIndex.m_CameraDMIndexHandle.GetIndex() && GetCameraBufferArrayPtr() != NULL )
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

HRESULT SVCameraImageTemplate::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "DigitizerID" ) )
	{
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		l_TempVariant = digitizerObjectID.ToVARIANT();

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVImageClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVCameraImageTemplate::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "DigitizerID" ) )
	{
		if( p_rVariantValue.vt == VT_BSTR )
		{
			digitizerObjectID = p_rVariantValue;

			m_LastUpdate = SVClock::GetTimeStamp();
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = SVImageClass::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVCameraImageTemplate::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	SvCl::SVObjectCStringArrayClass svCStringArray;

	if ( ( bOk = PDataObject->GetAttributeData( "DigitizerID", svCStringArray ) ) )
	{
		for( int i = 0; i < svCStringArray.GetSize(); i++ )
		{
			// convert the guidStr to a Guid
			AfxGetClassIDFromString( svCStringArray[i], &( digitizerObjectID.ToGUID() ) );
		}
	}
	else
	{
		hr = SVImageClass::SetObjectValue( PDataObject );

		m_uObjectAttributesAllowed |= SV_REMOTELY_SETABLE;

		return hr;
	}

	m_LastUpdate = SVClock::GetTimeStamp();

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

#ifdef USE_OBJECT_SCRIPT
void SVCameraImageTemplate::GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString script;

	// preallocate 4K
    script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	SVImageClass::GetObjectScript( script, RStrAliasTable, Indent );

	CString nameStrDelimiter = _T( "'" );

	// Generate indent...
	CString strIndent = _T( "\n" );
	if( Indent )
	{
		CString tabsStrIndent(_T( '\t' ), Indent);
		strIndent += tabsStrIndent;
	}

	// Name is delimited by single quotes - SEJ july 23,1999
	CString objectTag = nameStrDelimiter + _T( "_object_ID_" ) + GetObjectName();

	// Get the Data Values (Member Info, Values)
	CString tmp;

	// Add Digitizer ObjectID...
	CString guidStr;
	SV_GUID_TO_AFXSTRING( digitizerObjectID.ToGUID(), guidStr );
	tmp.Format( "STRING [ %s ]", guidStr );
	script += strIndent + objectTag + _T( ".DigitizerID" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	script.FreeExtra();

	RStrScript += script;
}
#endif

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

SVImageIndexStruct SVCameraImageTemplate::GetSourceImageIndex( SVProductInfoStruct* pProduct )
{
	SVImageIndexStruct svIndex;

	if ( mpCamera != NULL && pProduct != NULL )
	{
		 mpCamera->GetSourceImageIndex( svIndex.m_CameraDMIndexHandle, *pProduct );
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

	if( hr == S_OK )
	{
		mpCamera->GetImageInfo( &m_ImageInfo );

		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		if ( ResetObject() != S_OK )
		{
			hr = S_FALSE;
		}
	}

	return hr;
}

BOOL SVCameraImageTemplate::GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const
{
	BOOL bOk = TRUE;

	rsvIndex = m_CurrentIndex;

	return bOk;
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

	if ( mpCamera != NULL && mpCamera->mpsvDevice != NULL )
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

	if( ( nullptr == pCamera ) || ( nullptr != pOwner && pOwner->GetObjectType() == SVToolSetObjectType ) )
	{
		SVInspectionProcess* pInspection  = GetInspection();

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

			SVInspectionProcess* pInspection = GetInspection();

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

