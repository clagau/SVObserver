//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableTool.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOCore/SVEquation.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "SVOCore/TableColumnEquation.h"
#include "ObjectInterfaces\GlobalConst.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( TableTool, TableToolGuid );
#pragma endregion Declarations

#pragma region Constructor
TableTool::TableTool( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass( bCreateDefaultTaskList, pOwner, stringResourceID )
	,m_pClearEquation(nullptr)
	,m_pTable(nullptr)
{
	LocalInitialize ();
}

TableTool::~TableTool() 
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL TableTool::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( pCreateStructure );

	m_MaxRow.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	//set equation for table tool
	for( size_t j = 0; j < m_friendList.size(); j++ )
	{
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass *>(m_friendList[j].m_pObject);
		if (nullptr != pEquation)
		{
			if (SvOi::TableClearEquationName == SVString(pEquation->GetName()))
			{
				m_pClearEquation = pEquation;
				break;
			}
		}
	}

	m_pTable = dynamic_cast<TableFillByEquationObject*>(SvOi::FindObject(GetUniqueObjectID(), SVObjectTypeInfoStruct(TableObjectType,TableFillObjectType)));
	if (nullptr == m_pTable)
	{
		m_pTable = new TableFillByEquationObject(this);
		Add( m_pTable );
		if( !CreateChildObject(m_pTable) )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrStd Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
		}
	}

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Table Tool.
	drawToolFlag.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Table Tool.
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	m_isCreated = bOk;

	return bOk;
}

HRESULT TableTool::GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const
{
	HRESULT Status( SVTaskObjectListClass::GetOutputList( p_rOutputInfoList ) );

	if (nullptr != m_pTable)
	{
		p_rOutputInfoList.Add( &(m_pTable->GetObjectOutputInfo()) );
	}
	else
	{
		Status = E_FAIL;
	}

	return Status;
}

bool TableTool::DoesObjectHaveExtents() const
{
	return false;
}

bool TableTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	Result = ValidateLocal(pErrorMessages) && Result;

	if (Result)
	{
		Result = m_pTable->ResetObject(pErrorMessages);
		m_ColumnEquationList.clear();
		for( size_t j = 0; j < m_friendList.size(); j++ )
		{
			TableColumnEquation* equation = dynamic_cast<TableColumnEquation*>(m_friendList[j].m_pObject);
			if (nullptr != equation)
			{
				if (c_maxTableColumn > m_ColumnEquationList.size())
				{
					m_ColumnEquationList.push_back(equation);
				}
				else
				{
					Result = false;
					DestroyFriendObject(*equation, 0);
					j--;
					if (nullptr != pErrorMessages)
					{
						SVStringVector msgList;
						msgList.push_back(SvUl_SF::Format(_T("%d"), c_maxTableColumn));
						SvStl::MessageContainer message;
						message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableColumn_TooManyEquation, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back( message );
					}
				}
			}
		}
		try
		{
			long maxRow = 0;
			m_MaxRow.GetValue(maxRow);
			m_pTable->setColumnValueObjects(m_ColumnEquationList, maxRow);
		}
		catch( const SvStl::MessageContainer& rSvE )
		{
			Result = false;
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( rSvE.getMessage() );
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(rSvE);
			}
		}
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	if (returnValue)
	{
		//clear table if required
		if( m_pClearEquation->HasCondition() && m_pClearEquation->IsEnabled() && 0 != m_pClearEquation->GetYACCResult() )
		{
			returnValue = m_pTable->ResetObject(pErrorMessages);
		}
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
bool TableTool::ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const
{
	bool Result = true;
	if (nullptr == m_pTable)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	long maxRow = 0;
	m_MaxRow.GetValue(maxRow);
	if (SvOi::cTableMaxRowMin > maxRow || SvOi::cTableMaxRowMax < maxRow)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SVStringVector messageList;
			messageList.push_back(SvUl_SF::Format(_T("%d"), SvOi::cTableMaxRowMin));
			messageList.push_back(SvUl_SF::Format(_T("%d"), SvOi::cTableMaxRowMax));
			messageList.push_back(SvUl_SF::Format(_T("%d"), maxRow));
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_MaxRowWrongValue, messageList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}	
	return Result;
}

void TableTool::LocalInitialize ()
{
	BuildInputObjectList ();
	BuildEmbeddedObjectList ();

	//Clear Condition Equation
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().ConstructObject( SVEquationClassGuid, pObject );
	if( nullptr != pObject )
	{
		pObject->SetObjectOwner( this );
		pObject->SetName(SvOi::TableClearEquationName);
		AddFriend( pObject->GetUniqueObjectID() );
	}

	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVTableToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();
}

void TableTool::BuildInputObjectList ()
{
	addDefaultInputObjects();
}


void TableTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_MaxRow, TableTool_MaxRowGuid, IDS_OBJECTNAME_TABLE_MAXROW, true, SvOi::SVResetItemTool );
	m_MaxRow.SetDefaultValue( c_defaultMaxRow, TRUE );
}
#pragma endregion Private Methods
