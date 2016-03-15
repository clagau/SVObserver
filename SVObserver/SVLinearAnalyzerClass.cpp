// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearAnalyzerClass
// * .File Name       : $Workfile:   SVLinearAnalyzerClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   10 Jul 2014 17:44:52  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearAnalyzerClass.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVImageClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearAnalyzerClass, SVLinearAnalyzerClassGuid );


SVLinearAnalyzerClass::SVLinearAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					            :SVAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;

	m_svInputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_svInputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputImageObjectInfo, _T( "LinearAnalyzerImage" ) );

	m_svInputProfileOrientation.SetInputObjectType( SVProfileOrientationGuid, SVEnumValueObjectType );
	m_svInputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputProfileOrientation, _T( "LinearAnalyzerOrientation" ) );

	m_svInputUseRotationAngle.SetInputObjectType( SVLinearToolUseRotationGuid, SVBoolValueObjectType );
	m_svInputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputUseRotationAngle, _T( "LinearAnalyzerUseRotationAngle" ) );

	m_lPixelDepth = 0;
	m_dwMinThreshold = 0;
	m_dwMaxThreshold = 0;
	m_dwColorNumber = 0;

	m_bEnableDirection = FALSE;
	m_bEnableEdgeSelect = FALSE;
	m_bEnablePolarisation = FALSE;
	m_bEnablePosition = FALSE;
	m_bEnableThreshold = FALSE;

	m_svShowAllEdgeAOverlays.SetDefaultValue( FALSE, TRUE );
	m_svShowAllEdgeBOverlays.SetDefaultValue( FALSE, TRUE );

	addDefaultInputObjects();
}

SVLinearAnalyzerClass::~SVLinearAnalyzerClass()
{
}

BOOL SVLinearAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
  SVImageInfoClass ImageInfo;

	BOOL bOk = SVAnalyzerClass::CreateObject( PCreateStructure );

	bOk = bOk && GetPixelDepth() == S_OK;

  if ( bOk )
  {
		// Set range...
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( m_lPixelDepth ));	
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( m_lPixelDepth ));	
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( m_lPixelDepth ));

		m_svNormalizer.SetNormalRange( m_dwMinThreshold, m_dwMaxThreshold );
		m_svNormalizer.SetRealRange( m_dwMinThreshold, m_dwMaxThreshold );

		m_svShowAllEdgeAOverlays.ObjectAttributesAllowedRef() |= SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
		m_svShowAllEdgeBOverlays.ObjectAttributesAllowedRef() |= SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	}

	return bOk;
}

HRESULT SVLinearAnalyzerClass::ResetObject()
{
	HRESULT l_hrOk = SVAnalyzerClass::ResetObject();

	if( GetPixelDepth() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

  if ( l_hrOk == S_OK )
  {
		// Set range...
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( m_lPixelDepth ));	
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( m_lPixelDepth ));	
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( m_lPixelDepth ));

		m_svNormalizer.SetNormalRange( m_dwMinThreshold, m_dwMaxThreshold );
		m_svNormalizer.SetRealRange( m_dwMinThreshold, m_dwMaxThreshold );
	}

	return l_hrOk;
}

BOOL SVLinearAnalyzerClass::OnValidate()
{
	if( SVAnalyzerClass::OnValidate() )
	{
		if( GetInputImage() != NULL )
		{
			return TRUE;
		}
	}

	SetInvalid();
	return FALSE;
}

