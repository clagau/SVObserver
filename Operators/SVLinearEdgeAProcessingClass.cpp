// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgeAProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SvPb::EdgeADirectionEId, IDS_OBJECTNAME_EDGE_A_DIRECTION, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svPolarisation, SvPb::EdgeAPolarisationEId, IDS_OBJECTNAME_EDGE_A_POLARISATION, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svEdgeSelect, SvPb::EdgeAEdgeSelectEId, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SvPb::EdgeAEdgeSelectThisValueEId, IDS_OBJECTNAME_EDGE_A_EDGE_SELECT_THIS_VALUE, false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SvPb::EdgeAIsFixedEdgeMarkerEId, IDS_OBJECTNAME_EDGE_A_IS_FIXED_EDGE_MARKER, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svPosition, SvPb::EdgeAPositionEId, IDS_OBJECTNAME_EDGE_A_POSITION, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SvPb::EdgeAPositionOffsetEId, IDS_OBJECTNAME_EDGE_A_POSITION_OFFSET, false, SvOi::SVResetItemNone, true);

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SvPb::UseLowerThresholdSelectableEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SvPb::UseLowerThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SvPb::UseLowerThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SvPb::UseLowerThresholdMinPlusOffsetEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject(&m_svLowerThresholdValueObject, SvPb::EdgeALowerThresholdValueEId, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone, true);
	m_svLowerThresholdValueObject.SetOutputFormat(SvVol::OutputFormat_int);
	m_svLowerThresholdValueObject.setSaveValueFlag(false);
	m_svLowerThresholdValueObject.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	RegisterEmbeddedObject( &m_svLowerThresholdSelected, SvPb::EdgeALowerThresholdSelectedValueEId, IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_SELECTED, false, SvOi::SVResetItemNone, true);
	m_svLowerThresholdSelected.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SvPb::LowerThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone, true);
	m_svLowerMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SvPb::LowerThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone, true);
	m_svLowerMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SvPb::LowerThresholdMinPlusOffsetEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone, true);
	m_svLowerMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SvPb::UseUpperThresholdSelectableEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SvPb::UseUpperThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SvPb::UseUpperThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SvPb::UseUpperThresholdMinPlusOffsetEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_A, false, SvOi::SVResetItemNone, true);
	RegisterEmbeddedObject(&m_svUpperThresholdValueObject, SvPb::EdgeAUpperThresholdValueEId, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone, true);
	m_svUpperThresholdValueObject.SetOutputFormat(SvVol::OutputFormat_int);
	m_svUpperThresholdValueObject.setSaveValueFlag(false);
	m_svUpperThresholdValueObject.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	RegisterEmbeddedObject( &m_svUpperThresholdSelected, SvPb::EdgeAUpperThresholdSelectedValueEId, IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_SELECTED, false, SvOi::SVResetItemNone, true);
	m_svUpperThresholdSelected.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SvPb::UpperThresholdMaxMinusPercentDiffEId, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_A, false, SvOi::SVResetItemNone, true);
	m_svUpperMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SvPb::UpperThresholdMaxMinusOffsetEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_A, false, SvOi::SVResetItemNone, true);
	m_svUpperMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SvPb::UpperThresholdMinPlusOffsetEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_A, false, SvOi::SVResetItemNone, true);
	m_svUpperMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(&m_svLinearEdges, SvPb::LinearEdgesEId, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SvOi::SVResetItemNone, false );
}

SVLinearEdgeAProcessingClass::~SVLinearEdgeAProcessingClass()
{
}


bool SVLinearEdgeAProcessingClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if (m_svLowerThresholdSelected.GetName() == SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE) || m_svLowerThresholdSelected.GetName() == SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_SELECTED))
	{
		m_svLowerThresholdSelected.SetObjectName(SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_SELECTED).c_str());
	}
	if (m_svUpperThresholdSelected.GetName() == SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_VALUE))
	{
		m_svUpperThresholdSelected.SetObjectName(SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_UPPER_THRESHOLD_SELECTED).c_str());
	}

	return __super::CreateObject(rCreateStructure);
}
} //namespace SvOp
