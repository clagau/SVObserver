//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRegistry
//* .File Name       : $Workfile:   SVRegistry.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:43:56  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"
#include "SVContainerLibrary/SVVector.h"

typedef bool (CALLBACK *PFKEYENUMPROC) (LPCTSTR p_szKey, LPVOID pUserData);

//SVRegistryClass manages entries in the registry for 
//Seidenader Vision Inc. Applications.

class SVRegistryClass
{
	typedef SVVector<unsigned char> SVByteVector;

private:
	//Handle to opened registry key.
	HKEY mhKey;

public:
	//DWORD NumSubKeys()
	//
	//Returns the number of child keys owned by the current 
	//key.
	DWORD NumSubKeys();

	//bool GetRegistryValue (int iIndex, SVString 
	//&szValueName, SVString &szValue, LPDWORD pdwType)
	//
	//Return Value:
	//true if a REG_SZ value was found for szValueName.
	//
	//Parameters:
	//iIndex - zero based index identifying the value to 
	//retrieve.
	//szValueName -- Reference to a SVString that will receive 
	//the name 
	//of the value read from the registry.
	//szValue -- Reference to a SVString where the value will 
	//be stored.
	//
	//Remarks:
	//Use this function to enumerate through the list of 
	//values for a key.
	bool GetRegistryValue(DWORD dwIndex, SVString& szValueName, SVString& szValue, LPDWORD pdwType);

	//bool SetRegistryValue (SVString &szValueName, ByteArray 
	//&baValue, DWORD dwType, DWORD dwLength)
	//throws SVException
	//
	//Return Value:
	//true if the value was set.
	//
	//Parameters:
	//szValueName -- Reference to a SVString containing the 
	//name of the value to read from the registry.
	//baValue -- Value to be set.
	//dwType -- Type of data to set.  Valid values are:
	//    REG_SZ, REG_BINARY,
	//    REG_MULTI_SZ, REG_EXPAND_SZ,
	//    REG_DWORD
	//dwLength - length in bytes of szValue
	//
	//Remarks:
	//Sets the  registry value for the entry specified 
	//by szValueName.
	bool SetRegistryValue( LPCTSTR szValueName, SVByteVector& baValue, DWORD dwType, DWORD dwLength);

	//bool GetRegistryValue (SVString &szValueName, SVByteArray 
	//&baValue)
	//
	//Returns:
	//true if the value was found in the current key, false 
	//otherwise.
	//
	//Remarks:
	//Retrieves the value specified by szValueName and stores 
	//it in baValue.
	bool GetRegistryValue( LPCTSTR szValueName, SVByteVector& baValue );

	//bool SetRegistryValue (SVString &szValueName, SVByteArray 
	//&baValue)
	//
	//Returns:
	//true if the value was successfully set, false otherwise.
	//
	//Remarks:
	//Sets a registry value with the data supplied in baValue.
	bool SetRegistryValue( LPCTSTR szValueName, SVByteVector& baValue );

	//bool DeleteValue (SVString &szValueName)
	//
	//Returns:
	//true if the value was deleted, false otherwise.
	//
	//Remarks:
	//The value specified by szValueName is deleted from the
	//registry. If the value did not exist previous to the
	//call to DeleteValue, the function returns true.
	bool DeleteValue(LPCTSTR p_szValueName);

	//bool Import (LPCTSTR szFileName)
	//
	//Return Value:
	//true if the file was imported.
	//
	//Parameters:
	//szFileName - fully qualified name of the file where the 
	//registry
	//key will be imported.
	//
	//Remarks:
	//Imports the data in the specified file into the 
	//registry.
	bool Import(LPCTSTR szFileName);

	//bool Import (LPCTSTR szFileName)
	//
	//Return Value:
	//true if the file was imported.
	//
	//Remarks:
	//Imports the data in the default shadow file into the 
	//registry.
	bool Import();

	//bool Export (LPCTSTR szFileName)
	//
	//Return Value:
	//true if the key was exported.
	//
	//Parameters:
	//szFileName - fully qualified name of the file where the 
	//registry
	//key will be exported.
	//
	//Remarks:
	//Writes the contents of the current key & subkeys to the 
	//file
	//specified by szFileName.
	bool Export(LPCTSTR szFileName);

	//bool Export ()
	//
	//Return Value:
	//true if the key was exported.
	//
	//Remarks:
	//Writes the contents of the current key & subkeys to the 
	//default shadow file.
	bool Export();

	//SVRegistryClass ();
	//throws SVException
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//None.
	//
	//Remarks:
	//Opens (and creates if necessary) a registry key. The 
	//key opened is
	//"HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\appname" where 
	//appname is determined from the AfxGetAppName function.
	SVRegistryClass();

