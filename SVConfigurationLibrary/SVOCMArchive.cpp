// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:39:38  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOCMArchive.h"
#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVConfigurationTags.h"
#include "SVLibrary/SVBStr.h"							// SVBStr

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOCMArchive::SVOCMArchive()
{

}

SVOCMArchive::~SVOCMArchive()
{

}

HRESULT SVOCMArchive::SerializeGUID(CArchive &rArchive, GUID &rGuid)
{
	HRESULT hrOk = S_OK;

	rArchive >> rGuid.Data1 >> rGuid.Data2 >> rGuid.Data3;
	for ( int i = 0; i < 8; i++)
	{
		rArchive >> rGuid.Data4[i];
	}

	return hrOk;
}


HRESULT SVOCMArchive::GetSVOSavedVersionNumberFromConfigurationFile (
	BSTR				abstrFileName, 
	unsigned long*	apulSVOConfigVersion)
{
	CFile				fConfigurationFile;

	CFileException	fileException;

	long				lFileOpen;
	long				lNbrOfCharactersRead;
	unsigned long	ulConfigurationVersion;

	SVBStr			bstrConfigurationFileName;

	char				csBuffer [4000];


	HRESULT	hr;

	hr = 0;
	lFileOpen = FALSE;
	lNbrOfCharactersRead = 0;
	ulConfigurationVersion = 0;

	while (1)
	{
		bstrConfigurationFileName = abstrFileName;

		//-	Open configuration file.
		if (fConfigurationFile.Open (bstrConfigurationFileName,
			CFile::modeRead|CFile::shareExclusive,
			&fileException) == FALSE)
		{
			switch (fileException.m_cause)
			{
			case CFileException::fileNotFound:
				{
					hr = -1652;
					break;
				}
			default:
				{
					hr = -1653;
					break;
				}
			}
		}

		if (hr & 0xc0000000)
		{
			break;
		}

		lFileOpen = TRUE;
		//-	Configuration file successfully openned.

		//-   Set the file pointer to the beginning of the file.
		try
		{
			fConfigurationFile.Seek (0, CFile::begin);
		}

		catch (CFileException fileException2)
		{
			hr = -1654;
			break;
		}

		//-	Attempt to read first 399 characters; leave 1 character for NULL.
		lNbrOfCharactersRead = fConfigurationFile.Read (csBuffer, 
			sizeof(csBuffer) - 1);



		//-   Append NULL to acquired buffer.
		csBuffer [lNbrOfCharactersRead] = 0;

		//-   We are finished with the file and it will be closed.
		fConfigurationFile.Close ();
		lFileOpen = FALSE;


		//********** NEEDS DONE ************************

		//-	Always list latest and greatest formats first.		
		hr = FindVersion440_ (csBuffer, 
			&ulConfigurationVersion);
		if (hr & 0xc0000000)
		{
			break;
		}
		else
			if (ulConfigurationVersion != 0)
			{
				//-		Version number found.  No need to continue search.
				break;
			}

			hr = FindVersion401_430 (csBuffer, 
				&ulConfigurationVersion);

			if (hr & 0xc0000000)
			{
				break;
			}
			else
				if (ulConfigurationVersion != 0)
				{
					//-		Version number found.  No need to continue search.
					break;
				}

				//-	No version could be found, so it will be assumed that the 
				//-	configuration came from an early version of SVObserver 4.00.
				ulConfigurationVersion = 0x00040000;

				break;
	}

	if (lFileOpen == TRUE)
	{
		fConfigurationFile.Close ();
	}

	if (hr & 0xc0000000)
	{
		//-	Error occurred.
	}
	else
	{
		*apulSVOConfigVersion = ulConfigurationVersion;
	}

	return hr;
}


