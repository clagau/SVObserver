// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Sep 2014 15:50:28  $
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

HRESULT SVOCMArchive::GetSVOSavedVersionNumberFromConfigurationFile (
	BSTR				abstrFileName, 
	unsigned long*	apulSVOConfigVersion)
{
	CFile				fConfigurationFile;

	CFileException	fileException;

	long				lFileOpen = FALSE;
	long				lNbrOfCharactersRead = 0;
	unsigned long	ulConfigurationVersion = 0;

	SVBStr			bstrConfigurationFileName;

	char				csBuffer [4000];

	HRESULT hr = 0;

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
		//-	Configuration file successfully opened.

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

	long		lFound = FALSE;
	long		lVersionNumber = 0;

	HRESULT	hr = 0;
	*aplConfigurationVersion = 0;

	while (1)
	{
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
	HRESULT					hr = S_OK;

	unsigned long			l_ulVersionNumber = 0;

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

		//-	Assumptions that can be made at this point....
		//-     Only XML files will get to this point.
		//-	  "<?xml" exists.

		hr = FindVersion440__RevisionHistory (apczSourceString,
			&l_ulVersionNumber);

		break;
	}

	if (l_ulVersionNumber != 0)
	{
		//-Any version number that shows less than 4.00 and greater than 14.00 
		//-will be considered invalid.  The 14.00 is an arbitrary large version 
		//-number which is used to test for obscenely bizarre data.
		if ((l_ulVersionNumber <  0x00040000) ||	// 0x00040000 =  4.00
			(l_ulVersionNumber >= 0x00140000))		// 0x00140000 = 14.00
		{
			hr = -1732;
		}
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

	long		lFound = FALSE;
	long		lInQuotes = FALSE;
	long		lVersionNumber = 0;

	HRESULT	hr = 0;
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

		//-	Because XML doesn't guarantee the order of the attribute entries, we 
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

	long				lFound = FALSE;

	unsigned long	l_ulVersionNumber = 0;
	unsigned long	l_ulVersionNumber1;
	unsigned long	l_ulVersionNumber2;

	HRESULT	hr = 0;
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

