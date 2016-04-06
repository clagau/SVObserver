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

#pragma region Includes
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
#include "SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVObserver.h"
#include "SVTool.h"
#include "SVToolSet.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVImageLibrary\MatroxImageData.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

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

	l_bOk &= ( S_OK == UpdateFromToolInformation() );
	l_bOk &= ( S_OK == UpdateBufferArrays() );

	ObjectAttributesAllowedRef() |= SV_PUBLISH_RESULT_IMAGE | SV_CH_IMAGE | SV_DD_IMAGE;	// add this on older configs
	
	isCreated = l_bOk;
	
	return l_bOk;	
}

BOOL SVImageClass::CloseObject()
{
	BOOL rc = TRUE;
	
	rc = ( S_OK == ClearParentConnection() );

	if( nullptr != GetTool() )
	{
		GetTool()->SetToolImage( nullptr );
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
	SVImageClass* l_pParent = nullptr;
		
	if( ! m_ParentImageInfo.first.empty() )
	{
		if( SVObjectManagerClass::Instance().GetState() == SVObjectManagerClass::ReadOnly )
		{
			if( nullptr == m_ParentImageInfo.second )
			{
				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first );

				if( nullptr != l_pObject )
				{
					m_ParentImageInfo.second = dynamic_cast< SVImageClass* >( l_pObject );
				}
			}

			if( nullptr != m_ParentImageInfo.second )
			{
				l_pParent = m_ParentImageInfo.second;
			}
		}
		else
		{
			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first );

			if( nullptr != l_pObject )
			{
				l_pParent = dynamic_cast< SVImageClass* >( l_pObject );
			}

			m_ParentImageInfo.second = nullptr;
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

	m_ParentImageInfo.second = nullptr;

	m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );
		
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointX, SV_DEFAULT_WINDOWTOOL_LEFT );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointY, SV_DEFAULT_WINDOWTOOL_TOP );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, SV_DEFAULT_WINDOWTOOL_WIDTH );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, SV_DEFAULT_WINDOWTOOL_HEIGHT );
	m_ImageInfo.SetExtentProperty(	SVExtentPropertyWidthScaleFactor, 
									SV_DEFAULT_WINDOWTOOL_WIDTHSCALEFACTOR );
	m_ImageInfo.SetExtentProperty(	SVExtentPropertyHeightScaleFactor, 
									SV_DEFAULT_WINDOWTOOL_HEIGHTSCALEFACTOR );
}

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

BOOL SVImageClass::DestroyImage()
{
	BOOL bOk = TRUE;
	
	if ( isCreated )
	{
		bOk = Lock();

		ASSERT( bOk );

		if ( bOk )
		{
			bOk = ( S_OK == ClearParentConnection() );

			ASSERT( bOk );

			bOk = S_OK == RemoveChildren() && bOk;

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
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvO::ImageClass_DestroyError, StdMessageParams, SvOi::Err_10051 ); 
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

	if( nullptr != p_pParentImage )
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
		m_ParentImageInfo.second = nullptr;

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
			m_ParentImageInfo.second = nullptr;
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

		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Temp = RebuildStorage( p_ExcludePositionCheck );

		if( S_OK == l_Status )
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
		m_ParentImageInfo.second = nullptr;

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
			m_ParentImageInfo.second = nullptr;
		}

		HRESULT l_Temp = m_ImageInfo.SetExtents( p_rExtent );

		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		m_LastUpdate = SVClock::GetTimeStamp();

		l_Temp = ResetObject();

		if( S_OK == l_Status )
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
			m_ParentImageInfo.second = nullptr;
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
			m_ParentImageInfo.second = nullptr;
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
			m_ParentImageInfo.second = nullptr;
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

		l_bOk &= ( S_OK == ResetObject() );
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
	HRESULT hr = UpdateFromParentInformation();

	if( m_LastReset <= m_LastUpdate )
	{
		HRESULT l_Temp = SVObjectAppClass::ResetObject();

		if( S_OK == hr )
		{
			hr = l_Temp;
		}

		// One of the use cases for RebuildStorage() is, when a Tool is added 
		// (for all embedded images).  If the embedded image is a Logical 
		// Image, then the Parent wont be assigned yet.  The assignment 
		// happens in OnCreate of the Tool.  Therefore the following 
		// UpdatePosition() will return an E_FAIL.
		l_Temp = UpdatePosition();

		if( S_OK == hr )
		{
			hr = l_Temp;
		}

		// if update position failed, we should not be updating buffer 
		// arrays.
		if (S_OK == l_Temp)
		{
			l_Temp = UpdateBufferArrays( p_ExcludePositionCheck );
		}

		if( S_OK == hr )
		{
			hr = l_Temp;
		}

		// but we still update children?
		l_Temp = UpdateChildren();

		if( S_OK == hr )
		{
			hr = l_Temp;
		}
	} 

	if( S_OK == hr )
	{
		m_LastReset = SVClock::GetTimeStamp();
	}

	return hr;
}

