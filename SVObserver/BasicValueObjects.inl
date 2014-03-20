//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:22  $
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
BasicValueObject& BasicValueObjects::setValueObject(LPCSTR Name, ELEMENT_TYPE Value, SVObjectClass* pOwner)
{
	BasicValueObject* pValue = getValueObject(Name);
	if(nullptr == pValue)
	{
		pValue = new BasicValueObject(Name);
		pValue->setValue(Value);
		if(nullptr != pOwner)
		{
			pValue->SetObjectOwner(pOwner);
		}
		m_Values.push_back(pValue);
	}
	else
	{
		pValue->setValue(Value);
	}
	return *pValue;
}

inline const BasicValueObjects::ValueList& BasicValueObjects::getValueList() const
{
	return m_Values;
}

#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObjects.inl_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
