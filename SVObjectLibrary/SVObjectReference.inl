//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectReference
//* .File Name       : $Workfile:   SVObjectReference.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:26  $
//******************************************************************************

extern const GUID SVInvalidGUID;

inline SVObjectReference::SVObjectReference()
{
	init();
}

inline SVObjectReference::SVObjectReference( GUID guid )
{
	init();
	m_Guid = guid;
}

inline void SVObjectReference::init()
{
	m_pObject = nullptr;
	m_Guid = SVInvalidGUID;
	m_NameInfo.clear();
	m_ArrayIndex = -1;
	m_IsArray = false; 
}

inline SVObjectReference::SVObjectReference( const SVObjectReference& rhs )
{
	*this = rhs;
}

inline bool SVObjectReference::operator == ( const SVObjectReference& rhs ) const
{
	bool res = (m_pObject == rhs.m_pObject) && (m_ArrayIndex == rhs.m_ArrayIndex) && (m_IsArray == rhs.m_IsArray);
	return res;
}

inline SVObjectClass* SVObjectReference::operator -> ()
{
	return m_pObject;
}

inline const SVObjectClass* SVObjectReference::operator -> () const
{
	return m_pObject;
}

inline SVObjectClass* SVObjectReference::Object() const
{
	return m_pObject;
}

inline bool SVObjectReference::IsIndexPresent() const
{
	return m_IsArray;
}

inline long SVObjectReference::ArrayIndex() const
{
	long Index = -1;
	if(m_IsArray && m_ArrayIndex > -1)
	{
		Index = m_ArrayIndex ;
	}
	
	return Index;
}

inline CString SVObjectReference::DefaultValue() const
{
	return m_NameInfo.GetDefaultValue().c_str();
}

inline bool SVObjectReference::IsEntireArray() const
{
	return m_IsArray && m_ArrayIndex == -1;
	
}

inline GUID SVObjectReference::Guid() const
{
	return m_Guid;
}

inline bool SVObjectReference::operator < ( const SVObjectReference& rhs ) const
{
	return (m_pObject < rhs.m_pObject) || ((m_pObject == rhs.m_pObject) && (m_ArrayIndex < rhs.m_ArrayIndex));
}

inline const SVObjectReference& SVObjectReference::operator = ( const SVObjectReference& rhs )
{
	m_pObject = rhs.m_pObject;
	m_Guid = rhs.m_Guid != SVInvalidGUID ? rhs.m_Guid : (m_pObject ? GetObjectGuid( m_pObject ) : SVInvalidGUID);
	m_NameInfo = rhs.m_NameInfo;
	m_IsArray = rhs.m_IsArray;
	m_ArrayIndex = rhs.m_ArrayIndex;
	return *this;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( SVObjectClass* pObject )
{
	m_pObject = dynamic_cast <T*> (pObject);
	if ( m_pObject )
		m_Guid = m_pObject->GetUniqueObjectID();
	else
		m_Guid = SVInvalidGUID;
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( T* pObject )
{
	m_pObject = pObject;	// no dynamic_cast
	if ( m_pObject )
		m_Guid = m_pObject->GetUniqueObjectID();
	else
		m_Guid = SVInvalidGUID;
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( const SVObjectReference& rhs )
{
	*this = rhs;
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( const SVCheckedObjectReference<T>& rhs )
{
	*this = rhs;
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue )
{
	m_pObject = dynamic_cast <T*> (pObject);
	if ( m_pObject )
	{
		m_Guid = m_pObject->GetUniqueObjectID();
		if( nullptr != m_pObject )
		{
			m_NameInfo.ParseObjectName( static_cast< LPCTSTR >( m_pObject->GetCompleteObjectName() ) );
		}
		m_NameInfo.SetIsIndexPresent(true);
		m_NameInfo.SetIndex( SvUl_SF::Format(_T("%d"), lArrayIndex ) );
		m_NameInfo.SetIsDefaultValuePresent(true);
		m_NameInfo.SetDefaultValue( static_cast< LPCTSTR >( strDefaultValue ));

		m_IsArray = true;
		m_ArrayIndex = lArrayIndex;
	}
	else
	{
		m_Guid = SVInvalidGUID;
		m_NameInfo.clear();
	}
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( T* pObject, long lArrayIndex, CString strDefaultValue )
{
	m_pObject = pObject;	// no dynamic_cast
	if ( m_pObject )
	{
		m_Guid = m_pObject->GetUniqueObjectID();
		if( nullptr != m_pObject )
		{
			m_NameInfo.ParseObjectName( static_cast< LPCTSTR >( m_pObject->GetCompleteObjectName() ) );
		}
		m_NameInfo.SetIsIndexPresent(true);
		m_NameInfo.m_Index = _variant_t( lArrayIndex );
		m_NameInfo.m_DefaultValuePresent = true;
		m_NameInfo.m_DefaultValue = static_cast< LPCTSTR >( strDefaultValue );
		m_IsArray = true;
		m_ArrayIndex = lArrayIndex;
	}
	else
	{
		m_Guid = SVInvalidGUID;
		m_NameInfo.clear();
	}
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo )
{
	m_pObject = dynamic_cast <T*> (pObject);
	if ( m_pObject )
	{
		m_Guid = m_pObject->GetUniqueObjectID();
		m_NameInfo = p_rNameInfo;
		m_IsArray = p_rNameInfo.IsIndexPresent();
		m_ArrayIndex = p_rNameInfo.GetIndexValue();
	}
	else
	{
		m_Guid = SVInvalidGUID;
		m_NameInfo.clear();
		m_IsArray = false;
		m_ArrayIndex = -1;
	}
}

template <class T>
inline SVCheckedObjectReference<T>::SVCheckedObjectReference( T* pObject, const SVObjectNameInfo& p_rNameInfo )
{
	m_pObject = pObject;	// no dynamic_cast
	if ( m_pObject )
	{
		m_Guid = m_pObject->GetUniqueObjectID();
		m_NameInfo = p_rNameInfo;
		m_IsArray = p_rNameInfo.IsIndexPresent();
		m_ArrayIndex = p_rNameInfo.GetIndexValue();
	}
	else
	{
		m_Guid = SVInvalidGUID;
		m_NameInfo.clear();
		m_IsArray = false;
		m_ArrayIndex = -1;
	}
}

template <class T>
inline const SVCheckedObjectReference<T>& SVCheckedObjectReference<T>::operator = ( const SVObjectReference& rhs )
{
	SVObjectReference::operator = ( rhs );
	m_pObject = const_cast<T*> (dynamic_cast <const T*> (rhs.Object()));
	return *this;
}

template <class T>
inline const SVCheckedObjectReference<T>& SVCheckedObjectReference<T>::operator = ( const SVCheckedObjectReference<T>& rhs )
{
	SVObjectReference::operator = ( rhs );
	return *this;
}

template <class T>
inline T* SVCheckedObjectReference<T>::operator -> ()
{
	return static_cast <T*> ( m_pObject );
}

template <class T>
inline const T* SVCheckedObjectReference<T>::operator -> () const
{
	return static_cast <const T*> ( m_pObject );
}

template <class T>
inline T* SVCheckedObjectReference<T>::Object()
{
	return static_cast <T*> ( m_pObject );
}

template <class T>
inline const T* SVCheckedObjectReference<T>::Object() const
{
	return static_cast <const T*> ( m_pObject );
}

