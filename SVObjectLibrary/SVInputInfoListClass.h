//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputInfoListClass
//* .File Name       : $Workfile:   SVInputInfoListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:34  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVInObjectInfoStruct.h"
#pragma endregion Includes

namespace SvOl
{

class SVInputInfoListClass
{
public:
	typedef SVInObjectInfoStructPtrVector::value_type value_type;
	typedef SVInObjectInfoStructPtrVector::iterator iterator;
	typedef SVInObjectInfoStructPtrVector::const_iterator const_iterator;
	
	// Standard constructors
	SVInputInfoListClass();

	const SVInputInfoListClass& operator=(const SVInputInfoListClass& rRhs);

// Standard destructor
	virtual ~SVInputInfoListClass();

	iterator begin() { return m_InObjectInfoVector.begin(); };
	iterator end() { return m_InObjectInfoVector.end(); };

	const_iterator begin() const { return m_InObjectInfoVector.begin(); };
	const_iterator end() const { return m_InObjectInfoVector.end(); };

	iterator erase(iterator pWhere) { return m_InObjectInfoVector.erase(pWhere); };

	const SVInObjectInfoStruct* operator[](size_t Index) const { return m_InObjectInfoVector[Index]; };
	SVInObjectInfoStruct* operator[](size_t Index) { return m_InObjectInfoVector[Index]; };

	size_t size() const { return m_InObjectInfoVector.size(); };
	void clear() { m_InObjectInfoVector.clear(); };

	bool CheckExistence( int Index = -1 );

	int push_back( SVInObjectInfoStruct* );
	int remove(SVInObjectInfoStruct*);

protected:
	const SVInObjectInfoStructPtrVector& getInObjectInfoVector() const { return m_InObjectInfoVector; };
	bool checkDuplicates( SVInObjectInfoStruct* PInObjectInfo );

	SVInObjectInfoStructPtrVector m_InObjectInfoVector;
};
} //namespace SvOl
