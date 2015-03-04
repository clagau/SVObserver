// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBStr
// * .File Name       : $Workfile:   SVBStr.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 09:57:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVBStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// These are a few of Jim's current thoughts on Multi-threaded variable 
// access.
// Simple reads and writes to properly-aligned 32-bit variables are atomic. 
// In other words, when one thread is updating a 32-bit variable, you will 
// not end up with only one portion of the variable updated; all 32 bits are 
// updated in an atomic fashion.  This tells me that longs can be accessed 
// without locking, but doubles can not.


SVBStr::SVBStr()
{


// We are not going to pre-allocate here because of the lack of error 
// handling. ----------------------------------------------------------------
   svmlCurrentAllocationSize = 0;

   svmwcBStringBuffer = NULL;
   svmlpBStringLength = NULL;

   svmcpCharStringBuffer = NULL;
   svmlCharStringAllocationSize = 0;

	svmwcOverflow = 0;

   svmlLength = 0;

	m_lNULL = TRUE;

   InitializeCriticalSection(); 
}

SVBStr::~SVBStr()
{
   long* lTemp;
   char* cTemp;

   lTemp = svmlpBStringLength;
   cTemp = svmcpCharStringBuffer;

   EnterCriticalSection();

   svmwcBStringBuffer = NULL;
   svmlpBStringLength = NULL;
   svmcpCharStringBuffer = NULL;
   svmlLength = 0;

   delete [] lTemp;
   delete [] cTemp;

   LeaveCriticalSection();
   DeleteCriticalSection();

}


/*  Return a single wide character from the string.						      */
WCHAR SVBStr::operator[](int p_nIndex ) const
{
	long	l_lLength;

	WCHAR	l_wcReturnValue;


//-If an error occurs, it should result in a 0xffff (-1) being returned.  
//-Other than that, there is no way to detect an error condition.
	l_wcReturnValue = 0xffff;

	EnterCriticalSection();
	while (1)
	{
		l_lLength = InternalGetLength ();
		if (p_nIndex >= l_lLength)
		{
//-		Error;invalid index.
			break;
		}

		l_wcReturnValue = svmwcBStringBuffer [p_nIndex];
		break;
	}


	LeaveCriticalSection();

	return l_wcReturnValue;
}


WCHAR& SVBStr::operator[](int p_nIndex )
{

	long		hr;
	long		l_lLength;  // Number of WCHARs.

	unsigned long	l_lSizeOfWChar;

	WCHAR*	l_wcpReturnValue;


	l_wcpReturnValue = &svmwcOverflow;

	EnterCriticalSection();


	while (1)
	{
		hr = InternalPreAllocate (p_nIndex);
		if (hr & 0xc0000000)
		{
			break;
		}
		
		l_lSizeOfWChar = sizeof (WCHAR);
		
		l_lLength = InternalGetLength ();

		if (p_nIndex >= l_lLength)
		{
//-		We will put in a NULL terminator if we are exploring new space.  As 
//-		usual for this class, the NULL termination is not included in the 
//-		length value.
			svmlLength = p_nIndex + 1;
			*svmlpBStringLength = svmlLength * l_lSizeOfWChar;	
			svmwcBStringBuffer [p_nIndex + 1] = NULL;

			m_lNULL = FALSE;
		}
		
		l_wcpReturnValue = &(svmwcBStringBuffer [p_nIndex]);

		break;
	}

	LeaveCriticalSection();

	return *l_wcpReturnValue;
}


SVBStr::operator BSTR() 
{

	long	l_lLength;

	l_lLength = GetLength ();
	if ((l_lLength == 0) && (m_lNULL == TRUE))
	{
		return NULL;
	}
	else
	{
	   return svmwcBStringBuffer;
	}
}


SVBStr::operator const BSTR() const
{

	long	l_lLength;

	l_lLength = GetLength ();
	if ((l_lLength == 0) && (m_lNULL == TRUE))
	{
		return NULL;
	}
	else
	{
	   return svmwcBStringBuffer;
	}
}


SVBStr::operator BSTR*() 
{
   return &svmwcBStringBuffer;
}


SVBStr::operator const BSTR*() const
{
   return &svmwcBStringBuffer;
}


