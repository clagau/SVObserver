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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVCommandDataFacade.h"
#include "SVCommandAttributesFacade.h"
#include "SVCommandResultsFacade.h"
#pragma endregion Includes

class SVCommandDataHolder
{
public:
	typedef std::map<std::string, unsigned long> SVNameDataTypeMap;
	typedef std::map<std::string, SVCommandDataFacadePtr> SVNameDataMap;
	typedef SVCommandAttributesFacade<SVCommandDataHolder> SVCommandAttributes;
	typedef SVCommandResultsFacade<SVCommandDataHolder> SVCommandResults;

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

	HRESULT SetData( _bstr_t nameBstr, SVCommandDataFacadePtr& rDataPointer);
	HRESULT SetContainer( const _bstr_t& p_rName, const SVCommandDataHolder& p_rData, bool p_ReadOnly = false );
	HRESULT SetValue( const _bstr_t& p_rName, const _variant_t& p_rData, bool p_ReadOnly = false );
	HRESULT SetBlock( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly = false );
	HRESULT SetImage( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly = false );

protected:
	SVNameDataMap m_Data;

};


