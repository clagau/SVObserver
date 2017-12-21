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

SVRemoteInputTool::SVRemoteInputTool( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
, m_ElementIdentifier( 0 )
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::SVRemoteInputToolObjectType;

	// Register an empty input object
	m_InputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputObjectInfo, _T( "RemoteInputValue" ) );

	// Remove Embedded Extents
	removeEmbeddedExtents();

	RegisterEmbeddedObject( &m_MatchedValueId, SVRemoteInputToolMatchedValueIdGuid, IDS_OBJECTNAME_MATCHEDVALUEID, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_MatchedValue, SVRemoteInputToolMatchedValueGuid, IDS_OBJECTNAME_MATCHEDVALUE, false, SvOi::SVResetItemNone );

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

bool SVRemoteInputTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_Status = SVToolClass::CreateObject(rCreateStructure);

	l_Status &= ( S_OK == SVInputStreamManager::Instance().InsertInputStream( GetCompleteName().c_str(), GetUniqueObjectID() ) );

	m_isCreated = l_Status;

	m_MatchedValueId.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_MatchedValue.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return l_Status;
}

bool SVRemoteInputTool::CloseObject()
{
	bool l_Status = ( S_OK == SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() ) );
		
	l_Status &= SVToolClass::CloseObject();

	return l_Status;
}

bool SVRemoteInputTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
}

HRESULT SVRemoteInputTool::ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr )
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	std::string l_Command;

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
			SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

			if( nullptr != pInspection )
			{
				SVCommandTemplatePtr l_CommandPtr{ new SVCommandQueueElement(GetUniqueObjectID(), p_rDataPtr) };

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

HRESULT SVRemoteInputTool::SetInputObject(const std::string& rGuidName)
{
	HRESULT l_Status = S_OK;

	SVObjectReference ObjectRef{ rGuidName };
	m_InputObjectInfo.SetInputObject(ObjectRef);

	if( !( m_InputObjectInfo.IsConnected() ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVRemoteInputTool::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() );
	SVInputStreamManager::Instance().InsertInputStream( GetCompleteName().c_str(), GetUniqueObjectID() );
	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

SVObjectReference SVRemoteInputTool::GetInputObject() const
{
	if( m_InputObjectInfo.IsConnected() && nullptr != m_InputObjectInfo.GetInputObjectInfo().getObject())
	{
		return m_InputObjectInfo.GetInputObjectInfo().GetObjectReference();
	}
	return SVObjectReference();
}

bool SVRemoteInputTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = SVToolClass::onRun( rRunStatus, pErrorMessages );

	std::string MatchString;
	std::string MatchedString;
	long l_MatchedStringId = 0;
	long l_Identifier = 0;

	if (Result && S_OK != ProcessCommandQueue() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ProcessCommandQueueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	Result = Result && ValidateLocal(pErrorMessages);

	if( Result )
	{

		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (m_InputObjectInfo.GetInputObjectInfo().getObject());
		if( nullptr !=  pValueObject )
		{
				Result = ( S_OK == pValueObject->getValue( MatchString ) );
		}
		else
		{
			Result = false;
		}


		if (!Result)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( Result )
	{
		SVDataContainer::index_iterator< from >::type l_Iter = m_Elements.get< from >().find( SVDataElement( MatchString ) );

		if( l_Iter != m_Elements.get< from >().end() )
		{
			rRunStatus.SetPassed();

			l_Identifier = l_Iter->second;

			MatchedString = l_Iter->first.m_ElementData;
			l_MatchedStringId = l_Iter->first.m_ElementIdentifier;
		}
	}

	m_MatchedValueId.SetValue(l_MatchedStringId);
	m_MatchedValue.SetValue(_variant_t(MatchedString.c_str()));

	if( Result && ( l_Identifier != 0 ) )
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
		rRunStatus.SetFailed();
	}

	return Result;
}



HRESULT SVRemoteInputTool::ProcessCommandQueue()
{
	HRESULT l_Status = S_OK;

	while( !( m_Commands.IsEmpty() ) )
	{
		HRESULT l_TempStatus = S_OK;

		SVObjectCommandDataJsonPtr l_CommandElementPtr;

		if( m_Commands.RemoveHead( &l_CommandElementPtr ) && nullptr != l_CommandElementPtr )
		{
			Json::Reader l_Reader;
			Json::Value l_JsonValues;
			std::string l_Command;

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

	if(nullptr != m_JsonCommandPtr)
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( m_ObjectId );
		SVRemoteInputTool* l_pTool = dynamic_cast< SVRemoteInputTool* >( l_pObject );

		if( nullptr != l_pTool )
		{
			Json::Reader l_Reader;
			Json::Value l_JsonValues;
			std::string l_Command;

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

SVRemoteInputTool::SVDataElement::SVDataElement( const std::string& p_rElementData, long p_ElementIdentifier )
: m_ElementData( p_rElementData )
, m_ElementIdentifier( p_ElementIdentifier )
{
}

SVRemoteInputTool::SVDataElement::SVDataElement( const std::string& p_rElementData )
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

bool SVRemoteInputTool::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if ( !m_InputObjectInfo.IsConnected() || nullptr == m_InputObjectInfo.GetInputObjectInfo().getObject() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	
	return true;
}