//- If a version number is not found, this function will set 
//- aplConfigurationVersion to zero.
HRESULT SVOCMArchive::FindVersion401_430 (char* apczSourceString, 
																					unsigned long* aplConfigurationVersion)
{
	char*		pntr1;
	char*		pntr2;
	char		czWhiteSpace [40];
	//	char		czValueString [500];

	long		lFound;
	//   long		lIndex;

	long		lVersionNumber;

	HRESULT	hr;

	hr = 0;
	lFound = FALSE;
	lVersionNumber = 0;
	*aplConfigurationVersion = 0;

	while (1)
	{
		//		lLength = sizeof (czValueString);
		//      czValueString [0] = 0;

		//--- Yes, I am treating a comma as white space.
		strcpy (czWhiteSpace, "\a\b\f\n\r\t\v ,");
		pntr1 = apczSourceString;

		//--- Move passed any white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);

		if (strncmp (pntr1, "<?xml", 5) != 0)
		{
			//-		The XML header was not found, therefore this is not considered to be
			//-		a valid 401_430 configuration file.
			hr = -1658;
			break;
		}

		//-	Move passed the "<?xml" text.
		pntr1 = pntr1 + 5; 

		//-	Search the source text for the version number name.
		if ((pntr1 = strstr (pntr1, 
			"name=\"VersionNumber\"")) == NULL)
		{
			//-		The version number was not found.
			//-		This will be given a default version number.
			break;
		}

		//-   pntr1 moves ahead to "<" which must be part of "<value>".
		if ((pntr1 = strstr (pntr1, "<")) == NULL)
		{
			//-		The layout is not as expected.
			hr = -1659;
			break;
		}

		//-	Move passed "<".
		pntr1 = pntr1 + 1;


		//--- Move passed any white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);
		if (*pntr1 == NULL)
		{
			//-		The layout is not as expected.
			hr = -1660;
			break;
		}


		if (strncmp (pntr1, "VALUE", 5) != 0)
		{
			//-		The layout is not as expected.
			hr = -1655;
			break;
		}

		//-	Move passed "value"
		pntr1 = pntr1 + 5;

		//--- Move passed any white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);
		if (*pntr1 == NULL)
		{
			//-		The layout is not as expected, and the version number will be 
			//-		considered unfound.
			hr = -1656;
			break;
		}

		if (*pntr1 != '>')
		{
			//-		The layout is not as expected, and the version number will be 
			//-		considered unfound.
			hr = -1657;
			break;
		}

		//-	Move passed ">".
		pntr1 = pntr1 + 1;

		//-	Move passed white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);
		if (*pntr1 == NULL)
		{
			//-		The layout is not as expected.
			hr = -1656;
			break;
		}

		//-	It's now time to read the version number!!!
		//-	A base of 0 means that the function will automatically determine the 
		//-	base (of the input string).
		lVersionNumber = strtol (pntr1, &pntr2, 0);

		//-	Move passed version number.
		pntr1 = pntr2;

		//-	Any version number that shows less than 4.00 and greater than 10.99 
		//-	will be considered invalid.
		if ((lVersionNumber < 0x00040000) ||	// 0x00040000 = 4.00
			(lVersionNumber >= 0x000A63FF))		// 0x00041eff = 10.99
		{
			hr = -1661;
			break;
		}

		*aplConfigurationVersion = lVersionNumber;

		break;
	}

	return hr;
}


//- If a version number is not found, this function will set 
//- aplConfigurationVersion to zero.
HRESULT SVOCMArchive::FindVersion440_ (char*				apczSourceString, 
																			 unsigned long* p_ulpConfigurationVersion)
{
	HRESULT					hr;

	unsigned long			l_ulVersionNumber;

	hr = S_OK;
	l_ulVersionNumber = 0;

	while (1)
	{
		hr = FindVersion440__Standard (apczSourceString, 
			&l_ulVersionNumber);
		if (hr & 0xc0000000)
		{
			break;
		}

		if (l_ulVersionNumber != 0)
		{
			break;
		}

		//-	Assumtions that can be made at this point....
		//-     Only XML files will get to this point.
		//-	  "<?xml" exists.

		hr = FindVersion440__RevisionHistory (apczSourceString,
			&l_ulVersionNumber);
		if (hr & 0xc0000000)
		{

		}

		break;
	}


	if (l_ulVersionNumber == 0)
	{
	}
	else
		//-Any version number that shows less than 4.30 and greater than 14.00 
		//-will be considered invalid.  The 14.00 is an arbitrary large version 
		//-number which is used to test for obscenely bizar data.
		if ((l_ulVersionNumber <  0x00040000) ||	// 0x00041e00 =  4.30
			(l_ulVersionNumber >= 0x00140000))		// 0x00140000 = 14.00 
		{
			hr = -1732;
		}

		*p_ulpConfigurationVersion = l_ulVersionNumber;


		return hr;
}


