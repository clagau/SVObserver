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
#include "SVUtilityLibrary/SVString.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVInObjectInfoStruct.h"
#include "SVObjectClass.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

namespace
{
	class DebugOutput
	{
	public:
		DebugOutput( LPCTSTR strFunctionName ) : m_strFunction(strFunctionName) {}

		virtual ~DebugOutput();

		void Add( LPCTSTR sLine );

		std::vector< SVString > m_vecLog;
		SVString m_strFunction;
	};

	void DebugOutput::Add( LPCTSTR sLine )
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OILIST)
		::OutputDebugString( _T(".") );
#endif

		SVString l_Temp = sLine;
		l_Temp += _T("\n");
		m_vecLog.push_back( l_Temp );
	}

	DebugOutput::~DebugOutput()
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OILIST)
		if ( m_vecLog.size() > 0 )
		{
			::OutputDebugString( _T("\n") );
			__int64 iTimestamp = static_cast< __int64 >( SVClock::GetTimeStamp() );
			SVString strBegin;
			strBegin = SvUl_SF::Format(_T("--> %s  :  %I64u\n"), m_strFunction.c_str(), iTimestamp );
			::OutputDebugString( strBegin.c_str() );
			for ( unsigned long i=0; i < m_vecLog.size(); i++ )
			{
				::OutputDebugString( m_vecLog[i].c_str() );
			}
			SVString strEnd = SvUl_SF::Format(_T("<-- %s\n"), m_strFunction.c_str() );
			::OutputDebugString( strEnd.c_str() );
		}
#endif
	}
}// end namespace

