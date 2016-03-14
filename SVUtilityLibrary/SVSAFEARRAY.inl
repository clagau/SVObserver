//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAFEARRAY
//* .File Name       : $Workfile:   SVSAFEARRAY.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:24:28  $
//******************************************************************************

template< class InputIterator >
void SVSAFEARRAY::assign( InputIterator _First, InputIterator _Last )
{
	InputIterator l_Iter( _First );

	clear();

	while( l_Iter != _Last )
	{
		Add( *l_Iter );

		++l_Iter;
	}
}

