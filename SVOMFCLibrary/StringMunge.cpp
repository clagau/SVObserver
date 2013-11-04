// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : StringMunge
// * .File Name       : $Workfile:   StringMunge.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:58:06  $
// ******************************************************************************

#include "stdafx.h"
#include <math.h>			// for ApproxMatch

#include "StringMunge.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool StringMunge::StripChars(CString* psString, const TCHAR* tszChars)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	// this works like the PERL statement tr/abc//   -or-    s/[abc]//g
	// it will strip all occurances of any char in tszChars out of psString
#if 1
	int t;
	bool bModified=false;

//	while ((t=strcspn(*psString, szChars)) < psString->GetLength())
	while ((t=_tcscspn(*psString, tszChars)) < psString->GetLength())
		{
		bModified=true;
		*psString=psString->Left(t)+psString->Mid(t+1);
		}
#else
	CString sTemp;
	TCHAR tszTemp[2] = {0,NULL};
	// go through each char one-by-one, see if it is on the list.
	// if it isn't, add to sTemp.
	int i,s = psString->GetLength();
	for (i=0; i<s; i++)
		{
		tszTemp[0] = psString->GetAt(i);
		if (_tcscspn(tszTemp, tszChars) != 0)
			{
			sTemp += tszTemp;
			}
		else
			bModified = true;
		}
	*psString = sTemp;

#endif

	return bModified;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool StringMunge::KeepChars(CString* psString, const TCHAR* tszChars)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	// works like PERL's     s/[^abc]//g
	// psString will keep only those chars in tszChars

	int t;
	bool bModified=false;

#if 1
//	while ((t=strspn(*psString, szChars)) < psString->GetLength())
	while ((t=_tcsspn(*psString, tszChars)) < psString->GetLength())
		{
		bModified=true;
		*psString=psString->Left(t)+psString->Mid(t+1);
		}
#else
	CString sTemp;
	TCHAR tszTemp[2] = {0,NULL};
	// go through each char one-by-one, see if it is on the list.
	// if it is, add to sTemp.
	int i,s = psString->GetLength();
	for (i=0; i<s; i++)
		{
		tszTemp[0] = psString->GetAt(i);
		if (_tcsspn(tszTemp, tszChars) != 0)
			{
			sTemp += tszTemp;
			}
		else
			bModified = true;
		}
	*psString = sTemp;
#endif

	return bModified;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool StringMunge::KeepAlphaNum(CString* psString)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
//	return KeepChars(psString, _T(" abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"));

	// optimize for speed; common characters first
	return KeepChars(psString, _T(" etoanishrdlumwgycfpbvkITAHSWj0192345678qxzBCDEFGJKLMNOPQRUVXYZ"));
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool StringMunge::TranslateChars(CString* psString, const TCHAR* tszFrom, const TCHAR* tszTo)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	// works like PERL's tr/abc/ABC/ command
	// tszFrom & tszTo must be the same length


//	if (strlen(szFrom) != strlen(szTo))
	if (_tcslen(tszFrom) != _tcslen(tszTo))
		return false;
	
	bool bModified=false;
	TCHAR tc;
	TCHAR tszTemp[2]=_T(" ");
	long nPos;
	int len=_tcslen(tszFrom);

	int i,s=psString->GetLength();
	for (i=0; i<s; i++)
		{
		tc=psString->GetAt(i);
		tszTemp[0]=tc;
//		if (((nPos=strcspn(szFrom, szTemp)) < len) && (nPos >=0) )	
		if (((nPos=_tcscspn(tszFrom, tszTemp)) < len) && (nPos >= 0) )	
			{
//			c=*(szTo+nPos);
			tc=tszTo[nPos];
			psString->SetAt(i,tc);
			bModified=true;
			}
		}
	return bModified;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool StringMunge::ReplaceStrings(CString* psString, int nNumElements, const TCHAR* tszFrom, const TCHAR* tszTo)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	// work around the bug in ReplaceStringsImpl... if nNumElements is 1
	CString s(tszTo);
	if (nNumElements != 1 || s.Find(tszFrom) == -1)
		return ReplaceStringsImpl(psString, nNumElements, tszFrom, tszTo);
	else
		{
		bool b = ReplaceStringsImpl(psString, 1, tszFrom, _T("†¤„"));
		if (b)
			ReplaceStringsImpl(psString, 1, _T("†¤„"), tszTo);
		return b;
		}
	}

