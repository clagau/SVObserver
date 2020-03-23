//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputInfoListClass
//* .File Name       : $Workfile:   SVOutputInfoListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:26:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <typeinfo>
#include "SVOutputInfoListClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVObjectClass.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace
{
	class DebugOutput
	{
	public:
		explicit DebugOutput( LPCTSTR strFunctionName ) : m_strFunction(strFunctionName) {}

		virtual ~DebugOutput();

		void Add( LPCTSTR sLine );

		std::vector< std::string > m_vecLog;
		std::string m_strFunction;
	};

	void DebugOutput::Add( LPCTSTR sLine )
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OILIST)
		::OutputDebugString( _T(".") );
#endif

		std::string l_Temp = sLine;
		l_Temp += _T("\n");
		m_vecLog.push_back( l_Temp );
	}

	DebugOutput::~DebugOutput()
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OILIST)
		if ( m_vecLog.size() > 0 )
		{
			::OutputDebugString( _T("\n") );
			__int64 iTimestamp = static_cast< __int64 >( SvTl::GetTimeStamp() );
			std::string strBegin;
			strBegin = SvUl::Format(_T("--> %s  :  %I64u\n"), m_strFunction.c_str(), iTimestamp );
			::OutputDebugString( strBegin.c_str() );
			for ( unsigned long i=0; i < m_vecLog.size(); i++ )
			{
				::OutputDebugString( m_vecLog[i].c_str() );
			}
			std::string strEnd = SvUl::Format(_T("<-- %s\n"), m_strFunction.c_str() );
			::OutputDebugString( strEnd.c_str() );
		}
#endif
	}
}// end namespace

SVOutputInfoListClass::SVOutputInfoListClass()
{
	m_svObjectArray.clear();
}

SVOutputInfoListClass::~SVOutputInfoListClass()
{
}

SVOutputInfoListClass::iterator SVOutputInfoListClass::begin()
{
	return m_svObjectArray.begin();
}

SVOutputInfoListClass::iterator SVOutputInfoListClass::end()
{
	return m_svObjectArray.end();
}

SVOutputInfoListClass::const_iterator SVOutputInfoListClass::begin() const
{
	return m_svObjectArray.begin();
}

SVOutputInfoListClass::const_iterator SVOutputInfoListClass::end() const
{
	return m_svObjectArray.end();
}

SVOutputInfoListClass::iterator SVOutputInfoListClass::erase( iterator p_Where )
{
	return m_svObjectArray.erase( p_Where );
}

bool SVOutputInfoListClass::CheckExistence( int Index /*= -1*/ )
{
	bool Result{ false };
	long l_lSize = static_cast<int> (m_svObjectArray.size());

	if( Index < l_lSize && Index >= -1 )
	{
		if( Index > -1 )
		{
			SVOutObjectInfoStruct *pObjectInfo = m_svObjectArray[Index];
			// Check only this entry...
			if( nullptr != pObjectInfo )
			{
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pObjectInfo->getUniqueObjectID());
				Result = ( nullptr != pObject ) ? true : false;
			}
		}
		else
		{
			// Check all entries...
			Result = true;

			for( int i = 0; Result && i < l_lSize; ++i )
			{
				SVOutObjectInfoStruct *pObjectInfo = m_svObjectArray[i];

				if( nullptr != pObjectInfo )
				{
					SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pObjectInfo->getUniqueObjectID());
					Result = ( ( nullptr != pObject) ? true : false ) && Result;
				}
				else
				{
					Result = false;
				}
			}
		}
	}
	return Result;
}

int SVOutputInfoListClass::GetSize() const
{
	return static_cast<int> (m_svObjectArray.size());
}

SVOutObjectInfoStruct* SVOutputInfoListClass::GetAt( int Index )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetAt"));
	SVOutObjectInfoStruct* l_psvObjectInfo = m_svObjectArray[Index];

	if ( nullptr != l_psvObjectInfo )
	{
		if ( nullptr != l_psvObjectInfo )
		{
			if ( nullptr != l_psvObjectInfo->getObject() )
			{
				try
				{
					SVObjectClass* l_psvObject = dynamic_cast<SVObjectClass*>( l_psvObjectInfo->getObject() );

					if( nullptr == l_psvObject )
					{
						if( ! CheckExistence( Index ) )
						{
							l_psvObjectInfo = nullptr;
						}
					}
				}
				catch (std::bad_cast& e)
				{
					e;
					l_psvObjectInfo->GetObjectReference().clear();
					debug.Add(_T("BAD OBJECT (bad_cast)"));
				}
				catch( ... )
				{
					l_psvObjectInfo->GetObjectReference().clear();
					debug.Add(_T("BAD OBJECT (...)"));
				}
			}
			else
			{
				std::string Temp = SvUl::Format(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject %08X)"), l_psvObjectInfo->getObject());
				debug.Add( Temp.c_str() );
				l_psvObjectInfo->GetObjectReference().clear();
			}
		}
		else
		{
			debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
			m_svObjectSet.erase( l_psvObjectInfo );
			m_svObjectArray[Index] = nullptr;
			l_psvObjectInfo = nullptr;
		}
	}
	else
	{
		debug.Add(_T("NULL object"));
	}

	return l_psvObjectInfo;
}

