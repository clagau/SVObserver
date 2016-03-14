//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSingleLineAnalyzer
//* .File Name       : $Workfile:   SVSingleLineAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVSingleLineAnalyzer.h"
#include "SVEdge.h"
#include "SVGlobal.h"

SV_IMPLEMENT_CLASS( SVSingleLineAnalyzerClass, SVSingleLineAnalyzerClassGuid );

SVSingleLineAnalyzerClass::SVSingleLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
								:SVLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVSingleLineAnalyzerClass::init()
{
	// Do not use: This is only an intermediate class and should be derived from
	// Identify our type
	//outObjectInfo.ObjectTypeInfo.SubType = SVSingleLineAnalyzerObjectType;

	// New Thresholding Options
	RegisterEmbeddedObject( &m_voUseLowerThresholdSelectable, SVUseLowerThresholdSelectableObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiff, SVUseLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffset, SVUseLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMinPlusOffset, SVUseLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiff, SVLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusOffset, SVLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMinPlusOffset, SVLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voUseUpperThresholdSelectable, SVUseUpperThresholdSelectableObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiff, SVUseUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffset, SVUseUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMinPlusOffset, SVUseUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiff, SVUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusOffset, SVUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMinPlusOffset, SVUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET, false, SVResetItemNone );

	// New ValueObjects for the EdgeParameters
	RegisterEmbeddedObject( &m_voEdgeADirection, SVEdgeADirectionGuid, IDS_OBJECTNAME_EDGE_A_DIRECTION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPolarisation, SVEdgeAPolarisationGuid, IDS_OBJECTNAME_EDGE_A_POLARISATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAEdgeSelect, SVEdgeAEdgeSelectGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAEdgeSelectThisValue, SVEdgeAEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT_THIS_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAIsFixedEdgeMarker, SVEdgeAIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_A_IS_FIXED_EDGE_MARKER, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPosition, SVEdgeAPositionGuid, IDS_OBJECTNAME_EDGE_A_POSITION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPositionOffsetValue, SVEdgeAPositionOffsetGuid, IDS_OBJECTNAME_EDGE_A_POSITION_OFFSET, false, SVResetItemNone );
    
	RegisterEmbeddedObject( &m_voEdgeALowerThresholdValue, SVEdgeALowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAUpperThresholdValue, SVEdgeAUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE, false, SVResetItemNone );

	// New Thresholding Options
	m_voUseLowerThresholdSelectable.SetDefaultValue( TRUE, TRUE );
	m_voUseLowerThresholdMaxMinusPercentDiff.SetDefaultValue( FALSE,TRUE );
	m_voUseLowerThresholdMaxMinusOffset.SetDefaultValue( FALSE, TRUE );
	m_voUseLowerThresholdMinPlusOffset.SetDefaultValue( FALSE, TRUE );

	m_voUseUpperThresholdSelectable.SetDefaultValue( TRUE, TRUE );
	m_voUseUpperThresholdMaxMinusPercentDiff.SetDefaultValue( FALSE,TRUE );
	m_voUseUpperThresholdMaxMinusOffset.SetDefaultValue( FALSE, TRUE );
	m_voUseUpperThresholdMinPlusOffset.SetDefaultValue( FALSE, TRUE );

	// Set default for edge parameters
	m_voEdgeADirection.SetEnumTypes( g_strDirectionEnums );
	m_voEdgeADirection.SetDefaultValue(SV_UNDEFINED_DIRECTION,TRUE);
	m_voEdgeAPolarisation.SetEnumTypes( g_strPolarisationEnums );
	m_voEdgeAPolarisation.SetDefaultValue(SV_UNDEFINED_POLARISATION,TRUE);
	m_voEdgeAEdgeSelect.SetEnumTypes( g_strEdgeSelectEnums );
	m_voEdgeAEdgeSelect.SetDefaultValue(SV_UNDEFINED_EDGE,TRUE);
	m_voEdgeAEdgeSelectThisValue.SetDefaultValue(0L,TRUE);
	m_voEdgeAIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeAPosition.SetEnumTypes( g_strPositionEnums );
	m_voEdgeAPosition.SetDefaultValue(SV_UNDEFINED_POSITION,TRUE);
	m_voEdgeAPositionOffsetValue.SetDefaultValue(0.0F,TRUE);
	m_voEdgeALowerThresholdValue.SetDefaultValue(0,TRUE);
	m_voEdgeAUpperThresholdValue.SetDefaultValue(0,TRUE);

	// Identify our input type needs
	inputMinThresholdObjectInfo.SetInputObjectType( SVThresholdMinObjectGuid, SVDWordValueObjectType );
	inputMinThresholdObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputMinThresholdObjectInfo, _T( "SingleLineAnalyzerMinThreshold" ) );

	inputMaxThresholdObjectInfo.SetInputObjectType( SVThresholdMaxObjectGuid, SVDWordValueObjectType );
	inputMaxThresholdObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputMaxThresholdObjectInfo, _T( "SingleLineAnalyzerMaxThreshold" ) );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

