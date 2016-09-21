//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableAnaylzerTool.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class perform the Table Analyzer Tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableAnalyzerTool.h"
#include "SVObjectLibrary\SVClsids.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "SVGlobal.h"
#include "SVResultLong.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( TableAnalyzerTool, TableAnalyzerToolGuid );
#pragma endregion Declarations

#pragma region Constructor
TableAnalyzerTool::TableAnalyzerTool( BOOL bCreateDefaultTaskList, SVObjectClass* pOwner, int stringResourceID )
	:SVToolClass( bCreateDefaultTaskList, pOwner, stringResourceID )
{
	LocalInitialize ();
}

TableAnalyzerTool::~TableAnalyzerTool() 
{

}
#pragma endregion Constructor

#pragma region Public Methods
BOOL TableAnalyzerTool::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( pCreateStructure ); //  TRUE/FALSE

	bOk &= (nullptr != GetTool());
	bOk &= (nullptr != GetInspection());

	m_pResultTable = dynamic_cast<TableCopyObject*>(SvOi::FindObject(GetUniqueObjectID(), SVObjectTypeInfoStruct(TableObjectType, TableCopyObjectType)));
	if (nullptr == m_pResultTable)
	{
		m_pResultTable = new TableCopyObject(this);
		//This task must be the first in the list.
		Add( m_pResultTable, true );
		if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pResultTable), 0 ) != SVMR_SUCCESS )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
			addTaskMessage( message );
		}
	}

	// add input of result
	SVLongResultClass* pResult = dynamic_cast<SVLongResultClass*>(SvOi::FindObject(GetUniqueObjectID(), SVObjectTypeInfoStruct(SVResultObjectType, SVResultLongObjectType)));
	if (nullptr != pResult)
	{
		SVInputInfoListClass inputInterface;
		pResult->GetPrivateInputList( inputInterface );
		//find right input
		SVInputInfoListClass::const_iterator iter = std::find_if( inputInterface.begin(), inputInterface.end(), [](const SVInObjectInfoStruct* rInfo)->bool
		{
			return rInfo->GetInputName() == SvO::cInputTag_LongResultValue;
		} );
		if (inputInterface.end() != iter)
		{
			//connect to input
			ConnectToObject( *iter, m_pResultTable->getNumberOfRowObject() );
		}
		else
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
			addTaskMessage( message );
		}
	}
	else
	{
		bOk = false;
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogOnly );
		Msg.setMessage( message.getMessage() );
		addTaskMessage( message );
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

bool TableAnalyzerTool::DoesObjectHaveExtents() const
{
	return false;
}

HRESULT TableAnalyzerTool::ResetObject()
{
	HRESULT status = SVToolClass::ResetObject();

	if (OnValidateParameter(AllParameters))
	{
		m_pResultTable->setSourecTable(dynamic_cast<TableObject*>(m_sourceTableObjectInfo.GetInputObjectInfo().PObject));
		m_pResultTable->ResetObject();
	}
	else
	{
		status = S_FALSE;
	}

	return status;
}

void TableAnalyzerTool::InsertAt(int index, ITaskObject& rObject, int count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	SVTaskObjectListClass::InsertAt(index+1, pObject, count);
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableAnalyzerTool::ValidateOfflineParameters ()
{
	bool Result = SVToolClass::ValidateOfflineParameters();
	if (Result)
	{
		if (nullptr == m_pResultTable)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			addTaskMessage( message );
			Result = false;
		}

		SvOi::IObjectClass* pObject = m_sourceTableObjectInfo.GetInputObjectInfo().PObject;
		if (!m_sourceTableObjectInfo.IsConnected() || nullptr == dynamic_cast<TableObject*>(pObject))
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			addTaskMessage( message );
			Result = false;
		}
	}

	return Result;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableAnalyzerTool::LocalInitialize ()
{
	BuildInputObjectList ();

	// Set up your type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVTableAnalyzerToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	SVClassInfoStruct analyzerClassInfo;

	// Add the Sort Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.ObjectTypeInfo.SubType = TableAnalyzerSortType;
	analyzerClassInfo.ClassId = TableAnaylzerSortGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_TABLEANALYZER_SORT );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the Exclude Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.ObjectTypeInfo.SubType = TableAnalyzerExcludeType;
	analyzerClassInfo.ClassId = TableAnaylzerExcludeGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_TABLEANALYZER_EXCLUDE );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the Limit Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.ObjectTypeInfo.SubType = TableAnalyzerLimitType;
	analyzerClassInfo.ClassId = TableAnaylzerLimitGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_TABLEANALYZER_LIMIT );
	m_availableChildren.Add( analyzerClassInfo );

	// Setup the result
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;
	// Declare Input Interface
	interfaceInfo.EmbeddedID = TableObject_NumberOfRowsGuid;
	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType = SVResultLongObjectType;
	resultClassInfo.ClassId = SVLongResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS );
	strTitle.LoadString( IDS_OBJECTNAME_RESULT );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
	// Construct the result
	SVLongResultClass* pResult = ( SVLongResultClass * )resultClassInfo.Construct();
	Add( pResult );
}

void TableAnalyzerTool::BuildInputObjectList ()
{
	// Source Table.
	m_sourceTableObjectInfo.SetInputObjectType( TableObjectType );
	m_sourceTableObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_sourceTableObjectInfo, SvO::cInputTag_SourceTable );

	addDefaultInputObjects();
}
#pragma endregion Private Methods
