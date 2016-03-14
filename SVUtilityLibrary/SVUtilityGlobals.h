//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityGlobals
//* .File Name       : $Workfile:   SVUtilityGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#ifndef SVUTILITYGLOBALS_H
#define SVUTILITYGLOBALS_H

#include <vector>
#include <comdef.h>

typedef unsigned long SVHANDLE;

typedef std::vector< unsigned char > SVByteVector;

extern const GUID SV_GUID_NULL;

#define SV_SEVERITY( p_Status ) \
	( ( ( ( unsigned long ) p_Status ) >> 30 ) & 0x00000003 )

inline bool operator < ( const GUID& guid1, const GUID& guid2 )
{
	RPC_STATUS l_Status;
	return ::UuidCompare( const_cast< GUID* >( &guid1 ), const_cast< GUID* >( &guid2 ), &l_Status ) < 0;
}

struct SVGetNextGUID
{
	static void Next( GUID& p_rId )
	{
		switch( ::UuidCreate( &p_rId ) )
		{
			case RPC_S_OK:
			case RPC_S_UUID_LOCAL_ONLY:
			{
				break;
			}
			default:
			{
				p_rId = SV_GUID_NULL;

				break;
			}
		}
	}
};

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv);

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew );
HRESULT KeepPrevErrorReturnPrev( HRESULT& p_rhrPrev, HRESULT p_hrNew );
HRESULT KeepPrevErrorReturnNew( HRESULT& p_rhrPrev, HRESULT p_hrNew );


#endif // #ifndef SVUTILITYGLOBALS_H

