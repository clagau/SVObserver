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
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#include "SVGlobal.h"
#include "SVResultLong.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary\SVString.h"
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
		if( !CreateChildObject(m_pResultTable) )
		{
			bOk = false;
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrStd Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
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
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			SvStl::MessageMgrStd Msg( SvStl::LogOnly );
			Msg.setMessage( message.getMessage() );
		}
	}
	else
	{
		bOk = false;
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_CreateFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		SvStl::MessageMgrStd Msg( SvStl::LogOnly );
		Msg.setMessage( message.getMessage() );
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

bool TableAnalyzerTool::DoesObjectHaveExtents() const
{
	return false;
}

bool TableAnalyzerTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	Result = ValidateLocal(pErrorMessages) && Result;

	SVObjectClass* pObject = m_sourceTableObjectInfo.GetInputObjectInfo().m_pObject;
	if( !m_sourceTableObjectInfo.IsConnected() || nullptr == dynamic_cast<TableObject*> (pObject) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}
	else if( pObject->GetAncestorInterface(SVInspectionObjectType) != GetAncestorInterface(SVInspectionObjectType) )
	{
		pObject->DisconnectObjectInput( &m_sourceTableObjectInfo );
		m_sourceTableObjectInfo.SetInputObject(nullptr);
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidTableConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	if (Result)
	{
		m_pResultTable->setSourecTable( dynamic_cast<TableObject*> (m_sourceTableObjectInfo.GetInputObjectInfo().m_pObject) );
		Result = m_pResultTable->ResetObject(pErrorMessages);
	}
	else
	{
		m_pResultTable->setSourecTable(nullptr);
	}

	return Result;
}

void TableAnalyzerTool::InsertAt(int index, ITaskObject& rObject, int count)
{
	SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(&rObject);
	SVTaskObjectListClass::InsertAt(index+1, pObject, count);
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
bool TableAnalyzerTool::ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const
{
	bool Result = true;
	if (nullptr == m_pResultTable)
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TableObject_Nullptr, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	return Result;
}

void TableAnalyzerTool::LocalInitialize ()
{
	BuildInputObjectList ();

	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVTableAnalyzerToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	SVClassInfoStruct analyzerClassInfo;

	// Add the Sort Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = TableAnalyzerSortType;
	analyzerClassInfo.m_ClassId = TableAnaylzerSortGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_TABLEANALYZER_SORT );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the Exclude Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = TableAnalyzerExcludeType;
	analyzerClassInfo.m_ClassId = TableAnaylzerExcludeGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_TABLEANALYZER_EXCLUDE );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the Limit Analyzer
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = TableAnalyzerType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType = TableAnalyzerLimitType;
	analyzerClassInfo.m_ClassId = TableAnaylzerLimitGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_TABLEANALYZER_LIMIT );
	m_availableChildren.Add( analyzerClassInfo );

	// Setup the result
	SVClassInfoStruct resultClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;
	// Declare Input Interface
	interfaceInfo.EmbeddedID = TableObject_NumberOfRowsGuid;
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType = SVResultLongObjectType;
	resultClassInfo.m_ClassId = SVLongResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_TABLEOBJECT_NUMBEROFROWS );
	SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	// Construct the result
	SVLongResultClass* pResult = ( SVLongResultClass * )resultClassInfo.Construct();
	Add( pResult );
}

void TableAnalyzerTool::BuildInputObjectList ()
{
	// Source Table.
	m_sourceTableObjectInfo.SetInputObjectType( TableObjectType );
	m_sourceTableObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_sourceTableObjectInfo, SvOi::cInputTag_SourceTable );

	addDefaultInputObjects();
}
#pragma endregion Private Methods
