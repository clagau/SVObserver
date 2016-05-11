//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputTool
//* .File Name       : $Workfile:   SVRemoteInputTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   13 Nov 2014 10:10:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteInputTool.h"

#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVInputStreamManager.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVRemoteInputTool, SVRemoteInputToolGuid )

SVRemoteInputTool::SVRemoteInputTool( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
, m_ElementIdentifier( 0 )
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVRemoteInputToolObjectType;

	// Register an empty input object
	m_InputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputObjectInfo, _T( "RemoteInputValue" ) );

	// Remove Embedded Extents
	removeEmbeddedExtents();

	RegisterEmbeddedObject( &m_MatchedValueId, SVRemoteInputToolMatchedValueIdGuid, IDS_OBJECTNAME_MATCHEDVALUEID, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_MatchedValue, SVRemoteInputToolMatchedValueGuid, IDS_OBJECTNAME_MATCHEDVALUE, false, SVResetItemNone );

	m_MatchedValueId.SetDefaultValue( _variant_t( 0 ), true );
	m_MatchedValue.SetDefaultValue( _variant_t( _T( "" ) ), true );

	// Set default inputs and outputs
	addDefaultInputObjects();

	m_Commands.Create();
}

SVRemoteInputTool::~SVRemoteInputTool()
{
	m_Commands.Destroy();
}

BOOL SVRemoteInputTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_Status = SVToolClass::CreateObject( PCreateStructure );

	l_Status &= ( S_OK == SVInputStreamManager::Instance().InsertInputStream( static_cast< const char* >( GetCompleteObjectName() ), GetUniqueObjectID() ) );

	m_isCreated = l_Status;

	m_MatchedValueId.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_MatchedValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return l_Status;
}

BOOL SVRemoteInputTool::CloseObject()
{
	BOOL l_Status = ( S_OK == SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() ) );
		
	l_Status &= SVToolClass::CloseObject();

	return l_Status;
}

BOOL SVRemoteInputTool::OnValidate()
{
	BOOL l_Status = SVToolClass::OnValidate();

	l_Status = l_Status && ( m_InputObjectInfo.IsConnected() );
	l_Status = l_Status && ( nullptr != m_InputObjectInfo.GetInputObjectInfo().PObject );

	if( !l_Status )
	{
		SetInvalid();
	}

	return l_Status;
}