BOOL SVSingleLineAnalyzerClass::CreateObject(SVObjectLevelCreateStruct *PCreateStructure)
{
	isCreated = SVLineAnalyzerClass::CreateObject(PCreateStructure);

	// Set / Reset Printable Flags
	m_voUseLowerThresholdSelectable.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusPercentDiff.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMinPlusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusPercentDiff.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMinPlusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voUseUpperThresholdSelectable.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusPercentDiff.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMinPlusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusPercentDiff.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMinPlusOffset.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	//Value objects for Edge Parameters
	if( m_bEnableDirection )
	{
		m_voEdgeADirection.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeADirection.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnablePolarisation )
	{
		m_voEdgeAPolarisation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeAPolarisation.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnableEdgeSelect )
	{
		m_voEdgeAEdgeSelect.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeAEdgeSelectThisValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeAEdgeSelect.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeAEdgeSelectThisValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnablePosition )
	{
		m_voEdgeAIsFixedEdgeMarker.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeAPosition.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeAPositionOffsetValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeAIsFixedEdgeMarker.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeAPosition.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeAPositionOffsetValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else


	SetCalculatedThresholdPrintableFlags();

	return isCreated;
}


SVSingleLineAnalyzerClass::~SVSingleLineAnalyzerClass()
{
	CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVSingleLineAnalyzerClass::OnValidate()
{
	BOOL retVal = FALSE;
	
	// Validate our local needs
	if( NULL )
	{
		SetInvalid();
	}
	else
	{
		SetCalculatedThresholdPrintableFlags();

		retVal = TRUE;

		// Validate our base class needs
		retVal = SVLineAnalyzerClass::OnValidate() && retVal;

		// Set this object and all objects it owns, to invalid
		if( !retVal )
			SetInvalid();
	}
	return retVal;
}

void SVSingleLineAnalyzerClass::SetCalculatedThresholdPrintableFlags()
{
	bool bEnablePrintUpper = GetValue<bool>(m_voUseUpperThresholdSelectable);
	bool bEnablePrintLower = GetValue<bool>(m_voUseLowerThresholdSelectable);

	SetBits(m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
	        m_bEnableThreshold && bEnablePrintLower);

	SetBits(m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
	        m_bEnableThreshold && bEnablePrintUpper);
}

BOOL SVSingleLineAnalyzerClass::CalculateThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue )
{
	BOOL state;
	int iLastSet;

	const SVObjectInfoStruct& rObjectInfo = GetOwner()->GetObjectInfo();
	if( rObjectInfo.ObjectTypeInfo.SubType != SVToolProfileObjectType )
	{
		return FALSE;
	}// end if

	DWORD offset;
	DWORD percentDiff;
	DWORD minThreshold,maxThreshold;
	GetMinMaxThresholdValues( minThreshold, maxThreshold );

	m_voUseUpperThresholdSelectable.GetValue( state );
	if( !state )
	{
		m_voEdgeAUpperThresholdValue.GetLastSetIndex(iLastSet);
		m_voUseUpperThresholdMaxMinusPercentDiff.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusPercentDiff.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWUpperThresholdValue = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWUpperThresholdValue = maxThreshold;
			}
		}

		m_voUseUpperThresholdMaxMinusOffset.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusOffset.GetValue( offset );
			DWUpperThresholdValue = maxThreshold - offset;
		}

		m_voUseUpperThresholdMinPlusOffset.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMinPlusOffset.GetValue( offset );
			DWUpperThresholdValue = minThreshold + offset;
		}

		if( (long) DWUpperThresholdValue < 0L )
			DWUpperThresholdValue = 0L;

		if( (long) DWUpperThresholdValue > 255L )
			DWUpperThresholdValue = 255L;

		m_voEdgeAUpperThresholdValue.SetValue(1L,DWUpperThresholdValue);
		m_voEdgeAUpperThresholdValue.SetValue(iLastSet,DWUpperThresholdValue);
	}

	m_voUseLowerThresholdSelectable.GetValue( state );
	if( !state )
	{	
		m_voEdgeALowerThresholdValue.GetLastSetIndex(iLastSet);	
		m_voUseLowerThresholdMaxMinusPercentDiff.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusPercentDiff.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWLowerThresholdValue = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWLowerThresholdValue = maxThreshold;
			}
		}

		m_voUseLowerThresholdMaxMinusOffset.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusOffset.GetValue( offset );
			DWLowerThresholdValue = maxThreshold - offset;
		}

		m_voUseLowerThresholdMinPlusOffset.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMinPlusOffset.GetValue( offset );
			DWLowerThresholdValue = minThreshold + offset;
		}

		if( (long) DWLowerThresholdValue < 0L )
			DWLowerThresholdValue = 0L;

		if( (long) DWLowerThresholdValue > 255L )
			DWLowerThresholdValue = 255L;

		m_voEdgeALowerThresholdValue.SetValue(1L,DWLowerThresholdValue);
		m_voEdgeALowerThresholdValue.SetValue(iLastSet,DWLowerThresholdValue);
	}

	return TRUE;
}

