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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVRemoteInputToolObjectType;

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

	l_Status &= ( SVInputStreamManager::Instance().InsertInputStream( static_cast< const char* >( GetCompleteObjectName() ), GetUniqueObjectID() ) == S_OK );

	isCreated = l_Status;

	m_MatchedValueId.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_MatchedValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	return l_Status;
}

BOOL SVRemoteInputTool::CloseObject()
{
	BOOL l_Status = ( SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() ) == S_OK );
		
	l_Status &= SVToolClass::CloseObject();

	return l_Status;
}

BOOL SVRemoteInputTool::OnValidate()
{
	BOOL l_Status = SVToolClass::OnValidate();

	l_Status = l_Status && ( m_InputObjectInfo.IsConnected() );
	l_Status = l_Status && ( m_InputObjectInfo.GetInputObjectInfo().PObject != NULL );

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

	if( l_Status == S_OK )
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

				if( l_Status != S_OK )
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

	m_InputObjectInfo.SetInputObject( NULL );

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
	SVObjectClass* l_pObject = NULL;

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

	l_Status = l_Status && ( ProcessCommandQueue() == S_OK );
	l_Status = l_Status && ( m_InputObjectInfo.IsConnected() );
	l_Status = l_Status && ( m_InputObjectInfo.GetInputObjectInfo().PObject != NULL );

	if( l_Status )
	{
		SVValueObjectClass* l_pValueObject = dynamic_cast< SVValueObjectClass* >( m_InputObjectInfo.GetInputObjectInfo().PObject );

		l_Status = ( l_pValueObject != NULL );

		if( l_Status )
		{
			l_Status = ( l_pValueObject->GetValue( l_MatchString ) == S_OK );
		}
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
	//BOOL bResult;
	DWORD_PTR l_Status = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
		case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (DwMessageValue); // Object with new name
			LPCTSTR orgName = ( LPCTSTR )DwMessageContext;

			SVInputStreamManager::Instance().EraseInputStream( GetUniqueObjectID() );

			if( SVInputStreamManager::Instance().InsertInputStream( static_cast< const char* >( GetCompleteObjectName() ), GetUniqueObjectID() ) == S_OK )
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

			if( l_TempStatus == S_OK )
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

							if( l_TempStatus == S_OK )
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

		if( l_Status == S_OK )
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

		if( l_pTool != NULL )
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

			if( l_Status == S_OK )
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


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteInputTool.cpp_v  $
 * 
 *    Rev 1.4   13 Nov 2014 10:10:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  932
 * SCR Title:  Clean up GetInspectionItems and SVCommandInspectionGetItemsPtr (SVO-150)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   In method ProcessNotifyData fit SubmitCommand to new signature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 12:40:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 12:03:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:48:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Feb 2013 14:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Aug 2012 12:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Jul 2012 15:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jun 2012 14:12:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 13:43:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Notated and fixed issue with Json Library iterator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2012 09:40:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:36:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new remote functionality to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/