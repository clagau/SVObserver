//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	explicit SVCommandDataHolder(const SVNameDataMap& rData);
	SVCommandDataHolder( const SVCommandDataHolder& rObject );

	virtual ~SVCommandDataHolder();

	const SVCommandDataHolder& operator=( const SVCommandDataHolder& rObject );

	void clear();

	SVNameDataTypeMap GetDataInformation() const;

	unsigned long GetDataType( const _bstr_t& rName ) const;

	HRESULT GetData( const _bstr_t& rName, SVCommandDataFacadePtr& rData ) const;
	HRESULT GetContainer( const _bstr_t& rName, SVCommandDataHolder& rData ) const;
	HRESULT GetValue( const _bstr_t& rName, VARIANT& rData ) const;
	HRESULT GetBlock( const _bstr_t& rName, std::vector<unsigned char>& rData ) const;
	HRESULT GetImage( const _bstr_t& rName, std::vector<unsigned char>& rData ) const;

	HRESULT SetData( _bstr_t nameBstr, const SVCommandDataFacadePtr& rDataPointer);
	HRESULT SetContainer( const _bstr_t& rName, const SVCommandDataHolder& rData, bool readOnly = false );
	HRESULT SetValue( const _bstr_t& rName, const _variant_t& rData, bool ReadOnly = false );
	HRESULT SetBlock( const _bstr_t& rName, const std::vector<unsigned char>& rData, bool readOnly = false );
	HRESULT SetImage( const _bstr_t& rName, const std::vector<unsigned char>& rData, bool readOnly = false );

protected:
	SVNameDataMap m_Data;

};


