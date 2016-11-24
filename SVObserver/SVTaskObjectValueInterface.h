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
#include "SVValueObjectLibrary/SVValueObjectReference.h"
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

	virtual HRESULT GetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;

	virtual HRESULT GetValue( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, VARIANT& rValue ) const;

	template <class InIterator>
	HRESULT AddInputRequest(SVValueObjectReference ObjectRef, InIterator begin, InIterator end)
	{
		HRESULT Result( S_OK );

		ObjectRef->SetArraySize( static_cast< int >( std::distance( begin, end ) ) );

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

	virtual HRESULT AddInputRequest( const SVInputRequestStructMap& map );
	virtual HRESULT AddInputRequest( SVValueObjectReference objectRef, LPCTSTR szValue );
	virtual HRESULT AddInputRequest( SVValueObjectReference objectRef, double dValue );
	virtual HRESULT AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, LPCTSTR szValue );
	virtual HRESULT AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, double dValue );
	virtual HRESULT AddInputRequestMarker();
	virtual HRESULT RunOnce( const SVGUID& rToolId );

private:
	SVTaskObjectClass* m_pTaskObject;
};

