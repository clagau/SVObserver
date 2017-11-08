//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectValueInterface
//* .File Name       : $Workfile:   SVTaskObjectValueInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Dec 2014 15:17:52  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTaskObjectInterfaceInputRequestStruct.h"

#pragma endregion Includes

class SVTaskObjectClass;

class SVTaskObjectValueInterface
{
public:
	SVTaskObjectValueInterface();
	SVTaskObjectValueInterface( SVTaskObjectClass* pTaskObject );
	virtual ~SVTaskObjectValueInterface();

	void SetTaskObject( SVTaskObjectClass* pTaskObject );

	virtual HRESULT GetObjectValue( const SVGUID& rObjectId, const std::string& rValueName, _variant_t& rValue ) const;

	virtual HRESULT GetValue( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, _variant_t& rValue ) const;

	template <class InIterator>
	HRESULT AddInputRequest(SVObjectReference ObjectRef, InIterator begin, InIterator end)
	{
		HRESULT Result( S_OK );

		ObjectRef->SetArraySize( static_cast<int> ( std::distance( begin, end ) ) );

		long Index = 0;
		InIterator Iter( begin );
		while( end != Iter && S_OK == Result )
		{
			ObjectRef.SetArrayIndex( Index );
			Result = AddInputRequest( ObjectRef, *Iter );
			++Index;
			++Iter;
		}

		return Result;
	}

	virtual HRESULT AddInputRequest( const SVInputRequestStructMap& rMap );
	virtual HRESULT AddInputRequest( const SVObjectReference& rObjectRef, const _variant_t& rValue );
	virtual HRESULT AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, const _variant_t& rValue );
	virtual HRESULT AddInputRequestMarker();
	virtual HRESULT RunOnce( const SVGUID& rToolId );

private:
	SVTaskObjectClass* m_pTaskObject;
};

