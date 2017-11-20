// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeAProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeAProcessingClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:52:56  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearEdgeAProcessingClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearEdgeAProcessingClass, SVLinearEdgeAProcessingClassGuid );


SVLinearEdgeAProcessingClass::SVLinearEdgeAProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                   :SVLinearEdgeProcessingClass( POwner, StringResourceID )
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVLinearEdgeAProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SVEdgeADirectionGuid, IDS_OBJECTNAME_EDGE_A_DIRECTION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPolarisation, SVEdgeAPolarisationGuid, IDS_OBJECTNAME_EDGE_A_POLARISATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelect, SVEdgeAEdgeSelectGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SVEdgeAEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT_THIS_VALUE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SVEdgeAIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_A_IS_FIXED_EDGE_MARKER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPosition, SVEdgeAPositionGuid, IDS_OBJECTNAME_EDGE_A_POSITION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SVEdgeAPositionOffsetGuid, IDS_OBJECTNAME_EDGE_A_POSITION_OFFSET, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SVUseLowerThresholdSelectableObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SVUseLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SVUseLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SVUseLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerThresholdValue, SVEdgeALowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svLowerThresholdValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SVLowerThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusPercentDiffValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SVLowerThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusOffsetValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SVLowerThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone );
	m_svLowerMinPlusOffsetValue.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SVUseUpperThresholdSelectableObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SVUseUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SVUseUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SVUseUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperThresholdValue, SVEdgeUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svUpperThresholdValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SVUpperThresholdMaxMinusPercentDiffObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusPercentDiffValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SVUpperThresholdMaxMinusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusOffsetValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SVUpperThresholdMinPlusOffsetObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone );
	m_svUpperMinPlusOffsetValue.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject(&m_svLinearEdges, SVLinearEdgesClassGuid, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SvOi::SVResetItemNone );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgeAProcessingClass::~SVLinearEdgeAProcessingClass()
{
}

