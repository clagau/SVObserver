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
#include "SVGlobal.h"
#include "InspectionEngine/SVConditional.h"
#include "InspectionEngine/SVMathEquation.h"
#include "SVResultDouble.h"
#include "SVInspectionProcess.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVRange.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVMathToolClass, SVMathToolClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       : Constructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : This constructor inits all own members and the members of
//				: the parent class SVToolClass
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVImageClass*		Source				Pointer to an image object which is
//											the complete or partly source of
//											this tool
//	:LPCSTR				ObjectName			Pointer to a null terminated const
//	                                        string that contents an optional
//                                          class name.
//                                          Initiated in SVObjectClass.  
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::SVMathToolClass( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void SVMathToolClass::init(void)
{
	// Override base class exposure of the auxiliary extent variables
	// These values will not be exposed for this Tool.
	m_hasToolExtents = false;

	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::SVMathToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up the friend
	pMathEquation = new SVMathEquationClass( this );
	AddFriend( pMathEquation->GetUniqueObjectID() );

	// Setup the result
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Math Equation Result...
	interfaceInfo.EmbeddedID = SVMathEquationResultObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;

	// Construct the result
	SVDoubleResultClass* pResult = ( SVDoubleResultClass * )resultClassInfo.Construct();
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
	constexpr UINT cAttribute {SvDef::SV_SELECTABLE_ATTRIBUTES | SvDef::SV_PRINTABLE};
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
	SVRangeClass* pRangeObject = dynamic_cast<SVRangeClass*>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvDef::SVObjectTypeEnum::SVRangeObjectType)));
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