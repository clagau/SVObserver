//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableTool.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "Operators/SVEquation.h"
#include "Operators/TableColumnEquation.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableTool, TableToolGuid);

#pragma region Constructor
TableTool::TableTool( SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass( pOwner, stringResourceID )
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
bool TableTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	m_MaxRow.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	//set equation for table tool
	for( size_t j = 0; j < m_friendList.size(); j++ )
	{
		SvOp::SVEquationClass* pEquation = dynamic_cast<SvOp::SVEquationClass*> (m_friendList[j].getObject());
		if (nullptr != pEquation)
		{
			if (SvDef::TableClearEquationName == std::string(pEquation->GetName()))
			{
				m_pClearEquation = pEquation;
				break;
			}
		}
	}

	m_pTable = dynamic_cast<SvOp::TableFillByEquationObject*>(SvOi::FindObject(GetUniqueObjectID(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::TableFillObjectType)));
	if (nullptr == m_pTable)
	{
		m_pTable = new SvOp::TableFillByEquationObject(this);
		Add( m_pTable );
		if( !CreateChildObject(m_pTable) )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log );
			Msg.setMessage( message.getMessage() );
		}
	}

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

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
			SvOp::TableColumnEquation* equation = dynamic_cast<SvOp::TableColumnEquation*> (m_friendList[j].getObject());
			if (nullptr != equation)
			{
				if (c_maxTableColumn > m_ColumnEquationList.size())
				{
					m_ColumnEquationList.push_back(equation);
				}
				else
				{
					Result = false;
					DestroyChild(*equation, 0);
					j--;
					if (nullptr != pErrorMessages)
					{
						SvDef::StringVector msgList;
						msgList.push_back(SvUl::Format(_T("%d"), c_maxTableColumn));
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
			SvStl::MessageMgrStd e(SvStl::MsgType::Log );
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
			m_pTable->clearTable();
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
	if (SvDef::cTableMaxRowMin > maxRow || SvDef::cTableMaxRowMax < maxRow)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvDef::StringVector messageList;
			messageList.push_back(SvUl::Format(_T("%d"), SvDef::cTableMaxRowMin));
			messageList.push_back(SvUl::Format(_T("%d"), SvDef::cTableMaxRowMax));
			messageList.push_back(SvUl::Format(_T("%d"), maxRow));
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_MaxRowWrongValue, messageList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}	
	return Result;
}

void TableTool::LocalInitialize ()
{
	// Override base class exposure of the auxiliary extent variables
	// These values will not be exposed for this Tool.
	m_hasToolExtents = false;

	BuildInputObjectList ();
	BuildEmbeddedObjectList ();

	//Clear Condition Equation
	SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (SvOi::ConstructObject(SVEquationClassGuid));
	if( nullptr != pObject )
	{
		pObject->SetObjectOwner( this );
		pObject->SetName(SvDef::TableClearEquationName);
		AddFriend( pObject->GetUniqueObjectID() );
	}

	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVTableToolObjectType;

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
	m_MaxRow.SetDefaultValue( c_defaultMaxRow );
}
#pragma endregion Private Methods

} //namespace SvTo