	//SVRegistryClass* OpenSubKey (SVString szSubKey)
	//
	//Return Value:
	//Pointer to an SVRegistryClass object representing the 
	//sub-key.
	//
	//Parameters:
	//szSubKey -- Name of the subordinate key to open.
	//
	//Remarks:
	//Creates a new registry key subordinate to the current 
	//key.
	SVRegistryClass * OpenSubKey( LPCTSTR p_szSubKey );

	//SVRegistryClass& operator= (SVRegistryClass& svrRight);
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//svrRight -- Existing SVRegistryClass object.
	//
	//Remarks:
	//Closes the current key and opens the key
	SVRegistryClass& operator=(const SVRegistryClass &svrRight);

	//bool CreatedNewKey ()
	//
	//Return Value:
	//true if the key specified in the call to the 
	//SVRegistryClass constructor was a new key, false if the 
	//key existed before the call to the constructor.
	//
	//Parameters:
	//None.
	//
	//Remarks:
	//Returns true if the constructor had to create the key 
	//in order to open it.
	bool CreatedNewKey();

	//bool DeleteKey ()
	//
	//Return Value:
	//true if the key was deleted.
	//
	//Parameters:
	//None.
	//
	//Remarks:
	//Deletes the key and all its sub-keys  from the registry.
	bool DeleteKey ();

	//void EnumKeys (PFKEYENUMPROC pKeyEnumProc, LPVOID 
	//pUserData);
	//
	//Return Value:
	//true if all keys were enumerated.
	//
	//Parameters:
	//pKeyEnumProc -- Pointer to a function that will be 
	//called for each key enumerated.
	//pUserData -- Pointer to any application specific data 
	//that will be passed to the function referenced by 
	//pKeyEnumProc.
	//
	//Remarks:
	//Enumerates child keys of the current key. 
	//
	//User data may be passed into the enumeration by setting 
	//pUserData to a value that points to application 
	//specific data. pUserData will be passed to the 
	//pfnEnumProc with each key enumerated.
	//
	//Does not enumerate below the first generation (i.e. 
	//children of children are not enumerated).
	void EnumKeys(PFKEYENUMPROC pKeyEnumProc, LPVOID pUserData);

	//bool ReplaceKey (SVString &szHive);
	//
	//Return Value:
	//true if the registry was restored.
	//
	//Parameters:
	//szHive -- Name of the file where the registry 
	//information to load resides.
	//
	//Remarks:
	//Loads the registry with keys and values saved in a 
	//previous SaveKey operation.
	bool ReplaceKey (LPCTSTR p_szHive);

	//bool SaveKey (SVString &szHive);
	//
	//Return Value:
	//true if the registry was successfully saved.
	//
	//Parameters:
	//szHive -- Name of the file where the registry 
	//information is stored.
	//
	//Remarks:
	//Writes the contents of the registry key, its subkeys 
	//and values to a file specified by szFile.
	bool SaveKey (LPCTSTR p_szHive);

	//bool SetRegistryValue (SVString &szValueName, DWORD 
	//pdwValue)
	//throws SVException
	//
	//Return Value:
	//true if the value was successfully set, false otherwise.
	//
	//Parameters:
	//szValueName -- Reference to a SVString containing the 
	//name 
	//of the value to read from the registry.
	//dwValue -- DWORD value to set.
	//
	//Remarks:
	//Sets the registry value for the DWORD registry entry 
	//specified by szValueName.
	bool SetRegistryValue( LPCTSTR szValueName, DWORD dwValue );

	//bool GetRegistryValue (SVString &szValueName, DWORD 
	//*pdwValue)
	//
	//Return Value:
	//true if a DWORD value was found for szValueName.
	//
	//Parameters:
	//szValueName -- Reference to a SvString containing the 
	//name 
	//of the value to read from the registry.
	//pdwValue -- Pointer to a DWORD where the value will be 
	//stored.
	//
	//Remarks:
	//Reads the DWORD registry value for the entry specified 
	//by szValueName.
	bool GetRegistryValue( LPCTSTR szValueName, DWORD *pdwValue );

	//bool SetRegistryValue (SVString &szValueName, SVString 
	//&szValue)
	//throws SVException
	//
	//Return Value:
	//true if the REG_SZ value was set.
	//
	//Parameters:
	//szValueName -- Reference to a SvString containing the 
	//name 
	//of the value to read from the registry.
	//szValue -- SVString value to be set.
	//
	//Remarks:
	//Sets the REG_SZ registry value for the entry specified 
	//by szValueName.
	bool SetRegistryValue( LPCTSTR szValueName, LPCTSTR szValue );

	//bool GetRegistryValue (SVString &szValueName, SVString 
	//&szValue)
	//
	//Return Value:
	//true if a REG_SZ value was found for szValueName.
	//
	//Parameters:
	//szValueName -- Reference to a SVString containing the 
	//name 
	//of the value to read from the registry.
	//szValue -- Reference to a SVString where the value will 
	//be stored.
	//
	//Remarks:
	//Reads the REG_SZ registry value for the entry specified 
	//by szValueName.
	bool GetRegistryValue( LPCTSTR szValueName, SVString& szValue );

