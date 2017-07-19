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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageBuffer.h"
#include "SVImageObjectClass.h"
#include "SVImageProcessingClass.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITool.h"
#include "SVTaskObject.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVImageLibrary\MatroxImageData.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ISVOApp_Helper.h"
#pragma endregion Includes

#ifdef _DEBUG
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

	SetObjectAttributesAllowed( SvOi::SV_PUBLISH_RESULT_IMAGE | SvOi::SV_DD_IMAGE, SvOi::SetAttributeType::AddAttribute );	// add this to older configs
	
	m_isCreated = l_bOk;
	
	return l_bOk;	
}

BOOL SVImageClass::CloseObject()
{
	BOOL rc = true;
	
	rc = ( S_OK == ClearParentConnection() );

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (GetTool());
	if( nullptr != pTool )
	{
		pTool->SetToolImage( SV_GUID_NULL );
	}

	if ( m_isCreated )
	{
		rc = DestroyImage();
	}
	
	UnregisterAsSubObject();

	rc = SVObjectAppClass::CloseObject() && rc;
	
	return rc;
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

	m_ImageType = SvOi::SVImageTypeEnum::SVImageTypeUnknown;

	m_BufferArrayPtr = new SVImageObjectClass;
	
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVImageObjectType;
	
	// derived classes that are not result images (i.e. SVMainImageClass)
	// need to remove the PUBLISH attribute.
	SetObjectAttributesAllowed( SvOi::SV_ARCHIVABLE_IMAGE | SvOi::SV_PUBLISH_RESULT_IMAGE | SvOi::SV_DD_IMAGE, SvOi::SetAttributeType::OverwriteAttribute );

	m_ParentImageInfo.second = nullptr;

	m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );
		
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointX, SvOi::cDefaultWindowToolLeft );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyPositionPointY, SvOi::cDefaultWindowToolTop );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyWidth, SvOi::cDefaultWindowToolWidth );
	m_ImageInfo.SetExtentProperty( SVExtentPropertyHeight, SvOi::cDefaultWindowToolHeight );
	m_ImageInfo.SetExtentProperty(	SVExtentPropertyWidthScaleFactor, SvOi::cDefaultWindowToolWidthScaleFactor );
	m_ImageInfo.SetExtentProperty(	SVExtentPropertyHeightScaleFactor, SvOi::cDefaultWindowToolHeightScaleFactor );
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
	
	if ( m_isCreated )
	{
		bOk = Lock();

		assert( bOk );

		if ( bOk )
		{
			bOk = ( S_OK == ClearParentConnection() );

			assert( bOk );

			bOk = S_OK == RemoveChildren() && bOk;

			assert( bOk );

			if( !( m_BufferArrayPtr.empty() ) )
			{
				m_BufferArrayPtr->clear();
			}

			m_ImageType = SvOi::SVImageTypeEnum::SVImageTypeUnknown;

			m_isCreated = false;

			bOk = Unlock() && bOk;

			assert( bOk );
		}

		if ( ! bOk )
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ImageClass_DestroyError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10051 ); 
		}
	}

	return bOk;
}

HRESULT SVImageClass::InitializeImage( SvOi::SVImageTypeEnum p_ImageType )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType )
	{
		m_ImageType = p_ImageType;

		m_LastUpdate = SvTl::GetTimeStamp();
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

		m_LastUpdate = SvTl::GetTimeStamp();

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

		m_LastUpdate = SvTl::GetTimeStamp();

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

		m_LastUpdate = SvTl::GetTimeStamp();

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

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
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

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
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

		m_LastUpdate = SvTl::GetTimeStamp();

		bool bOk = ResetObject();

		if( S_OK == l_Status && !bOk )
		{
			l_Status = S_FALSE;
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

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SvOi::SVImageTypeEnum p_ImageType )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType )
	{
		m_ImageType = p_ImageType;

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SvOi::SVImageTypeEnum p_ImageType, const SVImageInfoClass& p_rImageInfo )
{
	HRESULT l_Status = S_OK;

	if( m_ImageType != p_ImageType || m_ImageInfo != p_rImageInfo )
	{
		m_ImageType = p_ImageType;

		m_ImageInfo = p_rImageInfo;
		m_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SvOi::SVImageTypeEnum p_ImageType, const GUID& p_rParentID )
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

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
	}

	return l_Status;
}

