//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLEncryptionClass
//* .File Name       : $Workfile:   SVXMLEncryptionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:51:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVXMLEncryptionClass.h"

#include "SVStatusLibrary/SVStatusCodes.h"

#include "SVDOM.h"		// SVDOMClass
#include "SVXMLBase.h"	// g_csSchema; g_wcsEncryption
#include "SVXMLLibraryGlobals.h"

const WCHAR*	g_wcsNameSeed				= L"NameSeed";
long	g_lTempDevelopment = 0;


long	SVXMLEncryptionClass::m_lLookUpTable1 [256] = 
{
/*   0 */	170, 153, 228, 254, 159, 197,  36, 193, 
/*   8 */	103, 180, 205, 151, 130,  45, 148, 106,
/*  16 */   177, 238, 225, 127,  92,   9,  86,   0,
/*  24 */	232, 147,  90,  20, 124,  40, 111, 143,
/*  32 */   185, 119,  68, 239,  96,  62, 172, 122, 
/*  40 */	227,  63, 195, 100, 237, 217,  50,  30,
/*  48 */	125, 209,  29, 221, 210, 213, 142, 145, 
/*  56 */	240, 116,  47, 233, 224, 196, 219, 189, 
/*  64 */   137,  34, 255,  74,  80, 138, 135, 236,
/*  72 */	 28, 156, 141, 220,   3, 191, 188, 208, 
/*  80 */	207, 146,  64,  60,  83, 203, 150, 206,
/*  88 */    78,  25,  42, 102, 231, 214, 163,  37,  
/*  96 */	 88,  54, 252, 104,   5,  13, 182,  18, 
/* 104 */	 70, 218,  52, 234, 139, 166, 175,  97,
/* 112 */	 89, 179, 230, 194,  51,  94, 250,  14, 
/* 120 */	118, 112,  58, 204, 149,  55, 165, 114,
/* 128 */	 95, 216,  24, 235,  79,  81,   1, 241,
/* 136 */	121,  46,   4, 242, 169, 108,  76, 162,
/* 144 */	244, 131,  12,   8,  43, 223,  15,  35,
/* 152 */	 16, 158,  27, 202, 184,  10, 136,  56,
/* 160 */	  7,  69, 134, 109, 161, 152, 253,  39,
/* 168 */	 19, 157, 115,  87, 200,  73, 211,  41,
/* 176 */	105, 178,  17,  72, 183, 226,  67,  66,
/* 184 */	 57,  32,  48, 173, 215,  26, 120,  21,
/* 192 */	 99,   6, 190, 249, 248,  84,  59, 155,
/* 200 */	 65,  11, 245, 201,  71, 132, 176, 174,
/* 208 */	 75, 107, 154, 212, 243,  23,  93, 126,
/* 216 */	171, 133, 164, 144,  82, 192,  98, 128,
/* 224 */     2,  53,  22, 101, 246,  77, 229, 222,
/* 232 */	160, 113, 117, 167,  33, 123, 140, 247,
/* 240 */	198,  38, 186,  49, 251, 187, 181,  31,
/* 248 */	129,  61,  91,  85,  44, 110, 199, 168
};


unsigned char SVXMLEncryptionClass::m_cCharTable [128] = 
{
//-                            1   1   1
//-                9   9   9   0   0   0
//-                7   8   9   0   1   2
/*   0 - 009 */	'a','b','c','d','e','f','g','h','i','j',
//-
//-                                    1           1
//-                                    1           1
//-                                    2           5
/*  10 - 019 */	'k','l','m','n','o','p','q','r','s','t',
//-
//-
//-                                        6
//-                                        5
/*  20 - 029 */   'u','v','w','x','y','z','A','B','C','D',
/*  30 - 039 */   'E','F','G','H','I','J','K','L','M','N',
/*  40 - 049 */	'O','P','Q','R','S','T','U','V','W','X',
//-
//-
//-                        6   4
//-                        4   8
/*  50 - 059 */	'Y','Z','@','0','1','2','3','4','5','6',
//-
//-                            1
//-                            3
//-                            2	
/*  60 - 063 */	'7','8','9','ä',
//-
//-                        2   
//-                3   3   5   2   4       4           5
//-                3   6   3   1   2       5           8
/*  64 - 073 */   '!','$','²','§','*','+','-','.','/',':',
//-
//-                        2   2   2   2   2   2       2
//-                5   6   5   4   3   4   4   3   1   4
//-                9   1   4   8   0   1   5   3   5   4
/*  74 - 083 */   ';','=','¦','°','µ','±',')','÷','¤','(',
//-
//-                2   1   2   1               1
//-                3   2   3   2               3
//-                7   6   8   8               3
/*  84 - 093 */   'ë','~','è','Ç','ü','é','â','à','å','ç',
//-
//-                1           1           1
//-                3   3   9   3           4           9
//-                6   8   1   9           2           2
/*  94 - 103 */	'ê','&','[','ï','î','ì','Ä','Å','É','\\',
//-
//-                1
//-                4
//-                6
/* 104 - 113 */   'Æ','ô','ö','ò','û','ù','ÿ','Ö','Ü','¢',
//-
//-                1           1                       1
//-                5       2   6                       6
//-                6       0   0                       6
/* 114 - 123 */   '£','¥','¶','á','í','ó','ú','ñ','º','ª',
//-
//-                            0
//-                9   3       1
//-                3   7       2
/* 124 - 127 */	']','%','ß','?'

};





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXMLEncryptionClass::SVXMLEncryptionClass()
{
	Init ();
}

SVXMLEncryptionClass::~SVXMLEncryptionClass()
{

}


HRESULT SVXMLEncryptionClass::Init ()
{
	HRESULT		hr;
	long			l_lCounter;

	hr = S_OK;

	while (1)
	{
		m_opParentDOM = NULL;
		m_opParentXML = NULL;

		m_lEncryptionMethod = 0;
		m_ulNameSeed = 0;
		m_lIsEncrypted = FALSE;

		m_lInitializedXMLInterface = FALSE;
		m_lInitializedDOMInterface = FALSE;


//-   Load map to do reverse encoding of m_cCharTable.
		for (l_lCounter = 0; l_lCounter < 128; l_lCounter = l_lCounter + 1)
		{
			m_oRevCharTableMap [m_cCharTable [l_lCounter]] = l_lCounter;
		}

//-   Load map to do reverse encoding of m_lLookUpTable1.
		for (l_lCounter = 0; l_lCounter < 256; l_lCounter = l_lCounter + 1)
		{
			m_oRevLookUpTable1Map [m_lLookUpTable1 [l_lCounter]] = l_lCounter;
		}


#ifdef _DEBUG
		for (l_lCounter = 0; l_lCounter < 20; l_lCounter++)
		{
			TRACE ("Counter = %d\n", l_lCounter);
			hr = Test ();
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				ASSERT (0);
				break;
			}
		}
#endif

		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::Test ()
{
	HRESULT	hr;

	long				l_lCharTableConversion;
	long				l_lLookUpTable1Conversion;
	long				l_lReverseConversion;
	long				l_lCounter;

	unsigned long	l_ulSeedValue;



	hr = S_OK;

	while (1)
	{
//-	Test of char table conversion and reverse conversion.
		for (l_lCounter = 0; l_lCounter < 128; l_lCounter++)
		{
			l_lCharTableConversion = m_cCharTable [l_lCounter];
			l_lReverseConversion = m_oRevCharTableMap [l_lCharTableConversion];

			if (l_lCounter != l_lReverseConversion)
			{
//-			Conversion/ReverseConversion does not revert to the original 
//-			value.
				ASSERT (0);
			}
		}


//-	Test of char table conversion and reverse conversion.
		for (l_lCounter = 0; l_lCounter < 256; l_lCounter++)
		{
			l_lLookUpTable1Conversion = m_lLookUpTable1 [l_lCounter];
			l_lReverseConversion = m_oRevLookUpTable1Map [l_lLookUpTable1Conversion];

			if (l_lCounter != l_lReverseConversion)
			{
//-			Conversion/ReverseConversion does not revert to the original 
//-			value.
				ASSERT (0);
			}
		}


//-	Test encryption decryption method 1.

		SVBStr l_bstrSourceString;
		SVBStr l_bstrEncryptedString;
		SVBStr l_bstrDecryptedString;


		hr = l_bstrSourceString.CopyFromWChar (L"breath");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (1,		// Encryption method 1.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}


		hr = l_bstrSourceString.CopyFromWChar (L"dragon1");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (1,		// Encryption method 1.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}


		hr = l_bstrSourceString.CopyFromWChar (L"12²Money");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (1,		// Encryption method 1.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = l_bstrSourceString.CopyFromWChar (L"Türkey1A2");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (1,		// Encryption method 1.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = l_bstrSourceString.CopyFromWChar (L"Once upon a time I wrote a string.  The purpose of the string was to test the program.  This test must encrypt and decrypt this string.  What happens if the string contains a tab? /t Hopefully all still goes well.  The End.");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (1,		// Encryption method 1.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
			break;
		}


//-	Start encryption method 2.


		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue;

//-   6 characters
		hr = l_bstrSourceString.CopyFromWChar (L"breath");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}


//-   7 characters, ending in a digit.
		hr = l_bstrSourceString.CopyFromWChar (L"dragon1");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue;

		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue;

		hr = l_bstrSourceString.CopyFromWChar (L"12²Money");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue;

		hr = l_bstrSourceString.CopyFromWChar (L"Türkey1A2");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue;

		hr = l_bstrSourceString.CopyFromWChar (L"Once upon a time I wrote a string.  The purpose of the string was to test the program.  This test must encrypt and decrypt this string.  What happens if the string contains a tab? /t Hopefully all still goes well.  The End.");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
			break;
		}


		hr = l_bstrSourceString.CopyFromWChar (g_wcsBaseNode);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = 302106898;
		hr = EncryptString (2,		// Encryption method 2.
								  l_bstrSourceString, 
								  &l_bstrEncryptedString);

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptString (l_bstrEncryptedString, 
								  &l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
			break;
		}

		m_ulNameSeed = 0;


		hr = l_bstrSourceString.CopyFromWChar (L"breath");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptNameAttribute (l_bstrSourceString, 
											&l_bstrEncryptedString);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptNameAttribute (l_bstrEncryptedString, 
											&l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}


		hr = l_bstrSourceString.CopyFromWChar (L"dragon1");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptNameAttribute (l_bstrSourceString, 
											&l_bstrEncryptedString);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptNameAttribute (l_bstrEncryptedString, 
											&l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}


		hr = l_bstrSourceString.CopyFromWChar (L"12²Money");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptNameAttribute (l_bstrSourceString, 
											&l_bstrEncryptedString);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptNameAttribute (l_bstrEncryptedString, 
											&l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = l_bstrSourceString.CopyFromWChar (L"Türkey1A2");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = EncryptNameAttribute (l_bstrSourceString, 
											&l_bstrEncryptedString);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptNameAttribute (l_bstrEncryptedString, 
											&l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
		}

		hr = l_bstrSourceString.CopyFromWChar (L"Once upon a time I wrote a string.  The purpose of the string was to test the program.  This test must encrypt and decrypt this string.  What happens if the string contains a tab? /t Hopefully all still goes well.  The End.");
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		hr = EncryptNameAttribute (l_bstrSourceString, 
											&l_bstrEncryptedString);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			ASSERT (0);
			break;
		}

		hr = DecryptNameAttribute (l_bstrEncryptedString, 
											&l_bstrDecryptedString);

		if (wcscmp (l_bstrSourceString, l_bstrDecryptedString) != 0)
		{
//-		Source and decrypted string not equal.
			ASSERT (0);
			break;
		}

		break;
	}

	return hr;
}





HRESULT SVXMLEncryptionClass::InitializeXMLInterface (SVXMLBase*		p_opParentXML)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		if (m_lInitializedXMLInterface == TRUE)
		{
			hr = 2;
			break;
		}

		if (p_opParentXML == NULL)
		{
			hr = -1891;
			break;
		}

		m_opParentXML = p_opParentXML;

		m_lInitializedXMLInterface = TRUE;

		m_lIsEncrypted = FALSE;
		m_lEncryptionMethod = 0;
		m_ulNameSeed = 0;

		break;
	}

	return hr;

}


