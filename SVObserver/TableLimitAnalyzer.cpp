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
#include "TableAnalyzerTool.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int cDefaultLimitValue = 50;
static const VARTYPE cVarType_Value = VT_I4;

SV_IMPLEMENT_CLASS(TableLimitAnalyzer, TableAnaylzerLimitGuid);

#pragma endregion Declarations

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
BOOL TableLimitAnalyzer::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = __super::CreateObject( pCreateStructure );

	m_LimitValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
	
	return l_bOk;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL TableLimitAnalyzer::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = __super::onRun( rRunStatus );
	
	if (returnValue)
	{
		long LimitCount;
		m_LimitValue.GetValue(LimitCount);
		
		TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*>(m_ownerObjectInfo.PObject);
		
		if (nullptr != pTool)
		{
			ValueObjectSortContainer sortContainer = pTool->getSortContainer();
			size_t sizeValues = sortContainer.size();
			if (LimitCount < sizeValues && 0 < LimitCount)
			{
				sortContainer.erase(sortContainer.begin()+LimitCount, sortContainer.end());
				pTool->setSortContainer(sortContainer, rRunStatus);
			}
		}
		else
		{
			returnValue = FALSE;
		}
	}

	return returnValue;
}

#pragma endregion Protected Methods

#pragma region Private Methods
void TableLimitAnalyzer::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = TableAnalyzerLimitType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableLimitAnalyzer::BuildEmbeddedObjectList()
{
	//set excludeHigh-Value
	RegisterEmbeddedObject( &m_LimitValue, TableAnaylzerLimitValueGuid, IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE, true, SVResetItemTool );
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.lVal = cDefaultLimitValue;
	m_LimitValue.SetDefaultValue( vtTemp, true );

	SVString ObjectName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_LimitValue.getLinkedName(), TableAnaylzerLimitValue_LinkGuid, ObjectName.c_str(), false, SVResetItemNone );
	m_LimitValue.getLinkedName().SetDefaultValue( _T(""), false );
}

void TableLimitAnalyzer::BuildInputObjectList()
{
	addDefaultInputObjects();
}
#pragma endregion Private Methods
