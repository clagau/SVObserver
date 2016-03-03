//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineAnalyzer
//* .File Name       : $Workfile:   SVLineAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:14:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVLineAnalyzer.h"

#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVGageTool.h"
#include "SVImageClass.h"
#include "SVImageToLineProject.h"
#include "SVLineROI.h"
#include "SVTool.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVLineAnalyzerClass, SVLineAnalyzerClassGuid );


SVLineAnalyzerClass::SVLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;

	// Identify our input type needs
	inputLineObjectInfo.SetInputObjectType( SVLineObjectType );
	inputLineObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputLineObjectInfo, _T( "LineAnalyzerLine" ) );

	// Register Embedded Objects
	/////////////////////////////////////////////////////////////////////////
	// Extents - These are shadows of the extents in the SVLineInfoClass,
	// so that they can be exposed and used by other objects in the system.
	// They are not updated directly (at this point in time).
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &extentX1, SVExtentRelativeX1PositionObjectGuid, IDS_OBJECTNAME_EXTENT_X1, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &extentY1, SVExtentRelativeY1PositionObjectGuid, IDS_OBJECTNAME_EXTENT_Y1, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &extentX2, SVExtentRelativeX2PositionObjectGuid, IDS_OBJECTNAME_EXTENT_X2, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &extentY2, SVExtentRelativeY2PositionObjectGuid, IDS_OBJECTNAME_EXTENT_Y2, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &extentLength, SVExtentLengthObjectGuid, IDS_OBJECTNAME_EXTENT_LENGTH, false, SVResetItemNone, _T("Extent Length") );

	m_svLineExtent.SetTool( NULL );
	m_svLineExtent.SetImageType( SVImageTypeUnknown );
	m_svLineExtent.SetTranslation( SVExtentTranslationLine );
	m_svLineExtent.SetExtentObject( SVExtentPropertyPositionPointX, &extentX1 );
	m_svLineExtent.SetExtentObject( SVExtentPropertyPositionPointY, &extentY1 );
	m_svLineExtent.SetExtentObject( SVExtentPropertyPositionPointEndOfLineX, &extentX2 );
	m_svLineExtent.SetExtentObject( SVExtentPropertyPositionPointEndOfLineY, &extentY2 );

	// Set Embedded defaults
	/////////////////////////////////////////////////////////////////////////
	// Set Default values for Shadowed Extents
	/////////////////////////////////////////////////////////////////////////
	extentX1.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	extentY1.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );

	extentX2.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTH + SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	extentY2.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHT + SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );

	extentLength.SetDefaultValue( 0.0, TRUE );

	m_bDisplayAnalyzeLine = FALSE;
	m_bDisplayAnalyzeEdge = TRUE;

	addLineAnalyzerFriend();
}

SVLineAnalyzerClass::~SVLineAnalyzerClass()
{

}

BOOL SVLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVAnalyzerClass::CreateObject( PCreateStructure );

	SVLineClass *l_psvLine = getInputLine();

	bOk = bOk && l_psvLine != NULL && GetTool() != NULL;

	if( bOk && dynamic_cast<SVGageToolClass *>( GetTool() ) == NULL )
	{
		SVImageExtentClass l_svExtents;

		double l_dAngle = 0.0;

		if( GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
			  l_psvLine->m_svLineExtent.GetExtentProperty( SVExtentPropertyOldRotationAngle, l_dAngle ) == S_OK )
		{
			SVExtentPointStruct l_svCenter;
			SVExtentPointStruct l_svEnd;

			extentX1.GetValue( l_svCenter.m_dPositionX );
			extentY1.GetValue( l_svCenter.m_dPositionY );
			extentX2.GetValue( l_svEnd.m_dPositionX );
			extentY2.GetValue( l_svEnd.m_dPositionY );

			double l_dLineAngle = ::SVGetRotationAngle( l_svCenter, l_svEnd );
			if( l_dLineAngle != l_dAngle )
			{
				l_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, l_dLineAngle );

				GetTool()->SetImageExtent( 1, l_svExtents );
			}

			l_psvLine->m_svLineExtent.ClearOldProperties();
		}
	}

	if( bOk )
	{
		extentLength.SetValue( 1, getInputLine()->mlLinePixelLength );
	}

	extentX1.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentY1.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentX2.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentY2.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentLength.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	extentX1.ObjectAttributesAllowedRef() &= (~SV_SETABLE_ONLINE & ~SV_REMOTELY_SETABLE);
	extentY1.ObjectAttributesAllowedRef() &= (~SV_SETABLE_ONLINE & ~SV_REMOTELY_SETABLE);
	extentX2.ObjectAttributesAllowedRef() &= (~SV_SETABLE_ONLINE & ~SV_REMOTELY_SETABLE);
	extentY2.ObjectAttributesAllowedRef() &= (~SV_SETABLE_ONLINE & ~SV_REMOTELY_SETABLE);

	m_bSetupEdgeA = TRUE;

	isCreated = bOk;

	return bOk;
}

