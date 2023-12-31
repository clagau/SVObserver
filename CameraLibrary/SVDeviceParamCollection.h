// ******************************************************************************
// * COPYRIGHT (c) 2002 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParamCollection
// * .File Name       : $Workfile:   SVDeviceParamCollection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   15 May 2013 19:42:26  $
// ******************************************************************************

#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>

#include "SVLibrary/SVTemplate.h"
#include "SVDeviceParam.h"


class SVDeviceParamCollection
{
public:
	SVDeviceParamCollection() {};
	SVDeviceParamCollection( const SVDeviceParamCollection& rhs );
	virtual ~SVDeviceParamCollection() {}	// this class is inheritable to provide access to runtime device-specific information
	HRESULT Clear();
	SVDeviceParamCollection& operator = ( const SVDeviceParamCollection& rhs );

	HRESULT SetParameterDefaults();

	HRESULT CreateParameter( SVDeviceParamEnum e, const VARIANT& rv );

	template <typename DERIVED> HRESULT SetParameter( SVDeviceParamEnum e, const DERIVED& rParam )
	{
		mapParameters[e] = rParam;
		mapParameters[e]->SetType( e );
		return S_OK;
	}

	HRESULT SetParameter( const SVDeviceParamWrapper& rw )
	{
		if ( nullptr != static_cast<const SVDeviceParam*> (rw))
		{
			mapParameters[rw->Type()] = rw;
			return S_OK;
		}
		return S_FALSE;
	}

	bool ParameterExists( SVDeviceParamEnum e ) const
	{
		return (mapParameters.find(e) != mapParameters.end()) && nullptr != mapParameters.find(e)->second;
	}

	const SVDeviceParamWrapper& Parameter( SVDeviceParamEnum e ) const
	{
		static SVDeviceParamWrapper dummy;
		SVDeviceParamMap::const_iterator iter = mapParameters.find(e);
		if ( iter == mapParameters.end() )
			return dummy;

		return iter->second;
	}

	SVDeviceParamWrapper& GetParameter( SVDeviceParamEnum e )
	{
		return mapParameters[e];
	}

	template <typename DERIVED> HRESULT GetParameter( SVDeviceParamEnum e, DERIVED*& rpParam )
	{
		if ( mapParameters.find(e) != mapParameters.end() )
		{
			return mapParameters[e].GetDerivedValue( rpParam );
		}
		return ERROR_NOT_FOUND;
	}

	// For custom parameters - uses visual name
	bool ParameterExists( const std::string& rVisualName ) const
	{
		bool bRetVal = false;

		for (SVDeviceParamMap::const_iterator it = mapParameters.begin(); !bRetVal && it != mapParameters.end();++it)
		{
			if (rVisualName == it->second->VisualName())
			{
				bRetVal = true;
			}
		}
		return bRetVal;
	}

	// For custom parameters - uses visual name
	const SVDeviceParamWrapper& Parameter( const std::string& rVisualName ) const
	{
		static SVDeviceParamWrapper dummy;
		for (SVDeviceParamMap::const_iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second;
		}
		return dummy;
	}

	// For custom parameters - uses visual name
	SVDeviceParamWrapper& GetParameter( const std::string& rVisualName )
	{
		static SVDeviceParamWrapper dummy;
		for (SVDeviceParamMap::iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second;
		}
		return dummy;
	}

	// For custom parameters - uses visual name
	template <typename DERIVED> HRESULT GetParameter( const std::string& rVisualName, DERIVED*& rpParam )
	{
		for (SVDeviceParamMap::iterator iter = mapParameters.begin(); iter != mapParameters.end();++iter)
		{
			if (rVisualName == iter->second->VisualName())
				return iter->second.GetDerivedValue( rpParam );
		}
		return ERROR_NOT_FOUND;
	}

	/* const DERIVED*& doesn't seems to want to work correctly, so we can't implement this function.
	template <typename DERIVED> HRESULT GetParameter( SVDeviceParamEnum e, const DERIVED*& rpParam ) const
	{
		SVDeviceParamMap::const_iterator iter = mapParameters.find(e);
		if ( iter != mapParameters.end() )
		{
			rpParam = iter->second.DerivedValue( rpParam );
			return nullptr != rpParam ? S_OK : TYPE_E_TYPEMISMATCH;
		}
		else
		{
			return ERROR_NOT_FOUND;
		}
	}
	*/

public:	// needed for iterators
	SVDeviceParamMap mapParameters;
};

typedef TDoubleFactory<SVDeviceParamEnum, std::string, SVDeviceParam> TheDeviceParamFactory;

class SVDeviceParamIndexer : public std::vector<SVDeviceParamEnum>
{
public:
	explicit SVDeviceParamIndexer(const SVDeviceParamMap& map);

private:
	struct keycomp
	{
	public:
		explicit keycomp(const SVDeviceParamMap& map) : m_mapIndexed(map) {}
		bool operator() (const SVDeviceParamEnum& rLhs, const SVDeviceParamEnum& rRhs) const;

	private:
		const SVDeviceParamMap& m_mapIndexed;
	};
	const SVDeviceParamMap& m_mapIndexed;
};
