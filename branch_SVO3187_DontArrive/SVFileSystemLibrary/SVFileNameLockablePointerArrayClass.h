//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableFileNameClass
//* .File Name       : $Workfile:   SVFileNameLockablePointerArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:37:04  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>
#include "SVSystemLibrary/SVLockableClass.h"
#include "Definitions/StringTypeDef.h"

class SVFileNameClass;

//This class maintains a list of pointers to SVFileNameClass objects.  
//This class also maintains a mechanism for allowing shared 
//access using a lock.
class SVFileNameLockablePointerArrayClass
{
public:
	typedef std::vector<SVFileNameClass*> SVFileNamePtrVector;
	typedef SVFileNamePtrVector::iterator iterator;
	typedef SVFileNamePtrVector::const_iterator const_iterator;

	//This operator is the default constructor for this 
	//class.  This method sets all attributes to their 
	//default values.
	SVFileNameLockablePointerArrayClass() = default;

	//This operator is the copy constructor.  This method 
	//will take all of the attributes from the supplies 
	//object and make a copy of them within the new object.
	SVFileNameLockablePointerArrayClass(const SVFileNameLockablePointerArrayClass& orig) = default;

	virtual ~SVFileNameLockablePointerArrayClass() = default;

	iterator begin() { return m_FileNamePtrVector.begin(); };
	iterator end() { return m_FileNamePtrVector.end(); };

	const_iterator begin() const { return m_FileNamePtrVector.begin(); };
	const_iterator end() const { return m_FileNamePtrVector.end(); };

	iterator erase(iterator pWhere) { return m_FileNamePtrVector.erase(pWhere); };

	//This is the assignment operator.  This method will take 
	//all of the attributes from the supplies object and make 
	//a copy of them within the new object.
	const SVFileNameLockablePointerArrayClass operator=(const SVFileNameLockablePointerArrayClass& rhs);

	const SVFileNameClass* operator[](size_t Index) const { return m_FileNamePtrVector[Index]; };
	SVFileNameClass* operator[](size_t Index) { return m_FileNamePtrVector[Index]; };

	void push_back(SVFileNameClass* pFileName) { m_FileNamePtrVector.push_back(pFileName); };
	iterator insert(const_iterator Iter, size_t Count, SVFileNameClass* pFileName) { return m_FileNamePtrVector.insert(Iter, Count, pFileName); };

	size_t size() const { return m_FileNamePtrVector.size(); };

	void clear() { m_FileNamePtrVector.clear(); };

	//list of file names contained within the vector.
	SvDef::StringVector GetFileNameList() const;

	//This operator locks the object.
	bool Lock();

	//This operation unlocks the object.
	bool Unlock();

private:
	const SVFileNamePtrVector& getFileNamePtrVector() const { return m_FileNamePtrVector; };

	SVFileNamePtrVector m_FileNamePtrVector;

	//This attribute is the shared lock for the object.
	SvSyl::SVLockableClass mcmArrayLock;
};
