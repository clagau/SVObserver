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

#pragma region Includes
#include "stdafx.h"
#include "SVLinearEdgeAProcessingClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearEdgeAProcessingClass, SvPb::LinearEdgeAProcessingClassId);


SVLinearEdgeAProcessingClass::SVLinearEdgeAProcessingClass(SVObjectClass* POwner, int StringResourceID)
	:SVLinearEdgeProcessingClass(POwner, StringResourceID)
{
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgeAProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SvPb::EdgeADirectionEId, IDS_OBJECTNAME_EDGE_A_DIRECTION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPolarisation, SvPb::EdgeAPolarisationEId, IDS_OBJECTNAME_EDGE_A_POLARISATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelect, SvPb::EdgeAEdgeSelectEId, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SvPb::EdgeAEdgeSelectThisValueEId, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT_THIS_VALUE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SvPb::EdgeAIsFixedEdgeMarkerEId, IDS_OBJECTNAME_EDGE_A_IS_FIXED_EDGE_MARKER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPosition, SvPb::EdgeAPositionEId, IDS_OBJECTNAME_EDGE_A_POSITION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SvPb::EdgeAPositionOffsetEId, IDS_OBJECTNAME_EDGE_A_POSITION_OFFSET, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SvPb::UseLowerThresholdSelectableEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SvPb::UseLowerThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SvPb::UseLowerThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SvPb::UseLowerThresholdMinPlusOffsetEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerThresholdValue, SvPb::EdgeALowerThresholdValueEId, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svLowerThresholdValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SvPb::LowerThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SvPb::LowerThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SvPb::LowerThresholdMinPlusOffsetEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone );
	m_svLowerMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SvPb::UseUpperThresholdSelectableEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SvPb::UseUpperThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SvPb::UseUpperThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SvPb::UseUpperThresholdMinPlusOffsetEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperThresholdValue, SvPb::EdgeUpperThresholdValueEId, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone );
	m_svUpperThresholdValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SvPb::UpperThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SvPb::UpperThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SvPb::UpperThresholdMinPlusOffsetEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone );
	m_svUpperMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(&m_svLinearEdges, SvPb::LinearEdgesEId, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SvOi::SVResetItemNone );

	//! This cannot be initialized direct during construction because m_EdgeEmbeddedIds is inherited
	m_EdgeEmbeddedIds = std::vector<SvPb::EmbeddedIdEnum>
	{
		SvPb::EdgeADirectionEId, SvPb::EdgeAPolarisationEId, SvPb::EdgeAEdgeSelectEId, SvPb::EdgeAEdgeSelectThisValueEId,
		SvPb::EdgeAIsFixedEdgeMarkerEId, SvPb::EdgeAPositionEId, SvPb::EdgeAPositionOffsetEId, SvPb::UseLowerThresholdSelectableEId,
		SvPb::UseLowerThresholdMaxMinusPercentDiffEId, SvPb::UseLowerThresholdMaxMinusOffsetEId, SvPb::UseLowerThresholdMinPlusOffsetEId,
		SvPb::EdgeALowerThresholdValueEId, SvPb::LowerThresholdMaxMinusPercentDiffEId, SvPb::LowerThresholdMaxMinusOffsetEId,
		SvPb::LowerThresholdMinPlusOffsetEId, SvPb::UseUpperThresholdSelectableEId, SvPb::UseUpperThresholdMaxMinusPercentDiffEId,
		SvPb::UseUpperThresholdMaxMinusOffsetEId, SvPb::UseUpperThresholdMinPlusOffsetEId, SvPb::EdgeUpperThresholdValueEId,
		SvPb::UpperThresholdMaxMinusPercentDiffEId, SvPb::UpperThresholdMaxMinusOffsetEId, SvPb::UpperThresholdMinPlusOffsetEId,
		SvPb::LinearEdgesEId
	};
}

SVLinearEdgeAProcessingClass::~SVLinearEdgeAProcessingClass()
{
}

} //namespace SvOp