SVBStr::operator const char*() const
{
   USES_CONVERSION;

   EnterCriticalSection();
   if (svmlCharStringAllocationSize < svmlCurrentAllocationSize)
   {
      delete [] svmcpCharStringBuffer;

      (char*) svmcpCharStringBuffer = new char [svmlCurrentAllocationSize];
   }

	strncpy (svmcpCharStringBuffer, W2A (svmwcBStringBuffer), svmlLength);

	svmcpCharStringBuffer [svmlLength] = 0;

	LeaveCriticalSection();

   return svmcpCharStringBuffer;
}

const SVBStr& SVBStr::operator=(const SVBStr& stringSrc)
{
//   WCHAR wcsWideCharacterSource [svmlAllocationPageSize]; //Arvid 2015-01-08 was: 4000
//   long  lNbrOfCharacters;

   while (1)
   {

      stringSrc.EnterCriticalSection ();

//		wcsncpy (wcsWideCharacterSource, 
//             stringSrc.svmwcBStringBuffer, 
//             stringSrc.svmlLength);

      CopyFromWChar (stringSrc);

      stringSrc.LeaveCriticalSection ();

//      lNbrOfCharacters = sizeof (wcsWideCharacterSource) / sizeof (WCHAR);
//      wcsWideCharacterSource [lNbrOfCharacters - 1] = 0;

//      CopyFromWChar (wcsWideCharacterSource);

      break;
   }

	return *this;
}


const SVBStr& SVBStr::operator=(const TCHAR& stringSrc)
{
   WCHAR wcsWideCharacterSource [svmlAllocationPageSize]; //Arvid 2015-01-08 was: 4000
   long  lNbrOfCharacters;
   
   USES_CONVERSION;

   while (1)
   {
//-	This limits us to the svmlAllocationPageSize character wide char array. 
      lNbrOfCharacters = sizeof (wcsWideCharacterSource) / sizeof (WCHAR);
	   wcsncpy (wcsWideCharacterSource, 
               T2W(&((TCHAR) stringSrc)), 
               lNbrOfCharacters);

      wcsWideCharacterSource [lNbrOfCharacters - 1] = 0;

      CopyFromWChar (wcsWideCharacterSource);

      break;
   }

	return *this;
}


const SVBStr& SVBStr::operator=(const BSTR& stringSrc)
{
   USES_CONVERSION;

   while (1)
   {
		if (stringSrc == 0)
		{
			Clear ();
		}
		else
		{
			CopyFromWChar (stringSrc);
		}

      break;
   }

	return *this;
}


const SVBStr& SVBStr::operator=(const CString& stringSrc)
{
	long	lErr;

   WCHAR wcsWideCharacterSource [svmlAllocationPageSize];
//   long  lNbrOfBytes;
   long  lNbrOfCharacters;
   TCHAR*	czpStringSrc;
   
   USES_CONVERSION;

   czpStringSrc = NULL;

	lErr = 0;

   while (1)
   {
// jab 040703 - Help file shows return of number of bytes in one place and
// number of characters in another.  I've been told number of characters is
// correct.
//      lNbrOfBytes = stringSrc.GetLength ();
//-	CString GetLength does not include NULL, so we will add 1 for the NULL. 
      lNbrOfCharacters = stringSrc.GetLength () + 1;
	  //Arvid 2015-01-08 suggestion: use svmlAllocationPageSize rather than sizeof (wcsWideCharacterSource) / sizeof (WCHAR))
		if (lNbrOfCharacters > (sizeof (wcsWideCharacterSource) / sizeof (WCHAR)))
		{
//-		We must do -1 here because we will do +1 later.
			lNbrOfCharacters = (sizeof (wcsWideCharacterSource) / sizeof (WCHAR));
		}

		czpStringSrc = ((CString&) stringSrc).GetBuffer (0);
// jab 040703 - Help file indicated 3rd param is number of characters, 
// appears to be number of bytes.
//	   wcsncpy (wcsWideCharacterSource, 
//               T2W(czpStringSrc), 
//               ((lNbrOfBytes + 1) / sizeof (TCHAR)) + 1);// lNbrOfBytes does 
//                                                         //  not include 
//                                                         //  NULL.

	   wcsncpy (wcsWideCharacterSource, 
               T2W(czpStringSrc), 
               lNbrOfCharacters);

      wcsWideCharacterSource [lNbrOfCharacters-1] = 0; //Arvid 2015-01-08 added -1 to avoid "out of Bounds warning" from Cppcheck 

      lErr = CopyFromWChar (wcsWideCharacterSource);
		if (lErr & 0xc0000000)
		{
			break;
		}

      break;
   }

	return *this;
}


