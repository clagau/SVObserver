//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImage
//* .File Name       : $Workfile:   SVImageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   26 Jun 2014 17:41:18  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageClass.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVGlobal.h"
#include "SVImageArithmetic.h"
#include "SVImageBuffer.h"
#include "SVImageObjectClass.h"
#include "SVImagePolarTransform.h"
#include "SVImageProcessingClass.h"
#include "SVImageToLineProject.h"
#include "SVInspectionProcess.h"
#include "SVLineAnalyzer.h"
#include "SVLineROI.h"
#include "SVObserver.h"
#include "SVPolarTransformationTool.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVImageLibrary\MatroxImageData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVImageClass, SVImageClassGuid );

SVImageClass::SVImageClass( LPCSTR ObjectName )
: SVObjectAppClass( ObjectName )
{
	init();
}

SVImageClass::SVImageClass( SVObjectClass* POwner, int StringResourceID )
: SVObjectAppClass( POwner, StringResourceID )
{
	init();
}

BOOL SVImageClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	BOOL l_bOk = SVObjectAppClass::CreateObject(PCreateStruct);

	RegisterAsSubObject();

	l_bOk &= ( UpdateFromToolInformation() == S_OK );
	l_bOk &= ( UpdateBufferArrays() == S_OK );

	ObjectAttributesAllowedRef() |= SV_PUBLISH_RESULT_IMAGE | SV_CH_IMAGE | SV_DD_IMAGE;	// add this on older configs
	
	isCreated = l_bOk;
	
	return l_bOk;	
}

BOOL SVImageClass::CloseObject()
{
	BOOL rc = TRUE;
	
	rc = ( ClearParentConnection() == S_OK );

	if( GetTool() != NULL )
	{
		GetTool()->SetToolImage( NULL );
	}

	if ( isCreated )
	{
		rc = DestroyImage();
	}
	
	UnregisterAsSubObject();

	rc = SVObjectAppClass::CloseObject() && rc;
	
	return rc;
}

SVImageTypeEnum SVImageClass::GetImageType() const
{
	return m_ImageType;
}

/*
This method now verifies the Parent Image pointer attribute.
If the attribute is invalid, the Parent Image pointer and the Parent Index are both cleared.
The Parent Image attribute should not be used unless it is validated first.
*/
SVImageClass *SVImageClass::GetParentImage() const
{
	SVImageClass* l_pParent = NULL;
		
	if( ! m_ParentImageInfo.first.empty() )
	{
		if( SVObjectManagerClass::Instance().GetState() == SVObjectManagerClass::ReadOnly )
		{
			if( m_ParentImageInfo.second == NULL )
			{
				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first );

				if( l_pObject != NULL )
				{
					m_ParentImageInfo.second = dynamic_cast< SVImageClass* >( l_pObject );
				}
			}

			if( m_ParentImageInfo.second != NULL )
			{
				l_pParent = m_ParentImageInfo.second;
			}
		}
		else
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first );

			if( l_pObject != NULL )
			{
				l_pParent = dynamic_cast< SVImageClass* >( l_pObject );
			}

			m_ParentImageInfo.second = NULL;
		}
	}

	return l_pParent;
}

const GUID& SVImageClass::GetParentImageID() const
{
	return m_ParentImageInfo.first.ToGUID();
}

void SVImageClass::init()
{
	try
	{
		::InitializeCriticalSection( &m_hCriticalSection );

		m_bCriticalSectionCreated = true;
	}
	catch ( ... )
	{
		m_bCriticalSectionCreated = false;
	}

	m_ImageType = SVImageTypeUnknown;

	m_BufferArrayPtr = new SVImageObjectClass;
	
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageObjectType;
	
	// derived classes that are not result images (i.e. SVMainImageClass)
	// need to remove the PUBLISH attribute.
	ObjectAttributesAllowedRef() = SV_ARCHIVABLE_IMAGE | SV_PUBLISH_RESULT_IMAGE | SV_CH_IMAGE | SV_DD_IMAGE;

	m_ParentImageInfo.second = NULL;

	m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );
		
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointX, SV_DEFAULT_WINDOWTOOL_LEFT );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointY, SV_DEFAULT_WINDOWTOOL_TOP );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, SV_DEFAULT_WINDOWTOOL_WIDTH );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, SV_DEFAULT_WINDOWTOOL_HEIGHT );
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
SVImageClass::~SVImageClass()
{
	CloseObject();

	ClearParentConnection();

	RemoveChildren();

	m_BufferArrayPtr.clear();

	if ( m_bCriticalSectionCreated )
	{
		::DeleteCriticalSection( &m_hCriticalSection );

		m_bCriticalSectionCreated = false;
	}
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
BOOL SVImageClass::DestroyImage()
{
	BOOL bOk = TRUE;
	
	if ( isCreated )
	{
		bOk = Lock();

		ASSERT( bOk );

		if ( bOk )
		{
			bOk = ( ClearParentConnection() == S_OK );

			ASSERT( bOk );

			bOk = RemoveChildren() == S_OK && bOk;

			ASSERT( bOk );

			if( !( m_BufferArrayPtr.empty() ) )
			{
				m_BufferArrayPtr->clear();
			}

			m_ImageType = SVImageTypeUnknown;

			isCreated = FALSE;

			bOk = Unlock() && bOk;

			ASSERT( bOk );
		}

		if ( ! bOk )
		{
			AfxMessageBox( "Failed to destroy the image object!", MB_ICONEXCLAMATION );
		}
	}

	return bOk;
}

HRESULT SVImageClass::InitializeImage( SVImageTypeEnum p_ImageType )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType )
	{
		m_ImageType = p_ImageType;

		m_LastUpdate = SVClock::GetTimeStamp();
	}

	return l_Status;
}

HRESULT SVImageClass::InitializeImage( SVImageClass* p_pParentImage )
{
	HRESULT l_Status = S_OK;

	SVGUID l_ImageID;

	if( p_pParentImage != NULL )
	{
		l_ImageID = p_pParentImage->GetUniqueObjectID();
	}

	l_Status = InitializeImage( l_ImageID );

	return l_Status;
}

HRESULT SVImageClass::InitializeImage( const GUID& p_rParentID )
{
	HRESULT l_Status = S_OK;

	if( m_ParentImageInfo.first != p_rParentID )
	{
		ClearParentConnection();

		m_ParentImageInfo.first = p_rParentID;
		m_ParentImageInfo.second = NULL;

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = UpdateFromParentInformation();
	}

	return l_Status;
}