HRESULT SVImageClass::UpdateImage( SvOi::SVImageTypeEnum p_ImageType, const GUID& p_rParentID, const SVImageInfoClass& p_rImageInfo )
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

		m_LastUpdate = SvTl::GetTimeStamp();

		l_Status = ( ResetObject() ? S_OK : S_FALSE );
	}

	return l_Status;
}

BOOL SVImageClass::SetImageDepth( long lDepth )
{
	bool l_bOk = true;

	if( lDepth != GetImageDepth() )
	{
		l_bOk = (TRUE == SVObjectAppClass::SetImageDepth( lDepth ));

		m_LastUpdate = SvTl::GetTimeStamp();

		l_bOk &= ResetObject();
	}

	return l_bOk;
}

const SvTl::SVTimeStamp& SVImageClass::GetLastResetTimeStamp() const
{
	return m_LastReset;
}
	
bool SVImageClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = (S_OK == RebuildStorage( false, pErrorMessages )) && __super::ResetObject(pErrorMessages);
	if (!ValidateImage())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InitImageFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

HRESULT SVImageClass::RebuildStorage( bool p_ExcludePositionCheck, SvStl::MessageContainerVector *pErrorMessages )
{
	HRESULT hr = UpdateFromParentInformation();

	if( m_LastReset <= m_LastUpdate )
	{
		// One of the use cases for RebuildStorage() is, when a Tool is added 
		// (for all embedded images).  If the embedded image is a Logical 
		// Image, then the Parent wont be assigned yet.  The assignment 
		// happens in OnCreate of the Tool.  Therefore the following 
		// UpdatePosition() will return an E_FAIL.
		HRESULT l_Temp = UpdatePosition();

		if( S_OK == hr )
		{
			hr = l_Temp;
		}

		// if update position failed, we should not be updating buffer 
		// arrays.
		if (S_OK == l_Temp)
		{
			l_Temp = UpdateBufferArrays( p_ExcludePositionCheck, pErrorMessages );
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
		m_LastReset = SvTl::GetTimeStamp();
	}

	if ( S_OK != hr && nullptr != pErrorMessages && pErrorMessages->empty() )
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RebuildFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		pErrorMessages->push_back(Msg);
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

	if( ( m_ImageType != SvOi::SVImageTypeEnum::SVImageTypeMain ) && ( m_ImageType != SvOi::SVImageTypeEnum::SVImageTypeRGBMain ) )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && ( m_LastReset < l_pParentImage->GetLastResetTimeStamp() || m_LastReset < m_LastUpdate ) )
		{
			SVImageExtentClass l_ImageExtent = GetImageExtents();
			SVImagePropertiesClass l_ImageProperties = m_ImageInfo.GetImageProperties();
			SVImageInfoClass l_ImageInfo = l_pParentImage->GetImageInfo();

			l_ImageInfo.SetOwnerImage( GetUniqueObjectID() );

			l_Status = l_ImageInfo.SetImageProperties( l_ImageProperties );

			if( m_ImageType == SvOi::SVImageTypeEnum::SVImageTypeDependent || m_ImageType == SvOi::SVImageTypeEnum::SVImageTypeVirtual )
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

				m_LastUpdate = SvTl::GetTimeStamp();

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
	SVTaskObjectClass*	pParentTask = dynamic_cast <SVTaskObjectClass*> (GetTool());
 	if( ( m_ImageType != SvOi::SVImageTypeEnum::SVImageTypeRGBMain ) && ( nullptr != pParentTask ) )
	{
		SVImageExtentClass TempExtent;

		if( ( SvOi::SVImageTypeEnum::SVImageTypeMain != m_ImageType ) && 
			( SvOi::SVImageTypeEnum::SVImageTypeFixed != m_ImageType ) && 
			( SvOi::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType ) && 
			( SvOi::SVImageTypeEnum::SVImageTypeDependent != m_ImageType ) && 
			( SvOi::SVImageTypeEnum::SVImageTypeVirtual != m_ImageType ) && 
			( S_OK == pParentTask->GetImageExtent( TempExtent ) ) )
		{


			RECT l_Rect;

			if (( SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical == m_ImageType)|| 
				(SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType))
			{
				// @Hack
				// It does not make sense that a logical buffer is not a 1:1 
				// pixel correlation to its parent phisical buffer.  For this 
				// reason the translation type will be ignored when retrieving
				// the logical rectangle.  
				// The usage that this is specifically excluded for is for 
				// creating a logical ROI buffer, which should not reflect the 
				// output buffer translation.
				TempExtent.SetTranslation (SVExtentTranslationShift);
				l_Status = TempExtent.GetLogicalRectangle (l_Rect);
			}
			else
			{
				l_Status = TempExtent.GetOutputRectangle( l_Rect );
			}

			if( S_OK == l_Status )
			{
				if(	0 < ( l_Rect.bottom - l_Rect.top + 1 ) && 0 < ( l_Rect.right - l_Rect.left + 1 ) )
				{
					l_ToolExtent = TempExtent;
				}
			}
		}

		if( SvOi::SVImageTypeEnum::SVImageTypeFixed != m_ImageType && 
			SvOi::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType && 
			SvOi::SVImageTypeEnum::SVImageTypeDependent != m_ImageType )
		{
			SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (GetTool());
			if( nullptr != pTool )
			{
				pTool->SetToolImage( GetUniqueObjectID() );
			}
		}

		l_ToolID = GetTool()->GetUniqueObjectID(); 
	}

	if( m_ImageInfo.GetOwnerID() != l_ToolID )
	{
		// Set the owning Tool before setting the extents
		m_ImageInfo.SetOwner( l_ToolID );

		m_LastUpdate = SvTl::GetTimeStamp();
	}

	if( ( SvOi::SVImageTypeEnum::SVImageTypeMain != m_ImageType ) && ( SvOi::SVImageTypeEnum::SVImageTypeIndependent != m_ImageType ) )
	{
		l_Status = m_ImageInfo.SetExtents( l_ToolExtent );

		if( S_OK == l_Status )
		{
			m_LastUpdate = SvTl::GetTimeStamp();
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

	m_LastUpdate = SvTl::GetTimeStamp();

	return l_hrOk;
}

HRESULT SVImageClass::IsValidChild( const GUID& p_rChildID, SVImageInfoClass p_svImageInfo ) const
{
	HRESULT l_hrOk = S_OK;

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		assert( nullptr != l_pParentImage );

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
		if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
			SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
			SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
		{
			SVImageClass* l_pParentImage = GetParentImage();

			assert( nullptr != l_pParentImage );

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

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
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
HRESULT SVImageClass::GetChildImageHandle( const GUID& p_rChildID, SVSmartHandlePointer& p_rsvBufferHandle ) const
{
	HRESULT l_hrOk = S_FALSE;

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType|| 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_rsvBufferHandle );
			if ( S_FALSE == l_hrOk )
			{
				SVString MsgStr ( _T("S_FALSE == l_pParentImage->GetChildImageHandle( p_rChildID, p_rsvBufferHandle )") );

				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_5053_CHILDIMAGEHANDLESFALSE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );
			}
		}
		else // really? there are 2 conditions to the if statement above...
		{
			SVString MsgStr( _T("nullptr == l_pParentImage") );

			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5054_NULLPARENTIMAGE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );

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
				if( l_Iter->second.m_pImageHandles->GetImageHandle( p_rsvBufferHandle ) )
				{
					l_hrOk = S_OK;
				}
				else
				{
					SVString MsgStr( _T("if (l_Iter->second.m_pImageHandles->GetImageHandle( p_rsvBufferHandle )") );

					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_5055_NULLCHILDHANDLE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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
HRESULT SVImageClass::GetChildImageHandle( const GUID& p_rChildID, SVImageIndexStruct p_svBufferIndex, SVSmartHandlePointer &p_rsvBufferHandle ) const
{
	HRESULT l_hrOk = S_FALSE;

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
		{
			l_hrOk = l_pParentImage->GetChildImageHandle( p_rChildID, p_svBufferIndex, p_rsvBufferHandle );
			if ( S_FALSE == l_hrOk )
			{
				SVString MsgStr( _T("S_FALSE == l_pParentImage->GetChildImageHandle( m_svChildIndexArray[ p_lChildIndex ], p_rsvBufferHandle )") );

				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_5056_CHILDIMAGEHANDLESFALSE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );
			}

		}
		else  // really? there are 2 conditions to the if statement above...
		{
			SVString MsgStr( _T("nullptr == l_pParentImage") );

			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5057_NULLPARENTIMAGE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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
				SVDataManagerHandle l_Handle;

				GetImageIndex( l_Handle, p_svBufferIndex );

				if ( l_Iter->second.m_pImageHandles->GetImageHandle( l_Handle.GetIndex(), p_rsvBufferHandle ) )
				{
					l_hrOk = S_OK;
				}
				else
				{
					SVString MsgStr( _T("null ptr == l_Iter->second.m_pImageHandles->GetImageHandle( l_Handle.GetIndex(), p_rsvBufferHandle )") );

					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_5058_NULLCHILDHANDLE, MsgStr.c_str(), SvStl::SourceFileParams(StdMessageParams) );
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
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5059_GETCHILDERROR, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
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
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_5060_GETCHILDERROR, SvStl::Tid_GetParentImageHandleError, SvStl::SourceFileParams(StdMessageParams) );
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
		
	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType|| 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
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
			bool bIsPublished = (ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE) > 0;
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
	bool Result( false );
		
	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType|| 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
	{
		Result = nullptr != GetParentImage() && GetParentImage() != this;
	}
	else
	{
		Result = !( m_BufferArrayPtr.empty() );
		
		SVDataManagerHandle l_Handle;

		GetImageIndex( l_Handle, svIndex );

		if ( Result )
		{
			Result = m_BufferArrayPtr->SetCurrentIndex( l_Handle );
		}

		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( !( l_Iter->second.m_pImageHandles.empty() ) && !( l_Iter->second.m_pImageHandles->empty() ) )
			{
				Result &= l_Iter->second.m_pImageHandles->SetCurrentIndex( l_Handle );
			}

			++l_Iter;
		}
	}
	return Result;
}

/*
Updated method to use GetParentImage() method which validates the Parent Image pointer attribute.
The Parent Image attribute should not be used unless it is validated first.
*/
BOOL SVImageClass::CopyImageTo( SVImageIndexStruct svIndex )
{
	bool Result( false );
		
	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType|| 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
	{
		SVImageClass* l_pParentImage = GetParentImage();

		if( nullptr != l_pParentImage && l_pParentImage != this )
		{
			SVImageIndexStruct l_svIndex;

			Result = l_pParentImage->GetImageHandleIndex( l_svIndex ) ? true : false;

			Result = Result && (l_svIndex == svIndex);
		}
	}
	else
	{
		Result = !( m_BufferArrayPtr.empty() );
		
		SVDataManagerHandle l_Handle;

		GetImageIndex( l_Handle, svIndex );

		if ( Result )
		{
			Result = m_BufferArrayPtr->CopyValue( l_Handle );
		}

		SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

		while( l_Iter != m_ChildArrays.end() )
		{
			if( nullptr != l_Iter->second.m_pImageHandles )
			{
				Result &= l_Iter->second.m_pImageHandles->CopyValue( l_Handle );
			}

			++l_Iter;
		}
	}
	return Result;
}

BOOL SVImageClass::GetImageHandle( SVSmartHandlePointer& p_rHandlePtr )
{
	BOOL bOk = FALSE;
		
	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType|| 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
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
		
	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
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
			HRESULT l_Code;

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

			l_bOk = Unlock() && l_bOk && S_OK == l_Code;
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
			HRESULT l_Code;

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

			l_bOk = Unlock() && l_bOk && S_OK == l_Code;
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
						
			HRESULT l_Code;

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

			l_bOk = Unlock() && l_bOk && S_OK == l_Code;
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
			
			HRESULT l_Code;

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

			bool Status( true );

			SVGuidImageChildMap::iterator l_Iter = m_ChildArrays.begin();

			while( l_Iter != m_ChildArrays.end() )
			{
				if( nullptr != l_Iter->second.m_pImageHandles )
				{
					SVDataManagerHandle l_Handle;

					GetImageIndex( l_Handle, p_svToIndex );

					Status &= l_Iter->second.m_pImageHandles->SetCurrentIndex( l_Handle );
				}

				++l_Iter;
			}

			assert( Status );
			if ( S_OK == l_hrOk )
			{
				l_hrOk = Status ? S_OK : S_FALSE;
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
			//! Double casting required to avoid warnings from 64 to 32 bit conversion
			if ( dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)) )
			{
				long lRecursionCount = f_mapCritSec[dwThreadId];
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
	//! Double casting required to avoid warnings from 64 to 32 bit conversion
	assert( dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)) );
#endif

	try
	{
#ifdef _DEBUG
		//! Double casting required to avoid warnings from 64 to 32 bit conversion
		if ( dwThreadId == static_cast<DWORD> (reinterpret_cast<LONGLONG> (m_hCriticalSection.OwningThread)) )
		{
			long lRecursionCount = f_mapCritSec[dwThreadId];
			//assert( lRecursionCount == m_hCriticalSection.RecursionCount );
		}
		f_mapCritSec[dwThreadId]--;

		// BRW - In Windows 7, LockCount doesn't mean what it meant in Windows XP.
		// http://msdn.microsoft.com/en-us/library/windows/hardware/ff541979(v=vs.85).aspx
#endif

		::LeaveCriticalSection( &m_hCriticalSection );

		l_bOk = true;
	}
	catch ( ... )
	{
		l_bOk = false;
		assert( l_bOk == true );
	}

	assert( l_bOk == true );

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

		m_LastUpdate = SvTl::GetTimeStamp();
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

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
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
	long TempValue = 0;

	// Add image pixel depth as trivial members
	m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, TempValue );
	_variant_t Value(TempValue);
	rWriter.WriteAttribute(scPixelDepthTag, Value);

	// Add Band Number as trivial members
	m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, TempValue );
	Value = TempValue;
	rWriter.WriteAttribute(scBandNumberTag, Value);

	// Add Band Link as trivial members
	m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandLink, TempValue );
	Value = TempValue;
	rWriter.WriteAttribute(scBandLinkTag, Value);
}

