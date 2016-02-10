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
	m_pObject = NULL;
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
		if( m_pObject != NULL )
		{
			m_NameInfo.ParseObjectName( static_cast< LPCTSTR >( m_pObject->GetCompleteObjectName() ) );
		}
		m_NameInfo.SetIsIndexPresent(true);
		m_NameInfo.SetIndex(_variant_t( lArrayIndex ));
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
		if( m_pObject != NULL )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectReference.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:00:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   26 Jul 2012 11:07:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified ArrayIndex() to translate from one based indexes. Also added a default bool parameter to allow a zero based translation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jun 2012 17:25:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jun 2012 17:20:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Aug 2005 14:10:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized construction and copying
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Aug 2005 10:19:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added operator <
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Aug 2005 10:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed member variable initialization bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Aug 2005 15:56:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected SVCheckedObjectReference( SVObjectNameParseInfoStruct& rhs ) to include Guid info
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Aug 2005 15:56:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected SVCheckedObjectReference( SVObjectNameParseInfoStruct& rhs ) to include Guid info
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jul 2005 15:47:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Guid information so we can store a reference to an object that doesn't exist yet (only store the guid)
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   14 Jul 2005 11:46:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operator ==
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Jul 2005 13:21:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added parsing for entire array syntax []
 * added constructor taking SVObjectNameParseInfoStruct
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Jul 2005 07:42:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed YAME (yet another merge error)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Jul 2005 07:31:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge errors (added missing functions)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jun 2005 15:11:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jun 2005 15:10:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jun 2005 09:29:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