SVOutputInfoListClass::SVOutputInfoListClass()
{
	m_svObjectArray.SetSize( 0 );
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

BOOL SVOutputInfoListClass::CheckExistence( int Index /*= -1*/ )
{
	BOOL BRetVal = FALSE;
	long l_lSize = m_svObjectArray.GetSize();

	if( Index < l_lSize && Index >= -1 )
	{
		if( Index > -1 )
		{
			SVOutObjectInfoStruct *l_psvObject = m_svObjectArray.GetAt( Index );
			// Check only this entry...
			if( nullptr != l_psvObject )
			{
				BRetVal = ( nullptr != ( l_psvObject->PObject = SVObjectManagerClass::Instance().GetObject( l_psvObject->UniqueObjectID ) ) ) ? true : false;
			}
		}
		else
		{
			// Check all entries...
			BRetVal = true;

			for( int i = 0; BRetVal && i < l_lSize; ++i )
			{
				SVOutObjectInfoStruct *l_psvObject = m_svObjectArray.GetAt( i );

				if( nullptr != l_psvObject )
				{
					BRetVal = ( ( nullptr != ( l_psvObject->PObject = SVObjectManagerClass::Instance().GetObject( l_psvObject->UniqueObjectID ) ) ) ? true : false ) && BRetVal;
				}
				else
				{
					BRetVal = false;
				}
			}
		}
	}
	return BRetVal;
}

int SVOutputInfoListClass::GetSize() const
{
	return m_svObjectArray.GetSize();
}

SVOutObjectInfoStruct* SVOutputInfoListClass::GetAt( int p_iIndex )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetAt"));
	SVOutObjectInfoStruct* l_psvObjectInfo = m_svObjectArray.GetAt( p_iIndex );

	if ( nullptr != l_psvObjectInfo )
	{
		if ( nullptr != l_psvObjectInfo )
		{
			if ( nullptr != l_psvObjectInfo->PObject )
			{
				try
				{
					SVObjectClass* l_psvObject = dynamic_cast<SVObjectClass*>( l_psvObjectInfo->PObject );

					if( nullptr == l_psvObject )
					{
						if( ! CheckExistence( p_iIndex ) )
						{
							l_psvObjectInfo = nullptr;
						}
					}
				}
				catch (std::bad_cast& e)
				{
					e;
					l_psvObjectInfo->PObject = nullptr;
					debug.Add(_T("BAD OBJECT (bad_cast)"));
				}
				catch( ... )
				{
					l_psvObjectInfo->PObject = nullptr;
					debug.Add(_T("BAD OBJECT (...)"));
				}
			}
			else
			{
				CString s; s.Format(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject %08X)"), l_psvObjectInfo->PObject);
				debug.Add(s);
				l_psvObjectInfo->PObject = nullptr;
			}
		}
		else
		{
			debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
			m_svObjectSet.erase( l_psvObjectInfo ); m_svObjectArray.SetAt(p_iIndex, nullptr);
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
		return m_svObjectArray.Add( POutObjectInfo );
	}

	return m_svObjectArray.GetSize();
}

////////////////////////////////////////////////////////////////////////////////
// Override for InsertAt - Do not allow duplicates,
// never more than one item inserted
////////////////////////////////////////////////////////////////////////////////
void SVOutputInfoListClass::InsertAt( int nIndex, SVOutObjectInfoStruct* POutObjectInfo )
{
	std::pair<SVStdSetSVOutObjectInfoStructPtr::iterator, bool> l_oPair;

	l_oPair = m_svObjectSet.insert( POutObjectInfo );

	if( true == l_oPair.second )
	{
		m_svObjectArray.InsertAt( nIndex, POutObjectInfo );
	}
}

SVOutObjectInfoStruct *&SVOutputInfoListClass::ElementAt( int p_iIndex )
{
	return m_svObjectArray.ElementAt( p_iIndex );
}

void SVOutputInfoListClass::RemoveAt( int nIndex )
{
	long l_lSize = m_svObjectArray.GetSize();

	if( 0 <= nIndex && nIndex < l_lSize )
	{
		m_svObjectSet.erase( m_svObjectArray.GetAt( nIndex ) );

		m_svObjectArray.RemoveAt( nIndex );
	}
}

void  SVOutputInfoListClass::RemoveAll()
{
	m_svObjectArray.RemoveAll();

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
	int nCount = m_svObjectArray.GetSize();
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray.GetAt(i);

		if ( nullptr != pInfoObject )
		{
			if ( nullptr != pInfoObject )
			{
				if ( nullptr != pInfoObject->PObject )
				{

					SVObjectClass* pObject = nullptr;

					try
					{
						pObject = dynamic_cast<SVObjectClass*>( pInfoObject->PObject );
					}
					catch ( std::bad_cast& e )
					{
						e;
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (bad_cast)"));
					}
					catch( ... )
					{
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
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
					pInfoObject->PObject = nullptr;
				}
			}// if validate pInfoObject
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
			}
		}
		else
		{
			debug.Add(_T("NULL object"));
		}
	}
}// end GetSetAttributesList

void SVOutputInfoListClass::GetObjectReferenceList( SVObjectReferenceVector& rvecObjects )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetObjectReferenceList"));
	int nCount = m_svObjectArray.GetSize();
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray.GetAt(i);

		if ( nullptr != pInfoObject )
		{

			if ( nullptr != pInfoObject )
			{	
				if ( nullptr != pInfoObject->PObject )
				{
					SVObjectClass* pObject = nullptr;

					try
					{
						pObject = dynamic_cast<SVObjectClass*>( pInfoObject->PObject );
					}
					catch ( std::bad_cast& e )
					{
						e;
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (bad_cast)"));
					}
					catch( ... )
					{
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (...)"));
					}

					if( pObject )
					{
						SVObjectReference ref = pInfoObject->GetObjectReference();
						if( ref.Object() )
						{
							if ( ref.Object()->IsArray() )
							{
								ref.SetEntireArray();
								rvecObjects.push_back( ref );

								for ( int i=0; i < ref.Object()->GetArraySize(); i++ )
								{
									ref.SetArrayIndex(i);
									rvecObjects.push_back( ref );
								}
							}
							else
							{
								rvecObjects.push_back( ref );
							}
						}
					}
				}
				else
				{
					debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject)"));
					pInfoObject->PObject = nullptr;
				}
			}
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
			}
		}
		else
		{
			debug.Add(_T("NULL object"));
		}
	}// end for( int i=0; i<nCount; i++)

}// end GetObjectReferenceList

