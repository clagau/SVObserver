//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataHolder
//* .File Name       : $Workfile:   SVCommandDataHolder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:32  $
//******************************************************************************

#ifndef SVCOMMANDDATAHOLDER_H
#define SVCOMMANDDATAHOLDER_H

#include <map>
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVCommandDataFacade.h"
#include "SVCommandAttributesFacade.h"
#include "SVCommandResultsFacade.h"

/*
*/
class SVCommandDataHolder
{
public:
	typedef std::map< SVString, unsigned long > SVNameDataTypeMap;
	typedef std::map< SVString, SVCommandDataFacadePtr > SVNameDataMap;
	typedef SVCommandAttributesFacade< SVCommandDataHolder > SVCommandAttributes;
	typedef SVCommandResultsFacade< SVCommandDataHolder > SVCommandResults;

	SVCommandDataHolder();
	SVCommandDataHolder( const SVNameDataMap& p_rData );
	SVCommandDataHolder( const SVCommandDataHolder& p_rObject );

	virtual ~SVCommandDataHolder();

	const SVCommandDataHolder& operator=( const SVCommandDataHolder& p_rObject );

	void clear();

	SVNameDataTypeMap GetDataInformation() const;

	unsigned long GetDataType( const _bstr_t& p_rName ) const;

	HRESULT GetData( const _bstr_t& p_rName, SVCommandDataFacadePtr& p_rData ) const;
	HRESULT GetContainer( const _bstr_t& p_rName, SVCommandDataHolder& p_rData ) const;
	HRESULT GetValue( const _bstr_t& p_rName, VARIANT& p_rData ) const;
	HRESULT GetBlock( const _bstr_t& p_rName, SVByteVector& p_rData ) const;
	HRESULT GetImage( const _bstr_t& p_rName, SVByteVector& p_rData ) const;

	HRESULT SetData( const _bstr_t& p_rName, SVCommandDataFacadePtr p_Data );
	HRESULT SetContainer( const _bstr_t& p_rName, const SVCommandDataHolder& p_rData, bool p_ReadOnly = false );
	HRESULT SetValue( const _bstr_t& p_rName, const _variant_t& p_rData, bool p_ReadOnly = false );
	HRESULT SetBlock( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly = false );
	HRESULT SetImage( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly = false );

protected:
	SVNameDataMap m_Data;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataHolder.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:30:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