HRESULT SVRemoteInputTool::ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr )
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	SVString l_Command;

	if( l_Reader.parse( p_rDataPtr->GetJsonCommand(), l_JsonValues, false ) )
	{
		if( l_JsonValues.isObject() )
		{
			Json::Value l_JsonCommand = l_JsonValues[ _T( "Command" ) ];

			if( l_JsonCommand.isString() )
			{
				l_Command = l_JsonCommand.asString();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( S_OK == l_Status )
	{
		if( l_Command == _T( "AddDataItems" ) )
		{
			m_Commands.AddTail( p_rDataPtr );

			p_rDataPtr->NotifyRequestComplete();
		}
		else if( l_Command == _T( "ClearDataItems" ) )
		{
			m_Commands.clear();
			m_Commands.AddTail( p_rDataPtr );

			p_rDataPtr->NotifyRequestComplete();
		}
		else if( l_Command == _T( "QueryDataItems" ) )
		{
			SVInspectionProcess* pInspection = GetInspection();

			if( nullptr != pInspection )
			{
				SVCommandTemplatePtr l_CommandPtr = new SVCommandQueueElement( GetUniqueObjectID(), p_rDataPtr );

				l_Status = SVObjectManagerClass::Instance().SubmitCommand( *pInspection, l_CommandPtr );

				if( S_OK != l_Status )
				{
					p_rDataPtr->NotifyRequestComplete();
				}
			}
			else
			{
				p_rDataPtr->NotifyRequestComplete();

				l_Status = E_FAIL;
			}
		}
		else
		{
			p_rDataPtr->NotifyRequestComplete();

			l_Status = E_FAIL;
		}
	}
	else
	{
		p_rDataPtr->NotifyRequestComplete();
	}

	return l_Status;
}

HRESULT SVRemoteInputTool::ClearInputObject()
{
	HRESULT l_Status = S_OK;

	m_InputObjectInfo.SetInputObject( nullptr );

	return l_Status;
}

HRESULT SVRemoteInputTool::SetInputObject( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	m_InputObjectInfo.SetInputObject( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );

	if( !( m_InputObjectInfo.IsConnected() ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVObjectClass* SVRemoteInputTool::GetInputObject() const
{
	SVObjectClass* l_pObject = nullptr;

	if( m_InputObjectInfo.IsConnected() )
	{
		l_pObject = m_InputObjectInfo.GetInputObjectInfo().PObject;
	}

	return l_pObject;
}

BOOL SVRemoteInputTool::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_Status = SVToolClass::onRun( RRunStatus );

	CString l_MatchString;
	CString l_MatchedString;
	long l_MatchedStringId = 0;
	long l_Identifier = 0;

	l_Status = l_Status && ( S_OK == ProcessCommandQueue() );
	l_Status = l_Status && ( m_InputObjectInfo.IsConnected() );
	l_Status = l_Status && ( nullptr != m_InputObjectInfo.GetInputObjectInfo().PObject );

	if( l_Status )
	{

		SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass*> (m_InputObjectInfo.GetInputObjectInfo().PObject);
		if( nullptr != pValueObject )
		{
			if( l_Status )
			{
				l_Status = ( S_OK == pValueObject->GetValue( l_MatchString ) );
			}
		}

		BasicValueObject* pBasicValueObject = dynamic_cast<BasicValueObject*> (m_InputObjectInfo.GetInputObjectInfo().PObject);
		if( nullptr != pBasicValueObject )
		{
			if( l_Status )
			{
				SVString Value;
				l_Status = ( S_OK == pBasicValueObject->getValue( Value ) );
				l_MatchString = Value.c_str();
			}
		}

		l_Status = ( nullptr != pValueObject || nullptr != pBasicValueObject );
	}

	if( l_Status )
	{
		SVDataContainer::index_iterator< from >::type l_Iter = m_Elements.get< from >().find( SVDataElement( static_cast< const char* >( l_MatchString ) ) );

		if( l_Iter != m_Elements.get< from >().end() )
		{
			RRunStatus.SetPassed();

			l_Identifier = l_Iter->second;

			l_MatchedString = l_Iter->first.m_ElementData.c_str();
			l_MatchedStringId = l_Iter->first.m_ElementIdentifier;
		}
	}

	m_MatchedValueId.SetValue( RRunStatus.m_lResultDataIndex, l_MatchedStringId );
	m_MatchedValue.SetValue( RRunStatus.m_lResultDataIndex, l_MatchedString );

	if( l_Status && ( l_Identifier != 0 ) )
	{
		SVDataContainer::index_iterator< to >::type l_Iter = m_Elements.get< to >().begin();

		while( l_Iter != m_Elements.get< to >().end() )
		{
			bool l_Done = ( l_Iter->second == l_Identifier );

			SVDataContainer::index_iterator< to >::type l_Temp = l_Iter++;

			m_Elements.get< to >().erase( l_Temp );

			if( l_Done )
			{
				break;
			}
		}
	}
	else
	{
		RRunStatus.SetFailed();
	}

	return l_Status;
}

DWORD_PTR SVRemoteInputTool::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR l_Status = SVMR_NOT_PROCESSED;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
		case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (DwMessageValue); // Object with new name
			LPCTSTR orgName = ( LPCTSTR )DwMessageContext;

			SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() );

			if( S_OK == SVInputStreamManager::Instance().InsertInputStream( static_cast< const char* >( GetCompleteObjectName() ), GetUniqueObjectID() ) )
			{
				l_Status = SVMR_SUCCESS;
			}
			else
			{
				l_Status = SVMR_NO_SUCCESS;
			}

			return l_Status;

			break;
		}
		default:
		{
			break;
		}
	}

	l_Status = SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | l_Status;

	return l_Status;
}

HRESULT SVRemoteInputTool::ProcessCommandQueue()
{
	HRESULT l_Status = S_OK;

	while( !( m_Commands.IsEmpty() ) )
	{
		HRESULT l_TempStatus = S_OK;

		SVObjectCommandDataJsonPtr l_CommandElementPtr;

		if( m_Commands.RemoveHead( &l_CommandElementPtr ) && !( l_CommandElementPtr.empty() ) )
		{
			Json::Reader l_Reader;
			Json::Value l_JsonValues;
			SVString l_Command;

			if( l_Reader.parse( l_CommandElementPtr->GetJsonCommand(), l_JsonValues, false ) )
			{
				if( l_JsonValues.isObject() )
				{
					Json::Value l_JsonCommand = l_JsonValues[ _T( "Command" ) ];

					if( l_JsonCommand.isString() )
					{
						l_Command = l_JsonCommand.asString();
					}
					else
					{
						l_TempStatus = E_FAIL;
					}
				}
				else
				{
					l_TempStatus = E_FAIL;
				}
			}
			else
			{
				l_TempStatus = E_FAIL;
			}

			if( S_OK == l_TempStatus )
			{
				if( l_Command == _T( "AddDataItems" ) )
				{
					const Json::Value& l_JsonDataItems = l_JsonValues[ _T( "DataItems" ) ];

					if( l_JsonDataItems.isArray() )
					{
						for (size_t i = 0;i < l_JsonDataItems.size();i++)
						{
							HRESULT l_Temp = S_OK;

							const Json::Value& l_JsonItem = l_JsonDataItems[static_cast<Json::Value::UInt>(i)];

							if( l_JsonItem.isObject() )
							{
								Json::Value l_JsonMatchString = l_JsonItem[ _T( "MatchString" ) ];
								Json::Value l_JsonMatchId = l_JsonItem[ _T( "MatchStringId" ) ];

								SVDataContainer::index_iterator< from >::type l_FindIter = m_Elements.get< from >().find( SVDataElement( l_JsonMatchString.asCString() ) );

								if( l_FindIter == m_Elements.get< from >().end() )
								{
									long l_Identifier = ::InterlockedExchangeAdd( &m_ElementIdentifier, 1 );

									m_Elements.insert( SVDataContainer::value_type( SVDataElement( l_JsonMatchString.asCString(), l_JsonMatchId.asInt() ), l_Identifier ) );
								}
							}
							else
							{
								l_Temp = E_FAIL;
							}

							if( S_OK == l_TempStatus )
							{
								l_TempStatus = l_Temp;
							}
						}
					}
					else
					{
						l_TempStatus = E_FAIL;
					}
				}
				else if( l_Command == _T( "ClearDataItems" ) )
				{
					m_Elements.clear();
				}
				else
				{
					l_TempStatus = E_FAIL;
				}
			}
		}

		if( S_OK == l_Status )
		{
			l_Status = l_TempStatus;
		}
	}

	return l_Status;
}