long SVBStr::CopyFromWChar (const WCHAR* awcpSource)
{
	long		lErr;

   long     lSourceLength;
//   long     lNumberOfPagesToAllocate;
//   long     lAllocationSize;
   long     lMaxNbrOfCharacters;
   long*    lpTemp;

	lpTemp = NULL;

	lErr = 0;

   while (1)
   {
		if (awcpSource == NULL)
		{
			lErr = 2;
			break;
		}

//--- This string length does not include the terminating NULL.
      lSourceLength = static_cast<long>(wcslen (awcpSource));

      EnterCriticalSection();

//--- If a new buffer was created, lpTemp will contain the address of the
//--- old buffer.  If it is neccessary to copy, then please do so.  The
//--- calling function MUST delete the returned pointer.  A returned 
//--- value of NULL means that a new buffer was not neccessary.
		lpTemp = DoWeNeedNewBuffer (lSourceLength * static_cast<long>(sizeof (WCHAR)));

      delete [] lpTemp;

		if ((lSourceLength > 0) && (svmlpBStringLength == NULL))
		{
			lErr = -1905;
			break;
		}

//-	Subract out 4 bytes for length at beginning of block and two bytes for 
//-	a terminating NULL.
      lMaxNbrOfCharacters = (svmlCurrentAllocationSize - 6) / sizeof (WCHAR);

	   wcsncpy (svmwcBStringBuffer, 
               awcpSource, 
               lMaxNbrOfCharacters);

 	   svmwcBStringBuffer [lMaxNbrOfCharacters] = NULL;

      svmlLength = static_cast<long>(wcslen (svmwcBStringBuffer));

      *svmlpBStringLength = svmlLength * sizeof (WCHAR);

		m_lNULL = FALSE;

      LeaveCriticalSection();

      break;
   }

   return lErr;
}



long SVBStr::GetLength () const
{
   long  lLength;

// We do not enter the critical section here because we believe that what we
// are currently doing merrits it, but to make sure that we are not locked 
// elsewhere performing operations on multiple pieces of related data.
   EnterCriticalSection();
   lLength = svmlLength;
   LeaveCriticalSection();

   return lLength;
}


long SVBStr::InternalGetLength () const
{
   return svmlLength;
}


long*	SVBStr::DoWeNeedNewBuffer (long	alSourceLength)
{
	long	hr;
	long	lNumberOfPagesToAllocate;
	long	lAllocationSize_Bytes;
	long	lAllocationSize_WCHARs;		
	
	long* lpTemp;

	lpTemp = NULL;

	while (1)
	{
	//--- The - 6 represents 4 bytes for the string length and 2 bytes for the 
	//--- terminating NULL character.
		if (alSourceLength > (svmlCurrentAllocationSize - 6))
		{
			lpTemp = svmlpBStringLength;

			svmlLength = 0;
			svmlpBStringLength = NULL;
			svmwcBStringBuffer = NULL;


			lNumberOfPagesToAllocate = 
				(((static_cast <long> (alSourceLength * 1.5) + 6) / svmlAllocationPageSize) + 1);


			lAllocationSize_Bytes = svmlAllocationPageSize * lNumberOfPagesToAllocate;
			lAllocationSize_WCHARs = lAllocationSize_Bytes / sizeof (WCHAR);
			svmlpBStringLength = (reinterpret_cast <long*> (new WCHAR [lAllocationSize_WCHARs]));

			svmlLength = 0;

			if (svmlpBStringLength == NULL)
			{
				svmlCurrentAllocationSize = 0;
				svmwcBStringBuffer = NULL;
			}
			else
			{
				svmlCurrentAllocationSize = lAllocationSize_Bytes;

	//------ The actual string buffer will always fall 4 bytes (2 wide character
	//------ elements) after the beginning of the allocated block of memory.
				svmwcBStringBuffer = (reinterpret_cast <WCHAR*> (svmlpBStringLength)) + 2;
				*svmlpBStringLength = 0;

				if (lpTemp == NULL)
				{
	//-			If the lpTemp was NULL when we started, then set the internal 
	//-			NULL flag.
					hr = InternalSetNULL ();
					if (hr & 0xc0000000)
					{
						break;
					}
				}
			} // if (svmlpBStringLength == NULL) ... else
		} // if (alSourceLength > (svmlCurrentAllocationSize - 6))

		break;
	} // while (1)

	return lpTemp;
}


