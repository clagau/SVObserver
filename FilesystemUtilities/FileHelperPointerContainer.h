///\copyright 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FileHelperPointerContainer.h

#pragma once

#include "SVSystemLibrary/SVLockableClass.h"
#include "Definitions/StringTypeDef.h"

namespace SvFs
{
class FileHelper;

//This class maintains a list of pointers to FileHelper objects.  
//This class also maintains a mechanism for allowing shared 
//access using a lock.
class FileHelperPointerContainer
{
public:
	typedef std::vector<FileHelper*> FileHelperPointers;
	typedef FileHelperPointers::iterator iterator;
	typedef FileHelperPointers::const_iterator const_iterator;

	//This operator is the default constructor for this 
	//class.  This method sets all attributes to their 
	//default values.
	FileHelperPointerContainer() = default;

	//This operator is the copy constructor.  This method 
	//will take all of the attributes from the supplies 
	//object and make a copy of them within the new object.
	FileHelperPointerContainer(const FileHelperPointerContainer& orig) = default;

	virtual ~FileHelperPointerContainer() = default;

	iterator begin() { return m_fileHelpers.begin(); };
	iterator end() { return m_fileHelpers.end(); };

	const_iterator begin() const { return m_fileHelpers.begin(); };
	const_iterator end() const { return m_fileHelpers.end(); };

	iterator erase(iterator pWhere) { return m_fileHelpers.erase(pWhere); };

	//This is the assignment operator.  This method will take 
	//all of the attributes from the supplies object and make 
	//a copy of them within the new object.
	const FileHelperPointerContainer operator=(const FileHelperPointerContainer& rhs);

	const FileHelper* operator[](size_t Index) const { return m_fileHelpers[Index]; };
	FileHelper* operator[](size_t Index) { return m_fileHelpers[Index]; };

	void push_back(FileHelper* pFileName) { m_fileHelpers.push_back(pFileName); };
	iterator insert(const_iterator Iter, size_t Count, FileHelper* pFileName) { return m_fileHelpers.insert(Iter, Count, pFileName); };

	size_t size() const { return m_fileHelpers.size(); };

	void clear() { m_fileHelpers.clear(); };

	//list of file names contained within the vector.
	SvDef::StringVector GetFileNameList() const;

	//This operator locks the object.
	bool Lock();

	//This operation unlocks the object.
	bool Unlock();

private:
	const FileHelperPointers& getFileNamePtrVector() const { return m_fileHelpers; };

	FileHelperPointers m_fileHelpers;

	//This attribute is the shared lock for the object.
	SvSyl::SVLockableClass mcmArrayLock;
};
}