SVRemoteInputTool::SVCommandQueueElement::SVCommandQueueElement( const SVGUID& p_rObjectId, SVObjectCommandDataJsonPtr& p_rJsonCommandPtr )
: SVCommandTemplate( 0 ), m_ObjectId( p_rObjectId ), m_JsonCommandPtr( p_rJsonCommandPtr )
{
}

SVRemoteInputTool::SVCommandQueueElement::~SVCommandQueueElement()
{
}

HRESULT SVRemoteInputTool::SVCommandQueueElement::Execute()
{
	HRESULT l_Status = S_OK;

	if( !( m_JsonCommandPtr.empty() ) )
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ObjectId );
		SVRemoteInputTool* l_pTool = dynamic_cast< SVRemoteInputTool* >( l_pObject );

		if( nullptr != l_pTool )
		{
			Json::Reader l_Reader;
			Json::Value l_JsonValues;
			SVString l_Command;

			if( l_Reader.parse( m_JsonCommandPtr->GetJsonCommand(), l_JsonValues, false ) )
			{
				if( l_JsonValues.isObject() )
				{
					Json::Value l_JsonCommand = l_JsonValues[ _T( "Command" ) ];

					if( l_JsonCommand.isString() )
					{
						l_Command = l_JsonCommand.asString();
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			if( S_OK == l_Status )
			{
				if( l_Command == _T( "QueryDataItems" ) )
				{
					l_pTool->ProcessCommandQueue();

					Json::FastWriter l_Writer;
					Json::Value l_Object(Json::objectValue);
					Json::Value l_Array(Json::arrayValue);

					SVDataContainer::index_iterator< to >::type l_Iter = l_pTool->m_Elements.get< to >().begin();

					while( l_Iter != l_pTool->m_Elements.get< to >().end() )
					{
						Json::Value l_Element(Json::objectValue);

						l_Element[ _T( "MatchString" ) ] = l_Iter->first.m_ElementData.c_str();
						l_Element[ _T( "MatchStringId" ) ] = l_Iter->first.m_ElementIdentifier;

						l_Array.append( l_Element );

						++l_Iter;
					}

					l_Object[ _T( "DataItems" ) ] = l_Array;
					l_Object[ _T( "Response" ) ] = l_Command.c_str();

					m_JsonCommandPtr->m_JsonResults = l_Writer.write( l_Object ).c_str();
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		m_JsonCommandPtr->NotifyRequestComplete();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVRemoteInputTool::SVDataElement::SVDataElement()
: m_ElementData( _T( "" ) )
, m_ElementIdentifier( 0 )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const SVString& p_rElementData, long p_ElementIdentifier )
: m_ElementData( p_rElementData )
, m_ElementIdentifier( p_ElementIdentifier )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const SVString& p_rElementData )
: m_ElementData( p_rElementData )
, m_ElementIdentifier( 0 )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const SVDataElement& p_rObject )
: m_ElementData( p_rObject.m_ElementData )
, m_ElementIdentifier( p_rObject.m_ElementIdentifier )
{
}

SVRemoteInputTool::SVDataElement::~SVDataElement()
{
}

bool SVRemoteInputTool::SVDataElement::operator==( const SVDataElement& p_rObject ) const
{
	return ( m_ElementData == p_rObject.m_ElementData );
}

bool SVRemoteInputTool::SVDataElement::operator<( const SVDataElement& p_rObject ) const
{
	return ( m_ElementData < p_rObject.m_ElementData );
}