void SVOutputInfoListClass::GetSetAttributesList( UINT uAttributeMask, SVObjectReferenceVector& rvecObjects )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetSetAttributesList"));
	int nCount = m_svObjectArray.GetSize();
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray.GetAt(i);

		if ( nullptr != pInfoObject )
		{
			if ( nullptr != pInfoObject )
			{
				if ( nullptr != pInfoObject->PObject )
				{
					SVObjectClass* pObject = nullptr;

					try
					{
						pObject = dynamic_cast<SVObjectClass*>( pInfoObject->PObject );
					}
					catch ( std::bad_cast& e )
					{
						e;
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (bad_cast)"));
					}
					catch( ... )
					{
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (...)"));
					}

					if( pObject )
					{
						SVObjectReference ref = pInfoObject->GetObjectReference();
						if( ref.Object() )
						{

							if ( ref.Object()->IsArray() )
							{
								for ( int j = 0; j < ref.Object()->GetArraySize(); j++ )
								{
									ref.SetArrayIndex( j );
									if( ( ref.ObjectAttributesSet() & uAttributeMask ) == uAttributeMask )
									{
										rvecObjects.push_back( ref );
									}
								}
							}
							else
							{
								UINT uAttributes = ref.ObjectAttributesSet();

								if( ( uAttributes & uAttributeMask ) == uAttributeMask )
								{
									rvecObjects.push_back( ref );
								}
							}
						}// end if( ref.Object() )
					}
				}// end if validate pObject
				else
				{
					debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject)"));
					pInfoObject->PObject = nullptr;
				}
			}// end if validate (pInfoObject)
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
			}
		}
		else
		{
			debug.Add(_T("NULL object"));
		}
	}// end for( int i=0; i<nCount; i++)
}// end GetSetAttributesList

/////////////////////////////////////////////////////////////////////////////
// Build a list of objects with desired attributes allowed.
/////////////////////////////////////////////////////////////////////////////
void SVOutputInfoListClass::GetAllowedAttributesList( UINT uAttributeMask, SVOutputInfoListClass * pFillList )
{
	DebugOutput debug(_T("SVOutputInfoListClass::GetAllowedAttributesList"));
	int nCount = m_svObjectArray.GetSize();
	for( int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoObject = m_svObjectArray.GetAt(i);

		if ( nullptr != pInfoObject )
		{
			if ( nullptr != pInfoObject )
			{
				if ( nullptr != pInfoObject->PObject )
				{

					SVObjectClass* pObject = nullptr;

					try
					{
						pObject = dynamic_cast<SVObjectClass*>( pInfoObject->PObject );
					}
					catch ( std::bad_cast& e )
					{
						e;
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (bad_cast)"));
					}
					catch( ... )
					{
						pObject = nullptr;
						m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
						debug.Add(_T("BAD OBJECT (...)"));
					}

					if( pObject )
					{
						UINT uAttributes = pObject->ObjectAttributesAllowed();

						if(( uAttributes & uAttributeMask ) == uAttributeMask)
						{
							pFillList->Add(pInfoObject);
						}
					}
				}
				else
				{
					CString s; s.Format(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject->PObject %08X)"), pInfoObject->PObject);
					debug.Add(s);
					pInfoObject->PObject = nullptr;
				}
			}
			else
			{
				debug.Add(_T("BAD OBJECT (VALIDATE_OBJECT pInfoObject)"));
				m_svObjectSet.erase( pInfoObject ); m_svObjectArray.SetAt(i, nullptr);
			}
		}
		else // nullptr
		{
			debug.Add(_T("NULL object"));
		}
	}
}

bool SVOutputInfoListClass::HasDependents()
{
	long l_lSize = m_svObjectArray.GetSize();

	for( int i = 0;i < l_lSize; i++ )
	{
		SVOutObjectInfoStruct* pOutObjectInfo = m_svObjectArray.GetAt( i );
		
		// is anyone using this output
		if( pOutObjectInfo && pOutObjectInfo->GetInputSize() )
		{
			return true;
		}
	}
	return false;
}

