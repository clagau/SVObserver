// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolDefinitionStructs
// * .File Name       : $Workfile:   SVDLLToolDefinitionStructs.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 10:18:32  $
// ******************************************************************************

#ifndef SVDLLTOOLDEFINITIONSTRUCTS_H
#define SVDLLTOOLDEFINITIONSTRUCTS_H

#include <comdef.h>
#include <vector>

#pragma pack (push, 1)
struct InputValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;
	BSTR bstrHelpText;
	BSTR bstrGroup;
	_variant_t vDefaultValue;
public:
	InputValueDefinitionStruct( );
	~InputValueDefinitionStruct( );
	InputValueDefinitionStruct(const InputValueDefinitionStruct& rhs );
	const InputValueDefinitionStruct& operator = (const InputValueDefinitionStruct& rhs );
	bool operator == (const InputValueDefinitionStruct& rhs);
private:
	void Clear();
	void Init();
};
#pragma pack (pop)


typedef std::vector<InputValueDefinitionStruct> InputValueDefinitionStructArray;

#pragma pack (push, 1)
struct ResultValueDefinitionStruct
{
	long lVT;
	BSTR bstrDisplayName;	// not used at this time
public:
	ResultValueDefinitionStruct( );
	~ResultValueDefinitionStruct( );
	ResultValueDefinitionStruct(const ResultValueDefinitionStruct& rhs );
	const ResultValueDefinitionStruct& operator = (const ResultValueDefinitionStruct& rhs );
private:
	void Clear();
	void Init();
};
#pragma pack (pop)

typedef std::vector<ResultValueDefinitionStruct> ResultValueDefinitionStructArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDLLToolDefinitionStructs.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:18:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Nov 2010 10:28:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 09:01:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Oct 2008 11:17:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  636
 * SCR Title:  Modify compiler settings to be consistent in SVObserver projects
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added pragma pack directives to force structure member alignment to 1 byte.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Jul 2007 11:47:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2004 12:40:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  417
 * SCR Title:  Create Wrapper Class for External Tool DLL
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
