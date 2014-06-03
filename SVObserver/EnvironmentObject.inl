//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   EnvironmentObject.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Jun 2014 08:47:08  $
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
static void EnvironmentObject::getEnvironmentValue( LPCSTR Name, ELEMENT_TYPE& rValue )
{
	EnvironmentObject *pEnvironment = nullptr;

	SVObjectManagerClass::Instance().GetRootChildObject(pEnvironment, SVObjectManagerClass::Environment);

	if(nullptr != pEnvironment)
	{
		pEnvironment->getValue( Name, rValue );
	}
}

template <typename ELEMENT_TYPE>
static BasicValueObject* EnvironmentObject::setEnvironmentValue( LPCSTR Name, const ELEMENT_TYPE& rValue )
{
	EnvironmentObject *pEnvironment = nullptr;
	BasicValueObject *pValue = nullptr;

	SVObjectManagerClass::Instance().GetRootChildObject(pEnvironment, SVObjectManagerClass::Environment);

	if(nullptr != pEnvironment)
	{
		pValue = pEnvironment->setValue( Name, rValue );
	}

	return pValue;
}

template <typename ELEMENT_TYPE>
HRESULT EnvironmentObject::getValue( LPCSTR Name, ELEMENT_TYPE& rValue ) const
{
	return m_EnvironmentValues.getValue( Name, rValue );
}

template <typename ELEMENT_TYPE>
BasicValueObject* EnvironmentObject::setValue( LPCSTR Name, const ELEMENT_TYPE Value )
{
	return m_EnvironmentValues.setValueObject( Name, Value, this );
}

#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.inl_v  $
 * 
 *    Rev 1.0   02 Jun 2014 08:47:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
