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
BasicValueObjectPtr RootChildren::setValue( LPCSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner, SVObjectTypeEnum ObjectType )
{
	return m_RootChildrenValues.setValueObject( DottedName, Value, pOwner, ObjectType );
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