BOOL SVLineAnalyzerClass::CloseObject()
{
	m_svLineExtent.SetToolImage( NULL );
	m_svLineExtent.SetSelectedImage( NULL );

	return SVAnalyzerClass::CloseObject();
}

BOOL SVLineAnalyzerClass::DisconnectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	BOOL l_Status( SVAnalyzerClass::DisconnectInput( pInObjectInfo ) && pInObjectInfo != NULL );

	if( pInObjectInfo != NULL )
	{
		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svLineExtent.GetToolImage() )
		{
			m_svLineExtent.SetToolImage( NULL );
		}

		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svLineExtent.GetSelectedImage() )
		{
			m_svLineExtent.SetSelectedImage( NULL );
		}
	}

	return l_Status;
}

HRESULT SVLineAnalyzerClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if( SVAnalyzerClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

// Add LineROI friend to GageTool LineAnalyzers.
// Add Project friend to ProfileTool LineAnalyzers.
void SVLineAnalyzerClass::addLineAnalyzerFriend()
{
	SVObjectClass* pOwner;
	if( pOwner = GetOwner() )
	{
		if( friendList.size() <= 0 )
		{
			const SVObjectTypeInfoStruct& rOwnerType = pOwner->GetObjectInfo().ObjectTypeInfo;
			switch( rOwnerType.SubType )
			{
				case SVToolProfileObjectType:
				{
					SVImageToLineProjectClass* pProject = new SVImageToLineProjectClass;
					if( pProject )
					{
						pProject->SetObjectOwner( this );
						AddFriend( pProject->GetUniqueObjectID() );
							
						// Remove Embedded Extents
						// for old profile tool
						if( pOwner->GetClassID() == SVProfileToolClassGuid )
							removeEmbeddedExtents();
					}
					break;
				}

				default:
				{
					SVLineROIClass* pROI = new SVLineROIClass;
					if( pROI )
					{
						pROI->SetObjectOwner( this );
						AddFriend( pROI->GetUniqueObjectID() );
					}
				}
			};

			// Add Default Inputs and Outputs
			addDefaultInputObjects();
		}
	}
}

BOOL SVLineAnalyzerClass::SetObjectOwner( SVObjectClass* PNewOwner )
{
	if( SVAnalyzerClass::SetObjectOwner( PNewOwner ) )
	{
		addLineAnalyzerFriend();
		return TRUE;
	}
	return FALSE;
}

// Set new object owner using owner GUID.
// Uses TheObjectManager to get the owner pointer.
// Returns: BOOL - FALSE, if owner not exists.
BOOL SVLineAnalyzerClass::SetObjectOwner( const GUID& RNewOwnerGUID )
{
	if( SVAnalyzerClass::SetObjectOwner( RNewOwnerGUID ) )
	{
		addLineAnalyzerFriend();
		return TRUE;
	}
	return FALSE;
}


SVLineClass* SVLineAnalyzerClass::getInputLine()
{
	if( inputLineObjectInfo.IsConnected() && inputLineObjectInfo.GetInputObjectInfo().PObject )
		return ( SVLineClass* ) inputLineObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVLineAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVAnalyzerClass::onRun( RRunStatus );

	if( GetTool() != NULL )
	{
		SVGageToolClass *l_psvGageTool = dynamic_cast<SVGageToolClass *>( GetTool() );

		if( l_psvGageTool == NULL )
		{
			SVImageExtentClass l_svExtents;
			SVExtentFigureStruct l_svFigure;

			double l_dValue = 0.0;

			if ( GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
					 l_svExtents.GetFigure( l_svFigure ) == S_OK )
			{
				if( extentX1.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svCenterLeft.m_dPositionX);

					extentX1.SetValue( RRunStatus.m_lResultDataIndex, l_lValue );
				}
				
				if( extentY1.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svCenterLeft.m_dPositionY);

					extentY1.SetValue( RRunStatus.m_lResultDataIndex, l_lValue );
				}

				if( extentX2.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svCenterRight.m_dPositionX);

					extentX2.SetValue( RRunStatus.m_lResultDataIndex, l_lValue );
				}
				
				if( extentY2.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					long l_lValue = static_cast<long>(l_svFigure.m_svCenterRight.m_dPositionY);

					extentY2.SetValue( RRunStatus.m_lResultDataIndex, l_lValue );
				}
			}
			else
			{
				l_bOk = false;
			}
		}
	}
	else
	{
		l_bOk = false;
	}

	l_bOk = l_bOk && getInputLine() != NULL;
	l_bOk = l_bOk && ( extentLength.SetValue( RRunStatus.m_lResultDataIndex, getInputLine()->mlLinePixelLength ) == S_OK );

	return l_bOk;
}

