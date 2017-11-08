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
	m_pValueObject = nullptr;
	m_Guid = SV_GUID_NULL;
	m_NameInfo.clear();
	m_ArrayIndex = -1;
}

inline SVObjectReference::SVObjectReference( const SVObjectReference& rhs )
{
	*this = rhs;
}

inline bool SVObjectReference::operator == ( const SVObjectReference& rhs ) const
{
	bool res = (m_pObject == rhs.m_pObject) && (m_ArrayIndex == rhs.m_ArrayIndex);
	return res;
}

inline SVObjectClass* SVObjectReference::getObject() const
{
	return m_pObject;
}

inline long SVObjectReference::ArrayIndex() const
{
	long Index = -1;

	if( isArray()  && m_ArrayIndex > -1)
	{
		Index = m_ArrayIndex ;
	}
	
	return Index;
}

inline long SVObjectReference::getValidArrayIndex() const
{
	return ( -1 != ArrayIndex() ) ? ArrayIndex() : 0;
}

inline std::string SVObjectReference::DefaultValue() const
{
	return m_NameInfo.GetDefaultValue();
}

inline bool SVObjectReference::isArray() const
{
	const SvOi::IValueObject* pValueObject = getValueObject();
	return nullptr != pValueObject ? pValueObject->isArray() : false;
}

inline bool SVObjectReference::isEntireArray() const
{
	return isArray() && m_ArrayIndex == -1 && m_NameInfo.IsIndexPresent();

}

inline GUID SVObjectReference::Guid() const
{
	return m_Guid;
}

inline bool SVObjectReference::operator < ( const SVObjectReference& rhs ) const
{
	return (m_pObject < rhs.m_pObject) || ((m_pObject == rhs.m_pObject) && (m_ArrayIndex < rhs.m_ArrayIndex));
}