HRESULT SVLinearAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( l_hrOk == S_OK && GetEdgeA() != NULL )
	{
		SVExtentLineStruct l_svLine;

		if( GetEdgeA()->GetSelectedEdgeOverlay( l_svLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	if( l_hrOk == S_OK && GetEdgeB() != NULL )
	{
		SVExtentLineStruct l_svLine;

		if( GetEdgeB()->GetSelectedEdgeOverlay( l_svLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent )
{
	SVExtentPointStruct l_svPosition;
	SVExtentPointStruct l_svPositionEndOfLine;
	SVImageExtentClass l_svExtents;

	long l_lWidth = 0;
	long l_lHeight = 0;

	double l_dAngle = 0.0;

	BOOL l_bUseRotation = FALSE;

	HRESULT l_hrOk = p_rsvImageExtent.Initialize();

	if( GetTool() == NULL || 
		  GetInputUseRotationAngle() == NULL ||
	    ( S_OK != GetInputUseRotationAngle()->GetValue( l_bUseRotation ) ) ||
		  GetInputProfileOrientation() == NULL ||
	    ( S_OK != GetInputProfileOrientation()->GetValue( l_dAngle ) ) ||
			GetTool()->GetImageExtent( l_svExtents ) != S_OK ||
			l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_lWidth ) != S_OK ||
			l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_lHeight ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if( l_hrOk == S_OK )
	{
		p_rsvImageExtent.SetTranslation( SVExtentTranslationLinear );

		if( l_bUseRotation )
		{
			p_rsvImageExtent.SetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
			p_rsvImageExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
			p_rsvImageExtent.SetExtentProperty( SVExtentPropertyWidth, l_lWidth );
			p_rsvImageExtent.SetExtentProperty( SVExtentPropertyHeight, l_lHeight );
		}
		else
		{
			if( l_dAngle == 0.0 )
			{
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyWidth, l_lWidth );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyHeight, l_lHeight );
			}
			else if( l_dAngle == 90.0 )
			{
				l_svPosition.m_dPositionX = l_lWidth - 1;

				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyRotationAngle, l_dAngle );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyWidth, l_lHeight );
				p_rsvImageExtent.SetExtentProperty( SVExtentPropertyHeight, l_lWidth );
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}

		l_hrOk = p_rsvImageExtent.UpdateData();
	}

	return l_hrOk;
}

SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeA()
{
	SVLinearEdgeProcessingClass *l_psvEdge = NULL;

	SVObjectTypeInfoStruct  info;

	info.ObjectType = SVLinearEdgeProcessingObjectType;
	info.SubType = SVLinearEdgeAProcessingObjectType;

	l_psvEdge = reinterpret_cast<SVLinearEdgeProcessingClass *>(SVSendMessage( this, 
																SVM_GETFIRST_OBJECT,
																NULL,
																reinterpret_cast<DWORD_PTR>(&info)));

	return l_psvEdge;
}

SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeB()
{
	SVLinearEdgeProcessingClass *l_psvEdge = NULL;

	SVObjectTypeInfoStruct  info;

	info.ObjectType = SVLinearEdgeProcessingObjectType;
	info.SubType = SVLinearEdgeBProcessingObjectType;

	l_psvEdge = reinterpret_cast<SVLinearEdgeProcessingClass *>(SVSendMessage( this, 
																	SVM_GETFIRST_OBJECT,
																	NULL,
																	reinterpret_cast<DWORD_PTR>(&info)));

	return l_psvEdge;
}

SVImageClass *SVLinearAnalyzerClass::GetInputImage()
{
	SVImageClass *l_psvImage = NULL;

	if( m_svInputImageObjectInfo.IsConnected() && 
		  m_svInputImageObjectInfo.GetInputObjectInfo().PObject != NULL )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(m_svInputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return l_psvImage;
}

HRESULT SVLinearAnalyzerClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;

	if( GetInputImage() != NULL )
	{
    SVImageInfoClass ImageInfo = GetInputImage()->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

SVEnumerateValueObjectClass *SVLinearAnalyzerClass::GetInputProfileOrientation()
{
	SVEnumerateValueObjectClass *l_psvValue = NULL;

	if( m_svInputProfileOrientation.IsConnected() && 
		  m_svInputProfileOrientation.GetInputObjectInfo().PObject != NULL )
	{
		l_psvValue = dynamic_cast<SVEnumerateValueObjectClass *>(m_svInputProfileOrientation.GetInputObjectInfo().PObject);
	}

	return l_psvValue;
}

SVBoolValueObjectClass* SVLinearAnalyzerClass::GetInputUseRotationAngle()
{
	if( m_svInputUseRotationAngle.IsConnected() && 
		  m_svInputUseRotationAngle.GetInputObjectInfo().PObject )
		return dynamic_cast<SVBoolValueObjectClass *>(m_svInputUseRotationAngle.GetInputObjectInfo().PObject);

	return NULL;
}


HRESULT SVLinearAnalyzerClass::onCollectOverlays(SVImageClass *p_Image,SVExtentMultiLineStructCArray &p_MultiLineArray)
{
	HRESULT l_hrRet = S_OK;
	
	SVExtentFigureStruct l_svFigure;
	SVExtentMultiLineStruct l_svMultiLine;

	SVImageExtentClass l_svToolExtents;
	SVImageExtentClass l_svAnalyzerExtents;
	BOOL l_bOk = TRUE;

	l_bOk = l_bOk && GetTool() != NULL && GetTool()->GetImageExtent( l_svToolExtents ) == S_OK;
	l_bOk = l_bOk && GetImageExtent( l_svAnalyzerExtents ) == S_OK;
	

	if ( l_svAnalyzerExtents.GetFigure(l_svFigure) == S_OK )
	{
		COLORREF l_Color = 0;

		statusColor.GetValue( l_Color );

		l_svMultiLine.m_Color = l_Color;

		l_svToolExtents.TranslateFromOutputSpace(l_svFigure,l_svFigure);
		l_svMultiLine.AssignExtentFigure( l_svFigure, l_Color );

		l_svMultiLine.m_csString = "";

		UpdateOverlayIDs( l_svMultiLine );

		p_MultiLineArray.Add( l_svMultiLine );
	
		l_svMultiLine.Initialize();

		BOOL l_bShow = FALSE;
		if ( GetEdgeA() != NULL )
		{ 
			if( m_svShowAllEdgeAOverlays.GetOwner() != NULL && 
				( m_svShowAllEdgeAOverlays.GetValue(l_bShow) == S_OK ) && l_bShow )
			{
				SVExtentLineStruct l_svLine;

				l_svLine.m_dwColor = l_Color;

				if ( GetEdgeA()->GetHistogramOverlay(l_svLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svMultiLine.m_svLineArray.Add(l_svLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( GetEdgeA()->GetThresholdBarsOverlay(l_svMultiLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( GetEdgeA()->GetEdgesOverlay(l_svMultiLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}

			}
		}
		if ( GetEdgeB() != NULL )
		{ 
			if( m_svShowAllEdgeBOverlays.GetOwner() != NULL && 
				( m_svShowAllEdgeBOverlays.GetValue(l_bShow) == S_OK ) && l_bShow )
			{
				SVExtentLineStruct l_svLine;

				l_svLine.m_dwColor = l_Color;

				if ( GetEdgeB()->GetHistogramOverlay(l_svLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svMultiLine.m_svLineArray.Add(l_svLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( GetEdgeB()->GetThresholdBarsOverlay(l_svMultiLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( GetEdgeB()->GetEdgesOverlay(l_svMultiLine) == S_OK )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.Add(l_svMultiLine);
					l_svMultiLine.Initialize();
				}

			}
		}

		l_hrRet = S_OK;
	}
	else
	{
		l_hrRet = S_FALSE;
	}

	if ( l_bOk )
	{
		SVExtentMultiLineStruct l_svMultiLine;

		if ( GetSelectedEdgeOverlays( l_svMultiLine ) == S_OK )
		{
			l_svToolExtents.TranslateFromOutputSpace( l_svMultiLine, l_svMultiLine );

			statusColor.GetValue(l_svMultiLine.m_Color);

			UpdateOverlayIDs( l_svMultiLine );

			p_MultiLineArray.Add( l_svMultiLine );
		}
	}
	return l_hrRet;
}

DWORD_PTR SVLinearAnalyzerClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
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

	return (DwResult | SVAnalyzerClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}
