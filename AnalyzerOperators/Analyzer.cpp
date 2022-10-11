//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzer.cpp
//* .File Name       : $Workfile:   SVAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Analyzer.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( Analyzer, SVAnalyzerClassId );

Analyzer::Analyzer( LPCSTR ObjectName )
: SVTaskObjectListClass( ObjectName ) 
{
	init();
}

Analyzer::Analyzer( SVObjectClass* POwner, int StringResourceID )
: SVTaskObjectListClass( POwner, StringResourceID ) 
{
	init();
}

// Initialization of newly Instantiated Object
void Analyzer::init()
{
	// Indentify our type in the Output List
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
}

Analyzer::~Analyzer()
{
}

bool Analyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure) && nullptr != GetInspection() && nullptr != GetTool();

	return m_isCreated;
}

SvOi::IObjectClass* Analyzer::GetResultObject()
{
	SvDef::SVObjectTypeInfoStruct TypeInfo{SvPb::SVResultObjectType};
	return getFirstObject(TypeInfo);
}

SvPb::InspectionCmdResponse Analyzer::getAnalyzerExtentProperties() const
{
	SvPb::InspectionCmdResponse cmdResponse;
	SvPb::GetExtentParameterResponse* pResponse = cmdResponse.mutable_getextentparameterresponse();
	m_imageExtent.getExtentProperties(*pResponse->mutable_parameters());
	pResponse->set_translationtype(m_imageExtent.GetTranslation());
	return cmdResponse;
}

bool Analyzer::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = this;
	
	return rChildObject.createAllObjects(createStruct);
}

void Analyzer::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = GetInspection();
	createStruct.m_pTool = GetTool();
	createStruct.m_pAnalyzer = this;

	rChildObject.ConnectObject(createStruct);
}

} //namespace SvAo
