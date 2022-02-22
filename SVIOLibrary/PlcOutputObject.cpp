//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PlcOutputObject.cpp
/// \brief Implementation of the class CifXCard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "PlcOutputObject.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#pragma endregion Includes

PlcOutputObject::PlcOutputObject( LPCSTR strObjectName ) :SVOutputObject( strObjectName )
{
	LocalInitialize();
}

PlcOutputObject::PlcOutputObject( SVObjectClass *pOwner, int StringResourceID )
:SVOutputObject( pOwner, StringResourceID )
{
	LocalInitialize();
}

void PlcOutputObject::Persist(SvOi::IObjectWriter& rWriter) const
{
	_variant_t svVariant;
	svVariant.SetString(GetName());
	rWriter.WriteAttribute(SvXml::CTAG_IO_ENTRY_NAME, svVariant);
	svVariant.Clear();

	svVariant = static_cast<int> (GetObjectSubType());
	rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
	svVariant.Clear();

	svVariant = GetChannel();
	rWriter.WriteAttribute(SvXml::CTAG_CHANNEL, svVariant);
	svVariant.Clear();

	svVariant = true;
	rWriter.WriteAttribute(SvXml::CTAG_IS_OUTPUT, svVariant);
	svVariant.Clear();

	svVariant = isCombined();
	rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED, svVariant);
	svVariant.Clear();

	svVariant = isAndACK();
	rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED_ACK, svVariant);
	svVariant.Clear();

}
void PlcOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	m_isCombined = bCombine;
	m_isAndACK = bCombineACK;
}

void PlcOutputObject::LocalInitialize()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::PlcOutputObjectType;
	m_startID = ObjectIdEnum::PlcOutputId;
}

