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

#ifndef SVOUTPUTINFOLISTCLASS_H
#define SVOUTPUTINFOLISTCLASS_H

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

	virtual int GetSize() const;

	virtual SVOutObjectInfoStruct* GetAt( int p_iIndex );

	virtual int  Add( SVOutObjectInfoStruct* );
	virtual void InsertAt( int nIndex, SVOutObjectInfoStruct* POutObjectInfo );
	virtual SVOutObjectInfoStruct*& ElementAt( int p_iIndex );

	virtual void RemoveAt( int nIndex );
	virtual void RemoveAll();

	// for those that don't care about arrays (e.g. Publish List):
	virtual void GetSetAttributesList( UINT uAttributeMask, SVOutputInfoListClass* pFillList );
	virtual void GetAllowedAttributesList( UINT uAttributeMask, SVOutputInfoListClass* pFillList );	// same for all elements of an array; array version unnecessary

	// for those that do care about arrays (e.g. Results List):
	virtual void GetSetAttributesList( UINT uAttributeMask, SVObjectReferenceVector&  rvecObjects );
	virtual void GetObjectReferenceList( SVObjectReferenceVector&  rvecObjects );

	virtual bool HasDependents();

	virtual HRESULT ValidateAll();

protected:
	SVOutObjectInfoStructPtrArray m_svObjectArray;
	SVStdSetSVOutObjectInfoStructPtr m_svObjectSet;
};

#endif

