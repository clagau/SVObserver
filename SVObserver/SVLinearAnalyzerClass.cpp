// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearAnalyzerClass
// * .File Name       : $Workfile:   SVLinearAnalyzerClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   15 May 2014 13:15:14  $
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

HRESULT SVLinearAnalyzerClass::GetThresholdBarsOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( l_hrOk == S_OK && p_psvEdge != NULL )
	{
		SVExtentMultiLineStruct l_svMultiLine;

		if( p_psvEdge->GetThresholdBarsOverlay( l_svMultiLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svMultiLine, l_svMultiLine );

			p_rsvMiltiLine.m_svLineArray.Append( l_svMultiLine.m_svLineArray );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetHistogramOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( l_hrOk == S_OK && p_psvEdge != NULL )
	{
		SVExtentLineStruct l_svLine;

		if( p_psvEdge->GetHistogramOverlay( l_svLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetEdgesOverlay( SVLinearEdgeProcessingClass *p_psvEdge, SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( l_hrOk == S_OK && p_psvEdge != NULL )
	{
		SVExtentMultiLineStruct l_svMultiLine;

		if( p_psvEdge->GetEdgesOverlay( l_svMultiLine ) == S_OK )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svMultiLine, l_svMultiLine );

			p_rsvMiltiLine.m_svLineArray.Append( l_svMultiLine.m_svLineArray );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
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
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearAnalyzerClass.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 13:15:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 11:13:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   02 May 2013 11:21:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:47:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   01 Aug 2012 12:53:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   27 Jul 2012 08:55:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   25 Jul 2012 14:27:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Jul 2012 13:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   16 Jul 2012 09:01:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   12 Jul 2012 14:58:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Jul 2012 17:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Jun 2012 12:57:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   13 Mar 2012 13:43:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SetupDialog to remove un-necessary calls through IPDoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Mar 2011 13:40:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   25 Feb 2011 12:17:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   27 Jan 2011 11:28:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   08 Dec 2010 12:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   01 Jun 2010 14:41:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Apr 2009 14:58:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Jun 2007 13:27:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   05 Oct 2005 11:06:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added code to check if value objects have an owner before access.  This prevent un-register embedded objects from being accessed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Aug 2005 09:42:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Aug 2005 09:36:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary casting from AddInputRequest method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   12 Aug 2005 13:50:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to fix issues with data synchrization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Aug 2005 16:07:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2005 08:11:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Mar 2005 11:19:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added processmessage so that the ResetObject would get called.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Mar 2005 14:39:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated GetImageExtent method to handle issues with invalid tool sizes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Mar 2005 13:44:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed onCollectOverlays to include the drawing of the selected edges.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
