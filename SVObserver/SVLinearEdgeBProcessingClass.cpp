// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeBProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeBProcessingClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:53:16  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "Definitions/LinearEdgeEnums.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearEdgeBProcessingClass, SVLinearEdgeBProcessingClassGuid );


SVLinearEdgeBProcessingClass::SVLinearEdgeBProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                   :SVLinearEdgeProcessingClass( POwner, StringResourceID )
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVLinearEdgeBProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SVEdgeBDirectionGuid, IDS_OBJECTNAME_EDGE_B_DIRECTION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPolarisation, SVEdgeBPolarisationGuid, IDS_OBJECTNAME_EDGE_B_POLARISATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelect, SVEdgeBEdgeSelectGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SVEdgeBEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT_THIS_VALUE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SVEdgeBIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_B_IS_FIXED_EDGE_MARKER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPosition, SVEdgeBPositionGuid, IDS_OBJECTNAME_EDGE_B_POSITION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SVEdgeBPositionOffsetGuid, IDS_OBJECTNAME_EDGE_B_POSITION_OFFSET, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SVUseLowerThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SVUseLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SVUseLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerThresholdValue, SVEdgeBLowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_LOWER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svLowerThresholdValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SVLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_B, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusPercentDiffValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SVLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_B, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusOffsetValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SVLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_B, false, SvOi::SVResetItemNone );
	m_svLowerMinPlusOffsetValue.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SVUseUpperThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SVUseUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SVUseUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperThresholdValue, SVEdgeBUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_UPPER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svUpperThresholdValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SVUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_B, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusPercentDiffValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SVUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_B, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusOffsetValue.SetOutputFormat(OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SVUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_B, false, SvOi::SVResetItemNone );
	m_svUpperMinPlusOffsetValue.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject(&m_svLinearEdges, SVLinearEdgesClassGuid, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SvOi::SVResetItemNone );

	m_svDirection.SetEnumTypes(SvDef::cDirectionEnums);
	m_svPolarisation.SetEnumTypes(SvDef::cPolarisationEnums);
	m_svEdgeSelect.SetEnumTypes(SvDef::cEdgeSelectEnums);
	m_svPosition.SetEnumTypes(SvDef::cPositionEnums);

	//! This cannot be initialized direct during construction because m_EdgeEmbeddedGuids is inherited
	m_EdgeEmbeddedGuids = SVGuidVector
	{
		SVEdgeBDirectionGuid, SVEdgeBPolarisationGuid, SVEdgeBEdgeSelectGuid, SVEdgeBEdgeSelectThisValueGuid,
		SVEdgeBIsFixedEdgeMarkerGuid, SVEdgeBPositionGuid, SVEdgeBPositionOffsetGuid, SVUseLowerThresholdSelectableBObjectGuid,
		SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid, SVUseLowerThresholdMaxMinusOffsetBObjectGuid, SVUseLowerThresholdMinPlusOffsetBObjectGuid,
		SVEdgeBLowerThresholdValueGuid, SVLowerThresholdMaxMinusPercentDiffBObjectGuid, SVLowerThresholdMaxMinusOffsetBObjectGuid,
		SVLowerThresholdMinPlusOffsetBObjectGuid, SVUseUpperThresholdSelectableBObjectGuid, SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid,
		SVUseUpperThresholdMaxMinusOffsetBObjectGuid, SVUseUpperThresholdMinPlusOffsetBObjectGuid, SVEdgeBUpperThresholdValueGuid,
		SVUpperThresholdMaxMinusPercentDiffBObjectGuid, SVUpperThresholdMaxMinusOffsetBObjectGuid, SVUpperThresholdMinPlusOffsetBObjectGuid,
		SVLinearEdgesClassGuid 
	};

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgeBProcessingClass::~SVLinearEdgeBProcessingClass()
{
}

