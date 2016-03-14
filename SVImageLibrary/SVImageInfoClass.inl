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
	HRESULT l_Status = S_OK;
	SVObjectClass* l_pObject = NULL;

	l_Status = GetOwnerImage( l_pObject );

	if( l_pObject != NULL )
	{
		p_rpObject = dynamic_cast< SV_OBJECT_TYPE* >( l_pObject );
	}
	else
	{
		p_rpObject = NULL;
	}

	if( p_rpObject == NULL )
	{
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