HRESULT SVImageClass::InitializeImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ParentImageInfo.first != p_rParentID || m_ImageInfo != p_rImageInfo )
	{
		if( m_ParentImageInfo.first != p_rParentID )
		{
			ClearParentConnection();

			m_ParentImageInfo.first = p_rParentID;
			m_ParentImageInfo.second = NULL;
		}

		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = UpdateFromParentInformation();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( const SVImageExtentClass& p_rExtent, bool p_ExcludePositionCheck )
{
	HRESULT l_Status = S_OK;

	if( m_ImageInfo.GetExtents() != p_rExtent )
	{
		HRESULT l_Temp = m_ImageInfo.SetExtents( p_rExtent );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Temp = RebuildStorage( p_ExcludePositionCheck );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ImageInfo != p_rImageInfo )
	{
		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( const GUID& p_rParentID )
{
	HRESULT l_Status = S_OK;

	if( m_ParentImageInfo.first != p_rParentID )
	{
		ClearParentConnection();

		m_ParentImageInfo.first = p_rParentID;
		m_ParentImageInfo.second = NULL;

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( const GUID& p_rParentID, const SVImageExtentClass& p_rExtent )
{
	HRESULT l_Status = S_OK;

	if( m_ParentImageInfo.first != p_rParentID || m_ImageInfo.GetExtents() != p_rExtent )
	{
		if( m_ParentImageInfo.first != p_rParentID )
		{
			ClearParentConnection();

			m_ParentImageInfo.first = p_rParentID;
			m_ParentImageInfo.second = NULL;
		}

		HRESULT l_Temp = m_ImageInfo.SetExtents( p_rExtent );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Temp = ResetObject();

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ParentImageInfo.first != p_rParentID || m_ImageInfo != p_rImageInfo )
	{
		if( m_ParentImageInfo.first != p_rParentID )
		{
			ClearParentConnection();

			m_ParentImageInfo.first = p_rParentID;
			m_ParentImageInfo.second = NULL;
		}

		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SVImageTypeEnum p_ImageType )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType )
	{
		m_ImageType = p_ImageType;

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SVImageTypeEnum p_ImageType, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType || m_ImageInfo != p_rImageInfo )
	{
		m_ImageType = p_ImageType;

		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType || m_ParentImageInfo.first != p_rParentID )
	{
		m_ImageType = p_ImageType;

		if( m_ParentImageInfo.first != p_rParentID )
		{
			ClearParentConnection();

			m_ParentImageInfo.first = p_rParentID;
			m_ParentImageInfo.second = NULL;
		}

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SVImageTypeEnum p_ImageType, const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType || m_ParentImageInfo.first != p_rParentID || m_ImageInfo != p_rImageInfo )
	{
		m_ImageType = p_ImageType;

		if( m_ParentImageInfo.first != p_rParentID )
		{
			ClearParentConnection();

			m_ParentImageInfo.first = p_rParentID;
			m_ParentImageInfo.second = NULL;
		}

		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Status = ResetObject();
	}

	return l_Status;
}

BOOL SVImageClass::SetImageDepth( long lDepth )
{
	BOOL l_bOk = true;

	if( lDepth != GetImageDepth() )
	{
		l_bOk = SVObjectAppClass::SetImageDepth( lDepth );

		m_LastUpdate = SVClock::GetTimeStamp();

		l_bOk &= ( ResetObject() == S_OK );
	}

	return l_bOk;
}

const SVClock::SVTimeStamp& SVImageClass::GetLastResetTimeStamp() const
{
	return m_LastReset;
}
	
HRESULT SVImageClass::ResetObject()
{
	return RebuildStorage( false );
}

HRESULT SVImageClass::RebuildStorage( bool p_ExcludePositionCheck )
{
	HRESULT l_Status = UpdateFromParentInformation();

	if( m_LastReset <= m_LastUpdate )
	{
		HRESULT l_Temp = SVObjectAppClass::ResetObject();

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		l_Temp = UpdatePosition();

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		l_Temp = UpdateBufferArrays( p_ExcludePositionCheck );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		l_Temp = UpdateChildren();

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	if( l_Status == S_OK )
	{
		m_LastReset = SVClock::GetTimeStamp();
	}

	return l_Status;
}

SVImageExtentClass SVImageClass::GetImageExtents()
{
	return m_ImageInfo.GetExtents();
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_ImageInfo.GetImageExtentsToFit( p_svInExtent, p_rsvOutExtent );
	
	return l_hrOk;
}

HRESULT SVImageClass::UpdateFromParentInformation()
{
	HRESULT l_Status = S_OK;

	if( ( m_ImageType != SVImageTypeMain ) && ( m_ImageType != SVImageTypeRGBMain ) )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && ( m_LastReset < l_pParentImage->GetLastResetTimeStamp() || m_LastReset < m_LastUpdate ) )
		{
			SVImageExtentClass l_ImageExtent = GetImageExtents();
			SVImagePropertiesClass l_ImageProperties = m_ImageInfo.GetImageProperties();
			SVImageInfoClass l_ImageInfo = l_pParentImage->GetImageInfo();

			l_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

			l_Status = l_ImageInfo.SetImageProperties( l_ImageProperties );

			if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
			{
				l_ImageExtent = l_ImageInfo.GetExtents();

				SVExtentPositionClass l_Position = l_ImageExtent.GetPosition();

				l_Status = l_Position.SetExtentProperty( SVExtentPropertyPositionPoint, 0.0 );

				if( l_Status == S_OK )
				{
					l_Status = l_ImageExtent.SetPosition( l_Position );
				}
			}

			if( l_Status == S_OK )
			{
				l_Status = l_ImageInfo.SetExtents( l_ImageExtent );
			}
	
			if( l_Status == S_OK )
			{
				m_ImageInfo = l_ImageInfo;

				m_LastUpdate = SVClock::GetTimeStamp();

				l_Status = UpdateFromToolInformation();
			}
		}
		else
		{
			l_Status = UpdateFromToolInformation();
		}
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateFromToolInformation()
{
	HRESULT l_Status = S_OK;

	SVGUID l_ToolID;
	SVImageExtentClass l_ToolExtent = m_ImageInfo.GetExtents();

	if( ( m_ImageType != SVImageTypeRGBMain ) && ( GetTool() != NULL ) )
	{
		SVImageExtentClass l_TempExtent;

		if( ( m_ImageType != SVImageTypeMain ) && 
			( m_ImageType != SVImageTypeFixed ) && 
			( m_ImageType != SVImageTypeIndependent ) && 
			( m_ImageType != SVImageTypeDependent ) && 
			( m_ImageType != SVImageTypeVirtual ) && 
			( GetTool()->GetImageExtent( l_TempExtent ) == S_OK ) )
		{
			RECT l_Rect;

			if( l_TempExtent.GetOutputRectangle( l_Rect ) == S_OK )
			{
				if(	0 < ( l_Rect.bottom - l_Rect.top + 1 ) && 0 < ( l_Rect.right - l_Rect.left + 1 ) )
				{
					l_ToolExtent = l_TempExtent;
				}
			}
		}

		if( m_ImageType != SVImageTypeFixed && m_ImageType != SVImageTypeIndependent && m_ImageType != SVImageTypeDependent )
		{
			GetTool()->SetToolImage( this );
		}

		l_ToolID = GetTool()->GetUniqueObjectID(); 
	}

	if( m_ImageInfo.GetOwnerID() != l_ToolID )
	{
		// Set the owning Tool before setting the extents
		m_ImageInfo.SetOwner( l_ToolID );

		m_LastUpdate = SVClock::GetTimeStamp();
	}

	if( ( m_ImageType != SVImageTypeMain ) && ( m_ImageType != SVImageTypeIndependent ) )
	{
		l_Status = m_ImageInfo.SetExtents( l_ToolExtent );

		if( l_Status == S_OK )
		{
			m_LastUpdate = SVClock::GetTimeStamp();
		}
	}

	return l_Status;
}

HRESULT SVImageClass::ClearParentConnection()
{
	HRESULT l_hrOk = S_OK;

	if( ! m_ParentImageInfo.first.empty() )
	{
		l_hrOk = SVObjectManagerClass::Instance().DisconnectObjects( m_ParentImageInfo.first, GetUniqueObjectID() );
	}

	m_LastUpdate = SVClock::GetTimeStamp();

	return l_hrOk;
}

HRESULT SVImageClass::IsValidChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo ) const
{
	HRESULT l_hrOk = S_OK;

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		ASSERT( l_pParentImage != NULL );

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			SVImageInfoClass l_svImageInfo = p_svImageInfo;
			SVImagePropertiesClass l_svImageProperties = m_ImageInfo.GetImageProperties();

			l_svImageInfo.SetImageProperties( l_svImageProperties );

			l_hrOk = l_pParentImage->IsValidChild( p_rChildID, l_svImageInfo );
		}
		else
		{
			l_hrOk = E_FAIL;
		}
	}
	else
	{
		SVGuidImageChildMap::const_iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			bool l_Status = false;
			
			l_Status = l_Status || ( l_Iter->second.m_pImageHandles.empty() );
			l_Status = l_Status || ( l_Iter->second.m_pImageHandles->empty() );
			l_Status = l_Status || ( l_Iter->second.m_ImageInfo != p_svImageInfo );

			if( l_Status )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::UpdateChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo )
{
	HRESULT l_hrOk = S_OK;

	if ( Lock() )
	{
		if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
		{
			SVImageClass* l_pParentImage = GetParentImage();

			ASSERT( l_pParentImage != NULL );

			if( l_pParentImage != NULL && l_pParentImage != this )
			{
				SVImageInfoClass l_svImageInfo = p_svImageInfo;
				SVImagePropertiesClass l_svImageProperties = m_ImageInfo.GetImageProperties();

				l_svImageInfo.SetImageProperties( l_svImageProperties );

				l_hrOk = l_pParentImage->UpdateChild( p_rChildID, l_svImageInfo );
			}
			else
			{
				l_hrOk = E_FAIL;
			}
		}
		else
		{
			SVImageChildStruct& l_rChild = m_ChildArrays[ p_rChildID ];

			l_rChild.m_ImageInfo = p_svImageInfo;

			if( l_rChild.m_pImageHandles.empty() )
			{
				l_rChild.m_pImageHandles = new SVImageObjectClass;
			}

			if( !( l_rChild.m_pImageHandles.empty() ) )
			{
				l_hrOk = UpdateChildBuffers( l_rChild.m_pImageHandles, l_rChild.m_ImageInfo );
			}
			else
			{
				l_hrOk = E_FAIL;
			}
		}

		if( !Unlock() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::RemoveChild( const GUID& p_rChildID )
{
	HRESULT l_hrOk = S_OK;

	if( Lock() )
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			SVImageClass* l_pImage = NULL;

			l_Iter->second.m_ImageInfo.GetOwnerImage( l_pImage );

			if( l_pImage != NULL )
			{
				l_hrOk = l_pImage->RemoveObjectConnection( GetUniqueObjectID() );
			}

			if( l_Iter->second.m_pImageHandles != NULL )
			{
				l_Iter->second.m_pImageHandles.clear();
				
				l_Iter->second.m_pImageHandles = NULL;
			}

			l_Iter->second.m_ImageInfo.Initialize();

			m_ChildArrays.erase( l_Iter );
		}

		m_ParentImageInfo.second = NULL;

		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->RemoveChild( p_rChildID );
		}

		if( !Unlock() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetChildImageInfo( const GUID& p_rChildID, SVImageInfoClass& p_rImageInfo ) const
{
	HRESULT l_hrOk = S_FALSE;

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageInfo( p_rChildID, p_rImageInfo );
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		SVGuidImageChildMap::const_iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			if( l_Iter->second.m_pImageHandles != NULL )
			{
				p_rImageInfo = l_Iter->second.m_pImageHandles->GetImageInfo();

				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetChildImageHandle( const GUID& p_rChildID, SVSmartHandlePointer& p_rsvBufferHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_rsvBufferHandle );
			if ( l_hrOk == S_FALSE )
			{
				CString sMsgStr;
				sMsgStr.Format("**GetChildImageHandle** l_hrOk = l_pParentImage->GetChildImageHandle( m_svChildIndexArray[ p_lChildIndex ], p_rsvBufferHandle ) == S_FALSE");

				SVException svE;
				SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
				svE.LogException(sMsgStr);
			}
		}
		else
		{
				CString sMsgStr;
				sMsgStr.Format("**GetChildImageHandle** l_pParentImage == NULL");

				SVException svE;
				SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
				svE.LogException(sMsgStr);

			l_hrOk = S_FALSE;
		}
	}
	else
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			if( l_Iter->second.m_pImageHandles != NULL )
			{
				if( l_Iter->second.m_pImageHandles->GetImageHandle( p_rsvBufferHandle ) )
				{
					l_hrOk = S_OK;
				}
				else
				{
					CString sMsgStr;
					sMsgStr.Format("**GetChildImageHandle** if ( m_svChildBufferArrays[ p_lChildIndex ]->GetValue( p_rsvBufferHandle ) )");

					SVException svE;
					SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
					svE.LogException(sMsgStr);
				}

			}
		}
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetChildImageHandle( const GUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer &p_rsvBufferHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_svBufferIndex, p_rsvBufferHandle );
			if ( l_hrOk == S_FALSE )
			{
				CString sMsgStr;
				sMsgStr.Format("**GetChildImageHandle 2 ** l_hrOk = l_pParentImage->GetChildImageHandle( m_svChildIndexArray[ p_lChildIndex ], p_rsvBufferHandle ) == S_FALSE");

				SVException svE;
				SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
				svE.LogException(sMsgStr);
			}

		}
		else
		{
				CString sMsgStr;
				sMsgStr.Format("**GetChildImageHandle 2 ** l_pParentImage == NULL");

				SVException svE;
				SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
				svE.LogException(sMsgStr);
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			if( l_Iter->second.m_pImageHandles != NULL )
			{
				SVDataManagerHandle l_Handle;

				GetImageIndex( l_Handle, p_svBufferIndex );

				if ( l_Iter->second.m_pImageHandles->GetImageHandle( l_Handle.GetIndex(), p_rsvBufferHandle ) )
				{
					l_hrOk = S_OK;
				}
				else
				{
					CString sMsgStr;
					sMsgStr.Format("**GetChildImageHandle** if ( m_svChildBufferArrays[ p_lChildIndex ]->GetValue( p_rsvBufferHandle ) )");

					SVException svE;
					SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
					svE.LogException(sMsgStr);
				}

			}
		}
	}

	return l_hrOk;
}


/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetParentImageHandle( SVSmartHandlePointer &p_rsvBufferHandle )
{
	HRESULT l_hrOk = S_FALSE;
	SVSmartHandlePointer imageStruct;

	SVImageClass* l_pParentImage = GetParentImage();

	if( l_pParentImage != NULL && l_pParentImage != this )
	{
		/*
		//what is the MIL handle for l_pParentImage
		l_pParentImage->GetImageHandle(imageStruct);

		if ( imageStruct.milImage.empty() == 0 )
		{
			CString sTmp;
			sTmp.Format( "***ERROR GETIMAGEHANDLE  ***l_pParentImage->GetImageHandle(imageStruct)" );

			SVException svE;
			SETEXCEPTION1(svE,SVMSG_SVO_42_CURRENT_INDEX,sTmp);
			svE.LogException(sTmp);
		}
		*/

		l_hrOk = l_pParentImage->GetChildImageHandle( GetUniqueObjectID(), p_rsvBufferHandle );

		if( l_hrOk != S_OK )
		{
			CString sMsgStr;
			sMsgStr.Format("ERROR: Cannot Get Default Child Image Handle");
			SVException svE;
			SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
			svE.LogException(sMsgStr);
		}

	}
	
	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::GetParentImageHandle( SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer &p_rsvBufferHandle )
{
	HRESULT l_hrOk = S_FALSE;

	SVImageClass* l_pParentImage = GetParentImage();

	if( l_pParentImage != NULL && l_pParentImage != this )
	{
		l_hrOk = l_pParentImage->GetChildImageHandle( GetUniqueObjectID(), p_svBufferIndex, p_rsvBufferHandle );

		if( l_hrOk != S_OK )
		{
			CString sMsgStr;
			sMsgStr.Format("ERROR: Cannot Get Indexed Child Image Handle");
			SVException svE;
			SETEXCEPTION1(svE,SVMSG_SVO_43_GENERAL,sMsgStr);
			svE.LogException(sMsgStr);
		}
	}

	return l_hrOk;
}
// JMS - New Image Object methods */

const SVImageInfoClass& SVImageClass::GetImageInfo() const
{
	return m_ImageInfo;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
BOOL SVImageClass::GetImageHandleIndex( SVImageIndexStruct& rsvIndex ) const
{
	BOOL bOk = FALSE;
		
	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			bOk = l_pParentImage->GetImageHandleIndex( rsvIndex );
		}
		else
		{
			rsvIndex.clear();
		}
	}
	else
	{
		bOk = !( m_BufferArrayPtr.empty() );
		
		rsvIndex.clear();
		
		if ( bOk )
		{
			SVDataManagerHandle l_DMIndexHandle;
			bOk = m_BufferArrayPtr->GetCurrentIndex( l_DMIndexHandle );
			bool bIsPublished = (ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE) > 0;
			if ( bIsPublished )
				rsvIndex.m_PublishedResultDMIndexHandle.Assign( l_DMIndexHandle, l_DMIndexHandle.GetLockType() );
			else
				rsvIndex.m_ResultDMIndexHandle.Assign( l_DMIndexHandle, l_DMIndexHandle.GetLockType() );
		}
	}
	
	return bOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
BOOL SVImageClass::SetImageHandleIndex( SVImageIndexStruct svIndex )
{
	BOOL bOk = FALSE;
		
	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		bOk = GetParentImage() != NULL && GetParentImage() != this;
	}
	else
	{
		bOk = !( m_BufferArrayPtr.empty() );
		
		SVDataManagerHandle l_Handle;

		GetImageIndex( l_Handle, svIndex );

		if ( bOk )
		{
			bOk = m_BufferArrayPtr->SetCurrentIndex( l_Handle );
		}

		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( !( l_Iter->second.m_pImageHandles.empty() ) && !( l_Iter->second.m_pImageHandles->empty() ) )
			{
				bOk &= l_Iter->second.m_pImageHandles->SetCurrentIndex( l_Handle );
			}

			++l_Iter;
		}
	}
	
	return bOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
BOOL SVImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	BOOL bOk = FALSE;
		
	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL && l_pParentImage != this )
		{
			SVImageIndexStruct l_svIndex;

			bOk = l_pParentImage->GetImageHandleIndex( l_svIndex );

			bOk = bOk && (l_svIndex == svIndex);
		}
	}
	else
	{
		bOk = !( m_BufferArrayPtr.empty() );
		
		SVDataManagerHandle l_Handle;

		GetImageIndex( l_Handle, svIndex );

		if ( bOk )
		{
			bOk = m_BufferArrayPtr->CopyValue( l_Handle );
		}

		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( l_Iter->second.m_pImageHandles != NULL )
			{
				bOk &= l_Iter->second.m_pImageHandles->CopyValue( l_Handle );
			}

			++l_Iter;
		}
	}
	
	return bOk;
}

