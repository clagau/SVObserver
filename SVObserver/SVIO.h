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

#ifndef SVIO_H
#define SVIO_H

#include "SVObjectLibrary/SVObjectClass.h"
#include "SVGlobal.h"

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIO.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:06:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   27 Jan 2011 11:20:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   16 Dec 2009 11:08:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Feb 2005 14:38:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 10:31:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Nov 2002 14:11:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added friend classes SVDigitalOutputObjectClass, SVDigitalInputObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   19 Apr 2001 14:16:58   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Jan 19 2000 15:04:58   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  89
 * SCR Title:  Outputs not set when going 'OnLine'
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added source code headers per software procedures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
