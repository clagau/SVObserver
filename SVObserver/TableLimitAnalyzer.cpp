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
	BOOL l_bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	m_LimitValue.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
	
	return l_bOk;
}

HRESULT TableLimitAnalyzer::ResetObject()
{
	HRESULT status = SVTaskObjectClass::ResetObject();

	if (!OnValidateParameter(AllParameters))
	{
		status = S_FALSE;
	}

	return status;
}


BOOL TableLimitAnalyzer::Validate()
{
	BOOL Result = SVTaskObjectClass::Validate();

	//if Task-validation is fail but no message in the Message-List, add one
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

BOOL TableLimitAnalyzer::OnValidate()
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
BOOL TableLimitAnalyzer::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = SVTaskObjectClass::onRun( rRunStatus );
	
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

bool TableLimitAnalyzer::ValidateOfflineParameters ()
{
	bool Result = SVTaskObjectClass::ValidateOfflineParameters();
	if (Result)
	{
		//@TODO[MZA][7.40][17.10.2016] This line must be uncomment yet, because the analyzer tool set this parameter invalid when another analyzer is invalid
		//This should be fix when Onvalidate-behavior is fixed.
		//Result &= m_LimitValue.IsValid() ? true : false;
	}

	return Result;
}

DWORD_PTR TableLimitAnalyzer::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
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

	CString ObjectName;
	ObjectName.LoadString( IDS_OBJECTNAME_TABLEANALYZERLIMIT_VALUE );
	ObjectName +=  SvO::cLinkName;
	RegisterEmbeddedObject( &m_LimitValue.getLinkedName(), TableAnaylzerLimitValue_LinkGuid, ObjectName, false, SVResetItemNone );
	m_LimitValue.getLinkedName().SetDefaultValue( _T(""), false );
}

void TableLimitAnalyzer::BuildInputObjectList()
{
	addDefaultInputObjects();
}
#pragma endregion Private Methods