BOOL SVImageClass::GetImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = FALSE;
		
	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		bOk = ( GetParentImageHandle( p_rHandlePtr ) == S_OK );
	}
	else
	{
		bOk = !( m_BufferArrayPtr.empty() );
		
		if ( bOk )
		{
			bOk = m_BufferArrayPtr->GetImageHandle( p_rHandlePtr );
		}
	}
	
	return bOk;
}

BOOL SVImageClass::GetImageHandle( SVImageIndexStruct svIndex, SVSmartHandlePointer& rHandle )
{
	BOOL bOk = FALSE;
		
	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		bOk = ( GetParentImageHandle( svIndex, rHandle ) == S_OK );
	}
	else
	{
		bOk = !( m_BufferArrayPtr.empty() );
	
		if ( bOk )
		{
			SVDataManagerHandle l_Handle;

			GetImageIndex( l_Handle, svIndex );

			bOk = m_BufferArrayPtr->GetImageHandle( l_Handle.GetIndex(), rHandle );
		}
	}
	
	return bOk;
}

BOOL SVImageClass::SafeImageCopyToHandle( SVSmartHandlePointer &p_rHandle )
{
	BOOL l_bOk = !p_rHandle.empty();

	if ( l_bOk )
	{
		if ( Lock() )
		{
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer() );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyToHandle( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( p_svFromIndex, l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer() );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyToBSTR( SVImageIndexStruct p_svFromIndex, BSTR &p_rbstrData )
{
	BOOL l_bOk = FALSE;
	
	if ( Lock() )
	{
		BITMAPINFO* pbmInfo = NULL;

		SVSmartHandlePointer l_svOrigHandle;
		SVSmartHandlePointer l_svCopyHandle;

		SVSmartHandlePointer l_psvHandle;

		l_bOk = GetImageHandle( p_svFromIndex, l_svOrigHandle ) &&
			!( l_svOrigHandle.empty() ) && l_svOrigHandle->GetBufferAddress() != NULL;

		if( l_bOk )
		{
			l_psvHandle = l_svOrigHandle;
		}
		else
		{
			l_bOk = SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, l_svCopyHandle ) == S_OK;

			l_bOk = l_bOk && SafeImageCopyToHandle( p_svFromIndex, l_svCopyHandle );
			l_bOk = l_bOk && !( l_svCopyHandle.empty() ) && l_svCopyHandle->GetBufferAddress() != NULL;

			if( l_bOk )
			{
				l_psvHandle = l_svCopyHandle;
			}
		}

		if ( l_bOk )
		{
			SVBitmapInfo l_BitmapInfo = l_psvHandle->GetBitmapInfo();
			const BITMAPINFO* pbmInfo = l_BitmapInfo.GetBitmapInfo();
			const BITMAPINFOHEADER* pbmhInfo = (LPBITMAPINFOHEADER) &pbmInfo->bmiHeader;

			// Source images seem to be flipped even though MIL is not supposed to flip them
			if( pbmhInfo->biHeight > 0 )
			{
				l_BitmapInfo.FlipHeight();
			}
			
			long l_lBitmapHeaderSize = sizeof( BITMAPINFOHEADER );
			long l_lColorTableSize = 0;
			long l_lDIBSize = pbmhInfo->biSizeImage;

			if( 0 < pbmhInfo->biClrUsed )
			{
				if( pbmhInfo->biBitCount < 16 )
				{
					l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );
				}
				else
				{
					l_lColorTableSize = pbmhInfo->biClrUsed * sizeof( RGBQUAD );

					if( pbmhInfo->biBitCount == 16 || pbmhInfo->biBitCount == 32 )
					{
						l_lColorTableSize += 3 * sizeof( DWORD );
					}
				}
			}

			// Calculate total size buffer needed for image
			long lBufSize = 0;
			
			lBufSize += l_lBitmapHeaderSize;
			lBufSize += l_lColorTableSize;
			lBufSize += l_lDIBSize;
			
			if( p_rbstrData != NULL )
			{
				::SysFreeString( p_rbstrData );

				p_rbstrData = NULL;
			}

			p_rbstrData = ::SysAllocStringByteLen(NULL, lBufSize);

			unsigned char *l_pucStart = (unsigned char *)(p_rbstrData);
			BITMAPINFO *l_pbmiInfo = (BITMAPINFO *)(p_rbstrData);

			memcpy( l_pucStart, pbmhInfo, l_lBitmapHeaderSize );

			l_pucStart += l_lBitmapHeaderSize;

			if( 0 < l_lColorTableSize )
			{
				memcpy( l_pucStart, pbmInfo->bmiColors, l_lColorTableSize );

				l_pucStart += l_lColorTableSize;
			}

			memcpy( l_pucStart, l_psvHandle->GetBufferAddress(), l_lDIBSize );
		}

		l_svCopyHandle.clear();

		l_bOk = Unlock() && l_bOk;
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyColorToHandle( SVSmartHandlePointer &p_rHandle, long p_lBandNumber )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_lBandNumber );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyColorToHandle( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, long p_lBandNumber )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( p_svFromIndex, l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_lBandNumber );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyClipToHandle( SVSmartHandlePointer &p_rHandle, long p_lLeftOffset, long p_lTopOffset )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_lLeftOffset, p_lTopOffset );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyClipToHandle( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, long p_lLeftOffset, long p_lTopOffset )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
			
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( p_svFromIndex, l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_lLeftOffset, p_lTopOffset );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageConvertToHandle( SVSmartHandlePointer &p_rHandle, SVImageOperationTypeEnum p_eConversionType )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
						
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxImageInterface::Convert( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_eConversionType );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk && l_Code == SVMEE_STATUS_OK;
		}
	}
	
	return l_bOk;
}

