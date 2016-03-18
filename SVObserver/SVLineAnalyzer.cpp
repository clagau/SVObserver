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

#pragma region Includes
#include "stdafx.h"
#include "SVLineAnalyzer.h"

#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVImageClass.h"
#include "SVImageToLineProject.h"
#include "SVLineROI.h"
#include "SVTool.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

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

	if( bOk )
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

			SVLineROIClass* pROI = new SVLineROIClass;
			if( pROI )
			{
				pROI->SetObjectOwner( this );
				AddFriend( pROI->GetUniqueObjectID() );
			}

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

HRESULT SVLineAnalyzerClass::GetRelativePositionPoints( SVExtentPointStruct &p_rsvPoint1, SVExtentPointStruct &p_rsvPoint2 )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_rsvPoint1.Initialize() == S_OK && p_rsvPoint2.Initialize() == S_OK )
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

	return l_hrOk;
}

SVTaskObjectClass *SVLineAnalyzerClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	return nullptr;
}

//** EOF **

   