long SVBStr::SetLength (long	p_lLength)
{

	while (1)
	{
		EnterCriticalSection();

		if (p_lLength >= (svmlCurrentAllocationSize / 
			               static_cast <long> (sizeof (WCHAR))) - 1)
		{
//-		This option requires allocation checks that already exist.
			(*this) [(int) p_lLength] = 0;
		}
		else
		{
//-		Set terminating null.  Will destroy existing character that is no 
//-      longer part of string.
			svmwcBStringBuffer [p_lLength] = 0;
			svmlLength = p_lLength;
			*svmlpBStringLength = p_lLength * sizeof (WCHAR);
		}

		break;
	}

   LeaveCriticalSection();

   return 0;
}


long SVBStr::PreAllocate (unsigned long p_ulNbrOfCharacters)
{
	long	hr;

	EnterCriticalSection();
	hr = InternalPreAllocate (p_ulNbrOfCharacters);
	LeaveCriticalSection();

	return hr;
}



long SVBStr::InternalPreAllocate (unsigned long p_ulNbrOfCharacters)
{
	long		hr;
	long		l_lMaxNbrOfCharacters;

	long*		l_lpTemp;

	unsigned long	l_lSizeOfWChar;



	hr = 0;

	while (1)
	{
		l_lSizeOfWChar = sizeof (WCHAR);

//-   If a new buffer was created, lpTemp will contain the address of the
//-   old buffer.  If it is neccessary to copy, then please do so.  The
//-   calling function MUST delete the returned pointer.  A returned 
//-   value of NULL means that a new buffer was not neccessary.
//-
//-   p_nIndex + 1. 
//-	  p_nIndex is the zero base index into the array and not the array 
//-	  size. The array size is p_nIndex + 1.  The +1 for
//-	  the NULL termination is dealt with within DoWeNeedNewBuffer ().
		l_lpTemp = DoWeNeedNewBuffer ((p_ulNbrOfCharacters + 1) * l_lSizeOfWChar);

		if (l_lpTemp != NULL)
		{
			if (svmlpBStringLength == NULL)
			{
//--------- If there was not enough room to allocate the correct buffer,
//--------- then the location of overflow will be handed back.
//				l_wcrReturnValue = svmwcOverflow;
				break;
			}
			else
			{
//--------- Subract out 4 bytes for length at beginning of block.
				l_lMaxNbrOfCharacters = (svmlCurrentAllocationSize - 4) / l_lSizeOfWChar;

//				wcsncpy (svmwcBStringBuffer, 
//							(WCHAR *) (l_lpTemp + 1), 
//							*l_lpTemp / l_lSizeOfWChar);

				memcpy (svmwcBStringBuffer, 
						  reinterpret_cast <WCHAR *> (l_lpTemp + 1), 
						  *l_lpTemp);

 				svmwcBStringBuffer [l_lMaxNbrOfCharacters - 1] = NULL;

				svmlLength = *l_lpTemp / l_lSizeOfWChar;
				*svmlpBStringLength = svmlLength * l_lSizeOfWChar;
				m_lNULL = FALSE;

			}
		} // if (l_lpTemp != NULL)
		else // if (l_lpTemp == NULL)
		{
		}

		break;
	}

   delete [] (reinterpret_cast <WCHAR*> (l_lpTemp));

	return hr;
}




long SVBStr::InitializeCriticalSection ()
{
   ::InitializeCriticalSection (&svmCriticalSection);
   return 0;
}


