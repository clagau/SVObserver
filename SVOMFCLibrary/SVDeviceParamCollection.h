// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
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

#include "SVLibrary/SVVisitor.h"
#include "SVTemplate.h"
#include "SVDeviceParam.h"
#include "SVUtilityLibrary/SVString.h"

typedef std::map <SVDeviceParamEnum, SVDeviceParamWrapper> SVDeviceParamMap;

class SVDeviceParamCollection
{
public:
	SVDeviceParamCollection() {};
	SVDeviceParamCollection( const SVDeviceParamCollection& rhs );
	virtual ~SVDeviceParamCollection() {}	// this class is inheritable to provide access to runtime device-specific information
	HRESULT Clear();
	SVDeviceParamCollection& operator = ( const SVDeviceParamCollection& rhs );

	HRESULT CreateParameter( SVDeviceParamEnum e, const VARIANT& rv );

	template <typename DERIVED> HRESULT SetParameter( SVDeviceParamEnum e, const DERIVED& rParam )
	{
		mapParameters[e] = rParam;
		mapParameters[e]->SetType( e );
		return S_OK;
	}

	HRESULT SetParameter( const SVDeviceParamWrapper& rw )
	{
		if ( ((const SVDeviceParam*) rw ) != NULL )
		{
			mapParameters[rw->Type()] = rw;
			return S_OK;
		}
		return S_FALSE;
	}

	bool ParameterExists( SVDeviceParamEnum e ) const
	{
		return (mapParameters.find(e) != mapParameters.end()) && mapParameters.find(e)->second != NULL;
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
	bool ParameterExists( const SVString& rVisualName ) const
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
	const SVDeviceParamWrapper& Parameter( const SVString& rVisualName ) const
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
	SVDeviceParamWrapper& GetParameter( const SVString& rVisualName )
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
	template <typename DERIVED> HRESULT GetParameter( const SVString& rVisualName, DERIVED*& rpParam )
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
			return rpParam != NULL ? S_OK : TYPE_E_TYPEMISMATCH;
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

typedef TDoubleFactorySingleton<SVDeviceParamEnum, CString, SVDeviceParam>::factory TheDeviceParamFactory;

class SVDeviceParamIndexer : public std::vector<SVDeviceParamEnum>
{
public:
	SVDeviceParamIndexer(const SVDeviceParamMap& map);

private:
	class keycomp : public std::binary_function<SVDeviceParamEnum, SVDeviceParamEnum, bool>
	{
	public:
		keycomp(const SVDeviceParamMap& map) : m_mapIndexed(map) {}
		bool operator () (const SVDeviceParamEnum& a, const SVDeviceParamEnum& b) const;

	private:
		const SVDeviceParamMap& m_mapIndexed;
	};
	const SVDeviceParamMap& m_mapIndexed;
};
