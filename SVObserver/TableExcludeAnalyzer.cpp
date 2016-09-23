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
	BOOL l_bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	m_excludeHigh.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
	m_excludeLow.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;

	return l_bOk;
}

HRESULT TableExcludeAnalyzer::ResetObject()
{
	HRESULT status = SVTaskObjectClass::ResetObject();

	if (!OnValidateParameter(AllParameters))
	{
		status = S_FALSE;
	}

	return status;
}


BOOL TableExcludeAnalyzer::Validate()
{
	BOOL Result = SVTaskObjectClass::Validate();

	//if Tool-validation is fail but no message in the Message-List, add one
	if (FALSE == Result && 0 == getFirstTaskMessage().getMessage().m_MessageCode)
	{
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_5072_INCONSISTENTDATA, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
	}

	if (Result)
	{
		Result = OnValidateParameter(AllParameters);
	}

	return Result;
}

BOOL TableExcludeAnalyzer::OnValidate()
{
	BOOL Result = (0 == getFirstTaskMessage().getMessage().m_MessageCode);

	if ( Result )
	{
		Result = SVTaskObjectClass::OnValidate();
		if( !Result && 0 != getFirstTaskMessage().getMessage().m_MessageCode)
		{
			SvStl::MessageContainer message;
			SVStringArray msgList;
			msgList.push_back(GetName());
			message.setMessage( SVMSG_SVO_5074_BASECLASSONVALIDATEFAILED, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
		}
	}

	if ( Result )
	{
		Result = OnValidateParameter(InspectionSettable);
	}

	if (! Result)
	{
		SetInvalid();
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL TableExcludeAnalyzer::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = SVTaskObjectClass::onRun( rRunStatus );
	
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
			returnValue = FALSE;
		}
	}

	return returnValue;
}

bool TableExcludeAnalyzer::ValidateOfflineParameters ()
{
	bool Result = SVTaskObjectClass::ValidateOfflineParameters();
	if (Result)
	{
		SvOi::IObjectClass* pObject = m_excludeColumnObjectInfo.GetInputObjectInfo().PObject;
		if (!m_excludeColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<DoubleSortValueObject*>(pObject))
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoValidColumnConnected, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			Result = false;
		}
		Result &= m_excludeHigh.IsValid() ? true : false;
		Result &= m_excludeLow.IsValid() ? true : false;

		if (!m_excludeLow.isIndirectValue() && !m_excludeHigh.isIndirectValue())
		{
			//check if high greater than low
			double excludeHigh;
			double excludeLow;
			m_excludeHigh.GetValue(excludeHigh);
			m_excludeLow.GetValue(excludeLow);
			if (excludeHigh<excludeLow)
			{
				SvStl::MessageContainer message;
				message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ExcludeHighMustBeHigher, SvStl::SourceFileParams(StdMessageParams) );
				addTaskMessage( message );
				Result = false;
			}
		}
	}

	return Result;
}

DWORD_PTR TableExcludeAnalyzer::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
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

	CString ObjectName;
	ObjectName.LoadString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_HIGHVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeHigh.getLinkedName(), TableAnaylzerExcludeHigh_LinkGuid, ObjectName, false, SVResetItemNone );
	m_excludeHigh.getLinkedName().SetDefaultValue( _T(""), false );

	//set excludeLow-Value
	RegisterEmbeddedObject( &m_excludeLow, TableAnaylzerExcludeLowGuid, IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE, true, SVResetItemTool );
	//vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = cDefaultLowValue;
	m_excludeLow.SetDefaultValue( vtTemp, true );

	ObjectName.LoadString( IDS_OBJECTNAME_TABLEANALYZEREXCLUDE_LOWVALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_excludeLow.getLinkedName(), TableAnaylzerExcludeLow_LinkGuid, ObjectName, false, SVResetItemNone );
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