HRESULT SVOCMArchive::FindVersion440__Standard (char* apczSourceString, 
																								unsigned long* aplConfigurationVersion)
{
	char*		pntr1;
	char*		pntr2;
	char		czWhiteSpace [40];
	//	char		czValueString [500];

	long		lFound;
	long		lInQuotes;
	//   long		lIndex;

	long		lVersionNumber;

	HRESULT	hr;


	hr = 0;
	lFound = FALSE;
	lInQuotes = FALSE;
	lVersionNumber = 0;
	*aplConfigurationVersion = 0;

	while (1)
	{


		//--- Yes, I am treating a comma as white space.
		strcpy (czWhiteSpace, "\a\b\f\n\r\t\v ,");
		pntr1 = apczSourceString;

		//--- Move passed any white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);

		if (strncmp (pntr1, "﻿", 3) == 0)
		{
			//-      NOTEPAD!
			//-		Recieved this prefix after massaging encrypted configuration with 
			//-		Notepad (inserted by Notepad).
			//-		Ignore.
			pntr1 = pntr1 + 3;
		}


		if (strncmp (pntr1, "<?xml", 5) != 0)
		{
			//-		The XML header was not found, therefore this is not considered to be
			//-		a valid 440+ configuration file.
			hr = -1724;
			break;
		}

		//-	Move passed the "<?xml" text.
		pntr1 = pntr1 + 5; 

		//-	Search the source text for the version number name.
		if ((pntr1 = strstr (pntr1, 
			"Name=\"VersionNumber\"")) == NULL)
			//                           "FormatVersion=\"")) == NULL)
		{
			//-		The version number was not found.
			//-		This will be given a default version number.
			break;
		}

		pntr2 = pntr1 - 1;

		//-	Move passed the "Name=\"VersionNumber\"" text.
		pntr1 = pntr1 + 20; 

		if (*pntr1 == NULL)
		{
			hr = -1725;
			break;
		}

		//-   Move pntr2 backwards from "Name=\"VersionNumber\"" to find the previous
		//-	"<".
		//-   JAB 011205 - After switching from maintaining the version number in a 
		//-	value attribute to maintaining the version number in the text data, 
		//-	there is currently no real reason that we need to find the beginning 
		//-	'<' of the opening tag.  However I'm going to keep the search here in 
		//-	case we want it some day.
		lInQuotes = FALSE;
		while (((*pntr2 != '<') || (lInQuotes == TRUE)) &&
			(pntr2 > apczSourceString))
		{
			//-		If the '<' character exists within quotes, we will assume that it is
			//-		part of an attribute value string.
			if (*pntr2 == '"')
			{
				if (lInQuotes == FALSE)
				{
					lInQuotes = TRUE;
				}
				else
				{
					lInQuotes = FALSE;
				}
			}
			pntr2--;
		}


		//-	Because XML doesn't guarentee the order of the attribute entries, we 
		//-	will find the entire entry between the "<" and the ">" and then search
		//-   for the Value attribute. ----------------------------------------------
		if (pntr2 == apczSourceString)
		{
			//-      Make sure pntr2 hasn't gone all the way back to the beginning of the
			//-   	file
			hr = -1727;
			break;
		}

		lInQuotes = FALSE;
		while (((*pntr1 != '>') || (lInQuotes == TRUE)) &&
			(*pntr1 != NULL))
		{
			if (*pntr1 == '"')
			{
				if (lInQuotes == FALSE)
				{
					lInQuotes = TRUE;
				}
				else
				{
					lInQuotes = FALSE;
				}
			}

			pntr1++;
		}

		if (*pntr1 == NULL)
		{
			//-      If pntr1 hasn't found the end of this entry prior to the end of the 
			//-   	string, then we need to read a larger string from the file.
			hr = -1728;
			break;
		}


		/*- VERSION AS ATTRIBUTE ----------------------------------------------------
		//-	Search this element entry for the Value attribute.
		if ((pntr2 = strstr (pntr2, 
		"Value=")) == NULL)
		{
		//-		value not found.
		hr = -1726;
		break;
		}

		if (pntr2 > pntr1)
		{
		hr = -1729;
		break;
		}

		//-   Move passed "Value=\"".
		pntr2 = pntr2 + 7;

		if (*pntr2 == NULL)
		{
		//-		This just shouldn't be.
		hr = -1730;
		break;
		}

		//--- Move passed any white space.
		pntr2 = pntr2 + strspn (pntr2, czWhiteSpace);

		if (*pntr2 == NULL)
		{
		hr = -1731;
		break;
		}
		*/

		//- VERSION AS TEXT ---------------------------------------------------------
		//-   Move past '>'.
		pntr2 = pntr1 + 1;
		//---------------------------------------------------------------------------

		//-	It's now time to read the version number!!!
		//-	A base of 0 means that the function will automatically determine the 
		//-	base (of the input string).
		lVersionNumber = strtol (pntr2, &pntr1, 0);

		*aplConfigurationVersion = lVersionNumber;

		break;
	}

	return hr;
}