/*static*/ bool StringMunge::ReplaceStringsImpl(CString* psString, int nNumElements, const TCHAR* tszFrom, const TCHAR* tszTo)
	{
	// will translate any set of chars in tszFrom (separated by NULLs) to the corresponding set of 
	// chars in tszTo (also separated by NULLs)
	// e.g.     ReplaceString(&sTemp, 2, _T("\n\0\t"), _T(", \0 "))
	// will translate carriage returns to comma space and tabs to a space.
	// the corresponding lists MUST contain the same number of sets of chars (no way to error check)
	// AND you MUST specify nNumElements correctly!!!
	//
	// Known Bug!:
	// if tszTo contains tszFrom, this will be caught in an endless loop.
	// e.g. tszFrom = _T("abc"), tszTo = _T("abcdef")
	//
	// Workaround: call function with dummy tszTo, then call with dummy as tszFrom and the real tszTo
	// e.g. ReplaceStrings(&sString, 1, _T("abc"), _T("¥"));
	//      ReplaceStrings(&sString, 1, _T("¥"), _T("abcdef"));
	//
	// Fix: ????

	bool bModified=false;
	int i,len;
	const TCHAR* ptszFromSet;
	const TCHAR* ptszToSet;
	long nFromPos;
	long nToPos;
	long nFoundPos;
	int nNumFound=1;

	while (nNumFound > 0)
		{
		nNumFound=nFromPos=nToPos=0;

		for (i=0; i<nNumElements; i++)
			{
//			pszFromSet=szFrom+nFromPos;
			ptszFromSet=&(tszFrom[nFromPos]);
//			pszToSet=szTo+nToPos;
			ptszToSet=&(tszTo[nToPos]);
			len=_tcslen(ptszFromSet)+1;
			nFromPos+=len;
			nToPos+=len;

			if ((nFoundPos=psString->Find(ptszFromSet)) >=0)
				{// found one of the sets; now translate
				*psString = psString->Left(nFoundPos) + ptszToSet + psString->Mid(nFoundPos + len - 1);
				nNumFound++;
				bModified=true;
				}
			}
		}

	return bModified;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/ double StringMunge::ApproxiMatch(const CString& s1, const CString& s2, double dPicky /*= 1.2*/)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	// by Eric Beyeler - Dec 7, 1998
	// ebeyeler@usa.com
	// extended from an algorithm by:
	// Jack Lynch, jlynch@english.upenn.edu. Written 11 December 1995 for Stuart Curran's English 205/505, "Electronic Literary Seminar." 
	// http://www.english.upenn.edu/~jlynch/Computing/compare.html
	
	// all of the const values were empirically determined.

	const double SPECIAL_ENDING_ADJUSTMENT	= 0.4;

	// MAXIMUM SCORES FOR 1-25 LETTER WORDS... USED FOR FAST COMPUTATION OF FINAL SCORE

	// this takes into account the language concept of root word; 
	// letters at the beginning of the word are given a higher significance
	// than towards the end of the word.
	const double dMaxScore[25]=
		{
		3,
		18,
		60,
		150,
		315,
		588,
		1008,
		1622,
		2475,
		3630,
		5168,
		7178,
		9755,
		13000,
		17020,
		21928,
		27843,
		34890,
		43200,
		52910,
		64183,
		77188,
		92100,
		109100,
		128375,
		};


	register int i, j, k, x, y;
	double dScore=0.0;
	bool bEnding=false;

	int nLength1 = s1.GetLength();
	int nLength2 = s2.GetLength();

	// special cases...
	if (nLength1 == 0 || nLength2 == 0)
		return 0.0;

	if (s1 == s2)
		return 1.0;

	// we don't handle words bigger than 25 chars (only limited by the dMaxScore array)
	if (nLength1 > 25 || nLength2 > 25)
		return -1.0;


	// check for common endings...
	CString sTemp;

	sTemp = s1+_T("s");
	if (sTemp == s2)
		bEnding = true;
	sTemp = s2+_T("s");
	if (sTemp == s1)
		bEnding = true;
	sTemp = s1+_T("es");
	if (sTemp == s2)
		bEnding = true;
	sTemp = s2+_T("es");
	if (sTemp == s1)
		bEnding = true;
	sTemp = s1+_T("ed");
	if (sTemp == s2)
		bEnding = true;
	sTemp = s2+_T("ed");
	if (sTemp == s1)
		bEnding = true;
	sTemp = s1+_T("ing");
	if (sTemp == s2)
		bEnding = true;
	sTemp = s2+_T("ing");
	if (sTemp == s1)
		bEnding = true;
	sTemp = s1+s1.Right(1)+_T("ing");
	if (sTemp == s2)
		bEnding = true;
	sTemp = s2+s2.Right(1)+_T("ing");
	if (sTemp == s1)
		bEnding = true;

	i = j = k = x = y = 0;

	const double POSITION_SCALE = 2.0;
	// calculate raw score
	for (i=0; i<nLength1; i++)
		{
		x = i;
		for (j=0; j<nLength2 && x<nLength1; j++)
			{
			y = j;
			k = 0;
			while ((x < nLength1 && y < nLength2) &&  s1[x] == s2[y])
				{
				k++;
				dScore += (double) (k*k);
				
				// position ranking...
				// rate matches towards the beginning of the word higher
				dScore += (double) POSITION_SCALE * ((nLength1-x)*(nLength2-y));

				x++;
				y++;
				}
			}
		}

	// calculate raw character difference

	BYTE arraysize = ~0;
	int naChar1[(BYTE)~0]={0}, naChar2[(BYTE)~0]={0};
	int nCharDiff=0;
	double dCharDiff=0.0;
	for (i=0; i<nLength1; i++)
		naChar1[(int)s1[i]]++;
	for (i=0; i<nLength2; i++)
		naChar2[(int)s2[i]]++;
	for (i=0; i<arraysize; i++)
		nCharDiff += abs(naChar1[i] - naChar2[i]);

//	const double CHARDIFF_SCALE = 1.2;
	double CHARDIFF_SCALE = dPicky;	// let the user modify this if they wish.
	dCharDiff = pow((double)nCharDiff, CHARDIFF_SCALE);
	double dMaxDiff = pow((double) (nLength1+nLength2), CHARDIFF_SCALE);
	dCharDiff = 1.0 - ((double) dCharDiff / dMaxDiff);



	//printf("%f\n", dScore);	// raw score

	// scale the score
	double dMaxScore1 = sqrt(dMaxScore[nLength1-1]);
	double dMaxScore2 = sqrt(dMaxScore[nLength2-1]);
	dScore = sqrt(dScore) / ((dMaxScore1+dMaxScore2)/2);
	//printf("%f\n", dScore);	// scaled score



	// take into account the character difference;
	const double CHARDIFF_MATCH_OFFSET = .2;
	if ((dScore > .3 && dScore < .75) && (dCharDiff > .5 || nCharDiff <= 2))
		dScore = pow(dScore, 1.0 - dCharDiff + CHARDIFF_MATCH_OFFSET);



	if (bEnding)
		dScore = pow(dScore, SPECIAL_ENDING_ADJUSTMENT);

	return dScore;	// final score
	}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\StringMunge.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:58:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/