HRESULT SVXMLEncryptionClass::InitializeDOMInterface (SVDOMClass*		p_opParentDOM)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		if (m_lInitializedDOMInterface == TRUE)
		{
			hr = 2;
			break;
		}

		if (p_opParentDOM == NULL)
		{
			hr = -1892;
			break;
		}

		m_opParentDOM = p_opParentDOM;

		m_lInitializedDOMInterface = TRUE;

		m_lIsEncrypted = FALSE;
		m_lEncryptionMethod = 0;
		m_ulNameSeed = 0;

		break;
	}

	return hr;

}


HRESULT SVXMLEncryptionClass::SetEncryption ()
{

	HRESULT	hr;

	unsigned long					l_ulSeedValue;

	CComVariant						l_ccvAttributeValue;

	VARIANT						   l_vElementValue;

	SVBStr							l_bstrEncryptionBranchName;
	SVBStr							l_bstrAttributeName;
	SVBStr							l_bstrNameSeedName;

	SVXML::IXMLDOMElementPtr	l_oEncryptionElementPtr;
	SVXML::IXMLDOMElementPtr	l_oRootElementPtr;
	SVXML::IXMLDOMElementPtr	l_oDOMDataPtr;

	hr = S_OK;

	while (1)
	{
		if (m_lInitializedXMLInterface == FALSE)
		{
			hr = -1899;
			break;
		}

		hr = GetEncryptionBaseNode (l_oEncryptionElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}
		else 
		if (hr != 2)
		{
//-		Encryption base node was found.  Not good.
//-		Encryption base node already exists.
			hr = 3;
			break;
		}
		else
		{
//-	hr = 2 means that the base node was not found.  This is good in this case.
			hr = 0;
		}


		m_lIsEncrypted = TRUE;


//-	Add Encryption branch base node.
//-	  <Encryption xmlns="x-schema:#SVR00001" IsActive="TRUE">
		hr = l_bstrEncryptionBranchName.CopyFromWChar( g_wcsEncryption );
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_opParentXML->CreateElement (l_bstrEncryptionBranchName, 
							       				  l_oEncryptionElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		hr = l_bstrAttributeName.CopyFromWChar (g_wcsIsActive);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_ccvAttributeValue = L"TRUE";

		l_oEncryptionElementPtr->setAttribute ((BSTR) l_bstrAttributeName, l_ccvAttributeValue);
		l_ccvAttributeValue.Clear ();

		hr = m_opParentXML->GetRootNode (l_oRootElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (l_oRootElementPtr == NULL)
		{
			hr = -1896;
			break;
		}

//-	Add "NameSeed" data element.
//-	  <DATA Name="#######" Type="######">######</DATA>
//-

		hr = l_bstrNameSeedName.CopyFromWChar (g_wcsNameSeed);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = CreateNameSeed (&l_ulSeedValue);
//		hr = CreateRandomLong (&l_lSeedValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

 		l_vElementValue.lVal = l_ulSeedValue;
		l_vElementValue.vt = VT_UI4;

//-   -1 will show that we are in encryption setup, where all attributes 
//-	should be encoded using Method 1.
		hr = SetEncryptionMethod (-1);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_opParentXML->CreateDOMData (l_oDOMDataPtr, l_bstrNameSeedName, &l_vElementValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = SetEncryptionMethod (0);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = m_opParentXML->AppendChildToDOMNode (l_oEncryptionElementPtr, l_oDOMDataPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		hr = m_opParentXML->AppendChildToDOMNode (l_oRootElementPtr, l_oEncryptionElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulSeedValue; // Name attribute encryption method 0

TRACE ("SetEncryption(1) - NameSeed written - %d\n", m_ulNameSeed);
		break;
	} // while (1)



	return hr;
}


HRESULT SVXMLEncryptionClass::LoadEncryption ()
{
	HRESULT							hr;

	long								l_lNameSeedCounter;
	long								l_lNbrOfChildren;
	long								l_lCounter;

	unsigned long					l_ulNameSeed;

	WCHAR*							l_wcpTmp1;

	SVBStr							l_bstrDecryptedName;
	SVBStr							l_bstrDecryptedNameSeed;

	_bstr_t							l_bstrValue;
	_bstr_t							l_bstrIsActiveAttribute;
	_bstr_t							l_bstrNameTitle;

	_variant_t						l_vIsActiveValue;
	_variant_t						l_vName;

	SVXML::IXMLDOMElementPtr	l_oEncryptionBaseNodePtr;
	SVXML::IXMLDOMElementPtr	l_oChildPtr;
	SVXML::IXMLDOMElementPtr	l_oOldPtr;


	hr = S_OK;
	l_lNameSeedCounter = 0;

	while (1)
	{
		if (m_lInitializedXMLInterface != TRUE)
		{
			hr = -1924;
			break;
		}

		if (m_lInitializedDOMInterface != TRUE)
		{
			hr = -1936;
			break;
		}


		hr = GetEncryptionBaseNode (l_oEncryptionBaseNodePtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


		if (l_oEncryptionBaseNodePtr == NULL)
		{
			m_lIsEncrypted = FALSE;
			hr = 3;	// No encryption section available.
			break;
		} // if (l_oEncryptionBaseNodePtr == NULL)


		l_bstrIsActiveAttribute = g_wcsIsActive;
		l_vIsActiveValue = l_oEncryptionBaseNodePtr->getAttribute (l_bstrIsActiveAttribute);

		if (l_vIsActiveValue.vt != VT_BSTR)
		{
			hr = -1925;
			break;
		}
	
		l_wcpTmp1 = l_vIsActiveValue.bstrVal;

		if (wcscmp (l_wcpTmp1, L"TRUE") != 0)
		{
//-		Found string does not equal "TRUE".
			m_lIsEncrypted = FALSE;
			break;
		}

//-	At this point we've determined that the file is encrypted.
		m_lIsEncrypted = TRUE;

		hr = m_opParentDOM->GetElementNbrOfChildren (l_oEncryptionBaseNodePtr, &l_lNbrOfChildren);

		for (l_lCounter = 0; l_lCounter < l_lNbrOfChildren; l_lCounter++)
		{
			if (l_lCounter == 0)
			{
//-			First time through.
				l_oChildPtr = m_opParentDOM->GetFirstElementChild(l_oEncryptionBaseNodePtr);
				if (l_oChildPtr == NULL)
				{
					hr = -1926;
					break;
				}
			}
			else
			{
				l_oChildPtr = m_opParentDOM->GetNextElementSibling(l_oOldPtr);
				if (l_oChildPtr == NULL)
				{
					hr = -1927;
					break;
				}
			}

			l_bstrNameTitle = g_wcsName;
			l_vName = l_oChildPtr->getAttribute (l_bstrNameTitle);

			if (l_vName.vt == VT_EMPTY)
			{
				hr = -1928;
				break;
			}
			else
			if (l_vName.vt != VT_BSTR)
			{
				hr = -1929;
				break;
			}
			
			hr = DecryptString (l_vName.bstrVal, &l_bstrDecryptedName);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}


			if (wcscmp (l_bstrDecryptedName, g_wcsNameSeed) == 0)
			{
//-			There should only be one name seed.
				if (l_lNameSeedCounter > 0)
				{
					hr = -1930;
					break;
				}
				else
				{
					l_lNameSeedCounter = l_lNameSeedCounter + 1;
				}

				l_bstrValue = l_oChildPtr->Gettext ();
				hr = DecryptString (l_bstrValue, &l_bstrDecryptedNameSeed);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				l_ulNameSeed = wcstol (l_bstrDecryptedNameSeed, NULL, 0);

			}
			else
			{
//-			If more type of records exist, then they would be added to this 
//-			"else if" list.
				hr = -1931;
			}


			l_oOldPtr = l_oChildPtr;
		} // for (l_lCounter = 0; ....

		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		m_ulNameSeed = l_ulNameSeed;

TRACE ("LoadEncryption(1) - Loaded NameSeed = %d\n", m_ulNameSeed);


		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::GetIsEncrypted (long*	p_lpIsEncrypted)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		*p_lpIsEncrypted = m_lIsEncrypted;
		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::GetEncryptionBaseNode (
								SVXML::IXMLDOMElementPtr& p_orEncryptionBaseNodePtr)
{
	HRESULT							hr;

	SVXML::IXMLDOMElementPtr	l_oRootElementPtr;

	WCHAR							   l_wcsNameSpace [128];

	_bstr_t							l_bstrSelectName;

	SVBStr    						l_bstrQueryNameSpace;


	hr = 0;

	while (1)
	{
		if (m_lInitializedXMLInterface == FALSE)
		{
			hr = -1890;
			break;
		}


		hr = m_opParentXML->GetRootNode (l_oRootElementPtr);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (l_oRootElementPtr == NULL)
		{
			hr = -1895;
			break;
		}

		hr = m_opParentXML->GetNameSpace (l_wcsNameSpace, sizeof (l_wcsNameSpace));
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	xmlns:SVR1="x-schema:#SVR00001"
		l_bstrQueryNameSpace = (BSTR)((_bstr_t) g_csXMLNS + ":SVR1=\"x-schema:#" + (_bstr_t) g_csSVR00001SchemaName + "\"");

		hr = m_opParentXML->SetQueryNameSpace (l_bstrQueryNameSpace);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-	/Root/SVR1:RevisionHistory  -  XPath string
		l_bstrSelectName = (_bstr_t) L"/" + g_wcsRoot + L"/SVR1:" + g_wcsEncryption;

//-	A FEW WORDS ABOUT selectSingleNode (). --------------------------------
//-	We use the XPath standard to define our search.   This requires that 
//-	we call setProperty ("SelectionLanguage","XPath"), prior to calling 
//-	selectSingleNode ().  This is now getting done within 
//-	SVDOMClass::InitializeDOM () which is accessed from SVXML by calling
//-	SVDOMClass::Initialize (), which happens in SVXML::Initialize ().  
//-	There is no reason for this value to be set or changed to anything 
//-	other than XPath.
//-
//-   We also must register all namespaces used in the DOM, separately for 
//-   the XPath query functions.  This is done with a call to 
//-	setProperty("SelectionNamespaces", <ccvProperty>) which is currently 
//-   being done imediately prior to the query.
		p_orEncryptionBaseNodePtr = l_oRootElementPtr->selectSingleNode (l_bstrSelectName);

		if (p_orEncryptionBaseNodePtr == NULL)
		{
//-		No revision history base node found.
			hr = 2;
		}

		break;
	} // while (1)

	return hr;
}


HRESULT SVXMLEncryptionClass::CreateRandomLong (unsigned long* p_ulpSeedValue)
{
	HRESULT		hr;

	long			l_lTime;
	long			l_lClock;
	long			l_lSeed;
	long			l_lRand1;
	long			l_lRand2;
	long			l_lRand3;
	long			l_lTemp1;
	long			l_lTemp2;

// l_lStaticValue -----------------------------------------------------------
// This value is never initialized, so it contains what it contains.
	static long	l_lStaticValue;


	hr = S_OK;

	while (1)
	{	
		l_lTime = (long)time (NULL);
		l_lClock = clock ();

		l_lSeed = (l_lTime << 4) + l_lClock + l_lStaticValue;

		srand (l_lSeed);

		l_lRand1 = rand ();

		l_lRand2 = rand ();

		l_lRand3 = rand ();

		l_lTemp1 = l_lRand2 & 0x6AAA;
		l_lTemp2 = l_lRand3 & 0x5557;

//  R = Rand

//  33322222222221111111111000000000
//  21098765432109876543210987654321
//  --------------------------------
//  RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
//  11111111111111122323232323232333

		*p_ulpSeedValue = l_lRand1 << 17 | (l_lTemp1 << 2) | l_lTemp2;

//TRACE ("CreateRandomLong, seed = %lx, rand = %lx\n", l_lSeed, *p_ulpSeedValue);

		l_lStaticValue = l_lStaticValue + l_lRand2;

		break;
	} // while (1)

	return hr;
}


HRESULT SVXMLEncryptionClass::SetEncryptionMethod (long	p_lEncryptionMethod)
{
	m_lEncryptionMethod = p_lEncryptionMethod;
	return S_OK;
}


HRESULT SVXMLEncryptionClass::EncryptNameAttribute (BSTR		p_bstrSourceString, 
															       SVBStr*	p_bstrpEncryptedString)
{
	HRESULT	hr;

	long			   l_lCounter;
	long				l_lCounter2;
	long				l_lSourceStringLength;
	long				l_lMethodMod;
	long				l_lLeftShift;
	long				l_lRightShift;
	long				l_lDestinationLength;
	long				l_lPartialStringLength;

	unsigned long	l_ulTempValue;
	unsigned long	l_ulTableIndex;

	unsigned char*	l_ucpIndex;
	unsigned char*	l_ucpSingleChar;

	unsigned char*	l_ucpPartialEncryption1;
	unsigned char*	l_ucpPartialEncryption2;

//	unsigned char	l_ucEncryptedValue;
	unsigned char  l_ucLeftShiftValue;
	unsigned char  l_ucRightShiftValue;

	hr = S_OK;

	l_ucpPartialEncryption1 = NULL;
	l_ucpPartialEncryption2 = NULL;

	while (1)
	{
		hr = p_bstrpEncryptedString->Clear ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (p_bstrSourceString == NULL)
		{
			break;
		}

		l_lSourceStringLength = SysStringLen (p_bstrSourceString);


//-	This will be encrypted per word (character in this case).


		l_lMethodMod = m_ulNameSeed % 23;

//-	The 1st of 23 encryption methods.
		switch (l_lMethodMod)
		{
		case 0:
			{
//-			We will fource a standard character pointer to point to the wide 
//-			character string.
				l_ucpIndex = (unsigned char*) p_bstrSourceString;

				l_lPartialStringLength = (l_lSourceStringLength + 2) / 3 * 3;

				l_ucpPartialEncryption1 = new unsigned char [l_lPartialStringLength];
				l_ucpPartialEncryption2 = new unsigned char [l_lPartialStringLength];

//-			The + 2 should force it to round up.
				l_lDestinationLength = ((l_lSourceStringLength + 2) / 3) * 4;

//				for (l_lCounter = 0; l_lCounter < (l_lSourceStringLength); l_lCounter++)
				for (l_lCounter = 0; l_lCounter < (l_lPartialStringLength); l_lCounter++)
				{
					if (l_lCounter < l_lSourceStringLength)
					{
						l_ucpSingleChar = &(l_ucpIndex [l_lCounter * 2]);
					}
					else
					{
						l_ucpSingleChar = (unsigned char*) "";
					}

//-				Look up the individual byte values in the lookup table.
					l_ucpPartialEncryption1 [l_lCounter] = (char) m_lLookUpTable1 [(*l_ucpSingleChar + l_lCounter) & 0x000000ff];

//-				Shift the word (part of the encryption).
					l_lLeftShift = (l_lCounter + 3) % 8;
					l_lRightShift = 8 - l_lLeftShift;
					l_ucLeftShiftValue = l_ucpPartialEncryption1 [l_lCounter] << l_lLeftShift;
					l_ucRightShiftValue = l_ucpPartialEncryption1 [l_lCounter] >> l_lRightShift;

					l_ucpPartialEncryption2 [l_lCounter] = (unsigned char)(l_ucLeftShiftValue + l_ucRightShiftValue + l_lCounter + m_ulNameSeed) & 0x000000ff;
				}




				for (l_lCounter = 0; l_lCounter < l_lPartialStringLength; l_lCounter = l_lCounter + 3)
				{
					l_ulTempValue = *((unsigned long*) &(l_ucpPartialEncryption2 [l_lCounter]));

//-				The significance of 39 hex, is that it is the "partial 
//-				encryption" result for 0 (NULL).  If the partial encryption 
//-				algorithms change, then this value must also change to the new 
//-				NULL value.
/*					if (l_lCounter + 1 == l_lSourceStringLength)
					{
						l_ulTempValue = l_ulTempValue & 0x000000ff;
					}
					else
					if (l_lCounter + 2 == l_lSourceStringLength)
					{
						l_ulTempValue = l_ulTempValue & 0x0000ffff;
					}
					else
					{
						l_ulTempValue = l_ulTempValue & 0x00ffffff;
					}
					*/

					for (l_lCounter2 = 0; l_lCounter2 < 4; l_lCounter2++)
					{
						l_ulTableIndex = l_ulTempValue >> (l_lCounter2 * 6) & 0x0000003f;

						(*p_bstrpEncryptedString) [(int) (l_lDestinationLength - ((l_lCounter / 3) * 4 + l_lCounter2 + 1))] = m_cCharTable [l_ulTableIndex]; 
					}
				} // for (l_lCounter = 0; l_lCounter < l_lSourceStringLength; l_lCounter = l_lCounter + 3)


				break;
			}
		default:
			{
				hr = -1907;
				break;
			}
		}

		break;
	}

	delete [] l_ucpPartialEncryption1;
	delete [] l_ucpPartialEncryption2;

	return hr;
}


HRESULT SVXMLEncryptionClass::DecryptNameAttribute (BSTR		p_bstrEncryptedString, 
																	 SVBStr*	p_bstrpDecryptedString)
{
	HRESULT		hr;

	long				l_lMethodMod;
	long				l_lCounter;
	long				l_lCounter2;
	long				l_lPartialIndex;
	long				l_lDecryptedStringLength;
	long				l_lNewStringLength;
	long				l_lEncryptedStringLength;
	long				l_lRightShift;
	long				l_lLeftShift;

	unsigned long	l_ulTempValue;
	unsigned long	l_ulRevTableIndex;

	unsigned char	l_ucTempValue;
	unsigned char	l_ucLeftShiftValue;
	unsigned char	l_ucRightShiftValue;
	unsigned char*	l_ucpPartialDecryption;

	hr = S_OK;
	l_ucpPartialDecryption = NULL;

	while (1)
	{
		hr = p_bstrpDecryptedString->Clear ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (p_bstrEncryptedString == NULL)
		{
			hr = p_bstrpDecryptedString->SetNULL ();
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		l_lEncryptedStringLength = SysStringLen (p_bstrEncryptedString);

//-	If the input encrypted string is empty.
		if (l_lEncryptedStringLength == 0) 
		{
			hr = p_bstrpDecryptedString->CopyFromWChar (L"");
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		l_lMethodMod = m_ulNameSeed % 23;

		switch (l_lMethodMod)
		{
		case 0:
			{
				l_lDecryptedStringLength = (l_lEncryptedStringLength / 4) * 3;

				l_ucpPartialDecryption = new unsigned char [l_lDecryptedStringLength];

				for (l_lCounter = 0; 
				     l_lCounter < l_lEncryptedStringLength; 
					  l_lCounter = l_lCounter + 4)
				{
					l_ulTempValue = 0;

					for (l_lCounter2 = 0; l_lCounter2 < 4; l_lCounter2++)
					{
						l_ulRevTableIndex = m_oRevCharTableMap [p_bstrEncryptedString [l_lEncryptedStringLength - (l_lCounter + l_lCounter2 + 1)]];
						l_ulTempValue = l_ulTempValue +  (l_ulRevTableIndex << (l_lCounter2 * 6));
					}

					l_lPartialIndex = (l_lCounter / 4) * 3;
					l_ucpPartialDecryption [l_lPartialIndex] = ((char*) &l_ulTempValue) [0];
					l_ucpPartialDecryption [l_lPartialIndex + 1] = ((char*) &l_ulTempValue) [1];
					l_ucpPartialDecryption [l_lPartialIndex + 2] = ((char*) &l_ulTempValue) [2];
				}


				for (l_lCounter = 0; 
				     l_lCounter < l_lDecryptedStringLength; 
					  l_lCounter++)
				{
					l_ucTempValue = l_ucpPartialDecryption [l_lCounter];

					l_ucTempValue = l_ucTempValue - (unsigned char)(l_lCounter + m_ulNameSeed);

					l_lRightShift = (l_lCounter + 3) % 8;
					l_lLeftShift = 8 - l_lRightShift;

					l_ucLeftShiftValue = l_ucTempValue << l_lLeftShift;
					l_ucRightShiftValue = l_ucTempValue >> l_lRightShift;

					l_ucTempValue = l_ucLeftShiftValue + l_ucRightShiftValue;

					l_ucTempValue = (unsigned char)m_oRevLookUpTable1Map [l_ucTempValue];

					(*p_bstrpDecryptedString) [(int) l_lCounter] = 
						(WCHAR)((l_ucTempValue - l_lCounter) & 0x000000ff);
				}

				l_lNewStringLength = static_cast< long >( wcslen( *p_bstrpDecryptedString ) );

				hr = p_bstrpDecryptedString->SetLength (l_lNewStringLength);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				break;
			}
		default:
			{
				hr = -1939;
				break;
			}
		}

		break;
	}


	delete [] l_ucpPartialDecryption;

	return hr;
}


HRESULT SVXMLEncryptionClass::JumpTracking1Initialization (long	p_lSourceStringLength, 
					                                            long**	p_lppTrackJumping, 
														                 long*	p_lpMaxTrackingLength)
{
	HRESULT	hr;

	long		l_lCounter;


	hr = S_OK;

	while (1)
	{
//-	String (length + 1) / 4, we will process the low byte of 4, 16 bit 
//-	characters at a time. * (times) 3 to allow for filler. +1 for 
//-	SourceStringLengths less than 4.
		*p_lpMaxTrackingLength = (((p_lSourceStringLength + 1) / 4) * 3) + 1;

		*p_lppTrackJumping = new long [*p_lpMaxTrackingLength];

		if (*p_lppTrackJumping == NULL)
		{
			hr = -1887;
			break;
		}

		for (l_lCounter = 0; l_lCounter < *p_lpMaxTrackingLength; l_lCounter++)
		{
			(*p_lppTrackJumping) [l_lCounter] = l_lCounter + 1;
		}

		break;
	}

	return hr;

}


HRESULT SVXMLEncryptionClass::EncryptString (long		p_lEncryptionMethod, 
															BSTR		p_bstrSourceString, 
															SVBStr*	p_bstrpEncryptedString)
{
	HRESULT	hr;

	long*				l_lpTrackJumping;
	long				l_lSourceStringLength;
	long				l_lMaxTrackingLength;
	long				l_lFirstTrackingUnused;
	long				l_lCurrentTrackingLengthThreshold;
	long				l_lChecksumSeed;
	long				l_lCurrentSourceIndex;
	long				l_lCurrentDestinationIndex;
	long				l_lNextDestinationIndex;
	long				l_lCounter;
	long				l_lCurrentLength;

	unsigned long	l_ulEmbeddedSeed;
	unsigned long	l_ulTempIndex;
	unsigned long	l_ulChecksumParity;

//	WCHAR*			l_wcpSource;
	char				l_cpFourChars [4];

	SVValue64Union	l_oNewValueUnion;

	hr = S_OK;
	l_lpTrackJumping = NULL;

	l_lFirstTrackingUnused = 0;
	l_lCurrentTrackingLengthThreshold = 0;
	l_lCurrentSourceIndex = 0;
	l_lCurrentDestinationIndex = 0;

long	l_lTempDevelopment = 0;
if (g_lTempDevelopment < 10)
{
	l_lTempDevelopment = 1;
}

	while (1)
	{
//TRACE ("EncryptString (0) - New String\n");


		hr = p_bstrpEncryptedString->Clear ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (p_bstrSourceString == NULL)
		{
			hr = p_bstrpEncryptedString->SetNULL ();
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		l_lSourceStringLength = SysStringLen (p_bstrSourceString);

		if (l_lSourceStringLength == 0)
		{
			hr = p_bstrpEncryptedString->CopyFromWChar (L"");
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		hr = p_bstrpEncryptedString->PreAllocate (l_lSourceStringLength * 4);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-   This switch statement is to initialize only.
		switch (p_lEncryptionMethod)
		{
		case 1:
			{
				hr = JumpTracking1Initialization (l_lSourceStringLength, 
					                               &l_lpTrackJumping, 
															 &l_lMaxTrackingLength);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				hr = CreateRandomLong (&l_ulEmbeddedSeed);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

//-			16777215 is the max 24 bit number.
				l_ulEmbeddedSeed = (unsigned long) (((double) l_ulEmbeddedSeed / (double) 0xffffffff) * 0x00ffffff);

				hr = CalculateEncryptionChecksum (p_bstrSourceString, 
															 0x0000000f,
															 (unsigned long*) &l_lChecksumSeed);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				l_lNextDestinationIndex = 0;

				break;
			}
		case 2:
			{
//-			For encryption method 2, the encrypted string must always encrypt 
//-			to the same value.  For this reason, the checksum is also used as 
//-			the embedded seed.
//				hr = JumpTracking1Initialization (l_lSourceStringLength, 
//					                               &l_lpTrackJumping, 
//															 &l_lMaxTrackingLength);
//				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
//				{
//					break;
//				}

				hr = CalculateEncryptionChecksum (p_bstrSourceString, 
															 0x00ffffff,
															 &l_ulEmbeddedSeed);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				l_lChecksumSeed = l_ulEmbeddedSeed & 0x0000000f;

//-			Encryption method 2 does not use Next Destination internally (it
//-			does not use jumping), and filler is not used, so this value can 
//-			be pre-calculated.
				l_lNextDestinationIndex = (l_lSourceStringLength / 4) + 1;

				break;
			}
		default:
			{
				hr = -1888;
				break;
			}
		}


//-   OK, in theory this architecture allows for the encryption method to be 
//-	changed during the encryption. Great care should be used when doing 
//-	this.  When using multiple encryption methods together, the encryption
//-   methods being used need to be compatible with each other. It should 
//-   not simply be assumed that all encrpytion methods will be compatible.
		while ((l_lCurrentSourceIndex <= l_lSourceStringLength) ||
			    (l_lFirstTrackingUnused < l_lCurrentTrackingLengthThreshold))
		{
//-      We are reading words and writing bytes.
			l_cpFourChars [0] = (char) (p_bstrSourceString [l_lCurrentSourceIndex + 0]);
			l_cpFourChars [1] = (char) (p_bstrSourceString [l_lCurrentSourceIndex + 1]);
			l_cpFourChars [2] = (char) (p_bstrSourceString [l_lCurrentSourceIndex + 2]);
			l_cpFourChars [3] = (char) (p_bstrSourceString [l_lCurrentSourceIndex + 3]);

			l_oNewValueUnion.int64Value = 0;

			switch (p_lEncryptionMethod)
			{
			case 1:
				{
					hr = Method1Encryption (l_lSourceStringLength,
													l_lMaxTrackingLength,
													l_lChecksumSeed,
													l_ulEmbeddedSeed,
													l_cpFourChars,
													&l_lCurrentSourceIndex,
													&l_lCurrentDestinationIndex,
													&l_lNextDestinationIndex,
													&l_lCurrentTrackingLengthThreshold,
													l_lpTrackJumping,
													&l_lFirstTrackingUnused,
													&l_oNewValueUnion);
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
					{
						break;
					}


					break;
				}
			case 2:
				{

					hr = Method2Encryption (l_lSourceStringLength,
													l_lChecksumSeed,
													l_cpFourChars,
													&l_lCurrentSourceIndex,
													&l_lCurrentDestinationIndex,
					       						&l_oNewValueUnion);
					if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
					{
						break;
					}

					break;
				}
			default:
				{
					hr = -1935;
					break;
				}
			}

			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}


//-		The following loop should take the 48 bits (contained in the int64), 
//-		and place them 6 bits at a time into 8 wide characters in the 
//-		encrypted string.
			for (l_lCounter = 0; l_lCounter < 8; l_lCounter++)
			{
				l_ulTempIndex = 
					(unsigned long) ((l_oNewValueUnion.int64Value >> 
											(l_lCounter * 6)) & 
										  0x000000000000003f);

				(*p_bstrpEncryptedString) [(int) (l_lCurrentDestinationIndex * 8 + l_lCounter)] =  m_cCharTable [l_ulTempIndex];


//TRACE ("EncryptString (1) - Current Index = %d, cd = %d, nd = %d, counter = %d\n", l_lCurrentDestinationIndex * 8 + l_lCounter, l_lCurrentDestinationIndex, l_lNextDestinationIndex, l_lCounter);
//Sleep (4);

				if ((*p_bstrpEncryptedString) [(int) (l_lCurrentDestinationIndex * 8 + l_lCounter)] == 0)
				{
					ASSERT (0);
				}

			}

//TRACE ("EncryptString (2) - Current Index = %d, cd = %d, nd = %d\n", l_lCurrentDestinationIndex * 8 + l_lCounter, l_lCurrentDestinationIndex, l_lNextDestinationIndex);
//Sleep (3);


		} // while (l_lCurrentSourceIndex <= l_lSourceStringLength)

		l_lCurrentLength = p_bstrpEncryptedString->GetLength ();

		for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
		{
			l_ulTempIndex = 
				(unsigned long) ((l_ulEmbeddedSeed >> 
				                  (l_lCounter * 6)) & 
				                 0x0000003f);

//			(*p_bstrpEncryptedString) [(int) (l_lNextDestinationIndex * 8 + l_lCounter)] =  m_cCharTable [l_ulTempIndex];
			(*p_bstrpEncryptedString) [(int) (l_lCurrentLength + l_lCounter)] =  m_cCharTable [l_ulTempIndex];

if (l_lTempDevelopment)
{
	TRACE ("???1Enc(1) embedded seed before Char Table: %lx.\n", l_ulTempIndex);
	TRACE ("???1Enc(2) embedded seed after Char Table:%lx.\n", (*p_bstrpEncryptedString) [(int) (l_lCurrentDestinationIndex * 8 + l_lCounter)]);
}
		}


		hr = FindChecksumParity (p_bstrpEncryptedString, l_lChecksumSeed, &l_ulChecksumParity);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-   Checksum parity must be a 4 bit value at most (0-15).
		if (l_ulChecksumParity < 4)
		{
			l_ulChecksumParity = l_ulChecksumParity + 'p';
		}
		else
		{
			l_ulChecksumParity = (l_ulChecksumParity - 4) + 'd';
		}

		(*p_bstrpEncryptedString) [(int) (l_lCurrentLength + 4)] = ((WCHAR*) &l_ulChecksumParity) [0];
//		(*p_bstrpEncryptedString) [(int) (l_lNextDestinationIndex * 8 + 4)] = ((WCHAR*) &l_ulChecksumParity) [0];


		{
//-		This code is for development debug only. To confirm that we have 
//-		correctly handled the checksum parity.
			long	l_lChecksumTest;

			hr = CalculateEncryptionChecksum (*p_bstrpEncryptedString, 
														 0x0000000f,
			                                  (unsigned long*) &l_lChecksumTest);

			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			if (l_lChecksumTest != l_lChecksumSeed)
			{
				ASSERT (0);
				hr = -1908;
				break;
			}
		}

		break;
	} // while (1)

	delete [] l_lpTrackJumping;
	l_lpTrackJumping = NULL;

	return hr;
}


HRESULT SVXMLEncryptionClass::Method1Encryption (
			long					p_lSourceStringLength,
			long					p_lMaxTrackingLength,
			long					p_lChecksumSeed,
			unsigned long		p_ulEmbeddedSeed,
			char*					p_cpSourceChars,
			long*					p_lpCurrentSourceIndex,
			long*					p_lpCurrentDestinationIndex,
			long*					p_lpNextDestinationIndex,
			long*					p_lpCurrentTrackingLengthThreshold,
			long*					p_lpTrackJumping,
			long*					p_lpFirstTrackingUnused,
			SVValue64Union*	p_opNewValueUnion)

{
	HRESULT			hr;

	long				l_lRandomChanceForFiller;
	long				l_lThereIsEnoughRoomForFillerWithinMaxTrackingLength;
	long				l_lThereAreCharactersLeftToProcess;
	long				l_lCounter;
	long				l_lFoundGoodIndex;
	long				l_lUseFiller;
	long				l_lNewDestinationIndex;
	long				l_lRelativeJump;
	long				l_lTempOffset;
	long				l_lJumpingOffset = 3;

	unsigned long	l_ulTempRand1;
	unsigned long	l_ulTempRand2;
	unsigned long	l_ulTempSourceValue;
	unsigned long	l_ulTempResultValue;
//	unsigned long	l_ulTempIndex;



	hr = S_OK;

	while (1)
	{
		*p_lpCurrentDestinationIndex = *p_lpNextDestinationIndex;

		hr = CreateRandomLong (&l_ulTempRand1);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

//-   Bit descriptions start at 1.

//-	4% chance of using filler, AND IF there is still enough room for 
//-	filler to be included.
//-	or....
//-	We done processing the string, and are using filler to fill in the 
//-	gaps.
		l_lRandomChanceForFiller = ((l_ulTempRand1 % 100) < 5);

		l_lThereIsEnoughRoomForFillerWithinMaxTrackingLength =
			((p_lSourceStringLength - (*p_lpCurrentSourceIndex - 4)) / 4) < 
			(p_lMaxTrackingLength - *p_lpCurrentTrackingLengthThreshold);

//-	Keep in mind that if the total number of characters is a multiple of 4, 
//-	there will be one last "data" pass with a padding of 4.
//-	  Ex. string length of 8 will be 8-1 = 7 which is less than 8 and will
//-       process.  Current source index should come through as 0, 4, and 8 
//-       for the 3 passes.
		l_lThereAreCharactersLeftToProcess = (*p_lpCurrentSourceIndex - 1 < p_lSourceStringLength);

		if ((l_lRandomChanceForFiller && l_lThereIsEnoughRoomForFillerWithinMaxTrackingLength) ||
			 !l_lThereAreCharactersLeftToProcess)
		{
//if (l_lRandomChanceForFiller)
//{
//	TRACE ("Random filler\n");
//}
//else
//{
//	TRACE ("No characters to process filler\n");
//}

			l_lUseFiller = TRUE;
		}
		else
		{
			l_lUseFiller = FALSE;
		}

//TRACE ("Source Index = %ld, Destination Index = %ld, Tracking Length Threshold = %ld\n", *p_lpCurrentSourceIndex, *p_lpCurrentDestinationIndex, *p_lpCurrentTrackingLengthThreshold);
		
		if (l_lUseFiller == TRUE)
		{
			hr = CreateRandomLong (&l_ulTempRand1);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			hr = CreateRandomLong (&l_ulTempRand2);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			p_opNewValueUnion->longValue [0] = l_ulTempRand1;
			p_opNewValueUnion->wcharValue [2] = (WCHAR) l_ulTempRand2;

//-		50% change bits 42&45 being cleared or set.  Both must be same.
			hr = SetFiller (TRUE, 2, p_opNewValueUnion);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}
		} // if (l_lUseFiller == TRUE)
		else // filler not used.
		{
			hr = SetFiller (FALSE, 3, p_opNewValueUnion);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

g_lTempDevelopment = g_lTempDevelopment + 1;
if (g_lTempDevelopment < 10)
{
			TRACE ("Method1Sub1(1), %c, %c, %c, %c.\n", p_cpSourceChars [0], p_cpSourceChars [1], p_cpSourceChars [2], p_cpSourceChars [3]);
}

			l_ulTempSourceValue = *((unsigned long *) p_cpSourceChars);


			hr = SetPadding (p_lSourceStringLength, *p_lpCurrentSourceIndex, &l_ulTempSourceValue);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

//-		Grab the next 4, 8 bit bytes for encryption.
//-		Add in the current character index and the local "for loop counter".
//-		Look the result up in the lookup table.
			for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
			{
				((char*) (&l_ulTempResultValue)) [l_lCounter] = 
					(char) (((char*) &l_ulTempSourceValue) [l_lCounter] + 
					        *p_lpCurrentSourceIndex + 
							  l_lCounter);

if (g_lTempDevelopment < 10)
{
			TRACE ("Method1Sub1(1.1), %x, counter = %d.\n", ((char*) (&l_ulTempResultValue)) [l_lCounter], l_lCounter);
}

				((unsigned char*) (&l_ulTempResultValue)) [l_lCounter] = (unsigned char) m_lLookUpTable1 [((unsigned char*) (&l_ulTempResultValue)) [l_lCounter]];

if (g_lTempDevelopment < 10)
{
			TRACE ("Method1Sub1(1.2), %x.\n", ((unsigned char*) (&l_ulTempResultValue)) [l_lCounter]);
}
			}

if (g_lTempDevelopment < 10)
{
			TRACE ("Method1Sub1(1.3), %x.\n", l_ulTempResultValue);
}

//-		Although we posess a 32 bit checksum, only the last 4 bits of 
//-		the checksum seed can actually be used. This is so that the 
//-		checksum parity concept can work properly.
			l_ulTempResultValue = l_ulTempResultValue + 
					                ((p_lChecksumSeed  & 0x0000000f) | 
					                 (p_ulEmbeddedSeed & 0x55555555));
			
			hr = CreateRandomLong (&l_ulTempRand1);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

if (g_lTempDevelopment < 10)
{
			TRACE ("Method1Sub1(2), %x, %x, %x, %x.\n", ((char*) (&l_ulTempResultValue)) [0], ((char*) (&l_ulTempResultValue)) [1], ((char*) (&l_ulTempResultValue)) [2], ((char*) (&l_ulTempResultValue)) [3]);
			TRACE ("Method1Sub1(3), %lx.\n", l_ulTempResultValue);
}

//-      5% change of setting character seed value, which if set will 
//-		shift all bits of the current encrypted value 1 bit left (bit 32
//-      shifts into bit 1 of wcNewValue [2]).
			if ((l_ulTempRand1 % 100) <= 6)
			{
				hr = SetCharSeed (TRUE, p_opNewValueUnion);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

//-				At the "long integer" level, l_ulTempResultValue appears as
//-            0x12 34 56 78.  In the final BSTR this will translate into 
//-				0x78 56 34 12,  So this code will actually be shifting bit 8
//-				of the resulting BSTR into l_wcNewValue [2].
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] | (WCHAR) (l_ulTempResultValue >> 31);
				p_opNewValueUnion->longValue [0] = l_ulTempResultValue << 1;

//-				Random setting of bit 1.
				if (((l_ulTempRand1 >> 1) % 2) == 0)
				{
					p_opNewValueUnion->longValue [0] = p_opNewValueUnion->longValue [0] | 0x00000001;
				}

			}
			else
			{
				hr = SetCharSeed (FALSE, p_opNewValueUnion);
				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				p_opNewValueUnion->longValue [0] = l_ulTempResultValue;
			}

//-		Pointer math, jump ahead 4 16bit characters.
			*p_lpCurrentSourceIndex = *p_lpCurrentSourceIndex + 4;

		} // if (l_lUseFiller == TRUE) else


		hr = SetMethod (1, p_opNewValueUnion);
//		p_opNewValueUnion->wcharValue [2] = (p_opNewValueUnion->wcharValue [2] & 0xff87) | 
//			                               0x0008;	// Encryption method 1.
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}
		
		if (*p_lpCurrentDestinationIndex >= p_lMaxTrackingLength)
		{
			hr = -1934;
			break;
		}

		p_lpTrackJumping [*p_lpCurrentDestinationIndex] = -1;

//-      This loop should only execute if we've just used the first unused.
		while (p_lpTrackJumping [*p_lpFirstTrackingUnused] == -1)
		{
			*p_lpFirstTrackingUnused = *p_lpFirstTrackingUnused + 1;
		}

		hr = CreateRandomLong (&l_ulTempRand1);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_lRelativeJump = l_ulTempRand1 % 8;

		l_lCounter = 0;
		l_lFoundGoodIndex = FALSE;

//-	We will loop to find an unused index to jump to (from 3 forward to 
//-   4 back).
		do
		{
			l_lTempOffset = ((l_lRelativeJump + l_lCounter) % 8) - l_lJumpingOffset;
			if ((*p_lpCurrentDestinationIndex + l_lTempOffset) < 0)
			{
//-			Before beginning.
			}
			else
			if ((*p_lpCurrentDestinationIndex + l_lTempOffset) >= p_lMaxTrackingLength)
			{
//-			Past end.
			}
			else
			if ((p_lSourceStringLength < *p_lpCurrentSourceIndex) &&
				 ((*p_lpCurrentDestinationIndex + l_lTempOffset) > *p_lpCurrentTrackingLengthThreshold))
			{
//-			Done processing string. Destination may no longer grow.
			}
			else
			if (p_lpTrackJumping [*p_lpCurrentDestinationIndex + l_lTempOffset] == -1)
			{
//-			Already in use.
			}
			else
			{
//-			Must be good.
//-			We will leave the loop when a good location is found.
				l_lFoundGoodIndex = TRUE;
				l_lNewDestinationIndex = *p_lpCurrentDestinationIndex + l_lTempOffset;
				l_lRelativeJump = l_lTempOffset + l_lJumpingOffset;
				break;
			}

			l_lCounter = l_lCounter + 1;
		} while (l_lCounter != 8);


		{
//-			Development code.
			if ((l_lRelativeJump != l_lJumpingOffset + 1) &&
				 (l_lFoundGoodIndex == TRUE))
			{
				short a = 1;
			}
		}

//-		If no good local (within 4 forward or 3 back) address was found, 
//-		then we will use the first available index
		if (l_lFoundGoodIndex == FALSE)
		{
//TRACE ("Method1Sub1(4), First unused = %d\n", *p_lpFirstTrackingUnused); 
			l_lNewDestinationIndex = *p_lpFirstTrackingUnused;
//-			The Relative Jump can not reference the current position. When 
//-			this happens it means that the first available was used.
			l_lRelativeJump = l_lJumpingOffset;
		}

		hr = SetRelativeJump (l_lRelativeJump, p_opNewValueUnion);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		*p_lpNextDestinationIndex = l_lNewDestinationIndex;
		if (*p_lpNextDestinationIndex > *p_lpCurrentTrackingLengthThreshold)
		{
			*p_lpCurrentTrackingLengthThreshold = *p_lpNextDestinationIndex;
		}

		break;
	} // while (1)


//	if ((*p_lpNextDestinationIndex - *p_lpCurrentDestinationIndex) > 4)
//	{
//		ASSERT (0);
//	}


	return hr;
}



HRESULT SVXMLEncryptionClass::Method2Encryption (
													long					p_lSourceStringLength,
													long					p_lChecksumSeed,
													char*					p_cpSourceChars,
													long*					p_lpCurrentSourceIndex,
													long*					p_lpCurrentDestinationIndex,
													SVValue64Union*	p_opNewValueUnion )
{
	HRESULT	hr;

	long				l_lCounter;
	long				l_lRelativeJump;

	unsigned long	l_ulTempSourceValue;
	unsigned long	l_ulTempResultValue;

	hr = S_OK;

	p_opNewValueUnion->int64Value = 0;

	while (1)
	{
//-	We will walk backwards.
		*p_lpCurrentDestinationIndex = (p_lSourceStringLength - *p_lpCurrentSourceIndex) / 4;

		hr = SetFiller (FALSE, -1, p_opNewValueUnion);  // -1 means that the 
																		// format will not be
																		// random.
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		hr = SetMethod (2, p_opNewValueUnion);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_ulTempSourceValue = *((unsigned long *) p_cpSourceChars);

		hr = SetPadding (p_lSourceStringLength, *p_lpCurrentSourceIndex, &l_ulTempSourceValue);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-	Grab the next 4, 8 bit bytes for encryption.
//-	Add in the local "for loop counter" and an arbitrary 8.
//-	Look the result up in the lookup table.
		for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
		{
			((char*) (&l_ulTempResultValue)) [l_lCounter] = 
				(char) (((char*) &l_ulTempSourceValue) [l_lCounter] + 
					     l_lCounter + 8);

if (g_lTempDevelopment < 10)
{
		TRACE ("Method2(1.1), %x, counter = %d.\n", ((char*) (&l_ulTempResultValue)) [l_lCounter], l_lCounter);
}

			((unsigned char*) (&l_ulTempResultValue)) [l_lCounter] = (unsigned char) m_lLookUpTable1 [((unsigned char*) (&l_ulTempResultValue)) [l_lCounter]];

if (g_lTempDevelopment < 10)
{
		TRACE ("Method2(1.2), %x.\n", ((unsigned char*) (&l_ulTempResultValue)) [l_lCounter]);
}
		}

		l_ulTempResultValue = ~l_ulTempResultValue + m_ulNameSeed;

		p_opNewValueUnion->longValue [0] = l_ulTempResultValue;

//-	The CharSeed (actually 4 chars) will not be used for anything in the 
//-	encryption calculation.  We are only devising a way to set the bit.
		if ((l_ulTempResultValue % 2) == 0)
		{
			hr = SetCharSeed (TRUE, p_opNewValueUnion);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}
		}
		else
		{
			hr = SetCharSeed (FALSE, p_opNewValueUnion);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}
		}

//-	Pointer math, jump ahead 4 16bit characters.
		*p_lpCurrentSourceIndex = *p_lpCurrentSourceIndex + 4;

//-   Relative Jump bits are set, but is not used.
		l_lRelativeJump = p_lChecksumSeed % 8;
		hr = SetRelativeJump (l_lRelativeJump, p_opNewValueUnion);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		break;
	} // while (1)

	return hr;
}



HRESULT SVXMLEncryptionClass::DecryptString (BSTR	   p_bstrEncryptedString, 
															SVBStr*	p_bstrpDecryptedString)
{
	HRESULT	hr;

	long				l_lEncryptedLength;
	long				l_lEncryptionMethod;
	long				l_lMaxTrackingLength;
	long				l_lDecryptedLength;
	long				l_lCounter;
	long				l_lCounter2;
	long				l_lCurrentSourceIndex;
	long				l_lCurrentDestinationIndex;
	long				l_lFillerUsed;
	long				l_lFirstTrackingUnused;
	long				l_lRelativeJump;
	long				l_lPadding;
	long				l_lJumpingOffset = 3;


	long*				l_lpTrackJumping;

	unsigned long	l_ulChecksumSeed;
	unsigned long	l_ulEmbeddedSeed;

	unsigned short l_usStep1EmbeddedSeed [4];
	unsigned short l_usStep2EmbeddedSeed [4];

	WCHAR				l_wcDecryptedCharacterArray [4];

	SVValue64Union l_oNewValueUnion;



	hr = S_OK;
	l_lpTrackJumping = NULL;
	l_lFirstTrackingUnused = 0;

	l_oNewValueUnion.int64Value = 0;

	while (1)
	{
		hr = p_bstrpDecryptedString->Clear ();
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		if (p_bstrEncryptedString == NULL)
		{
			hr = p_bstrpDecryptedString->SetNULL ();
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		l_lEncryptedLength = SysStringLen (p_bstrEncryptedString) - 5;

//-	If the input encrypted string is empty.
		if (l_lEncryptedLength == -5) 
		{
			hr = p_bstrpDecryptedString->CopyFromWChar (L"");
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}

			break;
		}

		hr = p_bstrpDecryptedString->PreAllocate (l_lEncryptedLength);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}


//-	Get checksum seed (checksum parity is last two bytes in string).
		hr = CalculateEncryptionChecksum (p_bstrEncryptedString, 
			                               0x0000000f,
			                               &l_ulChecksumSeed);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_lMaxTrackingLength = (l_lEncryptedLength) / 8;

		l_lpTrackJumping = new long [l_lMaxTrackingLength];
		if (l_lpTrackJumping == NULL)
		{
			hr = -1932;
			break;
		}

//-   Initialize array to track status of individual blocks (8 WCHARs per 
//-	block).
		for (l_lCounter = 0; l_lCounter < l_lMaxTrackingLength; l_lCounter++)
		{
			l_lpTrackJumping [l_lCounter] = l_lCounter + 1;
		}


		l_ulEmbeddedSeed = 0;

//-	Get embedded seed.  This comes after the encrypted string.
		for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
		{
			l_usStep1EmbeddedSeed [l_lCounter] = (unsigned short)p_bstrEncryptedString [l_lEncryptedLength + l_lCounter];
			l_usStep2EmbeddedSeed [l_lCounter] = (unsigned short)m_oRevCharTableMap [l_usStep1EmbeddedSeed [l_lCounter]];

			l_ulEmbeddedSeed = 
				l_ulEmbeddedSeed + 
				(((unsigned long) l_usStep2EmbeddedSeed [l_lCounter]) << 
				 (l_lCounter * 6));
		}


		l_lCurrentSourceIndex = 0;
		l_lCurrentDestinationIndex = 0;

//TRACE ("DecryptString (.01)\n");

//		while (l_lCurrentDestinationIndex < l_lMaxTrackingLength)
		while (l_lFirstTrackingUnused < l_lMaxTrackingLength)
		{
			l_oNewValueUnion.int64Value = 0;

//-		Break out packets of 8 from encrypted source.
			for (l_lCounter2 = 0; l_lCounter2 < 8; l_lCounter2++)
			{
				l_oNewValueUnion.int64Value = 
					l_oNewValueUnion.int64Value + 
					(((unsigned __int64)(m_oRevCharTableMap [p_bstrEncryptedString [(l_lCurrentSourceIndex * 8) + 
					                                                                  l_lCounter2]])) << 
					 (l_lCounter2 * 6));
			}
			
			l_lEncryptionMethod = (l_oNewValueUnion.wcharValue [2] & 0x0078) >> 3;

			if ((((l_oNewValueUnion.wcharValue [2] & 0x1200) == 0x1200) ||
				  ((l_oNewValueUnion.wcharValue [2] & 0x1200) == 0x0000)) &&
				 (l_lEncryptionMethod == 1))   // This needs to list the 
				                               // encryption methods that 
														 // support filler.
			{
				l_lFillerUsed = TRUE;
			}
			else
			{
//-			Encryption Method 2 does not support filler.
				l_lFillerUsed = FALSE;
			}

			l_lRelativeJump = (l_oNewValueUnion.wcharValue [2] & 0x0800) >> 11;
			l_lRelativeJump = l_lRelativeJump + ((l_oNewValueUnion.wcharValue [2] & 0x0100) >> 7);
			l_lRelativeJump = l_lRelativeJump + ((l_oNewValueUnion.wcharValue [2] & 0x0400) >> 8);

//-		Prepare packets of 4 for unencrypted destination.


			if (l_lFillerUsed == FALSE)
			{
				switch (l_lEncryptionMethod)
				{
				case 1:
					{
						hr = Method1Decryption (l_ulChecksumSeed,
														l_ulEmbeddedSeed,
														l_lCurrentDestinationIndex,
														&l_oNewValueUnion,
														l_wcDecryptedCharacterArray);
						break;
					}
				case 2:
					{
//-					Method 2 jumps through some hoops here because the string 
//-					is encrypted backwards (in reverse order).
						
						if ((l_lCurrentSourceIndex == 0) &&
							 (l_lCurrentDestinationIndex == 0))
						{
//-						This means that we haven't processed anything for this 
//-						method yet.  Nothing will be processed yet, but values
//-						will be re-initialized.

//-						This will force then next source to be set to the next
//-						available.
							l_lRelativeJump = l_lJumpingOffset;

							l_lFirstTrackingUnused = l_lMaxTrackingLength - 1;
						}
						else
						{
							hr = Method2Decryption (&l_oNewValueUnion,
															l_wcDecryptedCharacterArray);

//-						Relative Jumping is not used by this method, though the 
//-						bits are entered.  We must fudge a relative jump value.
							l_lRelativeJump = l_lJumpingOffset - 1;

							if (l_lCurrentSourceIndex == (l_lMaxTrackingLength - 1))
							{
//-							This should reset not correctly processing the 
//-							"first" time through.  This should always set 
//-							l_lCurrentDestinationIndex to 0.
								l_lCurrentDestinationIndex = l_lCurrentDestinationIndex - 1; 
								l_lFirstTrackingUnused = 0;
								l_lpTrackJumping [0] = 1;
							}

						}


						break;
					}
				default:
					{
						hr = -1933;
						break;
					}
				}


				if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
				{
					break;
				}

				for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
				{
					(*p_bstrpDecryptedString) [(int)((l_lCurrentDestinationIndex * 4) + l_lCounter)] = l_wcDecryptedCharacterArray [l_lCounter];
				}

				l_lCurrentDestinationIndex = l_lCurrentDestinationIndex + 1;

			} // if (l_lFillerUsed == FALSE)


			l_lpTrackJumping [l_lCurrentSourceIndex] = -1;

			while ((l_lpTrackJumping [l_lFirstTrackingUnused] == -1) &&
					 (l_lFirstTrackingUnused < l_lMaxTrackingLength))
			{
				l_lFirstTrackingUnused = l_lFirstTrackingUnused + 1;
//TRACE ("DecryptString (.1) first track unused = %d\n", l_lFirstTrackingUnused);
			}

			if ((l_lRelativeJump - l_lJumpingOffset) == 0)
			{
				l_lCurrentSourceIndex = l_lFirstTrackingUnused;
//TRACE ("DecryptString (.2) use unused\n");
			} // if ((l_lRelativeJump - l_lJumpingOffset == 0)
			else
			{
//-		We're setting up where the NEXT packet will come from.
				l_lCurrentSourceIndex = (l_lCurrentSourceIndex + l_lRelativeJump) - 
												l_lJumpingOffset;
//TRACE ("DecryptString (.3) current source index = %d\n", l_lCurrentSourceIndex);
			}


//TRACE ("DecryptString, First tracking unused = %d, Max tracking length = %d\n", l_lFirstTrackingUnused, l_lMaxTrackingLength);
		} // while (l_lFirstTrackingUnused < l_lMaxTrackingLength)


		l_lDecryptedLength = p_bstrpDecryptedString->GetLength ();
		l_lPadding = (*p_bstrpDecryptedString) [(int) (l_lDecryptedLength - 1)];
		p_bstrpDecryptedString->SetLength (l_lDecryptedLength - l_lPadding);

		break;
	}

	delete [] l_lpTrackJumping;

	return hr;
}


HRESULT SVXMLEncryptionClass::Method1Decryption (unsigned long		p_ulChecksumSeed,
																 unsigned long		p_ulEmbeddedSeed,
																 long					p_lCurrentDestinationIndex,
																 SVValue64Union*	p_opSourceValueUnion,
																 WCHAR*				p_wcpDecryptedCharacters)
{
	HRESULT			hr;

	long				l_lCharacterSeed;
	long				l_lCount;

	SVValue64Union	l_oTempHoldingUnion;

	hr = S_OK;

	while (1)
	{
		
		if ((p_opSourceValueUnion->wcharValue [2] & 0x2000) == 0x2000)
		{
			l_lCharacterSeed = 1;
		}
		else
		{
			l_lCharacterSeed = 0;
		}

		if (l_lCharacterSeed == 1)
		{
			l_oTempHoldingUnion.longValue [0] = p_opSourceValueUnion->longValue [0] >> 1;
			l_oTempHoldingUnion.longValue [0] = l_oTempHoldingUnion.longValue [0] | 
				                                 (((unsigned long) (p_opSourceValueUnion->wcharValue [2] & 0x0001)) << 31);
		}
		else
		{
			l_oTempHoldingUnion.longValue [0] = p_opSourceValueUnion->longValue [0];
		}

		l_oTempHoldingUnion.longValue [0] = l_oTempHoldingUnion.longValue [0] - 
			                              ((p_ulChecksumSeed & 0x0000000f) | (p_ulEmbeddedSeed & 0x55555555));

		for (l_lCount = 0; l_lCount < 4; l_lCount++)
		{
			p_wcpDecryptedCharacters [l_lCount] = (WCHAR)m_oRevLookUpTable1Map [(unsigned char) (l_oTempHoldingUnion.charValue [l_lCount])];

			p_wcpDecryptedCharacters [l_lCount] = (WCHAR)((p_wcpDecryptedCharacters [l_lCount] - ((p_lCurrentDestinationIndex * 4) + l_lCount)) &
				                                   0x00ff);
		}

		break;
	}


	return hr;
}



HRESULT SVXMLEncryptionClass::Method2Decryption (SVValue64Union*	p_opSourceValueUnion,
																 WCHAR*				p_wcpDecryptedCharacters)
{
	HRESULT			hr;

	long				l_lCount;

	SVValue64Union	l_oTempHoldingUnion;

	hr = S_OK;

	while (1)
	{

//-	True but doesn't matter.		
/*		if ((p_opSourceValueUnion->wcharValue [2] & 0x2000) == 0x2000)
		{
			l_lCharacterSeed = 1;
		}
		else
		{
			l_lCharacterSeed = 0;
		}

		if (l_lCharacterSeed == 1)
		{
			l_oTempHoldingUnion.longValue [0] = p_opSourceValueUnion->longValue [0] >> 1;
			l_oTempHoldingUnion.longValue [0] = l_oTempHoldingUnion.longValue [0] | 
				                                 (((unsigned long) (p_opSourceValueUnion->wcharValue [2] & 0x0001)) << 31);
		}
		else
		{
			l_oTempHoldingUnion.longValue [0] = p_opSourceValueUnion->longValue [0];
		}
*/

		l_oTempHoldingUnion.longValue [0] = ~(p_opSourceValueUnion->longValue [0] - m_ulNameSeed);

		for (l_lCount = 0; l_lCount < 4; l_lCount++)
		{
			p_wcpDecryptedCharacters [l_lCount] = (WCHAR)m_oRevLookUpTable1Map [(unsigned char) (l_oTempHoldingUnion.charValue [l_lCount])];

			p_wcpDecryptedCharacters [l_lCount] = (WCHAR)((p_wcpDecryptedCharacters [l_lCount] - (l_lCount + 8)) &
				                                   0x00ff);
		}

		break;
	}


	return hr;
}



HRESULT SVXMLEncryptionClass::CalculateEncryptionChecksum (BSTR				p_bstrString, 
																			  unsigned long	p_ulMask,
																		     unsigned long*	p_ulpCheckSum)
																			  
{
	HRESULT	hr;

	long		l_lLength;
	long		l_lLoop;
	long		l_lShiftLeft;
	long		l_lShiftRight;


	unsigned long	l_ulPart1;
	unsigned long  l_ulPart2;

	unsigned long	l_ulRawCheckSum;

	WCHAR*	l_wcpChar;

	hr = 0;
	l_ulRawCheckSum = 0;
	
	while (1)
	{
		l_lLength = SysStringLen (p_bstrString);
		l_wcpChar = p_bstrString;

		for (l_lLoop = 0; l_lLoop < l_lLength; l_lLoop++)
		{
//-		Each wide character (16 bits) will be shifted left 
//-		(l_lLoop + 2 % 3).  Basically from 0 to 2 bits.
//-      
//-		Each wide character will also be shifted right 4 minus the left 
//-		shift.  Basically 4 to 2 bits.  
//-
//-		The purpose of this algorith is to allow shifting of the bits, but
//-		to preserve all bits.
			l_lShiftLeft = (l_lLoop + 2) % 3;
			l_lShiftRight = 4 - l_lShiftLeft;

			l_ulPart1 = ((l_wcpChar [l_lLoop] & 0x000f) << l_lShiftLeft) & 0x0000000f;
			l_ulPart2 = ((l_wcpChar [l_lLoop] & 0x000f) >> l_lShiftRight) & 0x0000000f;
			l_ulRawCheckSum = l_ulRawCheckSum + l_ulPart1 + l_ulPart2;
		}

		break;
	}// while (1)

	if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
	{
		*p_ulpCheckSum = 0;
	}
	else
	{
		*p_ulpCheckSum = l_ulRawCheckSum & p_ulMask;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::FindChecksumParity (SVBStr*			p_bstrEncryptedString, 
																  unsigned long	p_ulChecksumSeed, 
																  unsigned long*  p_ulpChecksumParity)
{
	HRESULT	hr;

	long				l_lLength;
	long				l_lShiftRight;
	long				l_lShiftLeft;

	unsigned short	l_usPart1;
	unsigned short l_usPart2;
	unsigned short	l_usChecksumDifference;

	unsigned long	l_ulEncryptionChecksum;


	hr = 0;


	while (1)
	{
//-	The passed-in encryted string does not include the checksum parity.
		hr = CalculateEncryptionChecksum (*p_bstrEncryptedString, 
													 0x0000000f,
													 &l_ulEncryptionChecksum);
		if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
		{
			break;
		}

		l_usChecksumDifference = (unsigned short)(p_ulChecksumSeed - l_ulEncryptionChecksum);

		l_lLength = p_bstrEncryptedString->GetLength ();

//		l_lShiftRight = (l_lLength + 2 + 1) % 6;
		l_lShiftRight = (l_lLength + 2) % 3;
		l_lShiftLeft = 4 - l_lShiftRight;

		l_usPart1 = (l_usChecksumDifference << l_lShiftLeft) & 0x000f;
		l_usPart2 = (l_usChecksumDifference & 0x000f) >> l_lShiftRight;
		*p_ulpChecksumParity = l_usPart1 + l_usPart2;

		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::CreateNameSeed (unsigned long* p_ulpSeedValue)
{
	HRESULT				hr;

	unsigned long		l_ulSeedValue;
	unsigned long		l_ulRand1;
	unsigned long		l_ulMethod;

	double				l_fRandPercent;

	hr = 0;

	while (1)
	{

		hr = CreateRandomLong (&l_ulSeedValue);

		l_ulSeedValue = l_ulSeedValue & 0x3fffffff;

//-	23 is maximum number of Name Attribute encryption algorithms that we 
//-	will support.  The actual encryption method will be embedded within 
//-	the seed.  

		hr = CreateRandomLong (&l_ulRand1);

		l_fRandPercent = (double) l_ulRand1/(double) 0xffffffff;

		if (l_fRandPercent <= 1)
		{
			l_ulMethod = 0;
		}

//-	Only one of the 23 Name Attribute encryption algorithms that are 
//-	permitted, are currently implemented.

		*p_ulpSeedValue = (l_ulSeedValue / 23) * 23 + l_ulMethod; // Name attribute encryption method 0


		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::SetFiller (long				p_lValue,
													  long				p_lOption,
													  SVValue64Union* p_opNewValueUnion)
{
	HRESULT			hr;

	unsigned long	l_ulRand;
	long				l_lMod;


	hr = S_OK;

	l_lMod = 1;

	while (1)
	{
//-	Option -1 means hard coded first format.
//-	Option -2 means hard coded second format.
		if (p_lOption > 0)
		{
			l_lMod = p_lOption;

			hr = CreateRandomLong (&l_ulRand);
			if( SV_SEVERITY( hr ) != SV_LEVEL_SUCCESS )
			{
				break;
			}
		}
		else
		if (p_lOption == -1)
		{
			l_lMod = 2;
			l_ulRand = 2;
		}
		else // p_lOption == -2
		{
			l_lMod = 3;
			l_ulRand = 2;
		}
		
		if (p_lValue == TRUE)
		{
			if ((l_ulRand % p_lOption) == 0)
			{
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] | 0x1200;
			}
			else
			{
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] & 0xedff;
			}
		}
		else // FALSE
		{
			if ((l_ulRand % p_lOption) == 0)
			{
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] | 0x1000;
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] & 0xfdff;
			}
			else
			{
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] | 0x0200;
				p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] & 0xefff;
			}
		}

		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::SetPadding (long					p_lSourceStringLength, 
													   long					p_lCurrentSourceIndex, 
														unsigned long*		p_ulpSourceValue)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		if ((p_lCurrentSourceIndex + 1) == p_lSourceStringLength)
		{
//-		Pad last 3 bytes (first byte is part of string.		
			*p_ulpSourceValue =  (*p_ulpSourceValue & 0x000000ff) + 0x03000000;
		}
		else
		if ((p_lCurrentSourceIndex + 2) == p_lSourceStringLength)
		{
//-		Pad last 2 bytes (first 2 bytes are part of string.		
			*p_ulpSourceValue =  (*p_ulpSourceValue & 0x0000ffff) + 0x02000000;
		}
		else
		if ((p_lCurrentSourceIndex + 3) == p_lSourceStringLength)
		{
//-		Pad last 1 byte (first 3 bytes are part of string.		
			*p_ulpSourceValue =  (*p_ulpSourceValue & 0x00ffffff) + 0x01000000;
		}
		else
		if (p_lCurrentSourceIndex == p_lSourceStringLength)
		{
//-		Pad last 4 bytes, we are already done processing the string.
			*p_ulpSourceValue = 0x04000000;
		}

		break;
	}

	return hr;
}



HRESULT SVXMLEncryptionClass::SetCharSeed (long					p_lValue, 
														 SVValue64Union*	p_opNewValueUnion)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		if (p_lValue == TRUE)
		{
			p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] | 0x2000; // Set Seed1
		}
		else
		{
			p_opNewValueUnion->wcharValue [2] = p_opNewValueUnion->wcharValue [2] & 0xdfff; // Clear Seed1
		}

		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::SetRelativeJump (long				p_lRelativeJump, 
															  SVValue64Union*	p_opNewValueUnion)
{

	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		p_opNewValueUnion->wcharValue [2] = (WCHAR)((p_opNewValueUnion->wcharValue [2] & 0xf7ff) | 
			                              ((p_lRelativeJump & 1) << 11)); // result bit 44.
		p_opNewValueUnion->wcharValue [2] = (WCHAR)((p_opNewValueUnion->wcharValue [2] & 0xfeff) | 
			                              ((p_lRelativeJump & 2) << 7));  // result bit 41
		p_opNewValueUnion->wcharValue [2] = (WCHAR)((p_opNewValueUnion->wcharValue [2] & 0xfbff) | 
			                              ((p_lRelativeJump & 4) << 8));  // result bit 43
		break;
	}


	return hr;
}



HRESULT SVXMLEncryptionClass::SetMethod (long				p_lEncryptionMethod,
													  SVValue64Union*	p_opNewValueUnion)
{

	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		p_opNewValueUnion->wcharValue [2] = (WCHAR)((p_opNewValueUnion->wcharValue [2] & 0xff87) | 
			                                 (p_lEncryptionMethod << 3));	
		break;
	}


	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLEncryptionClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:51:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jun 2007 15:12:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/