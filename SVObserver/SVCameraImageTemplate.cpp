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

	SVObjectCStringArrayClass svCStringArray;

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

	SVVirtualCamera* l_pCamera = NULL;
	SVObjectClass* l_pOwner = GetOwner();

	if( !( digitizerObjectID.empty() ) )
	{
		l_pCamera = dynamic_cast< SVVirtualCamera* >( SVObjectManagerClass::Instance().GetObject( digitizerObjectID.ToGUID() ) );
	}

	if( ( l_pCamera == NULL ) || ( l_pOwner != NULL && l_pOwner->GetObjectType() == SVToolSetObjectType ) )
	{
		SVInspectionProcess* l_pInspect = GetInspection();

		if( l_pInspect != NULL )
		{
			SVVirtualCamera* l_pTempCamera = l_pInspect->GetFirstCamera();

			if( l_pTempCamera != NULL )
			{
				l_pCamera = l_pTempCamera;
			}
		}
	}

	if( l_pCamera != NULL )
	{
		if( mpCamera != l_pCamera )
		{
			SVImageObjectClassPtr l_ArrayPtr;

			if( l_pCamera->mpsvDevice != NULL )
			{
				l_ArrayPtr = l_pCamera->mpsvDevice->GetCircleBuffer();
			}

			if( !( l_ArrayPtr.empty() ) && ( 0 < l_ArrayPtr->size() ) )
			{
				SVImageInfoClass CameraImageInfo;

				l_pCamera->GetImageInfo( &CameraImageInfo );

				if( CreateBuffers( CameraImageInfo, l_ArrayPtr ) )
				{
					mpCamera = l_pCamera;
					digitizerObjectID = mpCamera->GetUniqueObjectID();
				}
			}

			SVInspectionProcess* l_pInspect = GetInspection();

			if( l_pInspect != NULL )
			{
				l_pInspect->AddSharedCamera( mpCamera );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCameraImageTemplate.cpp_v  $
 * 
 *    Rev 1.2   01 Oct 2013 12:16:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Apr 2013 14:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:49:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   05 Apr 2013 12:26:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method that uses inspection method with changed return type from BOOL to HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Feb 2013 13:03:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the attributes of all main images to be remotely setable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jan 2013 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Sep 2012 14:46:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Oct 2011 18:17:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with camera re-connect when a camera was deleted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Oct 2011 15:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with error condition used to initialize image information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Oct 2011 10:39:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated camera selection process to force a re-check of the carmera assiciated with the camera image object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Oct 2011 08:59:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Camera Image Object to fix problem when tool set main image was attaching to inspection selected camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Sep 2011 12:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Apr 2011 14:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
