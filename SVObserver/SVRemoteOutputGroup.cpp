//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputGroup
//* .File Name       : $Workfile:   SVRemoteOutputGroup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 07:58:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>
#include "SVRemoteOutputGroup.h"
#include "SVRemoteControlConstants.h"
#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"

#include "SVObjectLibrary\SVClsIds.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVObserver.h"
#include "SVOutputStreamManager.h"
#include "SVRemoteOutputObject.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputGroup, SVRemoteOutputGroupGUID );

SVRemoteOutputGroup::SVRemoteOutputGroup( LPCTSTR ObjectName )
: SVObjectClass( ObjectName )
, m_CriticalSection()
, m_StreamDataItems( 0 )
{
	Create();
}

SVRemoteOutputGroup::SVRemoteOutputGroup( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_CriticalSection()
, m_StreamDataItems( 0 )
{
	Create();
}

SVRemoteOutputGroup::~SVRemoteOutputGroup(void)
{
	if( m_bCreated )
	{
		Destroy();
	}
}

HRESULT SVRemoteOutputGroup::ProcessNotifyData( SVObjectCommandDataJsonPtr& p_rDataPtr )
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;
	std::string l_Command;

	if( l_Reader.parse( p_rDataPtr->GetJsonCommand(), l_JsonValues, false ) )
	{
		if( l_JsonValues.isObject() )
		{
			Json::Value l_JsonCommand = l_JsonValues[ SVRC::stream::command ];

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
		if( l_Command == SVRC::stream::startStream.c_str() )
		{
			::InterlockedExchange( &m_StreamDataItems, 1 );

			Json::FastWriter l_Writer;
			Json::Value l_Object(Json::objectValue);

			l_Object[ SVRC::stream::response ] = SVRC::stream::startStream;
			l_Object[ SVRC::stream::streamName ] = GetName();
			l_Object[ SVRC::stream::status ] = S_OK;

			p_rDataPtr->m_JsonResults = l_Writer.write( l_Object ).c_str();
		}
		else if( l_Command == SVRC::stream::stopStream.c_str() )
		{
			::InterlockedExchange( &m_StreamDataItems, 0 );

			Json::FastWriter l_Writer;
			Json::Value l_Object(Json::objectValue);

			l_Object[ SVRC::stream::response ] = SVRC::stream::stopStream;
			l_Object[ SVRC::stream::streamName ] = GetName();
			l_Object[ SVRC::stream::status ] = S_OK;

			p_rDataPtr->m_JsonResults = l_Writer.write( l_Object ).c_str();
		}
		else if( l_Command == SVRC::iobroker::queryDataItems.c_str() )
		{
			Json::FastWriter l_Writer;
			Json::Value l_Object(Json::objectValue);
			Json::Value l_Array(Json::arrayValue);

			if( !( m_RemoteOutputs.empty() ) )
			{
				SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

				if( l_AutoLock.Assign( &m_CriticalSection ) )
				{
					SVRemoteOutputObjectList::iterator l_it;

					for( l_it = m_RemoteOutputs.begin() ; l_it != m_RemoteOutputs.end(); ++l_it )
					{
						std::string l_Name = ( *l_it )->GetInputValueObjectName();

						if( !( l_Name.empty() ) )
						{
							l_Array.append( l_Name.c_str() );
						}
					}
				}
			}

			l_Object[ SVRC::stream::response ] = SVRC::iobroker::queryDataItems;
			l_Object[ SVRC::stream::streamName ] = GetName();
			l_Object[ SVRC::stream::dataItems ] = l_Array;

			p_rDataPtr->m_JsonResults = l_Writer.write( l_Object ).c_str();
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	p_rDataPtr->NotifyRequestComplete();

	return l_Status;
}

HRESULT SVRemoteOutputGroup::ObserverUpdate( const SVProductInfoStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	if( 1 == m_StreamDataItems )
	{
		std::string l_JsonResults;
		Json::FastWriter l_Writer;
		Json::Value l_Object(Json::objectValue);
		Json::Value l_Array(Json::arrayValue);
		long l_Index = p_rData.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

		if( 0 <= l_Index && !( m_RemoteOutputs.empty() ) )
		{
			SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

			if( l_AutoLock.Assign( &m_CriticalSection ) )
			{
				SVRemoteOutputObjectList::iterator l_it;

				for( l_it = m_RemoteOutputs.begin() ; l_it != m_RemoteOutputs.end(); ++l_it )
				{
					SVRemoteOutputObject* l_pOutput = *( l_it );

					if( nullptr != l_pOutput )
					{
						SVObjectClass* pObject = l_pOutput->GetValueObject();

						if( nullptr != pObject )
						{
							Json::Value l_ElementObject(Json::objectValue);
							Json::Value l_ArrayObject(Json::arrayValue);
							long l_TriggerCount = p_rData.ProcessCount();
							HRESULT l_TempStatus = S_OK;

							std::vector<_variant_t> l_VariantVector;

							SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
							if(nullptr != pValueObject )
							{
								pValueObject->getValues( l_VariantVector, l_Index  );
							}

							for( size_t i = 0; ( S_OK == l_TempStatus ) && ( i < l_VariantVector.size() ); ++i )
							{
								switch( l_VariantVector[ i ].vt )
								{
									case VT_BSTR:
									{
										l_ArrayObject.append( static_cast< LPCTSTR >( _bstr_t( l_VariantVector[ i ] ) ) );

										break;
									}
									case VT_BOOL:
									{
										l_ArrayObject.append( static_cast< bool >( l_VariantVector[ i ] ) );

										break;
									}
									case VT_R4:
									case VT_R8:
									{
										l_ArrayObject.append( static_cast< double >( l_VariantVector[ i ] ) );

										break;
									}
									case VT_INT:
									case VT_I1:
									case VT_I2:
									case VT_I4:
									case VT_I8:
									{
										l_ArrayObject.append( static_cast< int >( l_VariantVector[ i ] ) );

										break;
									}
									case VT_UINT:
									case VT_UI1:
									case VT_UI2:
									case VT_UI4:
									case VT_UI8:
									{
										l_ArrayObject.append( static_cast< unsigned int >( l_VariantVector[ i ] ) );

										break;
									}
									default:
									{
										l_ArrayObject.clear();
										l_TriggerCount = -1;
										l_TempStatus = E_FAIL;

										break;
									}
								}
							}

							l_ElementObject[ SVRC::vo::name ] = pObject->GetCompleteName().c_str();
							l_ElementObject[ SVRC::vo::array ] = l_ArrayObject;
							l_ElementObject[ SVRC::vo::count ] = l_TriggerCount;
							l_ElementObject[ SVRC::vo::status ] = l_TempStatus;

							l_Array.append( l_ElementObject );
						}
					}
				}
			}
		}

		l_Object[ SVRC::stream::notification ] = SVRC::stream::outputDataItems;
		l_Object[ SVRC::stream::streamName ] = GetName();
		l_Object[ SVRC::stream::dataItems ] = l_Array;

		l_JsonResults = l_Writer.write( l_Object ).c_str();

		l_Status = SVObjectManagerClass::Instance().UpdateObservers( std::string( SvO::cRemoteOutputGroupTag ), GetUniqueObjectID(), l_JsonResults );
	}

	return l_Status;
}

BOOL SVRemoteOutputGroup::Clear()
{
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection ) )
	{
		SVRemoteOutputObjectList::iterator l_it;

		for( l_it = m_RemoteOutputs.begin() ; l_it != m_RemoteOutputs.end(); ++l_it )
		{
			delete (*l_it);
		}
		m_RemoteOutputs.clear();
	}
	return TRUE;
}

BOOL SVRemoteOutputGroup::Create()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVRemoteOutputGroupType;

	m_bCreated = true;

	SVOutputStreamManager::Instance().InsertOutputStream( GetName(), GetUniqueObjectID() );

	return m_bCreated;
}// end Create

