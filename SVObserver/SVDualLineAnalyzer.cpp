//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDualLineAnalyzer
//* .File Name       : $Workfile:   SVDualLineAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:12:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVDualLineAnalyzer.h"

#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"

#include "SVEdge.h"
#include "SVImageToLineProject.h"
#include "SVImageClass.h"


SV_IMPLEMENT_CLASS( SVDualLineAnalyzerClass, SVDualLineAnalyzerClassGuid );

SVDualLineAnalyzerClass::SVDualLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
									 :SVLineAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVDualLineAnalyzerClass::init()
{
	m_bDisplayAnalyzeEdge = FALSE;

	// Do not use: This is only an intermediate class and should be derived from
	// Identify our type
	//outObjectInfo.ObjectTypeInfo.SubType = SVDualLineAnalyzerObjectType;
	
	// New Thresholding Options
	// Edge A
	RegisterEmbeddedObject( &m_voUseLowerThresholdSelectableA, SVUseLowerThresholdSelectableObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiffA, SVUseLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffsetA, SVUseLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMinPlusOffsetA, SVUseLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiffA, SVLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusOffsetA, SVLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMinPlusOffsetA, SVLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_A, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voUseUpperThresholdSelectableA, SVUseUpperThresholdSelectableObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiffA, SVUseUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffsetA, SVUseUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMinPlusOffsetA, SVUseUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiffA, SVUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusOffsetA, SVUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_A, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMinPlusOffsetA, SVUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_A, false, SVResetItemNone );

	// Edge B
	RegisterEmbeddedObject( &m_voUseLowerThresholdSelectableB, SVUseLowerThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiffB, SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffsetB, SVUseLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseLowerThresholdMinPlusOffsetB, SVUseLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiffB, SVLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMaxMinusOffsetB, SVLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voLowerThresholdMinPlusOffsetB, SVLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_B, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voUseUpperThresholdSelectableB, SVUseUpperThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiffB, SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffsetB, SVUseUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUseUpperThresholdMinPlusOffsetB, SVUseUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiffB, SVUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMaxMinusOffsetB, SVUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voUpperThresholdMinPlusOffsetB, SVUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_B, false, SVResetItemNone );

	// New Value Objects for Edge Parameters
	RegisterEmbeddedObject( &m_voEdgeADirection, SVEdgeADirectionGuid, IDS_OBJECTNAME_EDGE_A_DIRECTION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPolarisation, SVEdgeAPolarisationGuid, IDS_OBJECTNAME_EDGE_A_POLARISATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAEdgeSelect, SVEdgeAEdgeSelectGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAEdgeSelectThisValue, SVEdgeAEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT_THIS_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAIsFixedEdgeMarker, SVEdgeAIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_A_IS_FIXED_EDGE_MARKER, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPosition, SVEdgeAPositionGuid, IDS_OBJECTNAME_EDGE_A_POSITION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAPositionOffsetValue, SVEdgeAPositionOffsetGuid, IDS_OBJECTNAME_EDGE_A_POSITION_OFFSET, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voEdgeALowerThresholdValue, SVEdgeALowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeAUpperThresholdValue, SVEdgeAUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voEdgeBDirection, SVEdgeBDirectionGuid, IDS_OBJECTNAME_EDGE_B_DIRECTION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBPolarisation, SVEdgeBPolarisationGuid, IDS_OBJECTNAME_EDGE_B_POLARISATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBEdgeSelect, SVEdgeBEdgeSelectGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBEdgeSelectThisValue, SVEdgeBEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT_THIS_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBIsFixedEdgeMarker, SVEdgeBIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_B_IS_FIXED_EDGE_MARKER, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBPosition, SVEdgeBPositionGuid, IDS_OBJECTNAME_EDGE_B_POSITION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBPositionOffsetValue, SVEdgeBPositionOffsetGuid, IDS_OBJECTNAME_EDGE_B_POSITION_OFFSET, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_voEdgeBLowerThresholdValue, SVEdgeBLowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_LOWER_THRESHOLD_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_voEdgeBUpperThresholdValue, SVEdgeBUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_UPPER_THRESHOLD_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	// New Thresholding Options
	// Edge A
	m_voUseLowerThresholdSelectableA.SetDefaultValue(TRUE,TRUE);
	m_voUseLowerThresholdMaxMinusPercentDiffA.SetDefaultValue(FALSE,TRUE);
	m_voUseLowerThresholdMaxMinusOffsetA.SetDefaultValue(FALSE,TRUE);
	m_voUseLowerThresholdMinPlusOffsetA.SetDefaultValue(FALSE,TRUE);

	m_voUseUpperThresholdSelectableA.SetDefaultValue(TRUE,TRUE);
	m_voUseUpperThresholdMaxMinusPercentDiffA.SetDefaultValue(FALSE,TRUE);
	m_voUseUpperThresholdMaxMinusOffsetA.SetDefaultValue(FALSE,TRUE);
	m_voUseUpperThresholdMinPlusOffsetA.SetDefaultValue(FALSE,TRUE);

	// Edge B
	m_voUseLowerThresholdSelectableB.SetDefaultValue(TRUE,TRUE);
	m_voUseLowerThresholdMaxMinusPercentDiffB.SetDefaultValue(FALSE,TRUE);
	m_voUseLowerThresholdMaxMinusOffsetB.SetDefaultValue(FALSE,TRUE);
	m_voUseLowerThresholdMinPlusOffsetB.SetDefaultValue(FALSE,TRUE);

	m_voUseUpperThresholdSelectableB.SetDefaultValue(TRUE,TRUE);
	m_voUseUpperThresholdMaxMinusPercentDiffB.SetDefaultValue(FALSE,TRUE);
	m_voUseUpperThresholdMaxMinusOffsetB.SetDefaultValue(FALSE,TRUE);
	m_voUseUpperThresholdMinPlusOffsetB.SetDefaultValue(FALSE,TRUE);

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

	m_voEdgeBDirection.SetEnumTypes( g_strDirectionEnums );
	m_voEdgeBDirection.SetDefaultValue(SV_UNDEFINED_DIRECTION,TRUE);
	m_voEdgeBPolarisation.SetEnumTypes( g_strPolarisationEnums );
	m_voEdgeBPolarisation.SetDefaultValue(SV_UNDEFINED_POLARISATION,TRUE);
	m_voEdgeBEdgeSelect.SetEnumTypes( g_strEdgeSelectEnums );
	m_voEdgeBEdgeSelect.SetDefaultValue(SV_UNDEFINED_EDGE,TRUE);
	m_voEdgeBEdgeSelectThisValue.SetDefaultValue(0L,TRUE);
	m_voEdgeBIsFixedEdgeMarker.SetDefaultValue(FALSE,TRUE);
	m_voEdgeBPosition.SetEnumTypes( g_strPositionEnums );
	m_voEdgeBPosition.SetDefaultValue(SV_UNDEFINED_POSITION,TRUE);
	m_voEdgeBPositionOffsetValue.SetDefaultValue(0.0F,TRUE);

	m_voEdgeBLowerThresholdValue.SetDefaultValue(0,TRUE);
	m_voEdgeBUpperThresholdValue.SetDefaultValue(0,TRUE);

	// Identify our input type needs
	inputMinThresholdObjectInfo.SetInputObjectType( SVThresholdMinObjectGuid, SVDWordValueObjectType );
	inputMinThresholdObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputMinThresholdObjectInfo, _T( "MinThreshold" ) );

	inputMaxThresholdObjectInfo.SetInputObjectType( SVThresholdMaxObjectGuid, SVDWordValueObjectType );
	inputMaxThresholdObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputMaxThresholdObjectInfo, _T( "MaxThreshold" ) );

	// Set default inputs and outputs
	addDefaultInputObjects();
}


