//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTEquation
//* .File Name       : $Workfile:   SVLUTEquation.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLUTEquation.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLUTEquation, SvPb::LUTEquationClassId);

SVLUTEquation::SVLUTEquation( SVObjectClass* POwner, int StringResourceID )
				   :EquationArray( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVLUTEquation::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVEquationObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVLUTEquationObjectType;

	// Identify our input type needs - this is a bit different here
	// Since out inputs are dynamic via the script specified
	// So the input will be identified when the script is created.
	
	// Register Embedded Objects
	m_byteVectorResult.SetLegacyVectorObjectCompatibility();
	RegisterEmbeddedObject( &m_byteVectorResult, SvPb::LUTEquationResultEId, IDS_OBJECTNAME_LUTRESULT, false, SvOi::SVResetItemNone  );
	RegisterEmbeddedObject( &m_isLUTFormulaClipped, SvPb::LUTEquationClipFlagEId, IDS_OBJECTNAME_LUT_EQUATION_CLIP, false, SvOi::SVResetItemTool  );

	// Set Embedded defaults
	m_byteVectorResult.SetDefaultValue( 0 );
	m_byteVectorResult.setSaveValueFlag(false);
	m_isLUTFormulaClipped.SetDefaultValue( BOOL(true) );

	// NOTE: Vector Size Setting...
	// Set default vector size in CreateObject, since objectDepth must be set before!!!
	//m_byteVectorResult.SetSize( 256, TRUE );
	// Place another check/set vector size at the position you want to use the vector
	// i.e. in onRun(...) : 
	//	if( vector.GetSize() != targetSize  ) { vector.SetSize( targetSize ); }
}

SVLUTEquation::~SVLUTEquation()
{
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : Creates this object.
////////////////////////////////////////////////////////////////////////////////
bool SVLUTEquation::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVEquation::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	m_byteVectorResult.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute );

	//For old configuration replace old lutIndex-parameter with IDX-keyword.
	std::string Name = "\"" + GetObjectNameToObjectType() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_LUTINDEXVARIABLE) + "\"";
	auto text = GetEquationText();
	if (GetEquationText() != SvUl::searchAndReplace(text, Name.c_str(), SvDef::cIndexKeyword))
	{
		SetEquationText(text);
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVLUTEquation::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	Result = setRunValues(0, 255) && Result;

	// Resize to 256 entries...
	if( Result && 256 != m_byteVectorResult.getArraySize()  )
	{
		m_byteVectorResult.SetArraySize( 256 );
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetDefaultFormula
// -----------------------------------------------------------------------------
// .Description : Sets the default formula, which is in this case
//				: the LUT Index variable of this LUT Equation class.
//				:
//              : The LUT Index variable is running from 0 to 255, while  
//				: onRun(...) of this class is processed. 
//				: ( Refer to SVLUTEquation::onRun(...) ).
//				: 
//				: The result of this equation is then placed in the result vector
//				:
//				: NOTE
//				: Call this function only if you have an tool set ancestor!
//				: ( i.e. if your correlated tool is added to the tool set,
//				:   after the user clicked on 'Add XXXTool' ).
////////////////////////////////////////////////////////////////////////////////
bool SVLUTEquation::SetDefaultFormula(SvStl::MessageContainerVector *pErrorMessages)
{
	// Set equation...
	SetEquationText(SvDef::cIndexKeyword);

	// Update symbol table and test...
	if( Test(pErrorMessages).bPassed )
	{
		return true;
	}
		
	// something is wrong, flush equation...
	SetEquationText( std::string() );

	return false;
}

bool SVLUTEquation::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool   bRetVal   = true;
	// Resize to 256 entries...
	if( m_byteVectorResult.getArraySize() != 256 )
	{
		m_byteVectorResult.SetArraySize( 256 );
	}

	// Run equation.. 
	bRetVal = __super::onRun(rRunStatus, pErrorMessages);

	if (bRetVal && HasCondition() && IsEnabled())
	{
		// Get equation result...
		auto values = getValueArray();
		//Clip result if required
		BOOL isClipped(false);
		m_isLUTFormulaClipped.GetValue(isClipped);
		if (isClipped)
		{
			for (auto& rValue : values)
			{
				if (UCHAR_MAX < rValue)
				{
					rValue = (double)UCHAR_MAX;
				}
				else if (0.0 > rValue)
				{
					rValue = 0.0;
				}
			}
		}
#pragma warning(push)
#pragma warning( disable : 4244 ) //disable warning because possible overlay by convert from double to byte it is known and wanted.
		std::vector<byte> byteVec(values.begin(), values.end());
#pragma warning(pop)
		m_byteVectorResult.SetArrayValues(byteVec);
	}
	else
	{
		std::vector<byte> byteVec;
		byteVec.resize(256, 0);
		m_byteVectorResult.SetArrayValues(byteVec);
	}
		 
	if( ! bRetVal )
	{
		rRunStatus.SetInvalid();
	}

	return bRetVal;
}

} //namespace SvOp