BOOL SVRemoteOutputGroup::Destroy()
{
	SVOutputStreamManager::Instance().EraseOutputStream();

	Clear();

	m_bCreated = false;

	return TRUE;
}

BOOL SVRemoteOutputGroup::AttachStreamManager()
{
	SVOutputStreamManager::Instance().EraseOutputStream();

	bool m_bAttached = S_OK == SVOutputStreamManager::Instance().InsertOutputStream( GetGroupName().c_str(), GetUniqueObjectID() ) ;

	return m_bAttached;
}

// Parameters >> Tree ( Save )
BOOL SVRemoteOutputGroup::GetParameters(SvXml::SVObjectXMLWriter& rWriter ) const
{
	BOOL bOk = TRUE;

	std::string l_SubjectName;
	SVGUID l_SubjectID;
	
	SVObjectManagerClass::Instance().GetObserverSubject( "SVRemoteOutputGroup", GetUniqueObjectID(), l_SubjectID );

	if( !( l_SubjectID.empty() ) )
	{
		SVObjectClass* l_pSubject = SVObjectManagerClass::Instance().GetObject( l_SubjectID );

		if( nullptr != l_pSubject )
		{
			l_SubjectName = l_pSubject->GetName();
		}
	}

	_variant_t svVariant;

	// Unique Id
	svVariant = SVGUID( m_outObjectInfo.m_UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	// PPQ GUID...
	svVariant = m_PPQObjectId.ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_GROUP_PPQ, svVariant );
	svVariant.Clear();

	// Remote Outputs 
	for( size_t i = 0 ; i < m_RemoteOutputs.size(); i++ )
	{
		std::string Branch = SvUl::Format( _T("%s_%d"), SvXml::CTAG_REMOTE_OUTPUT_ENTRY, i + 1 );
		rWriter.StartElement( Branch.c_str() );
		m_RemoteOutputs[i]->GetParameters( rWriter );
		rWriter.EndElement();
	}
	return bOk;
}