////////////////////////////////////////////////////////////////////////////////
// Override for Add - Do not allow duplicates
////////////////////////////////////////////////////////////////////////////////
int SVOutputInfoListClass::Add( SVOutObjectInfoStruct* POutObjectInfo )
{
	std::pair<SVStdSetSVOutObjectInfoStructPtr::iterator, bool> l_oPair;

	l_oPair = m_svObjectSet.insert( POutObjectInfo );

	if( true == l_oPair.second )
	{
		m_svObjectArray.push_back( POutObjectInfo );
	}

	return static_cast<int> (m_svObjectArray.size());
}

void SVOutputInfoListClass::RemoveAt( int nIndex )
{
	long l_lSize = static_cast<int> (m_svObjectArray.size());

	if( 0 <= nIndex && nIndex < l_lSize )
	{
		m_svObjectSet.erase( m_svObjectArray[nIndex] );

		m_svObjectArray.erase( m_svObjectArray.begin() + nIndex );
	}
}

void  SVOutputInfoListClass::RemoveAll()
{
	m_svObjectArray.clear();

	m_svObjectSet.clear();
}

/////////////////////////////////////////////////////////////////////////////
//
// Build a list of objects with desired attributes set.
//
//
void SVOutputInfoListClass::GetSetAttributesList( UINT uAttributeMask, SVOutputInfoListClass * pFillList )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetSetAttributesList"));
	int nCount = static_cast<int> (m_svObjectArray.size());
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray[i];

		if ( nullptr != pInfoObject )
		{
			if ( nullptr != pInfoObject )
			{
				if ( nullptr != pInfoObject->getObject() )
				{

					SVObjectClass* pObject = nullptr;

					try
					{
						pObject = dynamic_cast<SVObjectClass*>( pInfoObject->getObject() );
					}
					catch ( std::bad_cast& e )
					{
						e;
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray[i] = nullptr;
						debug.Add(_T("BAD OBJECT (bad_cast)"));
					}
					catch( ... )
					{
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray[i] = nullptr;
						debug.Add(_T("BAD OBJECT (...)"));
					}

					if( pObject )
					{
						UINT uAttributes = pObject->ObjectAttributesSet();

						if(( uAttributes & uAttributeMask ) == uAttributeMask)
						{
							pFillList->Add(pInfoObject);
						}
					}
				}// if validate pObject
				else
				{
					debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject)"));
					pInfoObject->GetObjectReference().clear();
				}
			}// if validate pInfoObject
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray[i] =nullptr;
			}
		}
		else
		{
			debug.Add(_T("NULL object"));
		}
	}
}// end GetSetAttributesList

void SVOutputInfoListClass::GetSetAttributesList( UINT uAttributeMask, SVObjectReferenceVector& rvecObjects )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetSetAttributesList"));
	int nCount = static_cast<int> (m_svObjectArray.size());
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray[i];

		if ( nullptr != pInfoObject )
		{
			if ( nullptr != pInfoObject )
			{
				if ( nullptr != pInfoObject->getObject() )
				{
					SVObjectReference ObjectRef = pInfoObject->GetObjectReference();
					SvOi::IValueObject* pValueObject = ObjectRef.getValueObject();
					//! When Value object check if it is an array
					if( nullptr != pValueObject && pValueObject->isArray())
					{
						for ( int j = 0; j < pValueObject->getArraySize(); j++ )
						{
							ObjectRef.SetArrayIndex( j );
							if( ( ObjectRef.ObjectAttributesSet() & uAttributeMask ) == uAttributeMask )
							{
								rvecObjects.push_back( ObjectRef );
							}
						}
					}
					else
					{
						UINT uAttributes = ObjectRef.ObjectAttributesSet();

						if( ( uAttributes & uAttributeMask ) == uAttributeMask )
						{
							rvecObjects.push_back( ObjectRef );
						}
					}
				}
				else
				{
					debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject)"));
					pInfoObject->GetObjectReference().clear();
				}
			}// end if validate (pInfoObject)
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray[i] =nullptr;
			}
		}
		else
		{
			debug.Add(_T("NULL object"));
		}
	}// end for( int i=0; i<nCount; i++)
}// end GetSetAttributesList