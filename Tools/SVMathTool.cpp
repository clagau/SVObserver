//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Feb 2015 18:21:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMathTool.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Operators/SVRange.h"
#include "Operators/SVConditional.h"
#include "Operators/SVMathEquation.h"
#include "Operators/SVResultDouble.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVMathToolClass, SVMathToolClassGuid );

SVMathToolClass::SVMathToolClass( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVMathToolClass::init(void)
{
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVMathToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up the friend
	m_pMathEquation = new SvOp::SVMathEquationClass( this );
	AddFriend( m_pMathEquation->GetUniqueObjectID() );

	// Setup the result
	SvIe::SVClassInfoStruct resultClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Math Equation Result...
	interfaceInfo.EmbeddedID = SVMathEquationResultObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
	std::string strTitle = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_DOUBLE );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;

	// Construct the result
	SvOp::SVDoubleResultClass* pResult = dynamic_cast<SvOp::SVDoubleResultClass*> (resultClassInfo.Construct());
	Add( pResult );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::~SVMathToolClass()
{ 
}

///////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
bool SVMathToolClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVToolClass::CreateObject(rCreateStructure);

	// These values will not be exposed for this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	return m_isCreated;
}

SvOi::ParametersForML SVMathToolClass::getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const
{
	bool isNoError = true;
	SvOi::ParametersForML retList;
	retList.push_back(SvOi::ParameterPairForML(m_statusColor.GetCompleteName(), m_statusColor.GetUniqueObjectID()));
	if (nullptr != m_pToolConditional)
	{
		retList.push_back(m_pToolConditional->getResultData());
	}
	else
	{
		isNoError = false;
	}
	isNoError = addEntryToMonitorList(retList, SVValueObjectGuid) && isNoError;
	SvOp::SVRangeClass* pRangeObject = dynamic_cast<SvOp::SVRangeClass*>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVObjectTypeEnum::SVRangeObjectType)));
	if (nullptr != pRangeObject)
	{
		pRangeObject->addEntriesToMonitorList(std::back_inserter(retList));
	}
	else
	{
		isNoError = false;
	}

	if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}

	return retList;
}

} //namespace SvTo
