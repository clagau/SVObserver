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
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVEquation.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "TableColumnEquation.h"
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
	BOOL bOk = SVToolClass::CreateObject( pCreateStructure ); //  TRUE/FALSE

	m_MaxRow.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_VIEWABLE | SV_PRINTABLE | SV_SELECTABLE_FOR_EQUATION;

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	//set equation for table tool
	for( size_t j = 0; j < m_friendList.size(); j++ )
	{
		SVEquationClass* pEquation = dynamic_cast<SVEquationClass *>(m_friendList[j].PObject);
		if (nullptr != pEquation)
		{
			if (SvOi::TableClearEquationName == SVString(pEquation->GetName()))
			{
				m_pClearEquation = pEquation;
				break;
			}
		}
	}

	m_pTable = dynamic_cast<TableFillObject*>(SvOi::FindObject(GetUniqueObjectID(), SVObjectTypeInfoStruct(TableObjectType,TableFillObjectType)));
	if (nullptr == m_pTable)
	{
		m_pTable = new TableFillObject(this);
		Add( m_pTable );
		if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pTable), 0 ) != SVMR_SUCCESS )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams) );
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
			addTaskMessage( message );
		}
	}

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Table Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Table Tool.
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	m_isCreated = bOk;

	return bOk;
}

bool TableTool::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT TableTool::ResetObject()
{
	HRESULT status = SVToolClass::ResetObject();

	if (OnValidateParameter(AllParameters))
	{
		m_pTable->ResetObject();
		m_ColumnEquationList.clear();
		for( size_t j = 0; j < m_friendList.size(); j++ )
		{
			TableColumnEquation* equation = dynamic_cast<TableColumnEquation*>(m_friendList[j].PObject);
			if (nullptr != equation)
			{
				m_ColumnEquationList.push_back(equation);
			}
		}
		long maxArray = 0;
		m_MaxRow.GetValue(maxArray);
		try
		{
			m_pTable->setColumnValueObjects(m_ColumnEquationList, maxArray);
		}
		catch( const SvStl::MessageContainer& rSvE )
		{
			SvStl::MessageMgrDisplayAndNotify e( SvStl::LogOnly );
			e.setMessage( rSvE.getMessage() );
			addTaskMessage(rSvE);
			status = S_FALSE;
		}
	}
	else
	{
		status = S_FALSE;
	}

	return status;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL TableTool::onRun( SVRunStatusClass& rRunStatus )
{
	BOOL returnValue = S_FALSE;

	returnValue = SVToolClass::onRun( rRunStatus );
	if (returnValue)
	{
		//clear table if required
		if( m_pClearEquation->HasCondition() && m_pClearEquation->IsEnabled() && 0 != m_pClearEquation->GetYACCResult() )
		{
			m_pTable->ResetObject();
		}
	}

	return returnValue;
}

bool TableTool::ValidateOfflineParameters ()
{
	bool Result = SVToolClass::ValidateOfflineParameters();
	if (Result)
	{
		if (nullptr == m_pTable)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			Result = false;
		}

		long maxRow = 0;
		m_MaxRow.GetValue(maxRow);
		if (maxRow <= 0)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_MaxRowWrongValue, SvStl::SourceFileParams(StdMessageParams) );
			addTaskMessage( message );
			Result = false;
		}
	}

	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVTableToolObjectType;
}

void TableTool::BuildInputObjectList ()
{
	addDefaultInputObjects();
}


void TableTool::BuildEmbeddedObjectList ()
{
	RegisterEmbeddedObject( &m_MaxRow, TableTool_MaxRowGuid, IDS_OBJECTNAME_TABLE_MAXROW, true, SVResetItemTool );
	m_MaxRow.SetDefaultValue( c_defaultMaxRow, TRUE );
}
#pragma endregion Private Methods