// Sets parameters from Tree Control ( Restore )
BOOL SVRemoteOutputGroup::SetParameters( SVTreeType& p_rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = TRUE;
	_variant_t svVariant;

	bOk = SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		SVGUID ObjectID = svVariant;

		bOk = SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		if( bOk )
		{
			m_outObjectInfo.m_UniqueObjectID = ObjectID;

			bOk = SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}
		else
		{
			OutputDebugString( "Remote Output Unable to set RemoteOutputObject Unique Id\n");
		}
	}

	if ( bOk )
	{
		// Remote Output PPQ ID..
		bOk = SvXml::SVNavigateTree::GetItem( p_rTree, SvXml::CTAG_REMOTE_GROUP_PPQ, htiParent, svVariant );
		if( bOk )
		{
			m_PPQObjectId = svVariant;
		}
	}

	// Remote Output List
	if ( bOk )
	{
		BOOL l_bTmp = TRUE;
		SVRemoteOutputObject* l_TmpOutput = nullptr;
		long l_lEntryNum = 0;
		while( l_bTmp )
		{
			SVTreeType::SVBranchHandle htiBranch = nullptr;
			std::string Entry = SvUl::Format( _T("%s_%d"), SvXml::CTAG_REMOTE_OUTPUT_ENTRY, ++l_lEntryNum );
			l_bTmp = SvXml::SVNavigateTree::GetItemBranch( p_rTree, Entry.c_str(), htiParent, htiBranch );
			if ( l_bTmp )
			{
				l_TmpOutput = new SVRemoteOutputObject;
				l_bTmp = l_TmpOutput->SetParameters(p_rTree, htiBranch);
				if( l_bTmp )
				{
					m_RemoteOutputs.push_back( l_TmpOutput );
				}
				else
				{
					// delete the tmp object because something failed now we are done with it.
					OutputDebugString(_T("Remote Output Failed to set parameters\n"));
					delete l_TmpOutput;
				}
			}
		}
	}
	return bOk;
}

// Remote Identifier and new output object will be managed by SVRemoteOutputGroup
HRESULT SVRemoteOutputGroup::AddOutput( SVRemoteOutputObject* p_pOutputEntry )
{
	HRESULT l_hr = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection ) )
	{
		m_RemoteOutputs.push_back( p_pOutputEntry );
	}

	return l_hr;
}

HRESULT SVRemoteOutputGroup::Delete( SVRemoteOutputObject* pRemoteOutput )
{
	HRESULT l_hr = -3128 ;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection ) )
	{
		SVRemoteOutputObjectList::iterator l_it;

		l_it = std::find( m_RemoteOutputs.begin(), m_RemoteOutputs.end(), pRemoteOutput );
		if( l_it != m_RemoteOutputs.end() )
		{
			m_RemoteOutputs.erase( l_it );

			delete pRemoteOutput;
		}
	}
	return l_hr;
}

size_t SVRemoteOutputGroup::RemoteOutputListSize()
{
	return m_RemoteOutputs.size();
}

HRESULT SVRemoteOutputGroup::GetItem( long l_lIndex, SVRemoteOutputObject*& p_rItem )
{
	HRESULT l_hr = -3129 ;
	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_CriticalSection ) )
	{
		if( static_cast<long>(m_RemoteOutputs.size()) > l_lIndex  )
		{
			p_rItem = m_RemoteOutputs[l_lIndex];
			l_hr = S_OK;
		}
	}

	return l_hr;	
}

class EntrySortLess
{
public:
	bool operator()(  SVRemoteOutputObject* q,  SVRemoteOutputObject* p ) const
	{
		bool l_bGreater= false;
		// Figure which PPQ this is...
		std::string l_GroupName1 = q->GetGroupID();
		std::string l_GroupName2 = p->GetGroupID();

		if( l_GroupName2 < l_GroupName1 )
		{
			l_bGreater = true;
		}
		else
		if( l_GroupName2 == l_GroupName1 )
		{
			std::string l_l1 = p->GetInputValueObjectName();
			std::string l_l2 = q->GetInputValueObjectName();
			if( l_l2 < l_l1 )
			{
				l_bGreater = true;
			}
			else
			{
				// Compare channels
			}
		}
		return l_bGreater;
	}
};

