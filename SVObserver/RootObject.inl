//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This class is used to define the root node for the class object manager
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
static HRESULT RootObject::getRootChildValue( LPCSTR DottedName, ELEMENT_TYPE& rValue )
{
	HRESULT Result( S_FALSE );
	RootObject *pRoot( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if( nullptr != pRoot )
	{
		Result = pRoot->m_RootChildren.getValue( DottedName, rValue );
	}

	return Result;
}

template <typename ELEMENT_TYPE>
static SvVol::BasicValueObjectPtr RootObject::setRootChildValue( LPCSTR DottedName, const ELEMENT_TYPE& rValue )
{
	RootObject *pRoot( nullptr );
	SvVol::BasicValueObjectPtr pValue( nullptr );

	SVObjectManagerClass::Instance().GetRootChildObject( pRoot, SvDef::FqnRoot );

	if( nullptr != pRoot )
	{
		pValue = pRoot->m_RootChildren.setValue( DottedName, rValue );
	}

	return pValue;
}
#pragma endregion Public Methods