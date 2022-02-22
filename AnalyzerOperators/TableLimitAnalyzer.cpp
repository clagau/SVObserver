//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TableLimitAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for limiting the rows of the table.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableLimitAnalyzer.h"
#include "Tools/TableAnalyzerTool.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int cDefaultLimitValue = 50;
constexpr VARTYPE cVarType_Value = VT_I4;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableLimitAnalyzer, SvPb::TableAnaylzerLimitClassId);

#pragma region Constructor
TableLimitAnalyzer::TableLimitAnalyzer( LPCTSTR ObjectName )
	: SVTaskObjectClass(ObjectName)
{
	Initialize();
}

TableLimitAnalyzer::TableLimitAnalyzer( SVObjectClass* pOwner, int StringResourceID )
	: SVTaskObjectClass(pOwner, StringResourceID)
{
	Initialize();
}

TableLimitAnalyzer::~TableLimitAnalyzer()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableLimitAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = __super::CreateObject(rCreateStructure);

	m_LimitValue.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	
	return l_bOk;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableLimitAnalyzer::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	
	if (returnValue)
	{
		double Value;
		m_LimitValue.getValue( Value );
		long LimitCount = static_cast<long> (Value);
		
		SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*>(GetParent());
		
		if (nullptr != pTool)
		{
			SvVol::ValueObjectSortContainer& rSortContainer = pTool->getSortContainer();
			size_t sizeValues = rSortContainer.size();
			if (LimitCount < sizeValues && 0 < LimitCount)
			{
				rSortContainer.erase(rSortContainer.begin()+LimitCount, rSortContainer.end());
				pTool->UpdateNumberOfRows();
			
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return returnValue;
}

#pragma endregion Protected Methods

#pragma region Private Methods
void TableLimitAnalyzer::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::TableAnalyzerType;
	m_ObjectTypeInfo.m_SubType    = SvPb::TableAnalyzerLimitType;

	BuildEmbeddedObjectList();
}

void TableLimitAnalyzer::BuildEmbeddedObjectList()
{
	//set excludeHigh-Value
	RegisterEmbeddedObject( &m_LimitValue, SvPb::TableAnaylzerLimitValueEId, IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE, true, SvOi::SVResetItemTool );
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.lVal = cDefaultLimitValue;
	m_LimitValue.SetDefaultValue( vtTemp, true );
}
#pragma endregion Private Methods

} //namespace SvAo