	//SVRegistryClass (LPCTSTR szKey)
	//throws SVException
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//szKey -- Name of the key to open.
	//
	//Remarks:
	//Opens the key specified by szKey. If it does not exist, 
	//it is created,
	//and CreatedNewKey() function will return true when 
	//called.
	//
	//szKey can be an absolute key,  application key, or 
	//SVR-global key.
	//
	//An absolute key is a key completely specified by the 
	//calling function. It is specified
	//by beginning szKey with one of the following reserved 
	//key names followed by a
	//backslash :
	//
	//    HKEY_CLASSES_ROOT
	//    HKEY_CURRENT_USER
	//    HKEY_LOCAL_MACHINE
	//    HKEY_USERS
	//    HKEY_PERFORMANCE_DATA
	//    HKEY_CURRENT_CONFIG
	//
	//An application key is sub-ordinate to the registry key
	//"HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\appname"
	//where 'appname' is the name of the running application 
	//as returned
	//by the AfxGetAppName function. To specify an 
	//application key,
	//the first character of szKey should not be a backslash 
	//('\') character.
	//
	//An SVR-global key is subordinate to  the key 
	//"HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.". 
	//(note that 'appname' is not used to construct the key). 
	//This allows
	//keys to be shared between applications (or dlls). To 
	//specify an SVR-global
	//key, make sure the first character of szKey is a 
	//backslash ('\').
	//
	//Example:  Assuming an application named "MyApp":
	//
	//    SVRegistryClass (_T("HKEY_LOCAL_MACHINE\Software")) 
	//will open the
	//    key HKEY_LOCAL_MACHINE\Software.
	//
	//    SVRegistryClass (_T("MyAppKey") ) will open the key
	//    
	//HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\MyApp\MyAppKey.
	//
	//    SVRegistryClass (_T("\OtherApp\OtherAppKey")) will 
	//open the key
	//    
	//HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\OtherApp\OtherApp
	//Key.
	SVRegistryClass(LPCTSTR szKey);

	//delete pRegistry;
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//None.
	//
	//Remarks:
	//Closes the current key and destroys the SVRegistryClass 
	//object.
	virtual ~SVRegistryClass();

private:
	//int GetImportString (FILE *pFile, SVString &szName, 
	//SVByteArray &baValue, DWORD *pdwType)
	//
	//Reads a registry import file.
	//
	//Returns: one of
	//
	//	SV_ISERROR
	//	SV_ISEOF
	//	SV_ISKEY
	//	SV_ISVALUE
	//	SV_ISGARBAGE
	int GetImportString(FILE * pFile, SVString & szName, SVByteVector & baValue, DWORD * pdwType);

	//This is a supporting function for the Import function.
	bool ImportKeys(FILE * pFile);

	//Walks the registry tree structure from the current key
	//outputting the keys to the file specified by pFile.
	//
	//This is a supporting function for the Export function.
	bool ExportKeys(FILE * pFile);

	//Exports the values of the current key to the file 
	//specified by
	//pFile.
	bool ExportValues(FILE * pFile);

	//bool SetShadowFileName (LPCTSTR szFileName)
	//
	//Returns:
	//true if the SVShadowFile registry value was successfully
	//set for the current key. false otherwise.
	bool SetShadowFileName(LPCTSTR szFileName);

	//bool GetShadowFileName (SVString &szShadowFile)
	//
	//Returns:
	//true if a shadow filename was found for the current key.
	//false otherwise
	//
	//Remarks:
	//Gets the shadow filename for a key.
	bool GetShadowFileName(SVString & szShadowFile);

	//bool GetDefaultShadowFileName (SVString &szShadowFile)
	//
	//Returns:
	//true if a default shadow filename was successfully 
	//generated. false otherwise.
	//
	//Remarks:
	//Generates a filename to use for an Export or Import 
	//operation.
	bool GetDefaultShadowFileName(SVString & szShadowFile);

	//void InitRegistry (SVString szKey)
	//throws SVException
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//szKey -- Name of key to open.
	//
	//Remarks:
	//This is private scope.
	//Performs the open/create on behalf of the constructors.
	void InitRegistry( LPCTSTR p_szKey );

	//The name of the key opened or created during 
	//construction of the object.
	SVString mszKey;

	//Flag indicating if the key had to be created in the 
	//constructor or not. The value of this element can be 
	//read via the CreatedNewKey method.
	bool mbCreatedNewKey;

	//bool AdjustPrivileges (TCHAR *pszPrivilege);
	//
	//Return Value:
	//true if the request privilege was granted.
	//
	//Parameters:
	//Privilege to grant.
	//
	//Remarks:
	//This member function is private scope.
	//Alters the process's privileges to allow registry 
	//modification.
	bool AdjustPrivileges (TCHAR *pszPrivilege);

};