SVDualLineAnalyzerClass::~SVDualLineAnalyzerClass()
{
	CloseObject();
}


BOOL SVDualLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVLineAnalyzerClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	m_voUseLowerThresholdSelectableA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusPercentDiffA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMinPlusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusPercentDiffA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMinPlusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voUseUpperThresholdSelectableA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusPercentDiffA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMinPlusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusPercentDiffA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMinPlusOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voUseLowerThresholdSelectableB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusPercentDiffB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMaxMinusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseLowerThresholdMinPlusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusPercentDiffB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMaxMinusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voLowerThresholdMinPlusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_voUseUpperThresholdSelectableB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusPercentDiffB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMaxMinusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUseUpperThresholdMinPlusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusPercentDiffB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMaxMinusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_voUpperThresholdMinPlusOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

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

	// call SetCalculatedThresholdPrintableFlags() below

	if( m_bEnableDirection )
	{
		m_voEdgeBDirection.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeBDirection.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnablePolarisation )
	{
		m_voEdgeBPolarisation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeBPolarisation.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnableEdgeSelect )
	{
		m_voEdgeBEdgeSelect.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeBEdgeSelectThisValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeBEdgeSelect.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeBEdgeSelectThisValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	if( m_bEnablePosition )
	{
		m_voEdgeBIsFixedEdgeMarker.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeBPosition.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
		m_voEdgeBPositionOffsetValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	}// end if
	else
	{
		m_voEdgeBIsFixedEdgeMarker.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeBPosition.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		m_voEdgeBPositionOffsetValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}// end else

	SetCalculatedThresholdPrintableFlags();


	return isCreated;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate
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
BOOL SVDualLineAnalyzerClass::OnValidate()
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

void SVDualLineAnalyzerClass::SetCalculatedThresholdPrintableFlags()
{
	bool bEnablePrintUpperA = GetValue<bool>(m_voUseUpperThresholdSelectableA);
	bool bEnablePrintLowerA = GetValue<bool>(m_voUseLowerThresholdSelectableA);

	SetBits(m_voEdgeALowerThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
			m_bEnableThreshold && bEnablePrintLowerA);

	SetBits(m_voEdgeAUpperThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
			m_bEnableThreshold && bEnablePrintUpperA);

	bool bEnablePrintUpperB = GetValue<bool>(m_voUseUpperThresholdSelectableB);
	bool bEnablePrintLowerB = GetValue<bool>(m_voUseLowerThresholdSelectableB);

	SetBits(m_voEdgeBLowerThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
			m_bEnableThreshold && bEnablePrintLowerB);

	SetBits(m_voEdgeBUpperThresholdValue.ObjectAttributesAllowedRef(), SV_PRINTABLE,
			m_bEnableThreshold && bEnablePrintUpperB);

}

BOOL SVDualLineAnalyzerClass::CalculateThresholdValues( DWORD& DWUpperThresholdValueA, DWORD& DWLowerThresholdValueA, DWORD& DWUpperThresholdValueB, DWORD& DWLowerThresholdValueB )
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
	DWORD minThreshold, maxThreshold;
	GetMinMaxThresholdValues( minThreshold, maxThreshold );

	m_voUseUpperThresholdSelectableA.GetValue( state );
	if( !state )
	{
		m_voEdgeAUpperThresholdValue.GetLastSetIndex(iLastSet);
		m_voUseUpperThresholdMaxMinusPercentDiffA.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusPercentDiffA.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWUpperThresholdValueA = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWUpperThresholdValueA = maxThreshold;
			}
		}

		m_voUseUpperThresholdMaxMinusOffsetA.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusOffsetA.GetValue( offset );
			DWUpperThresholdValueA = maxThreshold - offset;
		}

		m_voUseUpperThresholdMinPlusOffsetA.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMinPlusOffsetA.GetValue( offset );
			DWUpperThresholdValueA = minThreshold + offset;
		}

		if( (long) DWUpperThresholdValueA < 0L )
			DWUpperThresholdValueA = 0L;

		if( (long) DWUpperThresholdValueA > 255L )
			DWUpperThresholdValueA = 255L;

		m_voEdgeAUpperThresholdValue.SetValue(1L,DWUpperThresholdValueA);
		m_voEdgeAUpperThresholdValue.SetValue(iLastSet,DWUpperThresholdValueA);
	}

	m_voUseLowerThresholdSelectableA.GetValue( state );
	if( !state )
	{
		m_voEdgeALowerThresholdValue.GetLastSetIndex(iLastSet);	
		m_voUseLowerThresholdMaxMinusPercentDiffA.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusPercentDiffA.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWLowerThresholdValueA = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWLowerThresholdValueA = maxThreshold;
			}
		}

		m_voUseLowerThresholdMaxMinusOffsetA.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusOffsetA.GetValue( offset );
			DWLowerThresholdValueA = maxThreshold - offset;
		}

		m_voUseLowerThresholdMinPlusOffsetA.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMinPlusOffsetA.GetValue( offset );
			DWLowerThresholdValueA = minThreshold + offset;
		}

		if( (long) DWLowerThresholdValueA < 0L )
			DWLowerThresholdValueA = 0L;

		if( (long) DWLowerThresholdValueA > 255L )
			DWLowerThresholdValueA = 255L;

		m_voEdgeALowerThresholdValue.SetValue(1L,DWLowerThresholdValueA);
		m_voEdgeALowerThresholdValue.SetValue(iLastSet,DWLowerThresholdValueA);			
	}
	
	m_voUseUpperThresholdSelectableB.GetValue( state );
	if( !state )
	{
		m_voEdgeBUpperThresholdValue.GetLastSetIndex(iLastSet);	
		m_voUseUpperThresholdMaxMinusPercentDiffB.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusPercentDiffB.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWUpperThresholdValueB = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWUpperThresholdValueB = maxThreshold;
			}
		}

		m_voUseUpperThresholdMaxMinusOffsetB.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMaxMinusOffsetB.GetValue( offset );
			DWUpperThresholdValueB = maxThreshold - offset;
		}

		m_voUseUpperThresholdMinPlusOffsetB.GetValue( state );
		if( state )
		{
			m_voUpperThresholdMinPlusOffsetB.GetValue( offset );
			DWUpperThresholdValueB = minThreshold + offset;
		}

		if( (long) DWUpperThresholdValueB < 0L )
			DWUpperThresholdValueB = 0L;

		if( (long) DWUpperThresholdValueB > 255L )
			DWUpperThresholdValueB = 255L;

		m_voEdgeBUpperThresholdValue.SetValue(1L,DWUpperThresholdValueB);
		m_voEdgeBUpperThresholdValue.SetValue(iLastSet,DWUpperThresholdValueB);
	}

	m_voUseLowerThresholdSelectableB.GetValue( state );
	if( !state )
	{
		m_voEdgeBLowerThresholdValue.GetLastSetIndex(iLastSet);
		m_voUseLowerThresholdMaxMinusPercentDiffB.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusPercentDiffB.GetValue( percentDiff );
			if( percentDiff )
			{
				double percent = ( double )percentDiff / 100.0;
				DWLowerThresholdValueB = (DWORD)(maxThreshold - ( ( maxThreshold - minThreshold ) * percent ) );
			}
			else
			{
				DWLowerThresholdValueB = maxThreshold;
			}
		}

		m_voUseLowerThresholdMaxMinusOffsetB.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMaxMinusOffsetB.GetValue( offset );
			DWLowerThresholdValueB = maxThreshold - offset;
		}

		m_voUseLowerThresholdMinPlusOffsetB.GetValue( state );
		if( state )
		{
			m_voLowerThresholdMinPlusOffsetB.GetValue( offset );
			DWLowerThresholdValueB = minThreshold + offset;
		}

		if( (long) DWLowerThresholdValueB < 0L )
			DWLowerThresholdValueB = 0L;

		if( (long) DWLowerThresholdValueB > 255L )
			DWLowerThresholdValueB = 255L;

		m_voEdgeBLowerThresholdValue.SetValue(1L,DWLowerThresholdValueB);
		m_voEdgeBLowerThresholdValue.SetValue(iLastSet,DWLowerThresholdValueB);
	}

	return TRUE;
}

