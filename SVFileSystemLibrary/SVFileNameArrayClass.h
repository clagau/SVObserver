//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:28  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVFileNameClass.h"
#pragma endregion Includes

//This class maintains an array of SVFileNameClass objects.
class SVFileNameArrayClass
{
public:
	//This operator is the default constructor for this 
	//class.  This method sets all attributes to their 
	//default values.
	SVFileNameArrayClass();

	//This operator is the copy constructor.  This method 
	//will take all of the attributes from the supplies 
	//object and make a copy of them within the new object.
	SVFileNameArrayClass(const SVFileNameArrayClass& orig);

	//This operator is the destructor for the class.  This 
	//object will perform any necessary clean-up of the 
	//object's data before it is destroyed.
	virtual ~SVFileNameArrayClass();

	//This is the assignment operator.  This method will take 
	//all of the attributes from the supplies object and make 
	//a copy of them within the new object.
	const SVFileNameArrayClass& operator=(const SVFileNameArrayClass& rhs);

	const SVFileNameClass& operator[](size_t Index) const { return m_FileNameVector[Index]; };
	SVFileNameClass& operator[](size_t Index) { return m_FileNameVector[Index]; };

	void push_back(const SVFileNameClass& rFileName) { m_FileNameVector.push_back(rFileName); };

	size_t size() const { return m_FileNameVector.size(); };

	void clear() { m_FileNameVector.clear(); };
	//This operator will clear the exising array of object 
	//and create an array of objects corresponding to each 
	//element in the semi-colon separated string list.
	bool SetFileNameList(LPCTSTR szFileNameList);

	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	LPCTSTR GetFileNameList();

private:
	const std::vector<SVFileNameClass>& getFileNameVector() const { return m_FileNameVector; };

	std::vector<SVFileNameClass> m_FileNameVector;
	//This is a temporary storage container for the string 
	//defining the list of file names contained in the array.
	mutable std::string mcsFileList;
};

