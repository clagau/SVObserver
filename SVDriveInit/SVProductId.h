//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductId
//* .File Name       : $Workfile:   SVProductId.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:32  $
//******************************************************************************

#pragma once

class SVProductId
{
public:
	SVProductId();

	bool UpdateProductId() const;

	LPCTSTR GetSubElement( size_t p_Index ) const;
	HRESULT SetSubElement( size_t p_Index, LPCTSTR p_szData );

private:
	SVProductId( const SVProductId& p_rObject );

	const SVProductId& operator=( const SVProductId& p_rObject );

	bool PrepareForReseal() const;

	bool m_ResealWindows;
	CString m_PreviousProductId;
	TCHAR m_szProductId[ 5 ][ 6 ];


};