/*
SVDrawObjectListClass* SVLineAnalyzerClass::GetDialogFigure()
{
	return &m_svDialogFigure;
}
*/

BOOL SVLineAnalyzerClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( inputLineObjectInfo.IsConnected() &&
		inputLineObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
		bRetVal = SVAnalyzerClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

HRESULT SVLineAnalyzerClass::GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	HRESULT l_Status = S_OK;

	SVImageToLineProjectClass *l_psvObject = (SVImageToLineProjectClass *)getImageToLineProject();

	if( l_psvObject != NULL )
	{
		l_psvObject->GetThresholdBars( p_rsvMiltiLine );
	}

	return l_Status;
}

HRESULT SVLineAnalyzerClass::GetHistogramOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	HRESULT l_Status = S_OK;

	SVImageToLineProjectClass *l_psvObject = (SVImageToLineProjectClass *)getImageToLineProject();

	if( l_psvObject != NULL )
	{
		l_psvObject->GetHistogram( p_rsvMiltiLine );
	}

	return l_Status;
}

HRESULT SVLineAnalyzerClass::GetEdgesOverlay( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_Status = S_OK;

	SVImageExtentClass l_svExtents;

	if ( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
	{
		bool l_bIsAGageTool = true;
		double l_dHeight = 0.0;
		double l_dLineCorrect = 1.0;

		SVLineClass* pLine = getInputLine();

		GetOverlayDrawlingAdjustments( l_bIsAGageTool, l_dHeight, l_dLineCorrect );

		if( m_bDisplayAnalyzeLine )	
		{
			// Build complete new set of draw objects for this figure...
			for( size_t i = 0; i < pLine->mResultArray.size(); ++i )
			{
				long l_lX = static_cast<long>(pLine->mResultArray[i] * l_dLineCorrect);

				SVExtentLineStruct l_Line;
				SVExtentPointStruct l_oGraphPoint;

				l_Line.m_bIsAngleValid = true;
				l_Line.m_dAngle = 90.0;

				l_oGraphPoint.m_dPositionX = l_lX;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 - 5;
				
				l_Line.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

				l_oGraphPoint.m_dPositionX = l_lX;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 + 5;
				
				l_Line.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

				l_Line.m_dwColor = SV_DEFAULT_SUB_FUNCTION_COLOR_2;

				if( l_bIsAGageTool )
				{
					l_svExtents.TranslateFromOutputSpace( l_Line, l_Line );
				}

				p_MultiLine.m_svLineArray.Add( l_Line );
			}
		}
	}

	return l_Status;
}

HRESULT SVLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_Status = S_OK;

	SVImageExtentClass l_svExtents;

	if ( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
	{
		bool l_bIsAGageTool = true;
		double l_dHeight = 0.0;
		double l_dLineCorrect = 1.0;

		SVLineClass* pLine = getInputLine();

		GetOverlayDrawlingAdjustments( l_bIsAGageTool, l_dHeight, l_dLineCorrect );

		if( pLine->mResultArray.size() > 0 && m_bDisplayAnalyzeEdge )
		{
			long l_lX = static_cast<long>(pLine->mResultArray[ 0 ] * l_dLineCorrect);

			SVExtentLineStruct l_Line;
			SVExtentPointStruct l_oGraphPoint;

			l_Line.m_bIsAngleValid = true;
			l_Line.m_dAngle = 90.0;

			l_oGraphPoint.m_dPositionX = l_lX;
			l_oGraphPoint.m_dPositionY = l_dHeight / 2 - 10;
			
			l_Line.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

			l_oGraphPoint.m_dPositionX = l_lX;
			l_oGraphPoint.m_dPositionY = l_dHeight / 2 + 10;
			
			l_Line.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

			l_Line.m_dwColor = GetObjectColor();

			if( l_bIsAGageTool )
			{
				l_svExtents.TranslateFromOutputSpace( l_Line, l_Line );
			}

			p_MultiLine.m_svLineArray.Add( l_Line );
		}
	}

	return l_Status;
}

HRESULT SVLineAnalyzerClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT l_Status = SVAnalyzerClass::onCollectOverlays( p_Image, p_MultiLineArray );

	SVExtentFigureStruct l_svFigure;
	SVExtentMultiLineStruct l_svMultiLine;

	SVImageExtentClass l_svToolExtents;
	BOOL l_bOk = TRUE;

	l_bOk = l_bOk && GetTool() != NULL && GetTool()->GetImageExtent( l_svToolExtents ) == S_OK;

	if( GetEdgesOverlay(l_svMultiLine) == S_OK )
	{
		l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
		p_MultiLineArray.Add(l_svMultiLine);
		l_svMultiLine.Initialize();
	}

	if ( l_bOk && GetTool()->WasEnabled() )
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

	return l_Status;
}