SVImageExtentClass SVImageClass::GetImageExtents()
{
	return m_ImageInfo.GetExtents();
}

HRESULT SVImageClass::GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent )
{
	HRESULT l_hrOk = m_ImageInfo.GetImageExtentsToFit( p_svInExtent, p_rsvOutExtent );
	
	return l_hrOk;
}

HRESULT SVImageClass::UpdateFromParentInformation()
{
	HRESULT l_Status = S_OK;

	if( ( m_ImageType != SVImageTypeMain ) && ( m_ImageType != SVImageTypeRGBMain ) )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && ( m_LastReset < l_pParentImage->GetLastResetTimeStamp() || m_LastReset < m_LastUpdate ) )
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

				if( S_OK == l_Status )
				{
					l_Status = l_ImageExtent.SetPosition( l_Position );
				}
			}

			if( S_OK == l_Status )
			{
				l_Status = l_ImageInfo.SetExtents( l_ImageExtent );
			}
	
			if( S_OK == l_Status )
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

	if (S_FALSE == l_Status)
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateFromToolInformation()
{
	HRESULT l_Status = S_OK;

	SVGUID l_ToolID;
	SVImageExtentClass l_ToolExtent = m_ImageInfo.GetExtents();

	// When initialized from CreateObject(), tool is nullptr.
 	if( ( m_ImageType != SVImageTypeRGBMain ) && ( nullptr != GetTool() ) )
	{
		SVImageExtentClass l_TempExtent;

		if( ( SVImageTypeMain != m_ImageType ) && 
			( SVImageTypeFixed != m_ImageType ) && 
			( SVImageTypeIndependent != m_ImageType ) && 
			( SVImageTypeDependent != m_ImageType ) && 
			( SVImageTypeVirtual != m_ImageType ) && 
			( S_OK == GetTool()->GetImageExtent( l_TempExtent ) ) )
		{


			RECT l_Rect;

			if (( SVImageTypeLogicalAndPhysical == m_ImageType)|| 
				(SVImageTypeLogical == m_ImageType))
			{
				// @Hack
				// It does not make sense that a logical buffer is not a 1:1 
				// pixel correlation to its parent phisical buffer.  For this 
				// reason the translation type will be ignored when retrieving
				// the logical rectangle.  
				// The usage that this is specifically excluded for is for 
				// creating a logical ROI buffer, which should not reflect the 
				// output buffer translation.
				l_TempExtent.SetTranslation (SVExtentTranslationShift);
				l_Status = l_TempExtent.GetLogicalRectangle (l_Rect);
			}
			else
			{
				l_Status = l_TempExtent.GetOutputRectangle( l_Rect );
			}

			if( S_OK == l_Status )
			{
				if(	0 < ( l_Rect.bottom - l_Rect.top + 1 ) && 0 < ( l_Rect.right - l_Rect.left + 1 ) )
				{
					l_ToolExtent = l_TempExtent;
				}
			}
		}

		if( SVImageTypeFixed != m_ImageType && 
			SVImageTypeIndependent != m_ImageType && 
			SVImageTypeDependent != m_ImageType )
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

	if( ( SVImageTypeMain != m_ImageType ) && ( SVImageTypeIndependent != m_ImageType ) )
	{
		l_Status = m_ImageInfo.SetExtents( l_ToolExtent );

		if( S_OK == l_Status )
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

	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		ASSERT( nullptr != l_pParentImage );

		if( nullptr != l_pParentImage && l_pParentImage != this )
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
		if( SVImageTypeDependent == m_ImageType || 
			SVImageTypeVirtual == m_ImageType ||
			SVImageTypeLogical == m_ImageType )
		{
			SVImageClass* l_pParentImage = GetParentImage();

			ASSERT( nullptr != l_pParentImage );

			if( nullptr != l_pParentImage && l_pParentImage != this )
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
			SVImageClass* l_pImage = nullptr;

			l_Iter->second.m_ImageInfo.GetOwnerImage( l_pImage );

			if( nullptr != l_pImage )
			{
				l_hrOk = l_pImage->RemoveObjectConnection( GetUniqueObjectID() );
			}

			if( nullptr != l_Iter->second.m_pImageHandles )
			{
				l_Iter->second.m_pImageHandles.clear();
				
				l_Iter->second.m_pImageHandles = nullptr;
			}

			l_Iter->second.m_ImageInfo.Initialize();

			m_ChildArrays.erase( l_Iter );
		}

		m_ParentImageInfo.second = nullptr;

		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
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

	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
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
			if( nullptr != l_Iter->second.m_pImageHandles )
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

	if( SVImageTypeDependent == m_ImageType|| 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_rsvBufferHandle );
			if ( S_FALSE == l_hrOk )
			{
				CString sMsgStr;
				sMsgStr.Format("S_FALSE == l_pParentImage->GetChildImageHandle( p_rChildID, p_rsvBufferHandle )");

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_5053_CHILDIMAGEHANDLESFALSE, sMsgStr, StdMessageParams );
			}
		}
		else // really? there are 2 conditions to the if statement above...
		{
			CString sMsgStr;
			sMsgStr.Format("nullptr == l_pParentImage");

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5054_NULLPARENTIMAGE, sMsgStr, StdMessageParams );

			l_hrOk = S_FALSE;
		}
	}
	else
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			if( nullptr != l_Iter->second.m_pImageHandles )
			{
				if( l_Iter->second.m_pImageHandles->GetImageHandle( p_rsvBufferHandle ) )
				{
					l_hrOk = S_OK;
				}
				else
				{
					CString sMsgStr;
					sMsgStr.Format("if (l_Iter->second.m_pImageHandles->GetImageHandle( p_rsvBufferHandle )");

					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_5055_NULLCHILDHANDLE, sMsgStr, StdMessageParams );
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

	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_svBufferIndex, p_rsvBufferHandle );
			if ( S_FALSE == l_hrOk )
			{
				CString sMsgStr;
				sMsgStr.Format("S_FALSE == l_pParentImage->GetChildImageHandle( m_svChildIndexArray[ p_lChildIndex ], p_rsvBufferHandle )");

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_5056_CHILDIMAGEHANDLESFALSE, sMsgStr, StdMessageParams );
			}

		}
		else  // really? there are 2 conditions to the if statement above...
		{
			CString sMsgStr;
			sMsgStr.Format("nullptr == l_pParentImage");

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5057_NULLPARENTIMAGE, sMsgStr, StdMessageParams );
			l_hrOk = S_FALSE;
		}
	}
	else
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.find( p_rChildID );

		if( l_Iter != m_ChildArrays.end() )
		{
			if( nullptr != l_Iter->second.m_pImageHandles )
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
					sMsgStr.Format("null ptr == l_Iter->second.m_pImageHandles->GetImageHandle( l_Handle.GetIndex(), p_rsvBufferHandle )");

					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_5058_NULLCHILDHANDLE, sMsgStr, StdMessageParams );
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

	if( nullptr != l_pParentImage && l_pParentImage != this )
	{
		l_hrOk = l_pParentImage->GetChildImageHandle( GetUniqueObjectID(), p_rsvBufferHandle );

		if( S_OK != l_hrOk )
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5059_GETCHILDERROR, nullptr, StdMessageParams );
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

	if( nullptr != l_pParentImage && l_pParentImage != this )
	{
		l_hrOk = l_pParentImage->GetChildImageHandle( GetUniqueObjectID(), p_svBufferIndex, p_rsvBufferHandle );

		if( S_OK != l_hrOk )
		{
			CString sMsgStr;
			sMsgStr.Format("ERROR: Cannot Get Indexed Child Image Handle");

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5060_GETCHILDERROR, sMsgStr, StdMessageParams );
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
		
	if( SVImageTypeDependent == m_ImageType|| 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
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
			{
				rsvIndex.m_PublishedResultDMIndexHandle.Assign( l_DMIndexHandle, l_DMIndexHandle.GetLockType() );
			}
			else
			{
				rsvIndex.m_ResultDMIndexHandle.Assign( l_DMIndexHandle, l_DMIndexHandle.GetLockType() );
			}
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
		
	if( SVImageTypeDependent == m_ImageType|| 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		bOk = nullptr != GetParentImage() && GetParentImage() != this;
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
		
	if( SVImageTypeDependent == m_ImageType|| 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
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
			if( nullptr != l_Iter->second.m_pImageHandles )
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
		
	if( SVImageTypeDependent == m_ImageType|| 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		bOk = ( S_OK == GetParentImageHandle( p_rHandlePtr ) );
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
		
	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		bOk = ( S_OK == GetParentImageHandle( svIndex, rHandle ) );
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
		}
	}
	return l_bOk;
}

BOOL SVImageClass::SafeImageCopyToBSTR( SVImageIndexStruct p_svFromIndex, BSTR &p_rbstrData )
{
	BOOL l_bOk = FALSE;
	
	if ( Lock() )
	{
		BITMAPINFO* pbmInfo = nullptr;

		SVSmartHandlePointer l_svOrigHandle;
		SVSmartHandlePointer l_svCopyHandle;

		SVSmartHandlePointer l_psvHandle;

		l_bOk = GetImageHandle( p_svFromIndex, l_svOrigHandle ) &&
			!( l_svOrigHandle.empty() ) && nullptr != l_svOrigHandle->GetBufferAddress();

		if( l_bOk )
		{
			l_psvHandle = l_svOrigHandle;
		}
		else
		{
			l_bOk = S_OK == SVImageProcessingClass::Instance().CreateImageBuffer( m_ImageInfo, l_svCopyHandle );

			l_bOk = l_bOk && SafeImageCopyToHandle( p_svFromIndex, l_svCopyHandle );
			l_bOk = l_bOk && !( l_svCopyHandle.empty() ) && nullptr != l_svCopyHandle->GetBufferAddress();

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
			
			if( nullptr != p_rbstrData )
			{
				::SysFreeString( p_rbstrData );

				p_rbstrData = nullptr;
			}

			p_rbstrData = ::SysAllocStringByteLen(nullptr, lBufSize);

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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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

			l_bOk = Unlock() && l_bOk && SVMEE_STATUS_OK == l_Code;
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
				if( nullptr != l_Iter->second.m_pImageHandles )
				{
					SVDataManagerHandle l_Handle;

					GetImageIndex( l_Handle, p_svToIndex );

					bOk &= l_Iter->second.m_pImageHandles->SetCurrentIndex( l_Handle );
				}

				++l_Iter;
			}

			ASSERT( bOk );
			if ( S_OK == l_hrOk )
			{
				l_hrOk = bOk ? S_OK : S_FALSE;
			}

		}// end if( Lock() )
	}// end if ( !( m_BufferArrayPtr.empty() ) )

	return l_hrOk;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
SVImageClass* SVImageClass::GetRootImage()
{
	SVImageClass* pRootImage = this;
	SVImageClass* pParentImage = GetParentImage();

	if( nullptr != pParentImage && pParentImage != this )
	{
		pRootImage = pParentImage->GetRootImage();
	}

	return pRootImage;
}

SVString SVImageClass::getDisplayedName() const
{
	const SVObjectTypeInfoStruct& rObjectTypeInfo = GetObjectInfo().ObjectTypeInfo;
	SVString strName;
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
				strName = GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType );
			}
			break;
		}// end default:
	}// end switch( rObjectTypeInfo.SubType )
	return strName;
}

