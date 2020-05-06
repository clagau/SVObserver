//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file PlcOutputObject.cpp
/// \brief Implementation of the class CifXCard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "PlcOutputObject.h"
#include "SVObjectLibrary/SVClsids.h"
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

void PlcOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	m_isCombined = bCombine;
	m_isAndACK = bCombineACK;
}

void PlcOutputObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::PlcOutputObjectType;
	m_startID = ObjectIdEnum::PlcOutputId;
}