BOOL SVDualLineAnalyzerClass::GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue )
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

BOOL SVDualLineAnalyzerClass::SetObjectOwner( SVObjectClass* PNewOwner )
{
	// check if owned by Profile Tool
	const SVObjectInfoStruct& rObjectInfo = PNewOwner->GetObjectInfo();
	if( rObjectInfo.ObjectTypeInfo.SubType != SVToolProfileObjectType )
	{
		DisableThresholdOutputs();
	}

	return SVLineAnalyzerClass::SetObjectOwner( PNewOwner );
}

void SVDualLineAnalyzerClass::DisableThresholdOutputs()
{
	// EdgeA
	hideEmbeddedObject( m_voUseLowerThresholdSelectableA );
	RemoveEmbeddedObject( &m_voUseLowerThresholdSelectableA );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusPercentDiffA );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiffA );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusOffsetA );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffsetA );

	hideEmbeddedObject( m_voUseLowerThresholdMinPlusOffsetA );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMinPlusOffsetA );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusPercentDiffA );
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiffA );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusOffsetA );
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusOffsetA );

	hideEmbeddedObject( m_voLowerThresholdMinPlusOffsetA );
	RemoveEmbeddedObject( &m_voLowerThresholdMinPlusOffsetA );

	hideEmbeddedObject( m_voUseUpperThresholdSelectableA );
	RemoveEmbeddedObject( &m_voUseUpperThresholdSelectableA );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusPercentDiffA );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiffA );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusOffsetA );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffsetA );

	hideEmbeddedObject( m_voUseUpperThresholdMinPlusOffsetA );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMinPlusOffsetA );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusPercentDiffA );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiffA );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusOffsetA );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusOffsetA );

	hideEmbeddedObject( m_voUpperThresholdMinPlusOffsetA );
	RemoveEmbeddedObject( &m_voUpperThresholdMinPlusOffsetA );

	// EdgeB
	hideEmbeddedObject( m_voUseLowerThresholdSelectableB );
	RemoveEmbeddedObject( &m_voUseLowerThresholdSelectableB );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusPercentDiffB );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusPercentDiffB );

	hideEmbeddedObject( m_voUseLowerThresholdMaxMinusOffsetB );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMaxMinusOffsetB );

	hideEmbeddedObject( m_voUseLowerThresholdMinPlusOffsetB );
	RemoveEmbeddedObject( &m_voUseLowerThresholdMinPlusOffsetB );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusPercentDiffB );
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusPercentDiffB );

	hideEmbeddedObject( m_voLowerThresholdMaxMinusOffsetB );
	RemoveEmbeddedObject( &m_voLowerThresholdMaxMinusOffsetB );

	hideEmbeddedObject( m_voLowerThresholdMinPlusOffsetB );
	RemoveEmbeddedObject( &m_voLowerThresholdMinPlusOffsetB );

	hideEmbeddedObject( m_voUseUpperThresholdSelectableB );
	RemoveEmbeddedObject( &m_voUseUpperThresholdSelectableB );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusPercentDiffB );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusPercentDiffB );

	hideEmbeddedObject( m_voUseUpperThresholdMaxMinusOffsetB );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMaxMinusOffsetB );

	hideEmbeddedObject( m_voUseUpperThresholdMinPlusOffsetB );
	RemoveEmbeddedObject( &m_voUseUpperThresholdMinPlusOffsetB );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusPercentDiffB );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusPercentDiffB );

	hideEmbeddedObject( m_voUpperThresholdMaxMinusOffsetB );
	RemoveEmbeddedObject( &m_voUpperThresholdMaxMinusOffsetB );

	hideEmbeddedObject( m_voUpperThresholdMinPlusOffsetB );
	RemoveEmbeddedObject( &m_voUpperThresholdMinPlusOffsetB );

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