HRESULT SVImageClass::GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const
{
	HRESULT hr = S_OK;

	if( _T("PixelDepth")  == rValueName )
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, Value );

		rValue = Value;
	}
	else if( _T("BandNumber") == rValueName )
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, Value );

		rValue = Value;
	}
	else if( _T("BandLink") == rValueName )
	{
		long Value = 0;

		m_ImageInfo.GetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandLink, Value );

		rValue = Value;
	}
	else
	{
		hr = SVObjectAppClass::GetObjectValue( rValueName, rValue );
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

	if ( ( bOk = PDataObject->GetAttributeData( "PixelDepth", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "BandNumber", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, svLongArray[i] );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "BandLink", svLongArray	) ) )
	{
		for ( int i = 0; i < svLongArray.GetSize(); i++ )
		{
			m_ImageInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandLink, svLongArray[i] );
		}
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue( PDataObject );

		m_LastUpdate = SvTl::GetTimeStamp();

		return hr;
	}

	m_LastUpdate = SvTl::GetTimeStamp();

	// At some point this can go away,because it's not longer being scripted
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

SVImageIndexStruct SVImageClass::GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap )
{
	SVImageIndexStruct svIndex;
	if( SvOi::SV_PUBLISH_RESULT_IMAGE == (ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE) )
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (GetInspection());
		assert( nullptr != pInspection );
		if ( nullptr != pHandle && nullptr != pInspection )
		{
			svIndex.m_PublishedResultDMIndexHandle.Assign( *pHandle, pHandle->GetLockType() );
		}
	}
	return svIndex;
}

