//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableSortAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for sorting the table.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableSortAnalyzer.h"
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
	
SV_IMPLEMENT_CLASS(TableSortAnalyzer, TableAnaylzerSortGuid);

#pragma endregion Declarations

#pragma region Constructor
TableSortAnalyzer::TableSortAnalyzer( LPCTSTR ObjectName )
	: SVTaskObjectClass(ObjectName)
	, m_tmpArraySize(0)
{
	Initialize();
}

TableSortAnalyzer::TableSortAnalyzer( SVObjectClass* pOwner, int StringResourceID )
	: SVTaskObjectClass(pOwner, StringResourceID)
	, m_tmpArraySize(0)
{
	Initialize();
}

TableSortAnalyzer::~TableSortAnalyzer()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL TableSortAnalyzer::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL l_bOk = SVTaskObjectClass::CreateObject( pCreateStructure );

	m_isASC.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;

	return l_bOk;
}

HRESULT TableSortAnalyzer::ResetObject()
{
	HRESULT status = SVTaskObjectClass::ResetObject();

	if (OnValidateParameter(AllParameters))
	{
		//allocate m_tmpValues
		DoubleSortValueObject* pColumnValues = dynamic_cast<DoubleSortValueObject*>(m_sortColumnObjectInfo.GetInputObjectInfo().PObject);
		if (nullptr != pColumnValues)
		{
			ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer();
			size_t sizeTmp = sortContainer.capacity();
			CheckAndResizeTmpArray(sizeTmp);
		}
	}
	else
	{
		status = S_FALSE;
	}

	return status;
}


BOOL TableSortAnalyzer::Validate()
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

BOOL TableSortAnalyzer::OnValidate()
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
BOOL TableSortAnalyzer::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = SVTaskObjectClass::onRun( rRunStatus );
	
	if (returnValue)
	{
		bool isASC = true;
		m_isASC.GetValue(isASC);
		TableAnalyzerTool* pTool = dynamic_cast<TableAnalyzerTool*>(m_ownerObjectInfo.PObject);
		DoubleSortValueObject* pColumnValues = dynamic_cast<DoubleSortValueObject*>(m_sortColumnObjectInfo.GetInputObjectInfo().PObject);
		if (nullptr != pTool && nullptr != pColumnValues)
		{
			ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer(rRunStatus.m_lResultDataIndex);
			size_t sizeValues = sortContainer.size();
			if (0 < sizeValues)
			{
				CheckAndResizeTmpArray(sizeValues);

				double* pValues = m_tmpValues.get();
				for (int i=0; i< sizeValues; ++i)
				{
					pColumnValues->GetValue(rRunStatus.m_lResultDataIndex, i, pValues[i]);
				}

				bool isSwitched = false;
				do
				{ 
					isSwitched = false;
					for(int n=0;n<sizeValues-1;n++)
					{
						if((pValues[n]>pValues[n+1] && isASC) || (pValues[n]<pValues[n+1] && !isASC))
						{
							std::swap(pValues[n], pValues[n+1]);
							std::swap(sortContainer[n], sortContainer[n+1]);
							isSwitched=true;
						}
					}
				}while(isSwitched);
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

bool TableSortAnalyzer::ValidateOfflineParameters ()
{
	bool Result = SVTaskObjectClass::ValidateOfflineParameters();
	if (Result)
	{
		SvOi::IObjectClass* pObject = m_sortColumnObjectInfo.GetInputObjectInfo().PObject;
		if (!m_sortColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<DoubleSortValueObject*>(pObject))
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoValidColumnConnected, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			Result = false;
		}
	}

	return Result;
}

DWORD_PTR TableSortAnalyzer::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
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
void TableSortAnalyzer::Initialize()
{
	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = TableAnalyzerSortType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableSortAnalyzer::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject( &m_isASC, TableAnaylzerSortIsASCGuid, IDS_OBJECTNAME_TABLEANALYZER_ISASC, true, SVResetItemTool );
	m_isASC.SetDefaultValue( true, TRUE );
}

void TableSortAnalyzer::BuildInputObjectList()
{
	// Source Table.
	m_sortColumnObjectInfo.SetInputObjectType( DoubleSortValueObjectType );
	m_sortColumnObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_sortColumnObjectInfo, SvO::cInputTag_SortColumn );

	addDefaultInputObjects();
}

void TableSortAnalyzer::CheckAndResizeTmpArray( size_t sizeTmp )
{
	if (sizeTmp > m_tmpArraySize || (0 < sizeTmp && m_tmpValues.empty()))
	{
		m_tmpValues.clear();
		m_tmpValues = SVSharedPtr<double>(new double[sizeTmp], [](double* p){delete[] p;});
		m_tmpArraySize = sizeTmp;
	}
}

#pragma endregion Private Methods
