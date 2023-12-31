//*****************************************************************************
/// \copyright (c) 2016,2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableTool.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "Operators/SVEquation.h"
#include "Operators/TableColumnEquation.h"
#include "Operators/TableFillByEquationObject.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/TextDefinesSvDef.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableTool, SvPb::TableToolClassId);

#pragma region Constructor
TableTool::TableTool( SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass(ToolExtType::None,pOwner, stringResourceID )
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
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		SvOp::SVEquation* pEquation = dynamic_cast<SvOp::SVEquation*> (m_friendList[j]);
		if (nullptr != pEquation)
		{
			if (SvDef::TableClearEquationName == std::string(pEquation->GetName()))
			{
				m_pClearEquation = pEquation;
				break;
			}
		}
	}

	m_pTable = dynamic_cast<SvOp::TableFillByEquationObject*>(SvOi::FindObject(getObjectId(), SvDef::SVObjectTypeInfoStruct(SvPb::TableObjectType, SvPb::TableFillObjectType)));
	if (nullptr == m_pTable)
	{
		m_pTable = new SvOp::TableFillByEquationObject(this);
		Add( m_pTable );
		if( !CreateChildObject(m_pTable) )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			SvStl::MessageManager Msg(SvStl::MsgType::Log );
			Msg.setMessage( message.getMessage() );
		}
	}

	// These values will not be exposed for the this Tool.
	m_drawToolFlag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);

	m_isCreated = bOk;

	return bOk;
}

void TableTool::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
{
	__super::getOutputList(inserter);

	if (nullptr != m_pTable)
	{
		inserter = m_pTable;
	}
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
			SvOp::TableColumnEquation* equation = dynamic_cast<SvOp::TableColumnEquation*> (m_friendList[j]);
			if (nullptr != equation)
			{
				if (SvDef::c_maxTableColumn > m_ColumnEquationList.size())
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
						msgList.push_back(std::format(_T("{}"), SvDef::c_maxTableColumn));
						SvStl::MessageContainer message;
						message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableColumn_TooManyEquation, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( rSvE.getMessage() );
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(rSvE);
			}
		}
	}

	return Result;
}

std::vector<std::string> TableTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 4> cToolAdjustNameList
	{
		_T("Parameters"),
		_T("Table"),
		_T("Conditional"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

#pragma endregion Public Methods

#pragma region Protected Methods
bool TableTool::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
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
			messageList.push_back(std::format(_T("{}"), SvDef::cTableMaxRowMin));
			messageList.push_back(std::format(_T("{}"), SvDef::cTableMaxRowMax));
			messageList.push_back(std::format(_T("{}"), maxRow));
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_MaxRowWrongValue, messageList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back( message );
		}
	}	
	return Result;
}

void TableTool::LocalInitialize ()
{
	BuildEmbeddedObjectList ();

	//Clear Condition Equation
	SVTaskObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().ConstructObject(SvPb::EquationClassId, pObject);
	if( nullptr != pObject )
	{
		pObject->SetObjectOwner( this );
		pObject->SetName(SvDef::TableClearEquationName);
		AddFriend( pObject );
	}

	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVTableToolObjectType;

	
}

void TableTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_MaxRow, SvPb::TableTool_MaxRowEId, IDS_OBJECTNAME_TABLE_MAXROW, true, SvOi::SVResetItemToolAndDependent, true);
	m_MaxRow.SetDefaultValue( c_defaultMaxRow );
}
#pragma endregion Private Methods

} //namespace SvTo
