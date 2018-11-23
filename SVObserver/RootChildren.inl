//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootChildren
//* .File Name       : $Workfile:   RootChildren.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Jun 2014 08:47:08  $
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
HRESULT RootChildren::getValue( LPCSTR DottedName, ELEMENT_TYPE& rValue ) const
{
	return m_RootChildrenValues.getValue( DottedName, rValue );
}

template <typename ELEMENT_TYPE>
BasicValueObjectPtr RootChildren::setValue( LPCSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner, SvPb::SVObjectSubTypeEnum ObjectType )
{
	return m_RootChildrenValues.setValueObject( DottedName, Value, pOwner, ObjectType );
}

#pragma endregion Public Methods

