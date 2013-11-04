//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAFEARRAY
//* .File Name       : $Workfile:   SVSAFEARRAY.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:24:12  $
//******************************************************************************

#ifndef SVSAFEARRAY_H
#define SVSAFEARRAY_H

#include <comdef.h>
#include <vector>

/**
@SVObjectName Safe Array

@SVObjectOverview

@SVObjectOperations

*/
class SVSAFEARRAY
{
public:
	typedef std::vector< long > SVIndex;
	typedef std::vector< SAFEARRAYBOUND > SVBounds;
	typedef size_t size_type;

	SVSAFEARRAY();
	SVSAFEARRAY( VARTYPE p_Type, size_type p_Size );
	SVSAFEARRAY( VARTYPE p_Type, const SVBounds& p_rBounds );
	SVSAFEARRAY( const SVSAFEARRAY& p_rObject );
	SVSAFEARRAY( const VARIANT& p_rVariant );
	SVSAFEARRAY( SAFEARRAY* p_pObject );

	virtual ~SVSAFEARRAY();

	bool empty() const;

	void clear();

	operator SAFEARRAY * ();

	operator _variant_t () const;
	
	const SVSAFEARRAY& operator=( const SVSAFEARRAY& p_rObject );
	const SVSAFEARRAY& operator=( const VARIANT& p_rVariant );
	const SVSAFEARRAY& operator=( SAFEARRAY* p_pObject );

	template< class InputIterator	>
  void assign( InputIterator _First, InputIterator _Last );

	HRESULT GetVartype( VARTYPE& p_rType ) const;
	HRESULT GetBounds( SVBounds& p_rBounds ) const;

	size_type size() const;

	unsigned int GetDim() const;

	HRESULT GetElement( size_type p_Index, _variant_t& p_rData ) const;
	HRESULT GetElement( const SVIndex& p_rIndex, _variant_t& p_rData ) const;

	HRESULT PutElement( size_type p_Index, const _variant_t& p_rData );
	HRESULT PutElement( const SVIndex& p_rIndex, const _variant_t& p_rData );

	HRESULT Add( const _variant_t& p_rData );

protected:
	HRESULT UpdateVariant( VARTYPE p_Type, const void* p_pData, _variant_t &p_rVariant ) const;
	HRESULT GetGeneralVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;
	HRESULT GetElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;
	HRESULT PutElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const;

	SAFEARRAY* m_pSafeArray;

private:
	void LocalDestroy();

};

#include "SVSAFEARRAY.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVSAFEARRAY.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:24:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2013 12:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Safe Array clas to fix issues when getting and putting strings into and out of Safe Array.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Feb 2013 12:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to get the bounds of the safe array.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

