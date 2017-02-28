//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableExcludeAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for sorting the table.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableExcludeAnalyzer.h"
#include "SVTool.h"
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

static const double cDefaultHighValue = 999999;
static const double cDefaultLowValue = -999999;
static const VARTYPE cVarType_Value = VT_R8;

SV_IMPLEMENT_CLASS(TableExcludeAnalyzer, TableAnaylzerExcludeGuid);

#pragma endregion Declarations

#pragma region Constructor
TableExcludeAnalyzer::TableExcludeAnalyzer( LPCTSTR ObjectName )
	: SVTaskObjectClass(ObjectName)
{
	Initialize();
}

TableExcludeAnalyzer::TableExcludeAnalyzer( SVObjectClass* pOwner, int StringResourceID )
	: SVTaskObjectClass(pOwner, StringResourceID)
{
	Initialize();
}

TableExcludeAnalyzer::~TableExcludeAnalyzer()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL TableExcludeAnalyzer::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = __super::CreateObject( pCreateStructure );

	m_excludeHigh.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
	m_excludeLow.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;

	return l_bOk;
}

bool TableExcludeAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SVObjectClass* pObject = m_excludeColumnObjectInfo.GetInputObjectInfo().PObject;
	if (!m_excludeColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<DoubleSortValueObject*>(pObject) 
		//check if column part of the right table object (The object must be from same tool as this analyzer.)
		|| nullptr == pObject->GetOwner() || pObject->GetOwner()->GetOwner() != m_ownerObjectInfo.PObject)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoValidColumnConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	if (!m_excludeLow.isIndirectValue() && !m_excludeHigh.isIndirectValue())
	{
		//check if high greater than low
		double excludeHigh;
		double excludeLow;
		m_excludeHigh.GetValue(excludeHigh);
		m_excludeLow.GetValue(excludeLow);
		if (excludeHigh<excludeLow)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ExcludeHighMustBeHigher, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back( message );
			}
		}
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableExcludeAnalyzer::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	
	if (returnValue)
	{
		double excludeHigh;
		double excludeLow;
		m_excludeHigh.GetValue(excludeHigh);
		m_excludeLow.GetValue(excludeLow);

		TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*>(m_ownerObjectInfo.PObject);
		DoubleSortValueObject* pColumnValues = dynamic_cast<DoubleSortValueObject*>(m_excludeColumnObjectInfo.GetInputObjectInfo().PObject);
		if (nullptr != pTool && nullptr != pColumnValues)
		{
			ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer(rRunStatus.m_lResultDataIndex);
			size_t sizeValues = sortContainer.size();
			if (0 < sizeValues)
			{
				double value;
				for (int i=static_cast<int>(sizeValues)-1; 0 <= i; --i)
				{
					pColumnValues->GetValue(rRunStatus.m_lResultDataIndex, i, value);
					if (excludeHigh<value || excludeLow>value)
					{
						sortContainer.erase(sortContainer.begin()+i);
					}
				}
				pTool->setSortContainer(sortContainer, rRunStatus);
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableExcludeAnalyzer::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = TableAnalyzerExcludeType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableExcludeAnalyzer::BuildEmbeddedObjectList()
{
	//set excludeHigh-Value
	RegisterEmbeddedObject( &m_excludeHigh, TableAnaylzerExcludeHighGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE, true, SVResetItemTool );
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultHighValue;
	m_excludeHigh.SetDefaultValue( vtTemp, true );

	SVString ObjectName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeHigh.getLinkedName(), TableAnaylzerExcludeHigh_LinkGuid, ObjectName.c_str(), false, SVResetItemNone );
	m_excludeHigh.getLinkedName().SetDefaultValue( _T(""), false );

	//set excludeLow-Value
	RegisterEmbeddedObject( &m_excludeLow, TableAnaylzerExcludeLowGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE, true, SVResetItemTool );
	//vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultLowValue;
	m_excludeLow.SetDefaultValue( vtTemp, true );

	ObjectName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeLow.getLinkedName(), TableAnaylzerExcludeLow_LinkGuid, ObjectName.c_str(), false, SVResetItemNone );
	m_excludeLow.getLinkedName().SetDefaultValue( _T(""), false );
}

void TableExcludeAnalyzer::BuildInputObjectList()
{
	// Source Table.
	m_excludeColumnObjectInfo.SetInputObjectType( DoubleSortValueObjectType );
	m_excludeColumnObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_excludeColumnObjectInfo, SvO::cInputTag_ExcludeColumn );

	addDefaultInputObjects();
}
#pragma endregion Private Methods
