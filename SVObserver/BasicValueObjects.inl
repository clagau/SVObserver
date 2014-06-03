//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Jun 2014 08:55:52  $
//******************************************************************************

#pragma region Public Methods
template <typename ELEMENT_TYPE>
HRESULT BasicValueObjects::getValue( LPCSTR Name, ELEMENT_TYPE& rValue ) const
{
	HRESULT Result = S_FALSE;

	BasicValueObject* pValue = getValueObject( Name );
	if(nullptr != pValue)
	{
		Result = pValue->getValue( rValue );
	}

	return Result;
}

template <typename ELEMENT_TYPE>
BasicValueObject* BasicValueObjects::setValueObject( LPCSTR Name, const ELEMENT_TYPE Value, SVObjectClass* pOwner )
{
	BasicValueObject* pValue = nullptr;

	pValue = getValueObject( Name );
	if(nullptr == pValue)
	{
		SVObjectNameInfo NameInfo;
		SVObjectClass* pParent = pOwner;

		HRESULT Result = SVObjectNameInfo::ParseObjectName( NameInfo, Name );

		if( S_OK == Result )
		{
			for( size_t i=0; i < NameInfo.m_NameArray.size(); i++ )
			{
				pValue = getValueObject( NameInfo.m_NameArray[i].c_str() );
				if( nullptr == pValue )
				{
					//Check if last name entry
					if( NameInfo.m_NameArray.size() == i + 1 )
					{
						pValue = new BasicValueObject( NameInfo.m_NameArray[i].c_str(), pParent );
						pValue->setValue(Value);
					}
					else
					{
						//Generate node and node becomes the new parent object
						pValue = new BasicValueObject( NameInfo.m_NameArray[i].c_str(), pParent, true );
						pParent = dynamic_cast<SVObjectClass*> (pValue);
					}
					m_Values.push_back( pValue );
				}
				else if( pValue->isNode() )
				{
					pParent = dynamic_cast<SVObjectClass*> ( pValue );
				}
			}
		}
	}
	else
	{
		pValue->setValue( Value );
	}

	return pValue;
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
 *    Rev 1.1   02 Jun 2014 08:55:52   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added method getValue.
 * Changed return type, parameter type for setValueObject.
 * setValueObject generates required nodes automatically.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
