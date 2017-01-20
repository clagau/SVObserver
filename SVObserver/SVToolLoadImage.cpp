//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVToolLoadImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"
#include "SVToolLoadImage.h"
#include "SVAnalyzer.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "ToolSizeAdjustTask.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SV_IMPLEMENT_CLASS( SVLoadImageToolClass, SVLoadImageToolClassGuid );

SVLoadImageToolClass::SVLoadImageToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					 :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVLoadImageToolClass::init()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVToolLoadImageObjectType;

	// Identify our input type needs
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_fileImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_currentPathName, SVPathNameObjectGuid, IDS_OBJECTNAME_PATHNAME, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_continuousReload, SVContinuousReloadObjectGuid, IDS_OBJECTNAME_CONTINUOUS_RELOAD, false, SVResetItemNone );

	// Set Embedded defaults
	m_currentPathName.SetDefaultValue( _T( "" ), true );
	m_continuousReload.SetDefaultValue( false, true );

	m_fileImage.InitializeImage( SVImageTypePhysical );

	// Default taskObjectList items:

	// Set Translation
	m_svToolExtent.SetTranslation(SVExtentTranslationFigureShift);
	ToolSizeAdjustTask::AddToFriendlist(this, true, true, false);
	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLoadImageToolClass::~SVLoadImageToolClass()
{ 
}

BOOL SVLoadImageToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		SVImageClass* pImage = nullptr;

		// Create Image...
		SVImageInfoClass l_svImageInfo = m_fileImage.GetImageInfo();

		// Setup...
		l_svImageInfo.SetOwner( GetUniqueObjectID() );
		l_svImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
		l_svImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
		l_svImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
		l_svImageInfo.SetImageProperty(SVImagePropertyPixelDepth,8);

		bOk = ( S_OK == m_fileImage.UpdateImage( l_svImageInfo ) );
	}

	// Set / Reset Printable Flags
	m_fileImage.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_currentPathName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_continuousReload.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	if (bOk)
	{
		bOk = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false)); 
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLoadImageToolClass::CloseObject()
{
	if( SVToolClass::CloseObject() )
	{
		BOOL bRetVal = m_fileImage.CloseObject();

		return bRetVal;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Make sure we have an image file to load.
//
BOOL SVLoadImageToolClass::OnValidate ()
{
	if (SVToolClass::OnValidate ())
	{
		SVString PathName;
		m_currentPathName.GetValue( PathName );
		
		if( ::SVFileExists( PathName.c_str() ) )
		{
			return true;
		}
	}
	SetInvalid ();
	return false;
}

BOOL SVLoadImageToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVToolClass::onRun( RRunStatus ) )
	{
		BOOL bReload = false;
		SVString ImagePathName;

		if( S_OK != m_continuousReload.GetValue( bReload ) )
		{
			RRunStatus.SetInvalid();
			SetInvalid();
			return false;
		}
		
		if( S_OK != m_currentPathName.GetValue( ImagePathName ) )
		{
			RRunStatus.SetInvalid();
			SetInvalid();
			return false;
		}

		if( bReload || m_bResetFileImage)
		{
			if( S_OK != m_fileImage.LoadImage( ImagePathName.c_str(), RRunStatus.Images ) )
			{
				RRunStatus.SetInvalid();
				SetInvalid();

				return false;
			}

			m_bResetFileImage = false;
		}
		else
		{
			//copy forward
			if( ! m_fileImage.CopyImageTo( RRunStatus.Images ) )
			{
				RRunStatus.SetInvalid();
				SetInvalid();

				return false;
			}
		}
		
		return true;
	}

	RRunStatus.SetInvalid();

	return FALSE;
}

HRESULT SVLoadImageToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;
	
	if ( S_OK != SVToolClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	m_bResetFileImage = S_OK == l_hrOk;

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::IsInputImage(SVImageClass *p_psvImage)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_psvImage )
	{
		if ( p_psvImage == m_pCurrentToolSet->getCurrentImage() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = SVToolClass::IsInputImage( p_psvImage );
		}
	}

	return l_hrOk;
}

SVTaskObjectClass *SVLoadImageToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
	    SVExtentLocationPropertyUnknown != l_svExtents.GetLocationPropertyAt( p_rsvPoint ) )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVLoadImageToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVLoadImageToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentFigureStruct l_svFigure;

	p_svImageExtent.GetFigure(l_svFigure);

	if ( (l_svFigure.m_svTopLeft.m_dPositionX >= 0) && (l_svFigure.m_svTopLeft.m_dPositionY >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::GetParentExtent( SVImageExtentClass& rParentExtent ) const
{
	HRESULT hr = S_OK;
	SVString ImagePathName;
	if( S_OK == hr )
	{
		hr = m_currentPathName.GetValue( ImagePathName );
	}
	if( S_OK == hr )
	{
		hr = SVImageObjectClass::GetImageExtentFromFile( ImagePathName.c_str(), rParentExtent );
	}
	return hr;
}

HRESULT SVLoadImageToolClass::SetImageExtentToParent(unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;
	SVString ImagePathName;

	if( S_OK == l_hrOk )
	{
		l_hrOk = m_currentPathName.GetValue( ImagePathName );
	}

	if( S_OK == l_hrOk )
	{
		l_hrOk = m_fileImage.LoadImageFullSize( ImagePathName.c_str(), l_NewExtent);
	}
		
	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

BOOL SVLoadImageToolClass::IsValid()
{
	BOOL bValid = TRUE;

	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if(nullptr != pToolSizeAdjustTask)
	{
			bValid =  pToolSizeAdjustTask->OnValidate();
	}

	return SVToolClass::IsValid() & bValid ;
}