//* JMS - New Image Object methods

HRESULT SVImageClass::UpdatePosition()
{
	HRESULT l_Status = S_OK;
	
	m_ParentImageInfo.second = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( m_ParentImageInfo.first ) );

	if( SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType )
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

			if( SvOi::SV_PUBLISH_RESULT_IMAGE == ( ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE ) )
			{
				l_Size = SvOi::GetSourceImageDepth();
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

		SvStl::MessageContainerVector errorMessage;
		bool Result = p_psvChildBuffers->ResetObject(&errorMessage);
		if (!Result && !errorMessage.empty())
		{
			l_Status = errorMessage[0].getMessage().m_MessageCode;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}
// JMS - New Image Object methods */

HRESULT SVImageClass::UpdateBufferArrays( bool p_ExcludePositionCheck, SvStl::MessageContainerVector *pErrorMessages )
{
	HRESULT		l_Status = S_OK;

	if ((SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical == m_ImageType ||
		 SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType) &&
		nullptr == m_ParentImageInfo.second)
	{
		// If the image type is logical, there MUST be a parent before 
		// attempting to set up image buffers.
		return l_Status;
	}

	bool	clear = false;
	SVGUID	parentGuid;
	parentGuid = m_ImageInfo.GetOwnerImageID ();
	if( SvOi::SVImageTypeEnum::SVImageTypeUnknown == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType || 
		SvOi::SVImageTypeEnum::SVImageTypeMain == m_ImageType ||
		SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType )
		
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

			if( SvOi::SV_PUBLISH_RESULT_IMAGE == ( ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE ) )
			{
				l_Size = SvOi::GetSourceImageDepth();
			}

			bool l_Reset = false;
			bool l_Update = false;

			SVImageObjectClass::SVImageObjectClassPtr parent = m_BufferArrayPtr->GetParentImageObject();

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
					SvStl::MessageContainerVector currentMessages;
					m_BufferArrayPtr->ResetObject(&currentMessages);

					if (!currentMessages.empty())
					{
						l_Status = currentMessages[0].getMessage().m_MessageCode;
						if (nullptr != pErrorMessages)
						{
							GUID thisGuid = GetUniqueObjectID();
							for( SvStl::MessageContainerVector::const_iterator Iter( currentMessages.begin() ); currentMessages.end() != Iter; ++Iter)
							{
								SvStl::MessageData messageData = Iter->getMessage();
								if (SVMSG_SVO_5067_IMAGEALLOCATIONFAILED == messageData.m_MessageCode)
								{
									SVStringVector msgList;
									msgList.push_back(GetCompleteName());
									messageData.m_AdditionalTextList = msgList;
								}

								SvStl::MessageContainer message;
								message.setMessage(messageData, thisGuid);
								pErrorMessages->push_back( message );
							}
						}
					}
				}
				else
				{
					m_BufferArrayPtr->UpdateTimeStamp();
				}
			}
		}
		else // if (!( m_BufferArrayPtr.empty() )
		{
			l_Status = E_FAIL;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if (S_FALSE == l_Status)
	{
		l_Status = E_FAIL;
	}
	return l_Status;
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

				if( nullptr == l_pImage || SvOi::SVImageTypeEnum::SVImageTypeDependent != m_ImageType || SvOi::SVImageTypeEnum::SVImageTypeVirtual != l_pImage->GetImageType() )
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

// Method to keep the Translation Offset in sync between the ToolExtents and ImageExtents
// Called by SVShitftTool::oRun
void SVImageClass::SetTranslationOffset(double offsetX, double offsetY)
{
	m_ImageInfo.SetExtentProperty(SVExtentPropertyTranslationOffsetX, offsetX);
	m_ImageInfo.SetExtentProperty(SVExtentPropertyTranslationOffsetY, offsetY);
	SVImageObjectClassPtr ptr = GetBufferArrayPtr();
	ptr->SetTranslationOffset(offsetX, offsetY);
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
SvOi::SVImageTypeEnum SVImageClass::GetImageType() const
{
	return m_ImageType;
}

SvOi::ISVImage* SVImageClass::GetParentImageInterface() const
{
	return GetParentImage();
}

SvOi::MatroxImageSmartHandlePtr SVImageClass::getImageData()
{
	SVSmartHandlePointer handle;
	SvOi::MatroxImageSmartHandlePtr dataSmartPointer;

	if ((SvOi::SVImageTypeEnum::SVImageTypeLogical == m_ImageType) ||
		(SvOi::SVImageTypeEnum::SVImageTypeDependent == m_ImageType) ||
		(SvOi::SVImageTypeEnum::SVImageTypeVirtual == m_ImageType))
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

SVString SVImageClass::getDisplayedName() const
{
	const SVObjectTypeInfoStruct& rObjectTypeInfo = GetObjectInfo().m_ObjectTypeInfo;
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

long SVImageClass::getBands() const
{
	long bandNumber = 0;
	m_ImageInfo.GetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, bandNumber);
	return bandNumber;
}

long SVImageClass::getPixelDepth() const
{
	long pixelDepth = 0;
	m_ImageInfo.GetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, pixelDepth);
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
		SVMatroxFileTypeEnum efileformat( SVMatroxImageInterface::getFileType( ext.c_str() ) );
	
		if (efileformat != SVFileUnknown)
		{
			SVString strPath = rFilename.c_str();
			HRESULT l_Code = SVMatroxBufferInterface::Export(MilHandle.GetBuffer(), strPath, efileformat);
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

bool SVImageClass::ValidateImage()
{
	if( IsCreated() )
	{
		switch( m_ImageType )
		{
			case SvOi::SVImageTypeEnum::SVImageTypePhysical:
			case SvOi::SVImageTypeEnum::SVImageTypeIndependent:
			case SvOi::SVImageTypeEnum::SVImageTypeFixed:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_BufferPtr );
				m_isObjectValid = m_isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				break;
			}
			case SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_BufferPtr );
				m_isObjectValid = m_isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				SVImageClass* l_pParent = GetParentImage();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_pParent );
				m_isObjectValid = m_isObjectValid && ( S_OK == l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) );

				break;
			}
			case SvOi::SVImageTypeEnum::SVImageTypeLogical:
			case SvOi::SVImageTypeEnum::SVImageTypeVirtual:
			case SvOi::SVImageTypeEnum::SVImageTypeDependent:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_BufferPtr );

				SVImageClass* l_pParent = GetParentImage();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_pParent );
				m_isObjectValid = m_isObjectValid && ( S_OK == l_pParent->IsValidChild( GetUniqueObjectID(), m_ImageInfo ) );

				break;
			}
			case SvOi::SVImageTypeEnum::SVImageTypeMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_BufferPtr );

				break;
			}
			case SvOi::SVImageTypeEnum::SVImageTypeRGBMain:
			{
				SVImageObjectClassPtr l_BufferPtr = GetBufferArrayPtr();

				m_isObjectValid = m_isObjectValid && ( nullptr != l_BufferPtr );
				m_isObjectValid = m_isObjectValid && ( m_ImageInfo == l_BufferPtr->GetImageInfo() );

				break;
			}
			default:
			{
				break;
			}
		}
	}
	return (TRUE == m_isObjectValid);
}

HRESULT SVImageClass::GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const
{
	HRESULT l_Status( S_OK );

	if( (ObjectAttributesSet() & SvOi::SV_PUBLISH_RESULT_IMAGE) > 0 )
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

	SVObjectManagerClass::Instance().RegisterSubObject(GetUniqueObjectID());
	return l_Status;
}

HRESULT SVImageClass::UnregisterAsSubObject()
{
	HRESULT l_Status = S_OK;

	SVObjectManagerClass::Instance().UnregisterSubObject(GetUniqueObjectID());

	return l_Status;
}