SVRemoteOutputObject* SVRemoteOutputGroup::GetItem( long l_lIndex ) const
{
	if( m_RemoteOutputs.size() > (size_t)l_lIndex )
	{
		return m_RemoteOutputs[l_lIndex];
	}
	return nullptr;
}

SVRemoteOutputObject* SVRemoteOutputGroup::GetLastObject( ) const
{
	if( m_RemoteOutputs.size() > 0 )
	{
		return *m_RemoteOutputs.rbegin();
	}
	return nullptr;
}

SVRemoteOutputObject* SVRemoteOutputGroup::GetFirstObject( ) const
{
	if( m_RemoteOutputs.size() > 0 )
	{
		return *m_RemoteOutputs.begin();
	}
	return nullptr;
}

// This function finds the objects index from the Object list.
HRESULT SVRemoteOutputGroup::Find( SVRemoteOutputObject* p_pObject, long& p_rlIndex )
{
	HRESULT l_hr = S_FALSE;
	for( size_t x = 0 ; x < m_RemoteOutputs.size() ; x++ )
	{
		if( m_RemoteOutputs[x] == p_pObject )
		{
			p_rlIndex = static_cast<long>(x);
			l_hr = S_OK;
			break;
		}
	}
	return l_hr;
}

std::string SVRemoteOutputGroup::GetGroupName() const
{
	std::string Result = GetPPQName();

	SVRemoteOutputObject* pFirst = GetFirstObject();

	if (nullptr != pFirst)
	{
		Result = pFirst->GetGroupID();
	}

	return Result;
}

const SVGUID& SVRemoteOutputGroup::GetPPQObjectId() const
{
	return m_PPQObjectId;
}

std::string SVRemoteOutputGroup::GetPPQName() const
{
	std::string l_SubjectName;
	SVGUID l_SubjectID;
	
	SVObjectManagerClass::Instance().GetObserverSubject( std::string( SvO::cPPQObjectTag ), GetUniqueObjectID(), l_SubjectID );

	if( !( l_SubjectID.empty() ) )
	{
		SVObjectClass* l_pSubject = SVObjectManagerClass::Instance().GetObject( l_SubjectID );

		if( nullptr != l_pSubject )
		{
			l_SubjectName = l_pSubject->GetCompleteName();
		}
	}
	else
	{
		SVObjectClass* l_pSubject = SVObjectManagerClass::Instance().GetObject( m_PPQObjectId );

		if( nullptr != l_pSubject  )
		{
			l_SubjectName = l_pSubject->GetCompleteName();
			HRESULT l_hr = SVObjectManagerClass::Instance().AttachObserver( std::string( SvO::cPPQObjectTag ), m_PPQObjectId, GetUniqueObjectID() );
		}
	}

	return l_SubjectName;
}

HRESULT SVRemoteOutputGroup::SetPPQName( const std::string& p_rPPQ )
{
	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObjectCompleteName( p_rPPQ.c_str() );

	if( nullptr != l_pObject )
	{
		HRESULT l_hr;
		SVGUID l_tmpGUID;
		m_PPQObjectId = l_pObject->GetUniqueObjectID();
		l_hr = SVObjectManagerClass::Instance().GetObserverSubject( std::string( SvO::cPPQObjectTag ), GetUniqueObjectID(), l_tmpGUID );

		if( GUID_NULL == l_tmpGUID )
		{
			// Attach Observer No previous attachment...
			SVObjectManagerClass::Instance().AttachObserver( std::string( SvO::cPPQObjectTag ), m_PPQObjectId, GetUniqueObjectID() );
		}
		else
		{
			if( l_tmpGUID != m_PPQObjectId )
			{
				// Detach Observer then attach new...
				l_hr = SVObjectManagerClass::Instance().DetachObserver( std::string( SvO::cPPQObjectTag ), l_tmpGUID, GetUniqueObjectID() );
				l_hr = SVObjectManagerClass::Instance().AttachObserver( std::string( SvO::cPPQObjectTag ), m_PPQObjectId, GetUniqueObjectID() );
			}
			else
			{
				// leave alone it is already attached..
			}
		}
	}
	return S_OK;
}

HRESULT SVRemoteOutputGroup::SetGroupName(const std::string& newName)
{
	for( SVRemoteOutputObjectList::iterator l_it = m_RemoteOutputs.begin() ; l_it != m_RemoteOutputs.end() ; ++l_it)
	{
		(*l_it)->SetGroupID(newName);
	}
	return S_OK;
}

