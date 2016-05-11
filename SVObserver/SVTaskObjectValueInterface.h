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
#include "SVValueObjectReference.h"
#include "SVTaskObjectInterfaceInputRequestStruct.h"
#pragma endregion Includes

class SVTaskObjectClass;
class SVToolClass;

class SVTaskObjectValueInterface
{
public:
	SVTaskObjectValueInterface();
	SVTaskObjectValueInterface( SVTaskObjectClass *p_psvTaskObject );
	virtual ~SVTaskObjectValueInterface();

	void SetTaskObject( SVTaskObjectClass *p_psvTaskObject );

	virtual HRESULT GetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, const _variant_t& p_rVariantValue );

	virtual HRESULT GetValue( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, VARIANT& p_rValue ) const;

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

	virtual HRESULT AddInputRequest( SVInputRequestStructMap p_map );
	virtual HRESULT AddInputRequest( SVValueObjectReference p_svObjectRef, LPCTSTR p_szValue );
	virtual HRESULT AddInputRequest( SVValueObjectReference p_svObjectRef, double p_dValue );
	virtual HRESULT AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, LPCTSTR p_szValue );
	virtual HRESULT AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, double p_dValue );
	virtual HRESULT AddInputRequestMarker();
	virtual HRESULT RunOnce( SVToolClass *p_psvTool = nullptr );

private:
	SVTaskObjectClass *m_psvTaskObject;
};

