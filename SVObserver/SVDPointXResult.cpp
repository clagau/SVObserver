//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointXResult
//* .File Name       : $Workfile:   SVDPointXResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointXResult.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDPointXResultClass, SVDPointXResultClassGuid );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointXResultClass::SVDPointXResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVResultDPointXObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVDPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DPointXResult" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &x, SVDXObjectGuid, IDS_OBJECTNAME_DX, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	x.SetDefaultValue( 0.0, TRUE );
	x.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVDXObjectGuid;
	rangeClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = (SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVDPointXResultClass::~SVDPointXResultClass()
{

}

bool SVDPointXResultClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(pCreateStructure);
	bOk &= nullptr != getInputPoint();

	// Set / Reset Printable Flag
	x.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	
	m_isCreated = bOk;

	return bOk;
}

SVDPointValueObjectClass* SVDPointXResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVDPointValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

bool SVDPointXResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( rRunStatus, pErrorMessages ) )
	{
		SVDPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		SVDPointClass DPoint;
		pPoint->GetValue( DPoint );

		// Set X
		x.SetValue(DPoint.x);

		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