DWORD_PTR SVImageClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = 0;

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	switch( dwPureMessageID )
	{
	case SVMSGID_GETFIRST_IMAGE_INFO:
		return reinterpret_cast<DWORD_PTR>( &m_ImageInfo );

	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
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
//Moved to precompiled header: #include <map>
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
		m_ParentImageInfo.second = nullptr;

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

	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage )
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

//@HACK ?? JAB 26.03.15 - Not sure why these properties are being 
// 		itteratively set to each array element. It looks to me like the 
//		final value will alway be svDoubleArray [GetSize() - 1].  Don't have 
//		time to look into now.  Many identical cases in this method.
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
	else if ( ( bOk = PDataObject->GetAttributeData( "HeightScaleFactor", svDoubleArray	) ) )
	{
		long	size = svDoubleArray.GetSize ();
		if (size > 0)
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyHeightScaleFactor, svDoubleArray [size-1] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "WidthScaleFactor", svDoubleArray	) ) )
	{
		long	size = svDoubleArray.GetSize ();
		if (size > 0)
		{
			m_ImageInfo.SetExtentProperty( SVExtentPropertyWidthScaleFactor, svDoubleArray [size-1] );
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

HRESULT SVImageClass::UpdatePosition()
{
	HRESULT l_Status = S_OK;
	
	m_ParentImageInfo.second = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first ) );

	if( SVImageTypeDependent == m_ImageType || 
		SVImageTypeLogicalAndPhysical == m_ImageType || 
		SVImageTypeLogical == m_ImageType ||
		SVImageTypeVirtual == m_ImageType )
	{
		if( nullptr != m_ParentImageInfo.second )
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

HRESULT SVImageClass::UpdateChildren()
{
	HRESULT l_hrOk = S_OK;

	if( Lock() )
	{
		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( S_OK != UpdateChild( l_Iter->first, l_Iter->second.m_ImageInfo ) )
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
			if( S_OK != RemoveChild( l_Iter->first ) )
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

			if( SV_PUBLISH_RESULT_IMAGE == ( ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE ) )
			{
				l_Size = TheSVObserverApp.GetSourceImageDepth();
			}
		}
		else
		{
			l_Size = l_BufferPtr->size();
		}

		// resize references the depth of the pool.
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
	HRESULT		l_Status = S_OK;
	HRESULT		hrTemp = S_OK;
	SVString	sTemp;
	bool		displayedTemp;

	if ((SVImageTypeLogicalAndPhysical == m_ImageType ||
		 SVImageTypeLogical == m_ImageType) &&
		nullptr == m_ParentImageInfo.second)
	{
		// If the image type is logical, there MUST be a parent before 
		// attempting to set up image buffers.
		return l_Status;
	}

	bool	clear = false;
	SVGUID	parentGuid;
	parentGuid = m_ImageInfo.GetOwnerImageID ();
	if( SVImageTypeUnknown == m_ImageType || 
		SVImageTypeDependent == m_ImageType || 
		SVImageTypeVirtual == m_ImageType || 
		SVImageTypeMain == m_ImageType ||
		SVImageTypeLogical == m_ImageType )
		
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

			if( SV_PUBLISH_RESULT_IMAGE == ( ObjectAttributesSet() & SV_PUBLISH_RESULT_IMAGE ) )
			{
				l_Size = TheSVObserverApp.GetSourceImageDepth();
			}

			bool l_Reset = false;
			bool l_Update = false;

			SVImageObjectClass::SVImageObjectParentPtr parent = m_BufferArrayPtr->GetParentImageObject();

			l_Reset = l_Reset || ( l_Size != m_BufferArrayPtr->size() );
			l_Reset = l_Reset || !( parent.empty() );

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
					SVToolClass*	parentTool = GetTool();
					if (nullptr == parentTool)
					{
						// this image does not belong to a Tool.  
					}
					else
					{
						// Because m_BufferArrayPtr->ResetObject() only 
						// returns an S_FALSE, error data will be tracked 
						// through the owning Tool (if present).
						hrTemp = parentTool->GetRunErrorCode();
						sTemp = parentTool->GetRunErrorData();
						displayedTemp = parentTool->GetRunDisplayed();
						parentTool->ClearRunError();
					}

					l_Status = m_BufferArrayPtr->ResetObject();
					if (S_FALSE == l_Status)
					{
						l_Status = SVMSG_SVO_5069_RESETOBJECTFAILED;
					}

					HRESULT	hrRun = S_OK;

					if (nullptr != parentTool)
					{
						hrRun = parentTool->GetRunErrorCode();
					}

					if ((S_OK == l_Status) && (S_OK != hrRun))
					{
						l_Status = SVMSG_SVO_5068_INCONSISTENTDATA;
					}
					else
					{
						l_Status = hrRun;
					}

					if (SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == l_Status)
					{
						const SVString& tString1 = GetCompleteName();
						parentTool->SetRunErrorData(tString1.c_str());
					}

					if ((nullptr != parentTool && SUCCEEDED (parentTool->GetRunErrorCode())))
					{
						// Why three (3) function calls to set this data?
						parentTool->SetRunErrorCode(hrTemp);
						parentTool->SetRunErrorData(sTemp);
						parentTool->SetRunDisplayed(displayedTemp);
					}
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

	if (S_FALSE == l_Status)
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVImageClass::ValidateAgainstAllExtents( SVImageExtentClass& p_rsvExtent )
{
	HRESULT l_hrOk = ValidateAgainstParentExtents( p_rsvExtent );
	
	if ( S_OK == l_hrOk )
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

	if( nullptr != l_pParentImage && l_pParentImage != this )
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
			if ( S_OK != p_rsvExtent.ValidateAgainstOutputSpace( l_Iter->second.m_ImageInfo.GetExtents() ) )
			{
				SVImageClass* l_pImage = nullptr;

				l_Iter->second.m_ImageInfo.GetOwnerImage( l_pImage );

				if( nullptr == l_pImage || SVImageTypeDependent != m_ImageType || SVImageTypeVirtual != l_pImage->GetImageType() )
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

	if( nullptr != p_pImage )
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

SvOi::MatroxImageSmartHandlePtr SVImageClass::getImageData()
{
	SVSmartHandlePointer handle;
	SvOi::MatroxImageSmartHandlePtr dataSmartPointer;

	if ((SVImageTypeLogical == m_ImageType) ||
		(SVImageTypeDependent == m_ImageType) ||
		(SVImageTypeVirtual == m_ImageType))
	{
		// For SVO Child buffers, the image data is actually allocated as 
		// the Parent Image Child, and not as part of the SVImageObjectClass.
		// getImageData should redirect.  

		SVSmartHandlePointer childHandle;
		if (S_OK == GetParentImageHandle(childHandle))
		{
			dataSmartPointer = new MatroxImageData(childHandle);
		}
	}
	else
	{
		// For all other buffer types, images are retrieved from 
		// SVImageObjectClass.

		if (GetImageHandle(handle))
		{
			dataSmartPointer = new MatroxImageData(handle);
		}
	}
	return dataSmartPointer;
}

SvOi::MatroxImageSmartHandlePtr SVImageClass::getParentImageData()
{
	SVSmartHandlePointer handle;
	SvOi::MatroxImageSmartHandlePtr dataSmartPointer;

	if (S_OK == GetParentImageHandle(handle))
	{
		MatroxImageData *data = new MatroxImageData(handle);
		data->setImageHandle(handle);
		dataSmartPointer = data;
	}
	return dataSmartPointer;
}

SvOi::IObjectClass* SVImageClass::getOwner() const
{
	return m_ImageInfo.GetOwner();
}

long SVImageClass::getBands() const
{
	long bandNumber = 0;
	m_ImageInfo.GetImageProperty(SVImagePropertyBandNumber, bandNumber);
	return bandNumber;
}

long SVImageClass::getPixelDepth() const
{
	long pixelDepth = 0;
	m_ImageInfo.GetImageProperty(SVImagePropertyPixelDepth, pixelDepth);
	return pixelDepth;
}

HRESULT SVImageClass::Save(const SVString& rFilename)
{
	HRESULT hr = S_OK;
	
	// Get output buffer handle...
	SVSmartHandlePointer hBuffer;
	if (GetImageHandle(hBuffer) && !hBuffer.empty())
	{
		SVImageBufferHandleImage MilHandle;
		hBuffer->GetData(MilHandle);

		SVString ext;
		size_t pos = rFilename.find_last_of(".");
		if (SVString::npos != pos)
		{
			ext = rFilename.substr(pos, rFilename.size() - pos);
		}
		SVMatroxFileTypeEnum efileformat = SVFileUnknown;
		if (0 == SvUl_SF::CompareNoCase(ext, _T(".mim")))
		{
			efileformat = SVFileMIL;
		}
		else if (0 == SvUl_SF::CompareNoCase(ext, _T(".tif")))
		{
			efileformat = SVFileTiff;
		}
		else if (0 == SvUl_SF::CompareNoCase(ext, _T(".bmp")))
		{
			efileformat = SVFileBitmap;
		}
	
		if (efileformat != SVFileUnknown)
		{
			SVMatroxString strPath = rFilename.c_str();
			SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::Export(MilHandle.GetBuffer(), strPath, efileformat);
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_HANDLE;
	}
	return hr;
}

RECT SVImageClass::GetOutputRectangle() const
{
	RECT rectTemp;
	m_ImageInfo.GetOutputRectangle( rectTemp );
	return rectTemp;
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

				isObjectValid = isObjectValid && ( nullptr != l_BufferPtr );
				isObjectValid = isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				break;
			}
			case SVImageTypeLogicalAndPhysical:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( nullptr != l_BufferPtr );
				isObjectValid = isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				SVImageClass* l_pParent = GetParentImage();

				isObjectValid = isObjectValid && ( nullptr != l_pParent );
				isObjectValid = isObjectValid && ( S_OK == l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) );

				break;
			}
			case SVImageTypeLogical:
			case SVImageTypeVirtual:
			case SVImageTypeDependent:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( nullptr != l_BufferPtr );

				SVImageClass* l_pParent = GetParentImage();

				isObjectValid = isObjectValid && ( nullptr != l_pParent );
				isObjectValid = isObjectValid && ( S_OK == l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) );

				break;
			}
			case SVImageTypeMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( nullptr != l_BufferPtr );

				break;
			}
			case SVImageTypeRGBMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				isObjectValid = isObjectValid && ( nullptr != l_BufferPtr );
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

	if( nullptr != GetTool() )
	{
		GetTool()->RegisterSubObject( this );
	}
	
	if( nullptr != GetInspection() )
	{
		GetInspection()->RegisterSubObject( this );
	}
	return l_Status;
}

HRESULT SVImageClass::UnregisterAsSubObject()
{
	HRESULT l_Status = S_OK;

	if( nullptr != GetInspection() )
	{
		GetInspection()->UnregisterSubObject( this );
	}

	if( nullptr != GetTool() )
	{
		GetTool()->UnregisterSubObject( this );
	}
	return l_Status;
}

