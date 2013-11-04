//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDualLineAnalyzer
//* .File Name       : $Workfile:   SVDualLineAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:08  $
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
										( DWORD ) &inputMinThresholdObjectInfo, NULL );
	
	::SVSendMessage( inputMaxThresholdObjectInfo.GetInputObjectInfo().UniqueObjectID,
										SVM_DISCONNECT_OBJECT_INPUT, 
										( DWORD ) &inputMaxThresholdObjectInfo, NULL );
	
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDualLineAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:23:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   02 Aug 2012 16:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to draw the analyzer in the correct color when tool is disabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   01 Aug 2012 12:49:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   27 Jul 2012 08:13:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   25 Jul 2012 14:04:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   18 Jul 2012 13:31:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   12 Jul 2012 14:37:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   19 Jun 2012 12:41:50   jspila
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
 *    Rev 1.19   16 Mar 2011 13:29:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   01 Jun 2010 14:19:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Dec 2009 09:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2007 12:27:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Jul 2005 12:25:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 Jun 2005 08:04:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Mar 2005 09:05:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Feb 2005 13:59:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Apr 2004 12:07:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method SetCalculatedThresholdPrintableFlags()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   04 Dec 2003 12:15:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Nov 2003 14:05:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with value objects not updating correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Nov 2003 08:14:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with the edge/threshold parameters with the value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Oct 2003 16:23:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Oct 2003 10:53:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added value objects for EdgeA and EdgeB
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 09:16:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jan 2003 15:35:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject to correctly set the isCreated flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:16:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 14:19:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Mar 2000 16:40:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed hideVariable method 
 * (now SVTaskObjectClass::hideEmbeddedObject)
 * Revised DisableThresholdOutput to call 
 * hideEmbeddedObject instead of
 * hideVariable
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:14:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. 
 * New Intermediate base class for Line Analyzers, contains new thresholding options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
