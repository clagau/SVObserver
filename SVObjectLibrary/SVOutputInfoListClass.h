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
	typedef SVOutObjectInfoStructPtrArray::value_type value_type;
	typedef SVOutObjectInfoStructPtrArray::arg_type arg_type;
	typedef SVOutObjectInfoStructPtrArray::iterator iterator;
	typedef SVOutObjectInfoStructPtrArray::const_iterator const_iterator;

	SVOutputInfoListClass();
	virtual ~SVOutputInfoListClass();

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	iterator erase( iterator p_Where );

	BOOL CheckExistence( int Index = -1 );

	int GetSize() const;

	SVOutObjectInfoStruct* GetAt( int p_iIndex );

	int  Add( SVOutObjectInfoStruct* );
	void InsertAt( int nIndex, SVOutObjectInfoStruct* POutObjectInfo );
	SVOutObjectInfoStruct*& ElementAt( int p_iIndex );

	void RemoveAt( int nIndex );
	void RemoveAll();

	// for those that don't care about arrays (e.g. Publish List):
	void GetSetAttributesList( UINT uAttributeMask, SVOutputInfoListClass* pFillList );
	void GetAllowedAttributesList( UINT uAttributeMask, SVOutputInfoListClass* pFillList );	// same for all elements of an array; array version unnecessary

	// for those that do care about arrays (e.g. Results List):
	void GetSetAttributesList( UINT uAttributeMask, SVObjectReferenceVector&  rvecObjects );
	void GetObjectReferenceList( SVObjectReferenceVector&  rvecObjects );

	bool HasDependents();

protected:
	SVOutObjectInfoStructPtrArray m_svObjectArray;
	SVStdSetSVOutObjectInfoStructPtr m_svObjectSet;
};

