//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathContainer
//* .File Name       : $Workfile:   SVMathContainer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMathContainer.h"
#include "SVMathEquation.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVMathContainerClass, SVMathContainerClassGuid )

SVMathContainerClass::SVMathContainerClass( SVObjectClass* POwner, int StringResourceID )
					 :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVMathContainerObjectType;

	// Identify our input type needs...

	// Input Math Result...
	m_inputMathResult.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::MathEquationResultEId);
	m_inputMathResult.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputMathResult, _T( "MathResult" ) );

	// Register Embedded Objects

	// Set Embedded defaults

	// Make a friend
	SVMathEquationClass* pMathEquation = new SVMathEquationClass( this );
	AddFriend( pMathEquation->GetUniqueObjectID() );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVMathContainerClass::~SVMathContainerClass()
{
	SVMathContainerClass::CloseObject();
}

bool SVMathContainerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure ) && nullptr != GetInspection() && 
		nullptr != GetTool() && nullptr != SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputMathResult);
	return m_isCreated;
}

bool SVMathContainerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_inputMathResult);

	if( nullptr == SvOl::getInput<SvVol::SVDoubleValueObjectClass>(m_inputMathResult) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

} //namespace SvOp
