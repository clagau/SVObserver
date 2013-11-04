//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:28  $
//******************************************************************************

#ifndef INC_SVFILENAMEARRAYCLASS_3A55CC450177_INCLUDED
#define INC_SVFILENAMEARRAYCLASS_3A55CC450177_INCLUDED

#include "SVContainerLibrary/SVVector.h"
#include "SVFileNameClass.h"

//This class is inheirated from the CArray class.  This class 
//maintains an array of SVFileNameClass objects.
class SVFileNameArrayClass : public SVVector< SVFileNameClass >
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
	//object will perform any neccesary clean-up of the 
	//object's data before it is destroyed.
	virtual ~SVFileNameArrayClass();

	//This is the assignment operator.  This method will take 
	//all of the attributes from the supplies object and make 
	//a copy of them within the new object.
	const SVFileNameArrayClass& operator=(const SVFileNameArrayClass& rhs);

	//This operator will clear the exising array of object 
	//and create an array of objects corresponding to each 
	//element in the semi-colon separated string list.
	BOOL SetFileNameList(LPCTSTR szFileNameList);

	//This operator will create a semicolon separated string 
	//list of file names contained within the array.
	LPCTSTR GetFileNameList();

private:

	//This is a temporary storage container for the string 
	//defining the list of file names contained in the array.
	CString mcsFileList;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileNameArrayClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:36:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Nov 2010 16:09:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:33:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 13:58:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Format changes only.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 14:03:34   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2001 16:16:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Jan 2001 13:18:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision add the newly created classes for the file management added to SVObserver Project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

