//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Jun 2014 08:47:08  $
//******************************************************************************

namespace SvVol
{

#pragma region Public Methods
template <typename ELEMENT_TYPE>
HRESULT BasicValueObject::setValue(const ELEMENT_TYPE Value )
{
	HRESULT Result = S_FALSE;

	_variant_t VariantValue;
	VariantValue = Value;
	Result = setValue( VariantValue );

	return Result;
}

//There are also specializations of this method
template <typename ELEMENT_TYPE>
HRESULT BasicValueObject::getValue( ELEMENT_TYPE& rValue ) const
{
	HRESULT Result = S_OK;

	RefreshOwner( SVObjectClass::PreRefresh );

	boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
	rValue = 0;
	switch(m_Value.vt)
	{
	case VT_BOOL:
		rValue = static_cast<ELEMENT_TYPE> (m_Value.boolVal ? true : false);
		break;
	case VT_INT:
	case VT_I4:
	case VT_I8:
	case VT_R4:
	case VT_R8:
		rValue = static_cast<ELEMENT_TYPE> (m_Value);
		break;

	case VT_BSTR:
	default:
		Result = S_FALSE;
		break;
	}
	return Result;
}

inline bool BasicValueObject::isNode() const
{
	return m_Node;
}

inline void BasicValueObject::setDescription( LPCTSTR Description )
{
	m_Description = (nullptr != Description) ? Description : std::string();
}

inline LPCTSTR BasicValueObject::getDescription() const
{
	return m_Description.c_str();
}
#pragma endregion Public Methods

} //namespace SvVol