BOOL SVImageClass::SafeImageConvertToHandle( SVImageIndexStruct p_svFromIndex, SVSmartHandlePointer& p_rHandle, SVImageOperationTypeEnum p_eConversionType )
{
	BOOL l_bOk = !p_rHandle.empty();
	
	if ( l_bOk )
	{
		if ( Lock() )
		{
			SVImageBufferHandleImage l_ToMilHandle;
			p_rHandle->GetData( l_ToMilHandle );
			
			SVMatroxImageInterface::SVStatusCode l_Code;

			SVSmartHandlePointer l_svHandle;

			l_bOk = GetImageHandle( p_svFromIndex, l_svHandle ) && !( l_svHandle.empty() );

			if ( l_bOk )
			{
				SVImageBufferHandleImage l_FromMilHandle;
				l_svHandle->GetData( l_FromMilHandle );

				l_Code = SVMatroxImageInterface::Convert( l_ToMilHandle.GetBuffer(), l_FromMilHandle.GetBuffer(), p_eConversionType );
			}
			else
			{
				l_Code = SVMatroxBufferInterface::ClearBuffer( l_ToMilHandle.GetBuffer(), 0.0 );
			}

			l_bOk = Unlock() && l_bOk;
		}
	}
	
	return l_bOk;
}

HRESULT SVImageClass::LoadImageFullSize( LPCTSTR p_szFileName, SVImageExtentClass& p_rNewExtent )
{
	HRESULT l_hrOk = S_FALSE;

	if ( !( m_BufferArrayPtr.empty() ) )
	{
		if( Lock() )
		{
			l_hrOk = m_BufferArrayPtr->LoadImageFullSize( p_szFileName, p_rNewExtent );

			if( ! Unlock() )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVImageClass::LoadImage( LPCTSTR p_szFileName, SVImageIndexStruct p_svToIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( !( m_BufferArrayPtr.empty() ) )
	{
		if( Lock() )
		{
			SVDataManagerHandle l_Handle;

			GetImageIndex( l_Handle, p_svToIndex );

			l_hrOk = m_BufferArrayPtr->LoadImage( p_szFileName, l_Handle );

			if( ! Unlock() )
			{
				l_hrOk = S_FALSE;
			}

			BOOL bOk = TRUE;

			SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

			while( l_Iter != m_ChildArrays.end() )
			{
				if( l_Iter->second.m_pImageHandles != NULL )
				{
					SVDataManagerHandle l_Handle;

					GetImageIndex( l_Handle, p_svToIndex );

					bOk &= l_Iter->second.m_pImageHandles->SetCurrentIndex( l_Handle );
				}

				++l_Iter;
			}

			ASSERT( bOk );
			if ( l_hrOk == S_OK )
				l_hrOk = bOk ? S_OK : S_FALSE;

		}// end if( Lock() )
	}// end if ( m_BufferArrayPtr != NULL )

	return l_hrOk;
}

long SVImageClass::getPixelDepth()
{
	long pixelDepth;

	m_ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, pixelDepth);

	return pixelDepth;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
SVImageClass* SVImageClass::GetRootImage()
{
	SVImageClass* pRootImage = this;
	SVImageClass* pParentImage = GetParentImage();

	if( pParentImage != NULL && pParentImage != this )
	{
		pRootImage = pParentImage->GetRootImage();
	}

	return pRootImage;
}

CString SVImageClass::getDisplayedName() const
{
	const SVObjectTypeInfoStruct& rObjectTypeInfo = GetObjectInfo().ObjectTypeInfo;
	CString strName;
	switch( rObjectTypeInfo.SubType )
	{
	case SVRGBMainImageObjectType:	// RGBMain image - Not selectable
		break;

	case SVMainImageObjectType:	// Main image
		if( GetOwner() )
		{
			strName = GetOwner()->GetName();
			strName += _T( ".Image1" );
		}
		break;

	default:
		{
			SVImageInfoClass imageInfo = GetImageInfo();
			if( imageInfo.GetOwner() )
			{
				strName = GetCompleteObjectNameToObjectType( NULL, SVToolObjectType );
			}
			break;
		}// end default:
	}// end switch( rObjectTypeInfo.SubType )
	return strName;
}

DWORD_PTR SVImageClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	switch( dwPureMessageID )
	{
	case SVMSGID_GETFIRST_IMAGE_INFO:
		return reinterpret_cast<DWORD_PTR>( &m_ImageInfo );

	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}

	return DwResult | SVObjectAppClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
}

#ifdef _DEBUG
#include <map>
namespace
{
	std::map<DWORD, long> f_mapCritSec;
};
#endif

bool SVImageClass::Lock() const
{
	bool l_bOk = false;

	try
	{
		BOOL bSuccess = TRUE;
		::EnterCriticalSection( &m_hCriticalSection );

		if ( bSuccess )
		{
			l_bOk = true;
#ifdef _DEBUG
			DWORD dwThreadId = ::GetCurrentThreadId();
			f_mapCritSec[dwThreadId]++;
			if ( (HANDLE) dwThreadId == (HANDLE) m_hCriticalSection.OwningThread )
			{
				long lRecursionCount = f_mapCritSec[dwThreadId];
				//ASSERT( lRecursionCount == m_hCriticalSection.RecursionCount );
			}
#endif
		}
		else
		{
		}
	}
	catch ( ... )
	{
		l_bOk = false;
	}

	return l_bOk;
}

bool SVImageClass::Unlock() const 
{
	bool l_bOk = false;

#ifdef _DEBUG
	DWORD dwThreadId = ::GetCurrentThreadId();
	ASSERT( (HANDLE) dwThreadId == (HANDLE) m_hCriticalSection.OwningThread );
#endif

	try
	{
#ifdef _DEBUG
		if ( (HANDLE) dwThreadId == (HANDLE) m_hCriticalSection.OwningThread )
		{
			long lRecursionCount = f_mapCritSec[dwThreadId];
			//ASSERT( lRecursionCount == m_hCriticalSection.RecursionCount );
		}
		f_mapCritSec[dwThreadId]--;

		// BRW - In Windows 7, LockCount doesn't mean what it meant in Windows XP.
		// http://msdn.microsoft.com/en-us/library/windows/hardware/ff541979(v=vs.85).aspx
		//ASSERT( m_hCriticalSection.LockCount >= 0 );
		//ASSERT( m_hCriticalSection.RecursionCount > 0 );
#endif

		::LeaveCriticalSection( &m_hCriticalSection );

//		ASSERT(   ( m_hCriticalSection.LockCount == -1 && m_hCriticalSection.RecursionCount == 0 )
//			   || ( m_hCriticalSection.LockCount >= 0 && m_hCriticalSection.RecursionCount > 0 ) );

		l_bOk = true;
	}
	catch ( ... )
	{
		l_bOk = false;
		ASSERT( l_bOk == true );
	}

	ASSERT( l_bOk == true );


	return l_bOk;
}

HRESULT SVImageClass::RemoveObjectConnection( const GUID& p_rObjectID )
{
	HRESULT l_Status = SVObjectAppClass::RemoveObjectConnection( p_rObjectID );

	SVGUID l_ObjectID = p_rObjectID;

	if( m_ParentImageInfo.first == l_ObjectID )
	{
		m_ParentImageInfo.first.clear();
		m_ParentImageInfo.second = NULL;

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	else
	{
		l_Status = RemoveChild( p_rObjectID );
	}

	return l_Status;
}

SVImageObjectClassPtr SVImageClass::GetBufferArrayPtr() const
{
	SVImageObjectClassPtr l_ArrayPtr;

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( l_pParentImage != NULL )
		{
			l_ArrayPtr = l_pParentImage->GetBufferArrayPtr();
		}
	}
	else
	{
		l_ArrayPtr = m_BufferArrayPtr;
	}

	return l_ArrayPtr;
}


#ifdef USE_OBJECT_SCRIPT
////////////////////////////////////////////////////////////////////////////////
// .Title       : GetObjectScript
// -----------------------------------------------------------------------------
// .Description : Get parseable script of this object.
//				: This script is human readable and is used to make this 
//				: object persistent. Use TheObjectScriptParser parse function
//				: to build in runtime an object based on this script.
//				: Be careful, if this object already exists, it would be 
//              : overwritten.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:25.01.2000 RO			Added BandNumber and BandLink Attributes.
////////////////////////////////////////////////////////////////////////////////
void SVImageClass::GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent )
{
	CString script;

	// preallocate 4K
	script.GetBuffer(4096);  
	script.ReleaseBuffer(); 
	
	SVObjectAppClass::GetObjectScript( script, RStrAliasTable, Indent );

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

	long l_lValue = 0;

	// Add image pixel depth as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lValue );
	tmp.Format( "LONG %d", l_lValue );
	script += strIndent + objectTag + _T( ".PixelDepth" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	// Add Band Number as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lValue );
	tmp.Format( "LONG %d", l_lValue );
	script += strIndent + objectTag + _T( ".BandNumber" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	// Add Band Link as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyBandLink, l_lValue );
	tmp.Format( "LONG %d", l_lValue );
	script += strIndent + objectTag + _T( ".BandLink" ) + nameStrDelimiter + _T( " = " ) + tmp + _T( ";" );

	script.FreeExtra();

	RStrScript += script;
}
#endif

void SVImageClass::Persist( SVObjectWriter& rWriter )
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	SVObjectAppClass::Persist( rWriter );

	PersistImageAttributes(rWriter);

	rWriter.EndElement();
}

void SVImageClass::PersistImageAttributes( SVObjectWriter& rWriter )
{
	CString tmp;

	long l_lValue = 0;

	// Add image pixel depth as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lValue );
	_variant_t value(l_lValue);
	rWriter.WriteAttribute(scPixelDepthTag, value);

	// Add Band Number as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lValue );
	value = l_lValue;
	rWriter.WriteAttribute(scBandNumberTag, value);

	// Add Band Link as trivial members
	m_ImageInfo.GetImageProperty( SVImagePropertyBandLink, l_lValue );
	value = l_lValue;
	rWriter.WriteAttribute(scBandLinkTag, value);
}

HRESULT SVImageClass::GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "PixelDepth" ) )
	{
		long l_lValue = 0;
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		m_ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, l_lValue );

		l_TempVariant = l_lValue;

		l_TempVariant.Detach();
	}
	else if( p_rValueName == _T( "BandNumber" ) )
	{
		long l_lValue = 0;
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		m_ImageInfo.GetImageProperty( SVImagePropertyBandNumber, l_lValue );

		l_TempVariant = l_lValue;

		l_TempVariant.Detach();
	}
	else if( p_rValueName == _T( "BandLink" ) )
	{
		long l_lValue = 0;
		_variant_t l_TempVariant;

		l_TempVariant.Attach( p_rVariantValue );

		m_ImageInfo.GetImageProperty( SVImagePropertyBandLink, l_lValue );

		l_TempVariant = l_lValue;

		l_TempVariant.Detach();
	}
	else
	{
		hr = SVObjectAppClass::GetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVImageClass::SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT hr = S_OK;

	if( p_rValueName == _T( "PixelDepth" ) )
	{
		m_ImageInfo.SetImageProperty( SVImagePropertyPixelDepth, p_rVariantValue );

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	else if( p_rValueName == _T( "BandNumber" ) )
	{
		m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, p_rVariantValue );

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	else if( p_rValueName == _T( "BandLink" ) )
	{
		m_ImageInfo.SetImageProperty( SVImagePropertyBandLink, p_rVariantValue );

		m_LastUpdate = SVClock::GetTimeStamp();
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue( p_rValueName, p_rVariantValue );
	}

	return hr;
}

HRESULT SVImageClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	SvCl::SVObjectDoubleArrayClass svDoubleArray;
	SvCl::SVObjectLongArrayClass svLongArray;
	SVObjectSVPointArrayClass svPointArray;

	if ( ( bOk = PDataObject->GetAttributeData( "ExtentLeft", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldPositionPointX, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentTop", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldPositionPointY, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentWidth", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentHeight", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentLeft", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldAbsPositionPointX, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentTop", svDoubleArray	) ) )
	{
		for ( int i = 0; i < svDoubleArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldAbsPositionPointY, svDoubleArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentWidth", svDoubleArray	) ) )
	{
		//   This value is no longer scripted.
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentHeight", svDoubleArray	) ) )
	{
		//   This value is no longer scripted.
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "RotationAngle", svDoubleArray	) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldRotationAngle, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "RotationCenter", svPointArray	) ) )
	{
		for ( int i = 0; i < svPointArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldRotationPoint, SVExtentPointStruct(svPointArray[i]) );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "PixelDepth", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SVImagePropertyPixelDepth, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "BandNumber", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SVImagePropertyBandNumber, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "BandLink", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SVImagePropertyBandLink, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "Translation", svPointArray	) ) )
	{
		for ( int i = 0; i < svPointArray.GetSize(); i++ )
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyOldTranslationOffset, SVExtentPointStruct(svPointArray[i]) );
		}
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue( PDataObject );

		m_LastUpdate = SVClock::GetTimeStamp();

		return hr;
	}

	m_LastUpdate = SVClock::GetTimeStamp();

	// At some point this can go away,because it's not longer being scripted
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

