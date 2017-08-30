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
#include "SVOCore/SVTool.h"
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
bool TableExcludeAnalyzer::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool l_bOk = __super::CreateObject( pCreateStructure );

	m_excludeHigh.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_excludeLow.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );

	return l_bOk;
}

bool TableExcludeAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SVObjectClass* pObject = m_excludeColumnObjectInfo.GetInputObjectInfo().m_pObject;
	if (!m_excludeColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<DoubleSortValueObject*> (pObject) 
		//check if column part of the right table object (The object must be from same tool as this analyzer.)
		|| nullptr == pObject->GetOwner() || pObject->GetOwner()->GetOwner() != m_ownerObjectInfo.m_pObject)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidColumnConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	if (!m_excludeLow.isIndirectValue() && !m_excludeHigh.isIndirectValue())
	{
		//check if high greater than low
		_variant_t TempValue;
		m_excludeHigh.GetValue( TempValue );
		double excludeHigh = static_cast<double> (TempValue);
		m_excludeLow.GetValue( TempValue );
		double excludeLow= static_cast<double> (TempValue);
		
		if( excludeHigh < excludeLow )
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ExcludeHighMustBeHigher, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
		_variant_t TempValue;
		m_excludeHigh.GetValue( TempValue );
		double excludeHigh = static_cast<double> (TempValue);
		m_excludeLow.GetValue( TempValue );
		double excludeLow= static_cast<double> (TempValue);

		TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*>(m_ownerObjectInfo.m_pObject);
		DoubleSortValueObject* pColumnValues = dynamic_cast<DoubleSortValueObject*>(m_excludeColumnObjectInfo.GetInputObjectInfo().m_pObject);
		if (nullptr != pTool && nullptr != pColumnValues)
		{
			ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer();
			size_t sizeValues = sortContainer.size();
			if (0 < sizeValues)
			{
				double Value;
				for (int i=static_cast<int>(sizeValues)-1; 0 <= i; --i)
				{
					pColumnValues->GetValue(Value, i);
					if (excludeHigh<Value || excludeLow>Value)
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = TableAnalyzerExcludeType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableExcludeAnalyzer::BuildEmbeddedObjectList()
{
	//set excludeHigh-Value
	RegisterEmbeddedObject( &m_excludeHigh, TableAnaylzerExcludeHighGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE, true, SvOi::SVResetItemTool );
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultHighValue;
	m_excludeHigh.SetDefaultValue( vtTemp, true );

	SVString ObjectName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeHigh.getLinkedName(), TableAnaylzerExcludeHigh_LinkGuid, ObjectName.c_str(), false, SvOi::SVResetItemNone );
	m_excludeHigh.getLinkedName().SetDefaultValue( _T(""), false );

	//set excludeLow-Value
	RegisterEmbeddedObject( &m_excludeLow, TableAnaylzerExcludeLowGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE, true, SvOi::SVResetItemTool );
	//vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultLowValue;
	m_excludeLow.SetDefaultValue( vtTemp, true );

	ObjectName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeLow.getLinkedName(), TableAnaylzerExcludeLow_LinkGuid, ObjectName.c_str(), false, SvOi::SVResetItemNone );
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
