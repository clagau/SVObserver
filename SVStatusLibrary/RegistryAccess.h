//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RegistryAccess
//* .File Name       : $Workfile:   RegistryAccess.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:43:56  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

typedef bool (CALLBACK *PFKEYENUMPROC) (LPCTSTR p_szKey, LPVOID pUserData);

//RegistryAccess manages entries in the registry for 
//Seidenader Vision Inc. Applications.

class RegistryAccess
{
	typedef std::vector<unsigned char> SVByteVector;

private:
	//Handle to opened registry key.
	HKEY mhKey;

public:
	//bool GetRegistryValue (int iIndex, std::string 
	//&szValueName, std::string &szValue, LPDWORD pdwType)
	//
	//Return Value:
	//true if a REG_SZ value was found for szValueName.
	//
	//Parameters:
	//iIndex - zero based index identifying the value to 
	//retrieve.
	//szValueName -- Reference to a std::string that will receive 
	//the name 
	//of the value read from the registry.
	//szValue -- Reference to a std::string where the value will 
	//be stored.
	//
	//Remarks:
	//Use this function to enumerate through the list of 
	//values for a key.
	bool GetRegistryValue(DWORD dwIndex, std::string& szValueName, std::string& szValue, LPDWORD pdwType);

	//bool SetRegistryValue (std::string &szValueName, ByteArray 
	//&baValue, DWORD dwType, DWORD dwLength)
	//throws SVException
	//
	//Return Value:
	//true if the value was set.
	//
	//Parameters:
	//szValueName -- Reference to a std::string containing the 
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
	bool SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector, DWORD dwType, DWORD dwLength);

	//bool GetRegistryValue (std::string &szValueName, SVByteArray 
	//&baValue)
	//
	//Returns:
	//true if the value was found in the current key, false 
	//otherwise.
	//
	//Remarks:
	//Retrieves the value specified by szValueName and stores 
	//it in baValue.
	bool GetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector );

	//bool SetRegistryValue (std::string &szValueName, SVByteArray 
	//&baValue)
	//
	//Returns:
	//true if the value was successfully set, false otherwise.
	//
	//Remarks:
	//Sets a registry value with the data supplied in baValue.
	bool SetRegistryValue( LPCTSTR szValueName, SVByteVector& rValueVector );

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

	//RegistryAccess ();
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
	RegistryAccess();

	//RegistryAccess& operator= (RegistryAccess& svrRight);
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//svrRight -- Existing RegistryAccess object.
	//
	//Remarks:
	//Closes the current key and opens the key
	RegistryAccess& operator=(const RegistryAccess &svrRight);

	//bool CreatedNewKey ()
	//
	//Return Value:
	//true if the key specified in the call to the 
	//RegistryAccess constructor was a new key, false if the 
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

	//bool SetRegistryValue (std::string &szValueName, DWORD 
	//pdwValue)
	//throws SVException
	//
	//Return Value:
	//true if the value was successfully set, false otherwise.
	//
	//Parameters:
	//szValueName -- Reference to a std::string containing the 
	//name 
	//of the value to read from the registry.
	//dwValue -- DWORD value to set.
	//
	//Remarks:
	//Sets the registry value for the DWORD registry entry 
	//specified by szValueName.
	bool SetRegistryValue( LPCTSTR szValueName, DWORD dwValue );

	//bool GetRegistryValue (std::string &szValueName, DWORD 
	//*pdwValue)
	//
	//Return Value:
	//true if a DWORD value was found for szValueName.
	//
	//Parameters:
	//szValueName -- Reference to a std::string containing the 
	//name 
	//of the value to read from the registry.
	//pdwValue -- Pointer to a DWORD where the value will be 
	//stored.
	//
	//Remarks:
	//Reads the DWORD registry value for the entry specified 
	//by szValueName.
	bool GetRegistryValue( LPCTSTR szValueName, DWORD *pdwValue );

	//bool SetRegistryValue (std::string &szValueName, std::string 
	//&szValue)
	//throws SVException
	//
	//Return Value:
	//true if the REG_SZ value was set.
	//
	//Parameters:
	//szValueName -- Reference to a std::string containing the 
	//name 
	//of the value to read from the registry.
	//szValue -- std::string value to be set.
	//
	//Remarks:
	//Sets the REG_SZ registry value for the entry specified 
	//by szValueName.
	bool SetRegistryValue( LPCTSTR szValueName, LPCTSTR szValue );

	//bool GetRegistryValue (std::string &szValueName, std::string 
	//&szValue)
	//
	//Return Value:
	//true if a REG_SZ value was found for szValueName.
	//
	//Parameters:
	//szValueName -- Reference to a std::string containing the 
	//name 
	//of the value to read from the registry.
	//szValue -- Reference to a std::string where the value will 
	//be stored.
	//
	//Remarks:
	//Reads the REG_SZ registry value for the entry specified 
	//by szValueName.
	bool GetRegistryValue( LPCTSTR szValueName, std::string& szValue );

	//RegistryAccess (LPCTSTR szKey)
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
	//    RegistryAccess (_T("HKEY_LOCAL_MACHINE\Software")) 
	//will open the
	//    key HKEY_LOCAL_MACHINE\Software.
	//
	//    RegistryAccess (_T("MyAppKey") ) will open the key
	//    
	//HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\MyApp\MyAppKey.
	//
	//    RegistryAccess (_T("\OtherApp\OtherAppKey")) will 
	//open the key
	//    
	//HKEY_LOCAL_MACHINE\Software\Seidenader Vision Inc.\OtherApp\OtherApp
	//Key.
	explicit RegistryAccess(LPCTSTR szKey);

	//delete pRegistry;
	//
	//Return Value:
	//None.
	//
	//Parameters:
	//None.
	//
	//Remarks:
	//Closes the current key and destroys the RegistryAccess 
	//object.
	virtual ~RegistryAccess();

private:
	//int GetImportString (FILE *pFile, std::string &szName, 
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
	int GetImportString(FILE * pFile, std::string & szName, SVByteVector& rValueVector, DWORD * pdwType);

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

	//bool GetShadowFileName (std::string &szShadowFile)
	//
	//Returns:
	//true if a shadow filename was found for the current key.
	//false otherwise
	//
	//Remarks:
	//Gets the shadow filename for a key.
	bool GetShadowFileName(std::string & szShadowFile);

	//bool GetDefaultShadowFileName (std::string &szShadowFile)
	//
	//Returns:
	//true if a default shadow filename was successfully 
	//generated. false otherwise.
	//
	//Remarks:
	//Generates a filename to use for an Export or Import 
	//operation.
	bool GetDefaultShadowFileName(std::string & szShadowFile);

	//void InitRegistry (std::string szKey)
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
	std::string m_keyString;

	//Flag indicating if the key had to be created in the 
	//constructor or not. The value of this element can be 
	//read via the CreatedNewKey method.
	bool mbCreatedNewKey;
};

