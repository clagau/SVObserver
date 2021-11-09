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

SV_IMPLEMENT_CLASS( SVLinearEdgeBProcessingClass, SvPb::LinearEdgeBProcessingClassId);


SVLinearEdgeBProcessingClass::SVLinearEdgeBProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                   :SVLinearEdgeProcessingClass( POwner, StringResourceID )
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearEdgeBProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SvPb::EdgeBDirectionEId, IDS_OBJECTNAME_EDGE_B_DIRECTION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPolarisation, SvPb::EdgeBPolarisationEId, IDS_OBJECTNAME_EDGE_B_POLARISATION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelect, SvPb::EdgeBEdgeSelectEId, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SvPb::EdgeBEdgeSelectThisValueEId, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT_THIS_VALUE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SvPb::EdgeBIsFixedEdgeMarkerEId, IDS_OBJECTNAME_EDGE_B_IS_FIXED_EDGE_MARKER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPosition, SvPb::EdgeBPositionEId, IDS_OBJECTNAME_EDGE_B_POSITION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SvPb::EdgeBPositionOffsetEId, IDS_OBJECTNAME_EDGE_B_POSITION_OFFSET, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SvPb::UseLowerThresholdSelectableBEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SvPb::UseLowerThresholdMaxMinusPercentDiffBEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SvPb::UseLowerThresholdMaxMinusOffsetBEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SvPb::UseLowerThresholdMinPlusOffsetBEId, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svLowerThresholdValueObject, SvPb::EdgeBLowerThresholdValueEId, IDS_OBJECTNAME_EDGE_B_LOWER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone);
	m_svLowerThresholdValueObject.SetOutputFormat(SvVol::OutputFormat_int);
	m_svLowerThresholdValueObject.setSaveValueFlag(false);
	m_svLowerThresholdValueObject.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	RegisterEmbeddedObject( &m_svLowerThresholdSelected, SvPb::EdgeBLowerThresholdSelectedValueEId, IDS_OBJECTNAME_EDGE_B_LOWER_THRESHOLD_SELECTED, false, SvOi::SVResetItemNone );
	m_svLowerThresholdSelected.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SvPb::LowerThresholdMaxMinusPercentDiffBEId, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_B, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SvPb::LowerThresholdMaxMinusOffsetBEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_B, false, SvOi::SVResetItemNone );
	m_svLowerMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SvPb::LowerThresholdMinPlusOffsetBEId, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_B, false, SvOi::SVResetItemNone );
	m_svLowerMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SvPb::UseUpperThresholdSelectableBEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SvPb::UseUpperThresholdMaxMinusPercentDiffBEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SvPb::UseUpperThresholdMaxMinusOffsetBEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SvPb::UseUpperThresholdMinPlusOffsetBEId, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svUpperThresholdValueObject, SvPb::EdgeBUpperThresholdValueEId, IDS_OBJECTNAME_EDGE_B_UPPER_THRESHOLD_VALUE, false, SvOi::SVResetItemNone);
	m_svUpperThresholdValueObject.SetOutputFormat(SvVol::OutputFormat_int);
	m_svUpperThresholdValueObject.setSaveValueFlag(false);
	m_svUpperThresholdValueObject.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	RegisterEmbeddedObject( &m_svUpperThresholdSelected, SvPb::EdgeBUpperThresholdSelectedValueEId, IDS_OBJECTNAME_EDGE_B_UPPER_THRESHOLD_SELECTED, false, SvOi::SVResetItemNone );
	m_svUpperThresholdSelected.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SvPb::UpperThresholdMaxMinusPercentDiffBEId, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_B, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusPercentDiffValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SvPb::UpperThresholdMaxMinusOffsetBEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_B, false, SvOi::SVResetItemNone );
	m_svUpperMaxMinusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SvPb::UpperThresholdMinPlusOffsetBEId, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_B, false, SvOi::SVResetItemNone );
	m_svUpperMinPlusOffsetValue.SetOutputFormat(SvVol::OutputFormat_int);

	RegisterEmbeddedObject(&m_svLinearEdges, SvPb::LinearEdgesEId, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SvOi::SVResetItemNone );

	m_svDirection.SetEnumTypes(SvDef::cDirectionEnums);
	m_svPolarisation.SetEnumTypes(SvDef::cPolarisationEnums);
	m_svEdgeSelect.SetEnumTypes(SvDef::cEdgeSelectEnums);
	m_svPosition.SetEnumTypes(SvDef::cPositionEnums);
}

SVLinearEdgeBProcessingClass::~SVLinearEdgeBProcessingClass()
{
}

bool SVLinearEdgeBProcessingClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if (m_svLowerThresholdSelected.GetName() == SvUl::LoadStdString(IDS_OBJECTNAME_EDGE_A_LOWER_THRESHOLD_VALUE))
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
