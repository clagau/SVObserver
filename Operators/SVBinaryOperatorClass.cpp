//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVBinaryOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:56:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVBinaryOperatorClass.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVBinaryOperatorClass, SVBinaryOperatorClassGuid )

SVBinaryOperatorClass::SVBinaryOperatorClass( SVObjectClass* POwner, int StringResourceID )
					  :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

SVBinaryOperatorClass::~SVBinaryOperatorClass()
{
}

void SVBinaryOperatorClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVBinaryOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
}

} //namespace SvOp