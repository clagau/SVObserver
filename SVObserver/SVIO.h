//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIO
//* .File Name       : $Workfile:   SVIO.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:06:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVGlobal.h"
#pragma region Includes

class SVIOEntryClass
{
	friend class SVIOClass;
	friend class SVIODoc;
	friend class SVPublishListClass;
	friend class SVIOEntryListClass;

protected:

	SVIOEntryClass( LPCTSTR PName, 
					        BOOL IsAnOutput, 
					        DWORD CH				= -1, 
					        SVIODLLBOOLDWORDBOOLFUNCTION	 POut	 = NULL, 
					        SVIODLLBOOLDWORDBOOLPTRFUNCTION	 PIn	 = NULL,	
					        SVIODLLBOOLDWORDBOOLBOOLFUNCTION PForce  = NULL,
					        SVIODLLBOOLDWORDBOOLFUNCTION	 PInvert = NULL,
					        BOOL IsAModuleIOListEntry = FALSE,
					        BOOL BDefaultValue		= FALSE,
					        const GUID& UniqueObjectID = SVInvalidGUID,
					        int ObjectAttribute		= -1,
					        BOOL BIsForced			= FALSE,
					        DWORD DwForcedValue		= FALSE,
					        BOOL BIsInverted		= TRUE );

	~SVIOEntryClass();

public:

	BOOL ResetIOEntry();
	BOOL RebuildIOEntry( SVIOEntryClass* PIOEntry );

	// Use BOOL BOutValue to set the Output Value.
	// Returns TRUE, if successfully.
	// Returns FALSE, if an error happens.
	BOOL Out( BOOL BOutValue );

	void SetIOAdjustDialogHWnd(	HWND HDlg );
	DWORD GetChannel();
	DWORD ResetChannel( DWORD DWNewChannel = -1 );

	// Init IO Entry/Channel...
	void Init();

	// Use BOOL& RBInValue to get the Input Value.
	// Returns TRUE, if successfully.
	// Returns FALSE, if an error happens.
	BOOL In( BOOL& RBInValue );
	LPCTSTR GetName();
	BOOL GetIsForced();
	BOOL GetForcedValue();
	BOOL GetIsInverted();
	BOOL GetValue();
	BOOL IsValidChannel();
	void Force( BOOL ShouldForce, BOOL IsHigh );

	// TRUE		--> Inverted
	// FALSE	--> Not Inverted
	void Invert( BOOL BInvert );

protected:

	LPTSTR	strIOEntryName;
	BOOL	isOutput;
	DWORD	channel;
	SVIODLLBOOLDWORDBOOLFUNCTION		pOutFunc;
	SVIODLLBOOLDWORDBOOLPTRFUNCTION		pInFunc;
	SVIODLLBOOLDWORDBOOLBOOLFUNCTION	pForceFunc;
	SVIODLLBOOLDWORDBOOLFUNCTION		pInvertFunc;
	BOOL	isModuleIOListEntry;
	BOOL	LastValue;
	BOOL	DefaultValue;
	BOOL	IsForced;
	BOOL	ForcedValue;
	BOOL    IsInverted;

	HWND hAdjustDialogHWnd;

	GUID	IOObjectID;
	int		IOObjectAttribute;
};

// If an io entry will be connected to the outside world or not, 
// depends on two facts:
// First the user has to assign an existing channel to a possible
// io entry or this is done by default, refer to IODoc::IODoc().
// Second in any part of this software must be an 
// IOControl::CreateIOEntry call to open this io entry for 
// reading or writing.
class SVIOEntryListClass
{
public:
	SVIOEntryListClass( int Grow = 5 );
	~SVIOEntryListClass();

	SVIOEntryClass* GetAt( int i );
	int GetSize();
	LPCTSTR GetNameAt( int i );
	SVIOEntryClass* Find( LPCTSTR PName );
	SVIOEntryClass* Find( const GUID& RUniqueObjectID, int ObjectAttribute );
	SVIOEntryClass* Find( const GUID& RUniqueObjectID, int ObjectAttribute, BOOL IsOutput );
	SVIOEntryClass* Find( LPCTSTR PName, BOOL IsOutput );
	SVIOEntryClass* Find( DWORD Ch, BOOL IsOutput, BOOL IsModule );
	SVIOEntryClass* FindNext( BOOL BIsOutput, BOOL BIsModule, int* PIndex );
	SVIOEntryClass* FindNext( DWORD DWChannel, BOOL BIsOutput, BOOL BIsModule, int* PIndex );
	BOOL Add( SVIOEntryClass* PIOEntry );
	BOOL Rename( LPCTSTR POldName, LPCTSTR PNewName );
	void Refresh();
	BOOL Remove( SVIOEntryClass* PIOEntry );
	void RemoveAll( BOOL AutoDelete = FALSE );
	BOOL ResetAll();

public:

	BOOL	IsChanged;

protected:

	int maxGrow;
	int currentSize;
	int lastValid;
	SVIOEntryClass** pIOEntryArray;
	HANDLE hProtectionMutex;
	DWORD dwWaitTime;

};


class SVIOClass
{

public:
	SVIOClass();

public:
	virtual ~SVIOClass();

public:
	BOOL Create( HWND HMainWnd );
	SVIOEntryClass* CreateIOEntry( LPCTSTR PName, BOOL IsAnOutput, GUID UniqueObjectID = SVInvalidGUID, int ObjectAttribute = -1 );
	
public:
	BOOL DestroyIOEntry( SVIOEntryClass* PIOEntry );
	
public:
	SVIOEntryListClass* GetIOEntryList();

protected:
	SVIOEntryListClass	ioEntryList;
	HWND				hMainFrame;

};

extern BOOL IsIOControlValid;
extern SVIOClass IOControl;


