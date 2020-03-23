//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputInfoListClass
//* .File Name       : $Workfile:   SVOutputInfoListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:20:56  $
//******************************************************************************

#pragma once

#pragma warning( disable : 4786 )

#pragma region Includes
#include "SVOutObjectInfoStruct.h"
#pragma endregion Includes

class SVOutputInfoListClass
{
public:
	typedef SVOutObjectInfoStructPtrVector::value_type value_type;
	typedef SVOutObjectInfoStructPtrVector::iterator iterator;
	typedef SVOutObjectInfoStructPtrVector::const_iterator const_iterator;

	SVOutputInfoListClass();
	virtual ~SVOutputInfoListClass();

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	iterator erase( iterator p_Where );

	bool CheckExistence( int Index = -1 );

	int GetSize() const;

	SVOutObjectInfoStruct* GetAt( int p_iIndex );

	int  Add( SVOutObjectInfoStruct* );

	void RemoveAt( int nIndex );
	void RemoveAll();

	// for those that don't care about arrays (e.g. Publish List):
	void GetSetAttributesList( UINT uAttributeMask, SVOutputInfoListClass* pFillList );

	// for those that do care about arrays (e.g. Results List):
	void GetSetAttributesList( UINT uAttributeMask, SVObjectReferenceVector&  rvecObjects );

protected:
	SVOutObjectInfoStructPtrVector m_svObjectArray;
	SVStdSetSVOutObjectInfoStructPtr m_svObjectSet;
};

