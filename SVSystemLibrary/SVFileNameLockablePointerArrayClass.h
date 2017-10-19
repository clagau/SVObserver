//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableFileNameClass
//* .File Name       : $Workfile:   SVFileNameLockablePointerArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:37:04  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVLockableClass.h"
#include "SVUtilityLibrary/SVString.h"

class SVFileNameClass;

//This class maintains a list of pointers to SVFileNameClass objects.  
//This class also maintains a mechanism for allowing shared 
//access using a lock.
class SVFileNameLockablePointerArrayClass : public SVVector<SVFileNameClass*>
{
public:
	//This operator is the default constructor for this 
	//class.  This method sets all attributes to their 
	//default values.
	SVFileNameLockablePointerArrayClass();

	//This operator is the copy constructor.  This method 
	//will take all of the attributes from the supplies 
	//object and make a copy of them within the new object.
	SVFileNameLockablePointerArrayClass(const SVFileNameLockablePointerArrayClass& orig);

	virtual ~SVFileNameLockablePointerArrayClass();

	//This is the assignment operator.  This method will take 
	//all of the attributes from the supplies object and make 
	//a copy of them within the new object.
	const SVFileNameLockablePointerArrayClass operator=(const SVFileNameLockablePointerArrayClass& rhs);

	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	LPCTSTR GetFileNameList() const;

	//This operator locks the object.
	bool Lock();

	//This operation unlocks the object.
	bool Unlock();

private:
	//This attribute is the shared lock for the object.
	SvSyl::SVLockableClass mcmArrayLock;

	//This is a temporary storage container for the string 
	//defining the list of file names contained in the array.
	mutable SVString mcsFileList;
};