void SVLineAnalyzerClass::GetOverlayDrawlingAdjustments( bool& p_rIsGageTool, double& p_rHeight, double& p_rLineCorrect )
{
	p_rIsGageTool = true;
	p_rHeight = 0.0;
	p_rLineCorrect = 1.0;

	SVLineClass* pLine = getInputLine();

	SVImageToLineProjectClass *l_psvObject = (SVImageToLineProjectClass *)getImageToLineProject();

	if( l_psvObject != NULL )
	{
		p_rIsGageTool = false;

		if( l_psvObject->getInputImage() != NULL )
		{
			SVImageExtentClass l_svExtents = l_psvObject->getInputImage()->GetImageExtents();
			l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, p_rHeight );
		}
	}
	else
	{
		if( pLine->mlLineArrayLength > 0 )
		{
			// Calculate for rotation line length on Gage tools
			SVValueObjectClass* l_pValueObject;
			double l_dX1,l_dY1, l_dX2, l_dY2;
			double l_dDX, l_dDY;
			m_svLineExtent.GetExtentObject( SVExtentPropertyPositionPointX, l_pValueObject );
			l_pValueObject->GetValue( l_dX1 );
			m_svLineExtent.GetExtentObject( SVExtentPropertyPositionPointY, l_pValueObject );
			l_pValueObject->GetValue( l_dY1 );
			m_svLineExtent.GetExtentObject( SVExtentPropertyPositionPointEndOfLineX, l_pValueObject );
			l_pValueObject->GetValue( l_dX2 );
			m_svLineExtent.GetExtentObject( SVExtentPropertyPositionPointEndOfLineY, l_pValueObject );
			l_pValueObject->GetValue( l_dY2 );

			l_dDX = l_dX1 - l_dX2;
			l_dDY = l_dY1 - l_dY2;
			double l_dWidth = sqrt(l_dDX * l_dDX + l_dDY * l_dDY);

			p_rLineCorrect = l_dWidth / pLine->mlLineArrayLength;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : processMessage member function of class SVLineAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : process messages sent via SVSendMessage
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :02.03.2000 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
DWORD_PTR SVLineAnalyzerClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
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

	return( SVAnalyzerClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : removeEmbeddedExtents member function of class SVLineAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : Hides and Removes the Shadowed Extent Variables 
//                from the Line Analyzers's outputs
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :13.03.2000 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVLineAnalyzerClass::removeEmbeddedExtents()
{
	hideEmbeddedObject( extentX1 );	 // Make it Un-Selectable for anything
	RemoveEmbeddedObject( &extentX1 ); // Remove it from the Embedded List so it is not scripted

	hideEmbeddedObject( extentY1 );
	RemoveEmbeddedObject( &extentY1 );

	hideEmbeddedObject( extentX2 );
	RemoveEmbeddedObject( &extentX2 );

	hideEmbeddedObject( extentY2 );
	RemoveEmbeddedObject( &extentY2 );

	hideEmbeddedObject( extentLength );
	RemoveEmbeddedObject( &extentLength );
}

SVObjectClass* SVLineAnalyzerClass::getImageToLineProject()
{
	SVImageToLineProjectClass *pProject = nullptr;

	// Get Friend Object ( SVImageToLineProjectClass )
	for( size_t i = 0; i < friendList.size(); i++ )
	{
		const SVObjectInfoStruct& friendObjectInfo = friendList[i];
		if( pProject = dynamic_cast<SVImageToLineProjectClass*>(friendObjectInfo.PObject) )
		{
			break;
		}
	}
	return pProject;
}

SVObjectClass* SVLineAnalyzerClass::getLineROI()
{
	SVLineROIClass* pLineROI = NULL;
	
	// Get Friend Object ( SVLineROIClass )
	for( size_t i = 0; i < friendList.size(); i++ )
	{
		const SVObjectInfoStruct& friendObjectInfo = friendList[i];
		if( pLineROI = dynamic_cast<SVLineROIClass*>(friendObjectInfo.PObject) )
		{
			break;
		}
	}
	return pLineROI;
}

bool SVLineAnalyzerClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVLineAnalyzerClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svLineExtent.GetImageExtent( p_rsvImageExtent );

	return l_hrOk;
}

HRESULT SVLineAnalyzerClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	SVGageToolClass *l_psvGage = dynamic_cast<SVGageToolClass *>( GetTool() );

	if( l_psvGage != NULL )
	{
		SVImageClass *l_psvImage = l_psvGage->getInputImage();

		if ( l_psvImage != NULL && l_psvImage->IsCreated() )
		{
			SVImageExtentClass l_svExtents;

			if( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
			{
				if( l_psvImage->ValidateAgainstOutputExtents( l_svExtents ) == S_OK )
				{
					l_hrOk = l_psvImage->ValidateAgainstOutputExtents( p_svImageExtent );
				}
				else
				{
					l_hrOk = S_OK;
				}

				if ( l_hrOk == S_OK )
				{
					l_hrOk = m_svLineExtent.SetImageExtent( p_ulIndex, p_svImageExtent );
				}
			}
		}
		else
		{
			l_hrOk = m_svLineExtent.SetImageExtent( p_ulIndex, p_svImageExtent );
		}
	}

	return l_hrOk;
}

HRESULT SVLineAnalyzerClass::SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	SVGageToolClass *l_psvGage = dynamic_cast<SVGageToolClass *>( GetTool() );

	if( l_psvGage != NULL )
	{
		SVImageClass *l_psvImage = l_psvGage->getInputImage();

		if ( l_psvImage != NULL && l_psvImage->IsCreated() )
		{
			SVImageExtentClass l_svExtents;

			l_hrOk = l_psvImage->GetImageExtentsToFit( p_svImageExtent, p_svImageExtent );

			if ( l_hrOk == S_OK )
			{
				l_hrOk = m_svLineExtent.SetImageExtent( p_ulIndex, p_svImageExtent );
			}
		}
		else
		{
			l_hrOk = m_svLineExtent.SetImageExtent( p_ulIndex, p_svImageExtent );
		}
	}

	return l_hrOk;
}

HRESULT SVLineAnalyzerClass::GetRelativePositionPoints( SVExtentPointStruct &p_rsvPoint1, SVExtentPointStruct &p_rsvPoint2 )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_rsvPoint1.Initialize() == S_OK && p_rsvPoint2.Initialize() == S_OK )
	{
		if ( dynamic_cast<SVGageToolClass *>( GetTool() ) == NULL )
		{
			SVImageExtentClass l_svExtents;

			if ( GetTool() != NULL && GetTool()->GetImageExtent( l_svExtents ) == S_OK )
			{
				long l_lHeight = 0;
				long l_lWidth = 0;

				if( l_svExtents.GetExtentProperty( SVExtentPropertyHeight, l_lHeight ) == S_OK &&
				    l_svExtents.GetExtentProperty( SVExtentPropertyWidth, l_lWidth ) == S_OK )
				{
					p_rsvPoint1.m_dPositionX = 0.0;
					p_rsvPoint1.m_dPositionY = l_lHeight / 2;

					p_rsvPoint2.m_dPositionX = l_lWidth;
					p_rsvPoint2.m_dPositionY = l_lHeight / 2;

					l_hrOk = S_OK;
				}
			}
		}
		else
		{
			long l_lX1 = 0;
			long l_lY1 = 0;
			long l_lX2 = 0;
			long l_lY2 = 0;

			if( ( extentX1.GetValue( l_lX1 ) == S_OK ) && ( extentY1.GetValue( l_lY1 ) == S_OK ) &&
			    ( extentX2.GetValue( l_lX2 ) == S_OK ) && ( extentY2.GetValue( l_lY2 ) == S_OK ) )
			{
				p_rsvPoint1.m_dPositionX = l_lX1;
				p_rsvPoint1.m_dPositionY = l_lY1;

				p_rsvPoint2.m_dPositionX = l_lX2;
				p_rsvPoint2.m_dPositionY = l_lY2;

				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

SVTaskObjectClass *SVLineAnalyzerClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVTaskObjectClass *l_psvObject = NULL;

	if ( dynamic_cast<SVGageToolClass *>( GetTool() ) != NULL )
	{
		SVImageExtentClass l_svExtents;

		if( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK &&
				l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
		{
			l_psvObject = this;
		}
	}

	return l_psvObject;
}

//** EOF **

   
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLineAnalyzer.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 13:14:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revsied processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 11:13:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.50   02 May 2013 11:21:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 11:47:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   16 Aug 2012 18:55:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   02 Aug 2012 16:05:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to draw the analyzer in the correct color when tool is disabled.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   01 Aug 2012 12:53:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   27 Jul 2012 08:55:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   25 Jul 2012 14:27:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code and updated overlay functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   18 Jul 2012 13:40:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   16 Jul 2012 09:01:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   12 Jul 2012 14:58:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   10 Jul 2012 12:18:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   02 Jul 2012 17:15:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   19 Jun 2012 12:57:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   21 Mar 2011 12:11:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   16 Mar 2011 13:40:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   04 Nov 2010 13:43:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   01 Jun 2010 14:41:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   16 Dec 2009 11:24:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   12 Oct 2009 16:00:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code that was included in the 4.82 branch.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   03 Sep 2009 10:29:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   30 Jul 2009 12:00:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branched changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   10 Jun 2008 12:18:10   JSpila
   Project:  SVObserver
   Change Request (SCR) nbr:  621
   SCR Title:  Fix Bug in the SVImageClass
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upated source code to clear parent image information from the embedded image objects and the tool extent object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   07 Mar 2008 11:54:58   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added support for Transfer Image Definition List Source Image Names.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   21 Jun 2007 13:27:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27.2.1   12 Oct 2009 13:03:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix GUI Overlay probelm when moving the tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27.2.0   14 Jul 2009 14:31:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code with new data manager objects and image methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   10 Aug 2005 12:34:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   29 Jul 2005 12:42:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   21 Jun 2005 08:11:54   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   18 Mar 2005 09:15:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   17 Feb 2005 14:37:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   16 Feb 2005 14:43:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   04 Dec 2003 12:15:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   30 Oct 2003 09:34:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  390
   SCR Title:  Update SVObserver to Version 4.22 Release and make Bug Fixes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to update the run once index and current index for the shadowed extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   22 Apr 2003 10:39:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   05 Mar 2003 12:13:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   30 Jan 2003 12:02:58   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated getLineExtent method to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   14 Jan 2003 13:59:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed attributes for when the object gets created.  Turned off certain attributes if the objects sub-type is SVToolProfileObjectType
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   10 Jan 2003 14:31:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function VerifyImageForOverlay to determine which object has overlays on which image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   03 Jan 2003 15:27:38   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added attributes for extent X1, X2, Y1, Y2 for set able online remotely and for extent
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   09 Dec 2002 19:33:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   19 Nov 2002 15:06:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed ValueObject semantics to work with buckets
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
