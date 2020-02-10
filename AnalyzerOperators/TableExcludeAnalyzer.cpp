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
#include "SVObjectLibrary\SVClsids.h"
#include "Tools/TableAnalyzerTool.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr double cDefaultHighValue = 999999;
constexpr double cDefaultLowValue = -999999;
constexpr VARTYPE cVarType_Value = VT_R8;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableExcludeAnalyzer, SvPb::TableAnaylzerExcludeClassId);

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
bool TableExcludeAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = __super::CreateObject(rCreateStructure);

	m_excludeHigh.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	m_excludeLow.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );

	return l_bOk;
}

bool TableExcludeAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_excludeColumnObjectInfo);

	SVObjectClass* pObject = m_excludeColumnObjectInfo.GetInputObjectInfo().getObject();
	if (!m_excludeColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<SvVol::DoubleSortValueObject*> (pObject)
		//check if column part of the right table object (The object must be from same tool as this analyzer.)
		|| nullptr == pObject->GetParent() || pObject->GetParent()->GetParent() != m_ownerObjectInfo.getObject())
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

		SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*>(m_ownerObjectInfo.getObject());
		SvVol::DoubleSortValueObject* pColumnValues = dynamic_cast<SvVol::DoubleSortValueObject*>(m_excludeColumnObjectInfo.GetInputObjectInfo().getObject());
		if (nullptr != pTool && nullptr != pColumnValues)
		{
			
			SvVol::ValueObjectSortContainer& rSortContainer = pTool->getSortContainer();
			size_t sizeValues = pColumnValues->getSortContainerSize();
			assert(sizeValues == rSortContainer.size());
			if (0 < sizeValues)
			{
				double Value;
				for (int i=static_cast<int>(sizeValues)-1; 0 <= i; --i)
				{
					pColumnValues->GetValue(Value, i);
					if (excludeHigh<Value || excludeLow>Value)
					{
						rSortContainer.erase(rSortContainer.begin()+i);
					}
				}
				pTool->UpdateNumberOfRows();
			
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::TableAnalyzerExcludeType;

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

	std::string ObjectName = SvUl::LoadStdString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE );
	ObjectName +=  SvDef::cLinkName;
	RegisterEmbeddedObject( &m_excludeHigh.getLinkedName(), TableAnaylzerExcludeHigh_LinkGuid, ObjectName.c_str(), false, SvOi::SVResetItemNone );
	m_excludeHigh.getLinkedName().SetDefaultValue( _T(""), false );

	//set excludeLow-Value
	RegisterEmbeddedObject( &m_excludeLow, TableAnaylzerExcludeLowGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE, true, SvOi::SVResetItemTool );
	//vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultLowValue;
	m_excludeLow.SetDefaultValue( vtTemp, true );

	ObjectName = SvUl::LoadStdString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE );
	ObjectName +=  SvDef::cLinkName;
	RegisterEmbeddedObject( &m_excludeLow.getLinkedName(), TableAnaylzerExcludeLow_LinkGuid, ObjectName.c_str(), false, SvOi::SVResetItemNone );
	m_excludeLow.getLinkedName().SetDefaultValue( _T(""), false );
}

void TableExcludeAnalyzer::BuildInputObjectList()
{
	// Source Table.
	m_excludeColumnObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::DoubleSortValueObjectType);
	m_excludeColumnObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_excludeColumnObjectInfo, SvDef::cInputTag_ExcludeColumn );

	addDefaultInputObjects();
}
#pragma endregion Private Methods

} //namespace SvAo
