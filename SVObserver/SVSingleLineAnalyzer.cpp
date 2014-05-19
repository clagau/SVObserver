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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSingleLineAnalyzer.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:50:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:08:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:06:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   12 Jul 2012 15:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Jun 2012 14:19:46   jspila
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
 *    Rev 1.16   27 Jan 2011 11:58:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   01 Jun 2010 15:13:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Dec 2009 12:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Jun 2007 14:49:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   29 Jul 2005 13:24:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Jun 2005 08:32:08   ebeyeler
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
 *    Rev 1.10   02 Apr 2004 12:07:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method SetCalculatedThresholdPrintableFlags()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Dec 2003 12:15:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Nov 2003 14:05:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with value objects not updating correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Oct 2003 16:23:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Oct 2003 11:10:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put value object for EdgeA parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 15:34:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2003 16:32:30   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject method to correctly use the isCreated flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 20:06:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 18:31:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Mar 2000 16:40:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed hideVariable method 
 * (now SVTaskObjectClass::hideEmbeddedObject)
 * Revised DisableThresholdOutput to call 
 * hideEmbeddedObject instead of
 * hideVariable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2000 09:14:36   sjones
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