BOOL SVSingleLineAnalyzerClass::GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue )
{
	if( inputMinThresholdObjectInfo.IsConnected() && inputMinThresholdObjectInfo.GetInputObjectInfo().PObject &&
		inputMaxThresholdObjectInfo.IsConnected() && inputMaxThresholdObjectInfo.GetInputObjectInfo().PObject )
	{
		( ( SVDWordValueObjectClass* ) inputMinThresholdObjectInfo.GetInputObjectInfo().PObject )->GetValue( DWMinValue );
		( ( SVDWordValueObjectClass* ) inputMaxThresholdObjectInfo.GetInputObjectInfo().PObject )->GetValue( DWMaxValue );
		return TRUE;
	}
	return FALSE;
}

BOOL SVSingleLineAnalyzerClass::SetObjectOwner( SVObjectClass* PNewOwner )
{
	// check if owned by Profile Tool
	const SVObjectInfoStruct& rObjectInfo = PNewOwner->GetObjectInfo();
	if( rObjectInfo.ObjectTypeInfo.SubType != SVToolProfileObjectType )
	{
		DisableThresholdOutputs();
	}

	return SVLineAnalyzerClass::SetObjectOwner( PNewOwner );
}

void SVSingleLineAnalyzerClass::DisableThresholdOutputs()
{
	hideEmbeddedObject( m_voUseLowerThresholdSelectable );
	RemoveEmbeddedObject( &m_voUseLowerThresholdSelectable );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusPercentDiff );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiff );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusOffset );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffset );

	hideEmbeddedObject( m_voUseLowerThresholdMinPlusOffset );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMinPlusOffset );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusPercentDiff );
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiff );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusOffset );
	// Bug - must remain missing or gage tools won't load
	/*
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusOffset );

	hideEmbeddedObject( m_voLowerThresholdMinPlusOffset );
	//*/
	RemoveEmbeddedObject( &m_voLowerThresholdMinPlusOffset );

	hideEmbeddedObject( m_voUseUpperThresholdSelectable );
	RemoveEmbeddedObject( &m_voUseUpperThresholdSelectable );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusPercentDiff );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiff );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusOffset );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffset );

	hideEmbeddedObject( m_voUseUpperThresholdMinPlusOffset );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMinPlusOffset );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusPercentDiff );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiff );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusOffset );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusOffset );

	hideEmbeddedObject( m_voUpperThresholdMinPlusOffset );
	RemoveEmbeddedObject( &m_voUpperThresholdMinPlusOffset );

	// Disconnect the Inputs
	::SVSendMessage( inputMinThresholdObjectInfo.GetInputObjectInfo().UniqueObjectID,
										SVM_DISCONNECT_OBJECT_INPUT, 
										reinterpret_cast<DWORD_PTR>(&inputMinThresholdObjectInfo), NULL );
	
	::SVSendMessage( inputMaxThresholdObjectInfo.GetInputObjectInfo().UniqueObjectID,
										SVM_DISCONNECT_OBJECT_INPUT, 
										reinterpret_cast<DWORD_PTR>(&inputMaxThresholdObjectInfo), NULL );

	// Remove from the Required Input List
	for( int i = inputInterfaceList.GetSize() - 1; i >= 0; i-- )
	{
		SVInObjectInfoStruct* pInputObjectInfo = inputInterfaceList.GetAt( i );

		if( pInputObjectInfo )
		{
			if( &inputMinThresholdObjectInfo == pInputObjectInfo )
			{
				inputInterfaceList.RemoveAt( i );
				continue;
			}
			if( &inputMaxThresholdObjectInfo == pInputObjectInfo )
			{
				inputInterfaceList.RemoveAt( i );
				continue;
			}
		}
	}
}

//** EOF **

