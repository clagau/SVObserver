// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : StringMunge
// * .File Name       : $Workfile:   StringMunge.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:58:22  $
// ******************************************************************************

// StringMunge.h
// Part of StandardLibrary
// last modified 2002 08 15
//
// routines for doing things to strings that PERL makes easy
//
// History:
//
// 7-96			EB	Created
// 11-7-97		EB	Cleaned Up
// 8-26-98		EB	TCHAR
// 12-7-98		EB	ApproxMatch
// 2001 11 08	EB	encapsulated in StringMunge class
// 2002 08 15	EB	wrapped ReplaceStrings (workaround fix)
//


#ifndef STRINGMUNGE_H
#define STRINGMUNGE_H

class StringMunge
{
	public:
		
	static double ApproxiMatch(const CString& sString1, const CString& sString2, double dPicky=1.2);
	// returns a value {0.0 , 1.0} that rates how similar two strings are
	// a value of > .70 should indicate some similarity
	// a value of > .80 should indicate strong similarity
	// a value of > .90 should indicate almost the same word
	// values of < { .50 , .60 } indicate dissimilar words
	//
	// dPicky determines how it matches dissimilar words with similar letters.
	// a reasonable range is {0.7 - 1.5} with the low end MORE picky than the high end.
	// for example, sort and torso: a high dPicky value (less picky) will give
	// a higher match %, whereas a low dPicky value (more picky) will give
	// a lower match %

	static bool StripChars(CString* psString, const TCHAR* tszChars);
	// this works like the PERL statement tr/abc//   -or-    s/[abc]//g
	// it will strip all occurances of any char in szChars out of psString


	static bool KeepChars(CString* psString, const TCHAR* tszChars);
	// works like PERL's     s/[^abc]//g
	// psString will keep only those chars in szChars


	static bool KeepAlphaNum(CString* psString);
	// calls KeepChars with alphabet (upper & lower), digits, and space


	static bool TranslateChars(CString* psString, const TCHAR* tszFrom, const TCHAR* tszTo);
	// works like PERL's tr/abc/XYZ/ command
	// szFrom & szTo must be the same length


	static bool ReplaceStrings(CString* psString, int nNumElements, const TCHAR* tszFrom, const TCHAR* tszTo);
	// will translate any set of chars in tszFrom (separated by NULLs) to the corresponding set of 
	// chars in tszTo (also separated by NULLs)
	// e.g.     ReplaceString(&sTemp, 2, _T("\n\0\t"), _T(", \0 "))
	// will translate carriage returns to comma space and tabs to a space.
	// the corresponding lists MUST contain the same number of sets of chars (no way to error check)
	// AND you MUST specify nNumElements correctly!!!
	//

	private:

	static bool ReplaceStringsImpl(CString* psString, int nNumElements, const TCHAR* tszFrom, const TCHAR* tszTo);
	// 2002 08 15
	// This is made private and wrapped by a function that handles the following bug:
	// Known Bug!:
	// if tszTo contains tszFrom, this will be caught in an endless loop.
	// e.g. tszFrom = _T("abc"), tszTo = _T("abcdef")
	//
	// Workaround: call function with dummy tszTo, then call with dummy as tszFrom and the real tszTo
	// e.g. ReplaceStrings(&sString, 1, _T("abc"), _T("¥"));
	//      ReplaceStrings(&sString, 1, _T("¥"), _T("abcdef"));
	//
};

#endif