HRESULT SVDualLineAnalyzerClass::GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	HRESULT l_Status = SVLineAnalyzerClass::GetEdgesOverlay( p_rsvMiltiLine );

	return l_Status;
}

HRESULT SVDualLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	HRESULT l_Status = SVLineAnalyzerClass::GetSelectedEdgeOverlays( p_MultiLine );

	SVImageExtentClass l_svExtents;

	if ( m_svLineExtent.GetImageExtent( l_svExtents ) == S_OK )
	{
		POINT edgeAPoint;
		POINT edgeBPoint;

		bool l_bIsAGageTool = true;
		double l_dHeight = 0.0;
		double l_dLineCorrect = 1.0;

		GetOverlayDrawlingAdjustments( l_bIsAGageTool, l_dHeight, l_dLineCorrect );

		if( ( edgeA.GetValue( edgeAPoint ) == S_OK ) &&
		    ( edgeB.GetValue( edgeBPoint ) == S_OK ) )
		{
			// check if this is a gage tool
			if( l_bIsAGageTool )
			{
				double l_dValue;

				l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX , l_dValue );
				edgeAPoint.x -= static_cast<long>(l_dValue);
				edgeBPoint.x -= static_cast<long>(l_dValue);

				l_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY , l_dValue );
				edgeAPoint.y -= static_cast<long>(l_dValue);
				edgeBPoint.y -= static_cast<long>(l_dValue);
			}

			if( !m_bDisplayAnalyzeLine || m_bSetupEdgeA )
			{
				SVExtentLineStruct l_EdgeALine;
				SVExtentPointStruct l_oGraphPoint;

				l_EdgeALine.m_bIsAngleValid = true;
				l_EdgeALine.m_dAngle = 90.0;

				l_oGraphPoint.m_dPositionX = edgeAPoint.x;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 - 10;
				
				l_EdgeALine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

				l_oGraphPoint.m_dPositionX = edgeAPoint.x;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 + 10;
				
				l_EdgeALine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

				if( l_bIsAGageTool )
				{
					l_svExtents.TranslateFromOutputSpace( l_EdgeALine, l_EdgeALine );
				}

				l_EdgeALine.m_dwColor = GetObjectColor();

				p_MultiLine.m_svLineArray.Add( l_EdgeALine );
			}

			// Update EdgeB Marker...
			// Update if we are not in the SetupDialog() or EdgeB page is current...
			if( !m_bDisplayAnalyzeLine || !m_bSetupEdgeA )
			{
				SVExtentLineStruct l_EdgeBLine;
				SVExtentPointStruct l_oGraphPoint;

				l_EdgeBLine.m_bIsAngleValid = true;
				l_EdgeBLine.m_dAngle = 90.0;

				l_oGraphPoint.m_dPositionX = edgeBPoint.x;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 - 10;
				
				l_EdgeBLine.m_svPointArray.SetAtGrow( 0, l_oGraphPoint );

				l_oGraphPoint.m_dPositionX = edgeBPoint.x;
				l_oGraphPoint.m_dPositionY = l_dHeight / 2 + 10;
				
				l_EdgeBLine.m_svPointArray.SetAtGrow( 1, l_oGraphPoint );

				l_EdgeBLine.m_dwColor = GetObjectColor();

				if( l_bIsAGageTool )
				{
					l_svExtents.TranslateFromOutputSpace( l_EdgeBLine, l_EdgeBLine );
				}

				p_MultiLine.m_svLineArray.Add( l_EdgeBLine );
			}
		}
	}

	return l_Status;
}