SVImageIndexStruct SVImageClass::GetSourceImageIndex( SVProductInfoStruct* pProduct )
{
	SVImageIndexStruct svIndex;
	if ( ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE )
	{
		SVInspectionProcess* pInspection = GetInspection();
		ASSERT( nullptr != pInspection );
		if ( nullptr != pInspection )
		{
			SVDataManagerHandle& l_rHandle = pProduct->oPPQInfo.m_ResultImagePublishedDMIndexHandle;

			svIndex.m_PublishedResultDMIndexHandle.Assign( l_rHandle, l_rHandle.GetLockType() );
		}
	}
	return svIndex;
}

//* JMS - New Image Object methods

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::UpdatePosition()
{
	HRESULT l_Status = S_OK;
	
	m_ParentImageInfo.second = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first ) );

	if( m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeLogical || m_ImageType == SVImageTypeVirtual )
	{
		if( m_ParentImageInfo.second != NULL )
		{
			l_Status = m_ParentImageInfo.second->UpdateChild( GetUniqueObjectID(), m_ImageInfo );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::UpdateChildren()
{
	HRESULT l_hrOk = S_OK;

	if( Lock() )
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( UpdateChild( l_Iter->first, l_Iter->second.m_ImageInfo ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			++l_Iter;
		}

		if( !Unlock() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVImageClass::RemoveChildren()
{
	HRESULT l_hrOk = S_OK;

	if( Lock() )
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( RemoveChild( l_Iter->first ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}

			l_Iter = m_ChildArrays.begin();
		}

		if( !Unlock() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVImageClass::UpdateChildBuffers( SVImageObjectClassPtr p_psvChildBuffers, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( !( p_psvChildBuffers.empty() ) )
	{
		SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();
		size_t l_Size = 0;

		if( l_BufferPtr.empty() )
		{
			l_Size = GetImageDepth();

			if( ( ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE ) == SV_PUBLISH_RESULT_IMAGE )
			{
				l_Size = TheSVObserverApp.GetSourceImageDepth();
			}
		}
		else
		{
			l_Size = l_BufferPtr->size();
		}

		p_psvChildBuffers->resize( static_cast<unsigned long>(l_Size) );
		p_psvChildBuffers->SetParentImageObject( l_BufferPtr );
		p_psvChildBuffers->SetImageInfo( p_rImageInfo );

		l_Status = p_psvChildBuffers->ResetObject();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}
// JMS - New Image Object methods */

HRESULT SVImageClass::UpdateBufferArrays( bool p_ExcludePositionCheck )
{
	HRESULT l_Status = S_OK;
	
	if( m_ImageType == SVImageTypeUnknown || m_ImageType == SVImageTypeDependent || m_ImageType == SVImageTypeVirtual || m_ImageType == SVImageTypeMain )
	{
		m_BufferArrayPtr.clear();
	}
	else
	{
		if( m_BufferArrayPtr.empty() )
		{
			m_BufferArrayPtr = new SVImageObjectClass;
		}

		if( !( m_BufferArrayPtr.empty() ) )
		{
			size_t l_Size = GetImageDepth();

			if( ( ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE ) == SV_PUBLISH_RESULT_IMAGE )
			{
				l_Size = TheSVObserverApp.GetSourceImageDepth();
			}

			bool l_Reset = false;
			bool l_Update = false;

			l_Reset = l_Reset || ( l_Size != m_BufferArrayPtr->size() );
			l_Reset = l_Reset || !( m_BufferArrayPtr->GetParentImageObject().empty() );

			if( p_ExcludePositionCheck )
			{
				l_Reset = l_Reset || !( m_ImageInfo.IsEqualExcludePosition( m_BufferArrayPtr->GetImageInfo() ) );
			}
			else
			{
				l_Reset = l_Reset || ( m_ImageInfo != m_BufferArrayPtr->GetImageInfo() );
			}

			if( l_Reset || !( m_ImageInfo.IsPositionEqual( m_BufferArrayPtr->GetImageInfo() ) ) )
			{
				if( l_Reset )
				{
					m_BufferArrayPtr->resize( static_cast<unsigned long>(l_Size) );
					m_BufferArrayPtr->SetParentImageObject( SVImageObjectClassPtr() );
				}

				m_BufferArrayPtr->SetImageInfo( m_ImageInfo );

				if( l_Reset )
				{
					l_Status = m_BufferArrayPtr->ResetObject();
				}
				else
				{
					l_Status = m_BufferArrayPtr->UpdateTimeStamp();
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	
	return l_Status;
}

HRESULT SVImageClass::ValidateAgainstAllExtents( SVImageExtentClass& p_rsvExtent )
{
	HRESULT l_hrOk = ValidateAgainstParentExtents( p_rsvExtent );
	
	if ( l_hrOk == S_OK )
	{
		l_hrOk = ValidateAgainstChildrenExtents( p_rsvExtent );
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::ValidateAgainstParentExtents( SVImageExtentClass& p_rsvExtent )
{
	HRESULT l_hrOk = S_OK;

	SVImageClass* l_pParentImage = GetParentImage();

	if( l_pParentImage != NULL && l_pParentImage != this )
	{
		l_hrOk = l_pParentImage->ValidateAgainstOutputExtents( p_rsvExtent );
	}
	
	return l_hrOk;
}

HRESULT SVImageClass::ValidateAgainstOutputExtents( const SVImageExtentClass& p_rsvExtent )
{
	return m_ImageInfo.ValidateAgainstOutputSpace( p_rsvExtent );
}

HRESULT SVImageClass::ValidateAgainstChildrenExtents( SVImageExtentClass& p_rsvExtent )
{
	HRESULT l_hrOk = S_OK;

	if( Lock() )
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if ( p_rsvExtent.ValidateAgainstOutputSpace( l_Iter->second.m_ImageInfo.GetExtents() ) != S_OK )
			{
				SVImageClass* l_pImage = NULL;

				l_Iter->second.m_ImageInfo.GetOwnerImage( l_pImage );

				if( l_pImage == NULL || m_ImageType != SVImageTypeDependent || l_pImage->GetImageType() != SVImageTypeVirtual )
				{
					l_hrOk = S_FALSE;
				}
			}

			++l_Iter;
		}

		if( !Unlock() )
		{
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
HRESULT SVImageClass::TranslateFromOutputSpaceToImage(SVImageClass* p_pImage, SVExtentPointStruct p_InPt, SVExtentPointStruct& p_OutPt)
{
	HRESULT l_hr = S_FALSE;

	p_OutPt.Initialize();

	if( p_pImage != NULL )
	{
		SVImageClass* l_pImage = this;
		SVImageExtentClass l_Extents;
		SVExtentPointStruct l_svPoint;
		
		do
		{
			l_Extents = l_pImage->GetImageExtents();
			l_Extents.TranslateFromOutputSpace( p_InPt, p_InPt );

			l_pImage = l_pImage->GetParentImage();
		}
		while( p_pImage != l_pImage && l_pImage != this && l_pImage) ;
		
		if( p_pImage == l_pImage )
		{
			p_OutPt = p_InPt;

			l_hr = S_OK;
		}
	}
	return l_hr;
}

void SVImageClass::GetChildExtents( SVChildExtentDeque& p_rChildExtents ) const
{
	SVGuidImageChildMap::const_iterator l_Iter = m_ChildArrays.begin();

	while( l_Iter != m_ChildArrays.end() )
	{
		SVChildExtentPair l_Data( l_Iter->first, l_Iter->second.m_ImageInfo.GetExtents() );

		p_rChildExtents.push_back( l_Data );

		++l_Iter;
	}
}

#pragma region virtual method (ISVImage)
SvOi::ISVImage* SVImageClass::GetParentImageInterface() const
{
	return GetParentImage();
}

SvOi::MatroxImageSmartHandlePointer SVImageClass::getImageData()
{
	SVSmartHandlePointer handle;
	SvOi::MatroxImageSmartHandlePointer dataSmartPointer;
	if (GetImageHandle(handle))
	{
		MatroxImageData *data = new MatroxImageData(handle);
		data->setImageHandle(handle);
		dataSmartPointer = data;
	}
	return dataSmartPointer;
}

SvOi::MatroxImageSmartHandlePointer SVImageClass::getParentImageData()
{
	SVSmartHandlePointer handle;
	SvOi::MatroxImageSmartHandlePointer dataSmartPointer;
	if (S_OK == GetParentImageHandle(handle))
	{
		MatroxImageData *data = new MatroxImageData(handle);
		data->setImageHandle(handle);
		dataSmartPointer = data;
	}
	return dataSmartPointer;
}
#pragma endregion virtual method (ISVImage)

BOOL SVImageClass::OnValidate()
{
	if( SVObjectAppClass::OnValidate() )
	{
		switch( m_ImageType )
		{
			case SVImageTypePhysical:
			case SVImageTypeIndependent:
			case SVImageTypeFixed:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( l_BufferPtr != NULL );
				isObjectValid = isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				break;
			}
			case SVImageTypeLogical:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( l_BufferPtr != NULL );
				isObjectValid = isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				SVImageClass* l_pParent = GetParentImage();

				isObjectValid = isObjectValid && ( l_pParent != NULL );
				isObjectValid = isObjectValid && ( l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) == S_OK );

				break;
			}
			case SVImageTypeVirtual:
			case SVImageTypeDependent:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( l_BufferPtr != NULL );

				SVImageClass* l_pParent = GetParentImage();

				isObjectValid = isObjectValid && ( l_pParent != NULL );
				isObjectValid = isObjectValid && ( l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) == S_OK );

				break;
			}
			case SVImageTypeMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( l_BufferPtr != NULL );

				break;
			}
			case SVImageTypeRGBMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( l_BufferPtr != NULL );
				isObjectValid = isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				break;
			}
			default:
			{
				break;
			}
		}
	}

	return isObjectValid;
}

HRESULT SVImageClass::GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const
{
	HRESULT l_Status( S_OK );

	if( (ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE) > 0 )
	{
		l_Status = p_rHandle.Assign( rIndex.m_PublishedResultDMIndexHandle, rIndex.m_PublishedResultDMIndexHandle.GetLockType() );
	}
	else
	{
		l_Status = p_rHandle.Assign( rIndex.m_ResultDMIndexHandle, rIndex.m_ResultDMIndexHandle.GetLockType() );
	}

	return l_Status;
}

HRESULT SVImageClass::RegisterAsSubObject()
{
	HRESULT l_Status = S_OK;

	if( GetTool() != NULL )
	{
		GetTool()->RegisterSubObject( this );
	}
	
	if( GetInspection() != NULL )
	{
		GetInspection()->RegisterSubObject( this );
	}

	return l_Status;
}

HRESULT SVImageClass::UnregisterAsSubObject()
{
	HRESULT l_Status = S_OK;

	if( GetInspection() != NULL )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( GetTool() != NULL )
	{
		GetTool()->UnregisterSubObject( this );
	}
	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageClass.cpp_v  $
 * 
 *    Rev 1.8   26 Jun 2014 17:41:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method getDisplayedName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 May 2014 12:44:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Feb 2014 10:42:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Oct 2013 14:31:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Aug 2013 10:04:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed include of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 May 2013 12:16:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   02 May 2013 11:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:16:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   29 Apr 2013 14:15:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   19 Apr 2013 08:56:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added LoadImageFullSize to the SVImageClass.  Also removed unused class LoadImage( filename).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:52:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   21 Jan 2013 10:59:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   11 Jan 2013 15:00:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated buffer checking and upating functionality to allow the extent data to be updated without rebuilding all of the image buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   04 Sep 2012 15:01:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   09 Aug 2012 16:02:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added additional validity check to child array functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   12 Jul 2012 14:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   21 Feb 2012 13:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issue associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   28 Sep 2011 12:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   15 Jul 2011 11:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  727
 * SCR Title:  Fix Acquisition Tool bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Image Class to include special handling for Main Image type to connect to parent tool if one is present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   24 May 2011 15:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image class to use Owner Object Id insted of pointer and updated Image View with additional checks to improve performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   16 May 2011 12:44:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with Independent image type when collecting image information from parent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   03 May 2011 12:23:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated GetParentImage method to add a object look-up to prevent crashes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   02 May 2011 16:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with virtual and dependent images to change position point to (0,0) when updating extents from parent image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   02 May 2011 15:59:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with virtual and dependent images to change position point to (0,0) when updating extents from parent image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   13 Apr 2011 19:15:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to handle new fixed image type and fix color regression issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   13 Apr 2011 15:19:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   07 Apr 2011 16:45:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image class to fix a problem with image properties and Parent ID collection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   05 Apr 2011 12:31:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix automatic moving/resizing issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   05 Apr 2011 09:02:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new image type for unattached images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   31 Mar 2011 13:02:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with child object rebuild.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   30 Mar 2011 16:20:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   21 Mar 2011 11:45:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image Info to contain the logic to automaticly move and resize image extents that was contained within the image class.  Updated Image Object and Image classes to handle auto resize events.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   18 Mar 2011 07:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   16 Mar 2011 13:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   14 Mar 2011 08:07:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to perform a partial validation of extents to prevent crashing or causing load failure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   11 Mar 2011 14:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   25 Feb 2011 12:11:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   10 Feb 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   01 Feb 2011 11:21:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a casting issues with the buffer management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   27 Jan 2011 11:20:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   08 Dec 2010 12:51:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   04 Nov 2010 13:39:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   28 Oct 2010 13:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   16 Dec 2009 11:00:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   23 Oct 2009 14:33:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Cleared parent image pointer to ensure valid connection to parent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   24 Sep 2009 09:38:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code with branch changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   30 Jul 2009 11:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   06 Jun 2008 16:02:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed check on parentindex  in UpdatePosition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   05 Jun 2008 10:24:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check in update position to make sure parentIndex >= -1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 May 2008 14:05:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added an additional check in the destructor for parentImageIndex to be greater than zero before calling DestroyChild.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   09 Apr 2008 09:29:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator and Set Parent operator with Set Parent Image operator.  These two operators now verify the parent pointer for validity.  The Parent Image pointer attribute was replaced with the Get Parent Image or Set Parent Image operators to guarentee correctness of the attribute before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   17 Mar 2008 15:00:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved code that destroys the child from the m_pParent from the destructor to the close object and added a check for a physical image type before trying to destroy the child of the parent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   07 Mar 2008 11:56:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Transfer Image Definition Attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   24 Jul 2007 13:06:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   21 Jun 2007 13:05:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28.3.1   16 Sep 2009 07:42:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated check for parent to include chaecking against self.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28.3.0   14 Jul 2009 13:41:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image class to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28.2.0   07 Apr 2008 09:49:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods that used the Parent Image attribute to use the new GetParentImage method instead.  This will force a RTTI check for the validity of the object in non-runtime methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Jan 2006 09:58:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History attributes
 * SVImageBufferClass was renamed to SVImageOverlayClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   13 Dec 2005 14:21:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to caculate the sizes for coping image data to a BSTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   08 Nov 2005 15:53:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   02 Nov 2005 09:07:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   20 Sep 2005 16:10:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated virtual image methods to use try/catch methodology to verify valid parent data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   08 Sep 2005 12:25:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to copy an image to a BSTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   26 Aug 2005 15:00:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  509
 * SCR Title:  Fix incorrect image problem with Window Tool on a Load Image Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed CopyImageTo and LoadImage child index problem
 * removed dead code
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   22 Aug 2005 10:13:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Clear image buffer on safe call when invalid index is used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   10 Aug 2005 12:31:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   22 Jul 2005 13:36:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fix bug with GetParentImageHandle - hr assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Jul 2005 11:54:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed HRESULT coding error in GetParentImageHandle and GetImageHandle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Jun 2005 09:26:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * SetObjectValue now returns SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   19 May 2005 15:00:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed HRESULT pass-through
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   07 Mar 2005 12:16:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed ResetObject problems with virtual images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   22 Feb 2005 10:44:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image object with new load file method to fix buffer leaks and disconnection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Feb 2005 11:48:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  382
 * SCR Title:  Add locking mechanism to internal image objects to eliminate access problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added safe copy functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Feb 2005 14:25:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Feb 2005 14:29:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jan 2004 14:08:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  405
 * SCR Title:  Modify Image Overlay drawing for the display and SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made short term fix to force overlays to update when an image is requested through the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Dec 2003 13:46:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  383
 * SCR Title:  Scrolling problems after editing a config or selecting a new display image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed extent check to allow parent to be moved when the child is the same size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Aug 2003 12:00:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for the new mask image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 09:23:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 May 2003 14:11:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  368
 * SCR Title:  Removing a dependant tool causes a crash
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug that didn't reset parent reference when the parent  image destroyed and detached itself from the child image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 10:07:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Mar 2003 12:00:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   In Destroy, call Destroy for all children first
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Mar 2003 17:37:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Destroy method to fix an issue with RGBMainImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Mar 2003 11:42:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed GetExtent and GetAbsoluteExtent to return TRUE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Feb 2003 08:42:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made change to SetImageDepth to ignore the derived buffer if it is not relevant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:03:06   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved source code to new file.  All methods in class has changed to handle new buffer array stucture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   16 Jan 2003 18:37:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added code to support image overlays.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   10 Jan 2003 18:28:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new functionality to determine which object has overlays on which image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   10 Jan 2003 17:26:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  309
   SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add Get CircleBuffer and Index fuctions for images so they can be locked correctly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   18 Dec 2002 08:16:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  299
   SCR Title:  SIAC needs to be able to request Source Images
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed merge error pCamera --> mpCamera
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   17 Dec 2002 15:18:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  299
   SCR Title:  SIAC needs to be able to request Source Images
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added
   long SVImageClass::GetSourceImageIndex( SVProductInfoStruct* pProduct );	// base class returns -1
   
   long SVMainImageClass::GetSourceImageIndex( SVProductInfoStruct* pProduct );	// forwards request to camera
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   19 Nov 2002 13:17:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed references to System to SVImageProcessingClass and SVVirtualCamera
   SVImageClass:
               Added muiFormat member variable
               Move, FreeLogicalChildrenBottomToTop,
               AllocateLogicalChildrenTopToBottom,            MoveAbsoluteOrigins,
   
          Removed ErrorEvent.PulseEvent calls
   SVMainImageClass:
                Create
                Completely redone to account for new architecture
                CreateImageControlList, DestroyImageControlList
                 Removed AcquisitionCircleBuffer locking
                 Added GetCurrentProductInfo function
                 Added TransformImage member function
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   21 Jan 2002 16:11:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  277
   SCR Title:  Gage Tool that touches right or bottom of image window won't update
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     No change - just testing
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   02 Oct 2001 15:03:46   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  241
   SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Added functions, 
   SVImageClass::GetSecondaryImageInputContextGuid, 
   SVImageClass::SetSecondaryDisplayContextImage.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   18 Sep 2001 17:00:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  221
   SCR Title:  Upgrade SVObserver to Version 3.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to remove warning messages about type conversions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   18 Sep 2001 15:47:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  195
   SCR Title:  Fix Random NAKs
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated method SVMainImageClass::ReleaseCurrentImage.
   
   Added method SVMainImageClass::ReleaseCurrentBuffer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   19 Apr 2001 14:21:38   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   17 Oct 2000 08:12:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  183
   SCR Title:  Fix Problem with RGB Main Image in Monochrome Mode
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     These changes fix the banding problem with the image source.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   19 Sep 2000 09:01:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  183
   SCR Title:  Fix Problem with RGB Main Image in Monochrome Mode
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     This problem fixes the color display when the SVIM is in monochrome mode.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   06 Sep 2000 16:30:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   09 Jun 2000 10:25:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  157
   SCR Title:  Inspection Hangs after loading and going online
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVMainImageClass::DoNextImage to first release the previous buffer then wait for the next buffer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   26 Apr 2000 18:48:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SetExtent,SetAbsoluteExtent,SetAbsolutePosition,
   SetAbsoluteleft,SetAbsoluteTop,SetWidth, and SetHeight
   methods to send the 
   SVM_SET_SHADOWED_ABSEXTENT message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   08 Mar 2000 22:03:30   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised MoveAbsoluteOrigins to use translation x and y
   parameters.
   Added NotiftyUsersOfImageExtentChanged method,
   primarily to keep SVLineClass object's absolute extents 
   updated when the input image changes (SVLineROIClass).
   Revised SetExtent to update the width and height in the
   AbsoluteExtent variable as well.
   Revised SetAbsoluteExtent to update the width and height in the Extent variable as well.
   Revised Move to call NotifyUsersOfImageExtentsChanged.
   Revised updateChildrenAbsoluteExtents to call
   NotifyUsersOfImageExtentsChanged.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   04 Mar 2000 11:41:46   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added updateChildrenAbsoluteExtents method to synchronize absolute extents between Physical images of
   ImageType 0 and their children when the Physical Images of
   Image Type 0  are moved.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   03 Mar 2000 16:09:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Integrated RESIZING Message handler into
   SVImageClass::processMessage(...).
   Renamed SVImageClass::SetExtent() to SetImageExtent().
   Renamed SVImageClass::SetAbsoluteExtent() to SetImageAbsoluteExtent().
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   03 Mar 2000 12:27:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Extent and AbsoluteExtent to be private.
   Added access Methods to get/set extents.
   Revised to use access methods to get/set extents.
   Revised GetObjectScript to write double value types for extents.
   Revised SetObjectValue to accept double value types for extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   26 Feb 2000 20:01:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Move method to add translation parameters.
   Revised GetTransform to correct a problem with the
   ImageTool and the LoadImageTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   25 Feb 2000 17:36:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added variable Translation (POINT) to SVImageInfoClass
   Revised SVImageClass getObjectScript and SetObjectValue
   to save/restore Translation variable.
   Added GetTransform method to SVImageClass to handle
   determination of transformation needed for drawing context.
   Revised isMoveable, isLResizeable, isRResizeable,
   isTResizeable, isBResizeable, isLTResizeable,
   isLBResizeable, isRTResizeable, isRBResizeable to
   call GetTransform and apply the InverseTransform to the
   mouse point.
   Added message handler for SVM_GET_CONTEXT_EXTENT
   Remove message handler for SVM_SET_EXTENT.
   Revised MoveAbsoluteOrigins to set/reset translation values.
   Revised SetExtent to document setting of relative extent
   Left and Top to zero for certain image types.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   21 Feb 2000 18:15:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised isMoveable, isLResizeable, isRResizeable, 
   isTResizeable, isBResizeable, isLTResizeable,
   isRTResizeable, isLBResizeable and
   isRBResizeable to inversely transform mouse point.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   21 Feb 2000 14:34:22   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised isMoveable() ,isLResizable(), ,isRResizable(),
   isTResizable(), isBResizable(), isLTResizable(),
   isLTResizable(), isLBResizable(), isRTResizable(), and
   isRBResizable() to use SVDrawContext* and to use relative extents.
   Removed SetChildListDisplacement (never called).
   Added GetRootImage(), GetImageInputContextGuid(),
   and SetDisplayContextImage().
   Removed dead code in ResetImage().
   Removed calls to MoveObjectsBase().
   Revised SetExtent to not hard code Left and 
   Top coordinates to zero for non main physical images.
   Added handler in processMessage for SVMSGID_SET_EXTENT.
   Added pDisplayImageContext (SVImageClass *) variable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Feb 21 2000 11:05:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added new global func. SVFillImageInfo...
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   07 Feb 2000 20:20:58   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Changed to work with Main Image Manager. Changed Scripting ( only additions ) to save new necessary information.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 16:33:32   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  70
   SCR Title:  Archive Tool fails to save selected toolset image in configuration.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed software to ignore 'attributes allowed' as saved in configuration files for image objects.  This repairs the problem noted by this SCR.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Nov 05 1999 10:40:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  56
   SCR Title:  Version 3.00 Beta 17 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     bandNumber = 1 as default instead of -1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   28 Oct 1999 18:37:50   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  53
   SCR Title:  Implement Regression Test Capability in SVObserver
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Implemented Regression Test
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   Oct 28 1999 09:04:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fix build reference bug regarding images that do not have derived child images.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   28 Oct 1999 08:42:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised GetObjectScript to address performance issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Oct 13 1999 14:59:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added input image and Output image.
   Introduced Logical image types.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   Oct 12 1999 08:16:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  44
   SCR Title:  Version 3.00 Beta 9 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Sep 20 1999 15:34:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Extent Checking.
   Introduced Broadcast code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Sep 13 1999 13:26:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced pure physical image class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Sep 10 1999 17:21:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed old image result buffer info.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   04 Sep 1999 17:29:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to save/restore extents, absolute extents,
   rotationAngle, rotationCenter, and pixelDepth
   via ObjectScript.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   30 Aug 1999 20:09:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added a case statement to handle 
   SVMSGID_CREATE_ALL_OBJECTS
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   Aug 23 1999 10:10:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  Steve Jones
   Change Description:  
     Revised to use new Digitizer Classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Aug 16 1999 09:01:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Revised to use base class processMessage for SVMSGID_GETFIRST_OBJECT,
   Revised to use SVMainImageObjectType as SubType
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   03 Feb 1999 10:49:30   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  5
   SCR Title:  Fix acquisition call back "dismiss bug", make correlated functions more robust
   Checked in by:  Robert Massinger
   Change Description:  
     Stabilzed acquisition circle buffer blocking system. ( Image Occupation )
   
   /////////////////////////////////////////////////////////////////////////////////////
*/