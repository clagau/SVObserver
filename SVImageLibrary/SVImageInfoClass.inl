//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:48  $
//******************************************************************************

template< typename SV_OBJECT_TYPE >
HRESULT SVImageInfoClass::GetOwnerImage( SV_OBJECT_TYPE*& p_rpObject ) const
{
	SVObjectClass* l_pObject = nullptr;
	HRESULT l_Status = GetOwnerImage( l_pObject );

	if( nullptr != l_pObject )
	{
		p_rpObject = dynamic_cast<SV_OBJECT_TYPE*> ( l_pObject );
	}
	else
	{
		p_rpObject = nullptr;
	}

	if( nullptr == p_rpObject )
	{
		if( S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