long SVBStr::EnterCriticalSection () const
{
   ::EnterCriticalSection (&((CRITICAL_SECTION) svmCriticalSection));
   return 0;
}


long SVBStr::LeaveCriticalSection () const
{
   ::LeaveCriticalSection (&((CRITICAL_SECTION) svmCriticalSection));
   return 0;
}


long SVBStr::DeleteCriticalSection ()
{
   ::DeleteCriticalSection (&svmCriticalSection);
   return 0;
}


long SVBStr::Clear ()
{
	long	hr;

   EnterCriticalSection();
	hr = InternalClear ();
   LeaveCriticalSection();

	return hr;
}



long SVBStr::InternalClear ()
{
   svmlLength = 0;

// If the main buffer block has been allocated, then set the size to 0 along
// with putting in the zero termination in the first position.
	if (svmlpBStringLength)
	{
		*svmlpBStringLength = svmlLength * 2;
		svmwcBStringBuffer [0] = NULL;
	}

	return 0;
}


long SVBStr::SetNULL ()
{
	long	hr;

   EnterCriticalSection();
	hr = InternalSetNULL ();
   LeaveCriticalSection();

	return hr;
}


long SVBStr::InternalSetNULL ()
{
	long	hr;

	hr = 0;

	while (1)
	{
		hr = InternalClear ();
		if (hr & 0xc0000000)
		{
			break;
		}

		m_lNULL = TRUE;
		break;
	}

	return hr;
}


long SVBStr::CharHexDump (char*	p_csHexDump, 
								  long	 p_lSizeOfDest)
{
	long	lErr;
	char*	l_cpIndex;

	long	l_lLoop;
	
	lErr = 0;

   EnterCriticalSection();

	l_cpIndex = p_csHexDump;

	l_cpIndex [0] = 0;

	for (l_lLoop = 0; l_lLoop < InternalGetLength (); l_lLoop++)
	{
		l_cpIndex = l_cpIndex + sprintf (l_cpIndex, "%d=<%x>", l_lLoop, svmwcBStringBuffer [l_lLoop]);
		if ((l_cpIndex - p_csHexDump) >= p_lSizeOfDest)
		{
			lErr = -1956;
			break;
		}
	}
   LeaveCriticalSection();


	return lErr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVBStr.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 09:57:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:31:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Oct 2010 08:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Oct 2007 15:13:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Operator= (CString, BSTR&, TCHAR, and SVBstr by  Modifications by JAB.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Oct 2005 10:47:54   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added CharHexDump () function to assist in debugging encryption problem.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2005 17:00:14   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  505
 * SCR Title:  Encrypt security file
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVBStr needed to be sygnificantly enhanced to support the XML encryption.
 * 
 * The following functions where modified:
 *  SVBStr::SVBStr ()
 *  SVBStr::operator const BSTR* ()
 *  SVBStr::operator = (const SVBStr&)
 *  SVBStr::operator = (const BSTR&)
 *  SVBStr::operator = (const BString&), and
 *  SVBStr::CopyFromWChar ()
 * 
 * SVBStr::operator BSTR () const has been removed.
 * 
 * The following functions have been added:
 *  SVBStr::WCHAR operator []
 *  SVBStr::WCHAR& operator []
 *  SVBStr::operator const BSTR () const
 *  SVBStr::operator BSTR* ()
 *  SVBStr::operator const BSTR* () const
 *  SVBStr::DoWeNeedNewBuffer ()
 *  SVBStr::SetLength ()
 *  SVBStr::PreAllocate ()
 *  SVBStr::InternalPreAllocate ()
 *  SVBStr::Clear ()
 *  SVBStr::InternalClear ()
 *  SVBStr::SetNULL (), and
 *  SVBStr::InternalSetNULL ()
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2004 09:23:20   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Refined the implementation of SVBStr to make it work within the new XML implementation.
 * 
 * Changed 
 *    long     CopyFromWChar (WCHAR* awcpSource)
 * to
 *    long     CopyFromWChar (const WCHAR* awcpSource).
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Apr 2003 15:53:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Bug in operator=(const CString& stringSrc) which was found when using nonUnicode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 13:31:50   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved to the SVLibrary Project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:15:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
