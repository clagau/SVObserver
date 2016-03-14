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

#ifndef INC_SVLOCKABLEFILENAMEARRAYCLASS_3A5357EC02CE_INCLUDED
#define INC_SVLOCKABLEFILENAMEARRAYCLASS_3A5357EC02CE_INCLUDED

class SVFileNameClass;

//This class is inheirated from the CArray class.  This class 
//maintains an array of pointers to SVFileNameClass objects.  
//This class also maintains a mechanism for allowing shared 
//access to the array using a lock.
class SVFileNameLockablePointerArrayClass : public CArray<SVFileNameClass *,SVFileNameClass *>
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
	LPCTSTR GetFileNameList();

	//This operator locks the object.
	BOOL Lock();

	//This operation unlocks the object.
	BOOL Unlock();

private:
	//This attribute is the shared lock for the object.
	CMutex mcmArrayLock;

	//This is a temporary storage container for the string 
	//defining the list of file names contained in the array.
	CString mcsFileList;

};

#endif

