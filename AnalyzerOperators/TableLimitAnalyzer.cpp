//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableLimitAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for limiting the rows of the table.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableLimitAnalyzer.h"
#include "SVObjectLibrary\SVClsids.h"
#include "Tools/TableAnalyzerTool.h"
#include "SVUtilityLibrary/StringHelper.h"
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

SV_IMPLEMENT_CLASS(TableLimitAnalyzer, TableAnaylzerLimitGuid);

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
bool TableLimitAnalyzer::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	
	if (returnValue)
	{
		double Value;
		m_LimitValue.getValue( Value );
		long LimitCount = static_cast<long> (Value);
		
		SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*>(m_ownerObjectInfo.getObject());
		
		if (nullptr != pTool)
		{
			SvVol::ValueObjectSortContainer sortContainer = pTool->getSortContainer();
			size_t sizeValues = sortContainer.size();
			if (LimitCount < sizeValues && 0 < LimitCount)
			{
				sortContainer.erase(sortContainer.begin()+LimitCount, sortContainer.end());
				pTool->setSortContainer(sortContainer, rRunStatus);
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::TableAnalyzerLimitType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableLimitAnalyzer::BuildEmbeddedObjectList()
{
	//set excludeHigh-Value
	RegisterEmbeddedObject( &m_LimitValue, TableAnaylzerLimitValueGuid, IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE, true, SvOi::SVResetItemTool );
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.lVal = cDefaultLimitValue;
	m_LimitValue.SetDefaultValue( vtTemp, true );

	std::string ObjectName = SvUl::LoadStdString( IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE );
	ObjectName +=  SvDef::cLinkName;
	RegisterEmbeddedObject( &m_LimitValue.getLinkedName(), TableAnaylzerLimitValue_LinkGuid, ObjectName.c_str(), false, SvOi::SVResetItemNone );
	m_LimitValue.getLinkedName().SetDefaultValue( _T(""), false );
}

void TableLimitAnalyzer::BuildInputObjectList()
{
	addDefaultInputObjects();
}
#pragma endregion Private Methods

} //namespace SvAo