HRESULT SVOCMArchive::FindVersion440__RevisionHistory (char*				p_czpSourceString, 
																											 unsigned long*	p_ulpConfigurationVersion)
{
	char*				pntr1;
	char*				pntr2;
	char				czWhiteSpace [40];

	long				lFound;

	unsigned long	l_ulVersionNumber;
	unsigned long	l_ulVersionNumber1;
	unsigned long	l_ulVersionNumber2;

	HRESULT	hr;

	hr = 0;
	lFound = FALSE;
	l_ulVersionNumber = 0;
	*p_ulpConfigurationVersion = 0;

	while (1)
	{


		//--- Yes, I am treating a comma as white space.
		strcpy (czWhiteSpace, "\a\b\f\n\r\t\v ,");
		pntr1 = p_czpSourceString;

		//-	Search the source text for the version number.
		//-	In theory, there will be more than one "FormatVersion=\"" entry,
		//-   However; the most revent Format Version entry should always be first.
		if ((pntr1 = strstr (pntr1, 
			"FormatVersion=\"")) == NULL)
		{
			//-		The version number was not found.
			//-		This will be given a default version number.
			break;
		}

		if (*pntr1 == NULL)
		{
			hr = -1912;
			break;
		}

		//-	Go passed "FormatVersion\"".
		pntr1 = pntr1 + 15;

		if (*pntr1 == NULL)
		{
			hr = -1916;
			break;
		}

		//- VERSION AS ATTRIBUTE ----------------------------------------------------
		//-	Search this element entry for the Value attribute.
		//--- Move passed any white space.
		pntr1 = pntr1 + strspn (pntr1, czWhiteSpace);

		if (*pntr1 == NULL)
		{
			hr = -1913;
			break;
		}


		//-	It's now time to read the version number!!!
		//-	A base of 0 means that the function will automatically determine the 
		//-	base (of the input string).
		l_ulVersionNumber1 = strtol (pntr1, &pntr2, 0);

		if (*pntr2 != '.')
		{
			hr = -1914;
			break;
		}

		pntr2 = pntr2 + 1;

		if (*(pntr2 + 2) != '"')
		{
			hr = -1915;
			break;
		}

		l_ulVersionNumber2 = strtol (pntr2, &pntr1, 0);

		l_ulVersionNumber = (l_ulVersionNumber1 << 16) + (l_ulVersionNumber2 << 8);

		*p_ulpConfigurationVersion = l_ulVersionNumber;

		break;
	}

	return hr;
}


// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVConfigurationLibrary\SVOCMArchive.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:39:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.11   22 Jun 2007 10:02:22   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.10   20 Jun 2007 13:44:58   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.9   10 Oct 2005 09:27:42   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Removed Test Code for encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.8   22 Sep 2005 16:01:16   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Add choice to Save to prevent encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.7   16 Sep 2005 16:16:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Encryption was generically implemented through the SVXMLClass.  Because the configurations use this class, their interface to the SVXMLClass needed modified.  
* 
* Also, the configuration handling was modified to handle actual excrypted configurations because this was believed the best way to test the XML enryption.  Getting the version number needed enhanced since the part getting the version number has no encryption knowledge.
* 
* The following functions were modified:
*  SVOMArchive::CreateConfigurationFileFromTree ()
*  SVOMArchive::CreateTreeFromConfigurationFile ()
*  SVOMArchive::GetSVOSavedVersionNumberFromConfigurationFile (), and
*  SVOMArchive::FindVersion440_ ()
* 
* The following functions were added:
*  SVOMArchive::FindVersion440_Standard (), and
*  SVOMArchive::FindVersion440_RevisionHistory () 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:44:12   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:43:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   22 Oct 2004 07:27:28   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   In order to implement the new XML parser classes, load routines where re-written and version detection methods where implemented so that old configuration files would load correctly.
* 
* Added headers for referencing:
*   USES_CONVERSION,
*   SVXMLClass,
*   SVCTreeControlClass,
*   SVBStr.
* 
* Rewrote and exchanged...
*   CreateTreeFromConfigurationFile () 
*   for
*   BuildTreeFromXMLDoc (),
* and
*   CreateConfigurationFileFromTree ()
*   for
*   CreateXMLDocFromTree ().
* 
* Added member functions:
*   GetSVOSavedVersionNumberFromConfigurationFile (),
*   FindVersion401_430 (), and
*   FindVersion440_ ().
* 
* 
* 
* 
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   02 Oct 2003 10:43:02   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  386
* SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Assigned old light reference values that had never been set to a new flag value for processing in the loading process.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   29 Sep 2003 14:47:40   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Fixed archive code to only conditionally read new combined settings.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   25 Sep 2003 14:22:38   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Added new configuration flags to support combined digital outputs.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   09 Jan 2003 16:05:56   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  317
* SCR Title:  Load SEC improperly imports Light Reference data for RGB Color
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Modified the ArchiveSVLightReferenceArrayClass and ArchiveSVLightReferenceClass functions to take as a parameter whether the system is a RGB Color or not (bIsColorMode).
* In ArchiveSVLightReferenceClass, if setting a default value for RGB contrast, check this flag before assigning the value. If color mode, default value=10000 otherwise default is 8100.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.0   18 Nov 2002 09:56:16   ryoho
* Project:  SVObserver
* Change Request (SCR) nbr:  226
* SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
* Checked in by:  rYoho;  Rob Yoho
* Change Description:  
*   first version of the file
* 
* /////////////////////////////////////////////////////////////////////////////////////
*/
