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
#include <msxml6.h>
#include "SVXMLEncryptionClass.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

namespace  SvXml
{
const WCHAR* const	cNameSeed = L"NameSeed";
long	g_lTempDevelopment = 0;

constexpr long c_JumpingOffset = 3;

constexpr long c_lookUpTable1[256] =
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


//! Disables compile warning conversion from char to unsigned char
//! This type has to remain unsigned char for the encryption to function
#pragma warning (push)
#pragma warning (disable : 4245)
constexpr unsigned char c_CharTable[128] =
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
#pragma warning (pop)




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVXMLEncryptionClass::SVXMLEncryptionClass()
{
	Init();
}

SVXMLEncryptionClass::~SVXMLEncryptionClass()
{

}



HRESULT SVXMLEncryptionClass::Init()
{
	m_lEncryptionMethod = 0;
	m_ulNameSeed = 0;
	m_lIsEncrypted = FALSE;

	m_lInitializedXMLInterface = FALSE;
	m_lInitializedDOMInterface = FALSE;


	//-   Load map to do reverse encoding of m_cCharTable.
	for (long i = 0; i < 128; ++i)
	{
		m_oRevCharTableMap[c_CharTable[i]] = i;
	}

	//-   Load map to do reverse encoding of m_lLookUpTable1.
	for (long i = 0; i < 256; ++i)
	{
		m_oRevLookUpTable1Map[c_lookUpTable1[i]] = i;
	}

	return S_OK;
}


bool SVXMLEncryptionClass::LoadEncryptionFromSaxData(LPCWSTR lpName, LPCWSTR lpContent)
{

	_bstr_t  bstrName(lpName);
	_bstr_t bstrContent(lpContent);
	_bstr_t decryptedName;

	m_ulNameSeed = 0;
	m_lIsEncrypted = TRUE;
	m_lEncryptionMethod = 0;


	HRESULT hr = DecryptString(bstrName, decryptedName);
	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
		return false;
	}


	if (wcscmp (decryptedName, cNameSeed) == 0)
	{
		hr = DecryptString (bstrContent, decryptedName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			return false;
		}
		m_ulNameSeed = wcstol(decryptedName, nullptr, 0);
#if defined (TRACE_THEM_ALL) || defined (TRACE_XMLENCRYPT)
		TRACE ("LoadEncryptionFromSaxData - Loaded NameSeed = %d\n", m_ulNameSeed);
#endif
		return true;	
	}
	return false;
}

unsigned long SVXMLEncryptionClass::GetNameSeed()
{
	return m_ulNameSeed;
}



long SVXMLEncryptionClass::CreateRandomLong()
{
	long result{0L};

	// l_lStaticValue -----------------------------------------------------------
	// This value is never initialized, so it contains what it contains.
	static long	staticValue;

	long ltime = (long) time(nullptr);
	long lclock = clock();

	long seed = (ltime << 4) + lclock + staticValue;

	srand(seed);

	long rand1 = rand();

	long rand2 = rand() & 0x6AAA;

	long rand3 = rand() & 0x5557;

	//  R = Rand

	//  33322222222221111111111000000000
	//  21098765432109876543210987654321
	//  --------------------------------
	//  RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	//  11111111111111122323232323232333

	result = rand1 << 17 | (rand2 << 2) | rand3;

	staticValue += rand2;

	return result;
}


HRESULT SVXMLEncryptionClass::SetEncryptionMethod(long	p_lEncryptionMethod)
{
	m_lEncryptionMethod = p_lEncryptionMethod;
	return S_OK;
}



HRESULT SVXMLEncryptionClass::GetIsEncrypted(long*	p_lpIsEncrypted)
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

HRESULT SVXMLEncryptionClass::EncryptNameAttribute(const _bstr_t& rsourceString, _bstr_t& rEncryptedString)
{
	HRESULT	result{S_OK};

	rEncryptedString = L"";

	long sourceLength = rsourceString.length();

	if(0 == sourceLength)
	{
		return result;
	}

	//-	The 1st of 23 encryption methods.
	switch (m_ulNameSeed % 23)
	{
		case 0:
		{
			unsigned char* pStartSource  = reinterpret_cast<unsigned char*> (static_cast<wchar_t*> (rsourceString));
			long partialLength = (sourceLength + 2) / 3 * 3;

			std::vector<unsigned char> partialEncryption2;
			partialEncryption2.resize(partialLength);

			//-			The + 2 should force it to round up.
			long destinationLength = ((sourceLength + 2) / 3) * 4;

			for(int i = 0; i < partialLength; ++i)
			{
				char singleChar = (i < sourceLength) ? pStartSource[i*2] : '\0';

				//-				Look up the individual byte values in the lookup table.
				unsigned char partialEncryption1 = static_cast<unsigned char> (c_lookUpTable1[(singleChar + i) & 0x000000ff]);

				//-				Shift the word (part of the encryption).
				long leftShift = (i + 3) % 8;
				long rightShift = 8 - leftShift;
				unsigned char leftShiftValue = partialEncryption1 << leftShift;
				unsigned char rightShiftValue = partialEncryption1 >> rightShift;

				partialEncryption2[i] = static_cast<unsigned char> ((leftShiftValue + rightShiftValue + i + m_ulNameSeed) & 0x000000ff);
			}


			std::vector<wchar_t> encryptedVector;
			encryptedVector.resize(destinationLength + 1, L'\0');
			for(int i = 0; i < partialLength; i += 3)
			{
				long value = *(reinterpret_cast<long*> (&(partialEncryption2[i])));

				for(int j = 0; j < 4; ++j)
				{
					long tableIndex = value >> (j * 6) & 0x0000003f;

					encryptedVector[(destinationLength - ((i / 3) * 4 + j + 1))] = c_CharTable[tableIndex];
				}
			}
			rEncryptedString = &encryptedVector.at(0);
			break;
		}
		default:
		{
			result = -1907;
			break;
		}
	}

	return result;
}

HRESULT SVXMLEncryptionClass::DecryptNameAttribute(const _bstr_t& rEncryptedString, _bstr_t& rDecryptedString)
{
	HRESULT result{S_OK};

	rDecryptedString = L"";

	long encryptedLength = rEncryptedString.length();

	//-	If the input encrypted string is empty.
	if (0 == encryptedLength)
	{
		return result;
	}

	switch (m_ulNameSeed % 23)
	{
		case 0:
		{
			long decryptedLength = (encryptedLength / 4) * 3;
			std::vector<unsigned char> partialDecryptVector;
			partialDecryptVector.resize(decryptedLength);
			wchar_t* pStartEncrypted = rEncryptedString;
			for(int i = 0; i < encryptedLength; i += 4)
			{
				long value{0UL};

				for(int j = 0; j < 4; ++j)
				{
					long revTableIndex = m_oRevCharTableMap[pStartEncrypted[encryptedLength - (i + j + 1)]];
					value = value + (revTableIndex << (j * 6));
				}

				long partialIndex = (i / 4) * 3;
				partialDecryptVector[partialIndex] = (reinterpret_cast<unsigned char*> (&value))[0];
				partialDecryptVector[partialIndex + 1] = (reinterpret_cast<unsigned char*> (&value))[1];
				partialDecryptVector[partialIndex + 2] = (reinterpret_cast<unsigned char*> (&value))[2];
			}

			std::vector<wchar_t> decryptedVector;
			decryptedVector.resize(encryptedLength);

			for(int i = 0; i < decryptedLength; ++i)
			{
				unsigned char value = partialDecryptVector[i];

				value = value - static_cast<unsigned char> (i + m_ulNameSeed);
				long rightShift = (i + 3) % 8;
				long leftShift = 8 - rightShift;

				unsigned char leftShiftValue = static_cast<unsigned char> (value << leftShift);
				unsigned char rightShiftValue = static_cast<unsigned char> (value >> rightShift);
				unsigned char index = leftShiftValue + rightShiftValue;

				index = static_cast<unsigned char> (m_oRevLookUpTable1Map[index]);

				decryptedVector[i] = static_cast<wchar_t> ((index - i) & 0x000000ff);
			}
			rDecryptedString = &decryptedVector.at(0);
			break;
		}
		default:
		{
			result = -1939;
			break;
		}
	}

	return result;
}


HRESULT SVXMLEncryptionClass::EncryptString(long encryptionMethod, const _bstr_t& rSourceString, _bstr_t& rEncryptedString)
{
	HRESULT	result{S_OK};

	rEncryptedString = L"";

	long lengthString = rSourceString.length();

	if (lengthString == 0)
	{
		return result;
	}

	long nextDestinationIndex{0L};
	long maxTrackingLength{0L};
	std::vector<long> trackJumping;
	long checksumSeed{0L};
	unsigned long	embeddedSeed {0L};

	//-   This switch statement is to initialize only.
	switch (encryptionMethod)
	{
	case 1:
		{
			//-	String (length + 1) / 4, we will process the low byte of 4, 16 bit 
			//-	characters at a time. * (times) 3 to allow for filler. +1 for 
			//-	SourceStringLengths less than 4.
			maxTrackingLength = (((lengthString + 1) / 4) * 3) + 1;

			trackJumping.resize(maxTrackingLength);

			for (int i= 0; i < maxTrackingLength; ++i)
			{
				trackJumping[i] = i + 1;
			}

			embeddedSeed = CreateRandomLong();
			//-			16777215 is the max 24 bit number.
			embeddedSeed = static_cast<unsigned long> (((double) embeddedSeed / (double) 0xffffffff) * 0x00ffffff);

			checksumSeed = CalculateEncryptionChecksum (rSourceString, 0x0000000f);

			nextDestinationIndex = 0L;

			break;
		}
	case 2:
		{
			//-			For encryption method 2, the encrypted string must always encrypt 
			//-			to the same value.  For this reason, the checksum is also used as 
			//-			the embedded seed.

			embeddedSeed = CalculateEncryptionChecksum(rSourceString, 0x0000000f);

			checksumSeed = embeddedSeed & 0x0000000f;

			//-			Encryption method 2 does not use Next Destination internally (it
			//-			does not use jumping), and filler is not used, so this value can 
			//-			be pre-calculated.
			nextDestinationIndex = (lengthString / 4) + 1;

			break;
		}
	default:
		{
			result = -1888;
			break;
		}
	}

	char pFourChars[4] = {0,0,0,0};
	long sourceIndex {0L};
	long destinationIndex {0L};
	long firstTrackingUnused{0L};
	long trackingLengthThreshold{0L};
	wchar_t* pSource = rSourceString;
	SVValue64Union	valueUnion;
	std::vector<wchar_t>  encryptedVector;
	constexpr long c_MaxEncryptSize = 512;
	encryptedVector.resize(c_MaxEncryptSize, L'\0');

	//-   OK, in theory this architecture allows for the encryption method to be 
	//-	changed during the encryption. Great care should be used when doing 
	//-	this.  When using multiple encryption methods together, the encryption
	//-   methods being used need to be compatible with each other. It should 
	//-   not simply be assumed that all encryption methods will be compatible.
	while ((sourceIndex <= lengthString) || (firstTrackingUnused < trackingLengthThreshold))
	{
		//-      We are reading words and writing bytes.
		pFourChars[0] = static_cast<char> (pSource[sourceIndex + 0]);
		pFourChars[1] = static_cast<char> (pSource[sourceIndex + 1]);
		pFourChars[2] = static_cast<char> (pSource[sourceIndex + 2]);
		pFourChars[3] = static_cast<char> (pSource[sourceIndex + 3]);

		valueUnion.int64Value = 0ULL;

		switch (encryptionMethod)
		{
		case 1:
			{
				result = Method1Encryption (lengthString,
					maxTrackingLength,
					checksumSeed,
					embeddedSeed,
					pFourChars,
					&sourceIndex,
					&destinationIndex,
					&nextDestinationIndex,
					&trackingLengthThreshold,
					&trackJumping[0],
					&firstTrackingUnused,
					&valueUnion);
				if( SEV_SUCCESS != SV_SEVERITY( result ) )
				{
					break;
				}


				break;
			}
		case 2:
			{

				result = Method2Encryption (lengthString,
					checksumSeed,
					pFourChars,
					&sourceIndex,
					&destinationIndex,
					&valueUnion);
				if( SEV_SUCCESS != SV_SEVERITY(result) )
				{
					break;
				}

				break;
			}
		default:
			{
				result = -1935;
				break;
			}
		}

		if( SEV_SUCCESS != SV_SEVERITY( result) )
		{
			break;
		}

		//-		The following loop should take the 48 bits (contained in the int64), 
		//-		and place them 6 bits at a time into 8 wide characters in the 
		//-		encrypted string.
		for (int i = 0; i < 8; ++i)
		{
			long index = static_cast<long> ((valueUnion.int64Value >>  (i * 6)) & 0x000000000000003f);

			encryptedVector[(destinationIndex * 8 + i)] =  c_CharTable[index];
			if (encryptedVector[(destinationIndex * 8 + i)] == 0)
			{
				assert(false);
			}

		}
	}
		
	long currentLength{0L};
	for(const auto& rValue : encryptedVector)
	{
		if(rValue == L'\0')
		{
			break;
		}
		currentLength++;
	}
	for (int i = 0; i < 4; ++i)
	{
		unsigned long index = static_cast<unsigned long> ((embeddedSeed >> (i * 6)) & 0x0000003f);
		encryptedVector[(currentLength + i)] = c_CharTable[index];
	}

	_bstr_t checksumString = &encryptedVector[0];
	long checksumParity = FindChecksumParity(checksumString, checksumSeed);
	//-   Checksum parity must be a 4 bit value at most (0-15).
	checksumParity += (checksumParity < 4) ? 'p' : ('d' - 4);

	encryptedVector[currentLength + 4] = ((wchar_t*) &checksumParity) [0];

	rEncryptedString = &encryptedVector.at(0);
	//-		This code is for development debug only. To confirm that we have 
	//-		correctly handled the checksum parity.
	long checksumTest = CalculateEncryptionChecksum(rEncryptedString, 0x0000000f);

	if (checksumTest != checksumSeed)
	{
		assert(false);
		result = -1908;
	}

	return result;
}


HRESULT SVXMLEncryptionClass::Method1Encryption(
	long					lengthString,
	long					maxTrackingLength,
	long					checksumSeed,
	unsigned long			embeddedSeed,
	char*					pSourceChars,
	long*					pSourceIndex,
	long*					pDestinationIndex,
	long*					pNextDestinationIndex,
	long*					pTrackingLengthThreshold,
	long*					pTrackJumping,
	long*					pFirstTrackingUnused,
	SVValue64Union*			pValueUnion)

{
	HRESULT			hr;

	long				l_lNewDestinationIndex{0L};
	long				l_lTempOffset{0L};

	unsigned long	l_ulTempRand1{0UL};
	unsigned long	l_ulTempSourceValue{0UL};
	unsigned long	l_ulTempResultValue{0UL};



	hr = S_OK;

	while (1)
	{
		*pDestinationIndex = *pNextDestinationIndex;

		unsigned long rand1 = CreateRandomLong();
		unsigned long rand2 = CreateRandomLong();

		//-   Bit descriptions start at 1.

		//-	4% chance of using filler, AND IF there is still enough room for 
		//-	filler to be included.
		//-	or....
		//-	We done processing the string, and are using filler to fill in the 
		//-	gaps.
		bool randomChanceFiller = ((rand1 % 100) < 5);

		bool enoughRoomForFiller = ((lengthString - (*pSourceIndex - 4)) / 4) <
			(maxTrackingLength - *pTrackingLengthThreshold);

		//-	Keep in mind that if the total number of characters is a multiple of 4, 
		//-	there will be one last "data" pass with a padding of 4.
		//-	  Ex. string length of 8 will be 8-1 = 7 which is less than 8 and will
		//-       process.  Current source index should come through as 0, 4, and 8 
		//-       for the 3 passes.
		bool charLeftToProcess = (*pSourceIndex - 1 < lengthString);

		bool useFiller = (randomChanceFiller && enoughRoomForFiller) || !charLeftToProcess;


		if (useFiller)
		{
			pValueUnion->longValue[0] = rand1;
			pValueUnion->wcharValue[2] = static_cast<wchar_t> (rand2);

			//-		50% change bits 42&45 being cleared or set.  Both must be same.
			hr = SetFiller(TRUE, 2, pValueUnion);
			if (SEV_SUCCESS != SV_SEVERITY(hr))
			{
				break;
			}
		} // if (l_lUseFiller == TRUE)
		else // filler not used.
		{
			hr = SetFiller(FALSE, 3, pValueUnion);
			if (SEV_SUCCESS != SV_SEVERITY(hr))
			{
				break;
			}

			l_ulTempSourceValue = *(reinterpret_cast<unsigned long*> (pSourceChars));


			hr = SetPadding(lengthString, *pSourceIndex, &l_ulTempSourceValue);
			if (SEV_SUCCESS != SV_SEVERITY(hr))
			{
				break;
			}

			//-		Grab the next 4, 8 bit bytes for encryption.
			//-		Add in the current character index and the local "for loop counter".
			//-		Look the result up in the lookup table.
			for (int i = 0; i < 4; ++i)
			{
				(reinterpret_cast<char*> (&l_ulTempResultValue))[i] = 
					static_cast<char>((reinterpret_cast<char*> (&l_ulTempSourceValue))[i] + *pSourceIndex + i);

				(reinterpret_cast<unsigned char*>(&l_ulTempResultValue))[i] = static_cast<unsigned char> (c_lookUpTable1[(reinterpret_cast<unsigned char*>(&l_ulTempResultValue))[i]]);
			}

			//-		Although we posess a 32 bit checksum, only the last 4 bits of 
			//-		the checksum seed can actually be used. This is so that the 
			//-		checksum parity concept can work properly.
			l_ulTempResultValue = l_ulTempResultValue +
				((checksumSeed & 0x0000000f) |
				(embeddedSeed & 0x55555555));

			l_ulTempRand1 = CreateRandomLong();

			//-      5% change of setting character seed value, which if set will 
			//-		shift all bits of the current encrypted value 1 bit left (bit 32
			//-      shifts into bit 1 of wcNewValue [2]).
			if ((l_ulTempRand1 % 100) <= 6)
			{
				hr = SetCharSeed(TRUE, pValueUnion);
				if (SEV_SUCCESS != SV_SEVERITY(hr))
				{
					break;
				}

				//-				At the "long integer" level, l_ulTempResultValue appears as
				//-            0x12 34 56 78.  In the final BSTR this will translate into 
				//-				0x78 56 34 12,  So this code will actually be shifting bit 8
				//-				of the resulting BSTR into l_wcNewValue [2].
				pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] | static_cast<wchar_t> (l_ulTempResultValue >> 31);
				pValueUnion->longValue[0] = l_ulTempResultValue << 1;

				//-				Random setting of bit 1.
				if (((l_ulTempRand1 >> 1) % 2) == 0)
				{
					pValueUnion->longValue[0] = pValueUnion->longValue[0] | 0x00000001;
				}

			}
			else
			{
				hr = SetCharSeed(FALSE, pValueUnion);
				if (SEV_SUCCESS != SV_SEVERITY(hr))
				{
					break;
				}

				pValueUnion->longValue[0] = l_ulTempResultValue;
			}

			//-		Pointer math, jump ahead 4 16bit characters.
			*pSourceIndex = *pSourceIndex + 4;

		} // if (l_lUseFiller == TRUE) else


		hr = SetMethod(1, pValueUnion);
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}

		if (*pDestinationIndex >= maxTrackingLength)
		{
			hr = -1934;
			break;
		}

		pTrackJumping[*pDestinationIndex] = -1;

		//-      This loop should only execute if we've just used the first unused.
		while (pTrackJumping[*pFirstTrackingUnused] == -1)
		{
			*pFirstTrackingUnused += 1;
		}

		l_ulTempRand1 = CreateRandomLong();

		long l_lRelativeJump = l_ulTempRand1 % 8;

		long l_lCounter = 0;
		long l_lFoundGoodIndex = FALSE;

		//-	We will loop to find an unused index to jump to (from 3 forward to 
		//-   4 back).
		do
		{
			l_lTempOffset = ((l_lRelativeJump + l_lCounter) % 8) - c_JumpingOffset;
			if ((*pDestinationIndex + l_lTempOffset) < 0)
			{
				//-			Before beginning.
			}
			else
				if ((*pDestinationIndex + l_lTempOffset) >= maxTrackingLength)
				{
					//-			Past end.
				}
				else
					if ((lengthString < *pSourceIndex) &&
						((*pDestinationIndex + l_lTempOffset) > *pTrackingLengthThreshold))
					{
						//-			Done processing string. Destination may no longer grow.
					}
					else
						if (pTrackJumping[*pDestinationIndex + l_lTempOffset] == -1)
						{
							//-			Already in use.
						}
						else
						{
							//-			Must be good.
							//-			We will leave the loop when a good location is found.
							l_lFoundGoodIndex = TRUE;
							l_lNewDestinationIndex = *pDestinationIndex + l_lTempOffset;
							l_lRelativeJump = l_lTempOffset + c_JumpingOffset;
							break;
						}

			l_lCounter = l_lCounter + 1;
		} while (l_lCounter != 8);


		//-		If no good local (within 4 forward or 3 back) address was found, 
		//-		then we will use the first available index
		if (l_lFoundGoodIndex == FALSE)
		{
			l_lNewDestinationIndex = *pFirstTrackingUnused;
			//-			The Relative Jump can not reference the current position. When 
			//-			this happens it means that the first available was used.
			l_lRelativeJump = c_JumpingOffset;
		}

		hr = SetRelativeJump(l_lRelativeJump, pValueUnion);
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}

		*pNextDestinationIndex = l_lNewDestinationIndex;
		if (*pNextDestinationIndex > *pTrackingLengthThreshold)
		{
			*pTrackingLengthThreshold = *pNextDestinationIndex;
		}

		break;
	}

	return hr;
}



HRESULT SVXMLEncryptionClass::Method2Encryption(
	long					p_lSourceStringLength,
	long					p_lChecksumSeed,
	char*					p_cpSourceChars,
	long*					p_lpCurrentSourceIndex,
	long*					p_lpCurrentDestinationIndex,
	SVValue64Union*	p_opNewValueUnion)
{
	HRESULT	hr;

	long				l_lCounter;

	unsigned long	l_ulTempSourceValue;
	unsigned long	l_ulTempResultValue;

	hr = S_OK;

	p_opNewValueUnion->int64Value = 0;

	while (1)
	{
		//-	We will walk backwards.
		*p_lpCurrentDestinationIndex = (p_lSourceStringLength - *p_lpCurrentSourceIndex) / 4;

		hr = SetFiller(FALSE, -1, p_opNewValueUnion);  // -1 means that the 
		// format will not be
		// random.
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}

		hr = SetMethod(2, p_opNewValueUnion);
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}

		l_ulTempSourceValue = *((unsigned long *)p_cpSourceChars);

		hr = SetPadding(p_lSourceStringLength, *p_lpCurrentSourceIndex, &l_ulTempSourceValue);
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}


		//-	Grab the next 4, 8 bit bytes for encryption.
		//-	Add in the local "for loop counter" and an arbitrary 8.
		//-	Look the result up in the lookup table.
		for (l_lCounter = 0; l_lCounter < 4; l_lCounter++)
		{
			((char*)(&l_ulTempResultValue))[l_lCounter] =
				(char)(((char*)&l_ulTempSourceValue)[l_lCounter] +
					   l_lCounter + 8);

			if (g_lTempDevelopment < 10)
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_XMLENCRYPT)
				TRACE("Method2(1.1), %x, counter = %d.\n", ((char*)(&l_ulTempResultValue))[l_lCounter], l_lCounter);
#endif
			}

			((unsigned char*)(&l_ulTempResultValue))[l_lCounter] = (unsigned char)c_lookUpTable1[((unsigned char*)(&l_ulTempResultValue))[l_lCounter]];

			if (g_lTempDevelopment < 10)
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_XMLENCRYPT)
				TRACE("Method2(1.2), %x.\n", ((unsigned char*)(&l_ulTempResultValue))[l_lCounter]);
#endif
			}
		}

		l_ulTempResultValue = ~l_ulTempResultValue + m_ulNameSeed;

		p_opNewValueUnion->longValue[0] = l_ulTempResultValue;

		//-	The CharSeed (actually 4 chars) will not be used for anything in the 
		//-	encryption calculation.  We are only devising a way to set the bit.
		if ((l_ulTempResultValue % 2) == 0)
		{
			hr = SetCharSeed(TRUE, p_opNewValueUnion);
			if (SEV_SUCCESS != SV_SEVERITY(hr))
			{
				break;
			}
		}
		else
		{
			hr = SetCharSeed(FALSE, p_opNewValueUnion);
			if (SEV_SUCCESS != SV_SEVERITY(hr))
			{
				break;
			}
		}

		//-	Pointer math, jump ahead 4 16bit characters.
		*p_lpCurrentSourceIndex = *p_lpCurrentSourceIndex + 4;

		//-   Relative Jump bits are set, but is not used.
		long l_lRelativeJump = p_lChecksumSeed % 8;
		hr = SetRelativeJump(l_lRelativeJump, p_opNewValueUnion);
		if (SEV_SUCCESS != SV_SEVERITY(hr))
		{
			break;
		}

		break;
	} // while (1)

	return hr;
}



HRESULT SVXMLEncryptionClass::DecryptString(const _bstr_t& rEncryptedString, _bstr_t& rDecryptedString)
{
	HRESULT	result{S_OK};

	rDecryptedString = L"";

	long encryptedLength = rEncryptedString.length() - 5;

	//-	If the input encrypted string is empty.
	if (encryptedLength == -5)
	{
		return result;
	}
		
	unsigned long	checksumSeed{0UL};
	//-	Get checksum seed (checksum parity is last two bytes in string).
	checksumSeed = CalculateEncryptionChecksum(rEncryptedString, 0x0000000f);

	long maxTrackingLength = encryptedLength / 8;

	std::vector<long> trackJumping;
	trackJumping.resize(maxTrackingLength);

	std::vector<wchar_t>  decryptedVector;
	decryptedVector.resize(encryptedLength + 1, L'\0');

	//-   Initialize array to track status of individual blocks (8 WCHARs per block).
	for(long i = 0; i < maxTrackingLength; ++i)
	{
		trackJumping[i] = i + 1;
	}


	unsigned long embeddedSeed{0UL};

	//-	Get embedded seed.  This comes after the encrypted string.
	wchar_t* pstartEncrypt = rEncryptedString;
	for (int i = 0; i < 4; ++i)
	{
		unsigned short embeddedSeedStep1 = static_cast<unsigned short> (pstartEncrypt[encryptedLength + i]);
		unsigned short embeddedSeedStep2 = static_cast<unsigned short> (m_oRevCharTableMap[embeddedSeedStep1]);

		embeddedSeed = embeddedSeed + ((static_cast<unsigned long> (embeddedSeedStep2)) << (i * 6));
	}

	long sourceIndex{0L};
	long destinationIndex{0L};
	bool fillerUsed{false};
	long firstTrackingUnused{0L};
	SVValue64Union valueUnion;
	while (firstTrackingUnused < maxTrackingLength)
	{
		valueUnion.int64Value = 0;

		//-		Break out packets of 8 from encrypted source.
		for (long i = 0; i < 8; ++i)
		{
			valueUnion.int64Value = valueUnion.int64Value +
				((static_cast<unsigned __int64>((m_oRevCharTableMap[pstartEncrypt[(sourceIndex * 8) + i]])) << (i * 6)));
		}

		long encryptionMethod = (valueUnion.wcharValue[2] & 0x0078) >> 3;

		if ((((valueUnion.wcharValue[2] & 0x1200) == 0x1200) ||
			((valueUnion.wcharValue[2] & 0x1200) == 0x0000)) &&
			(encryptionMethod == 1))   // This needs to list the 
			// encryption methods that 
			// support filler.
		{
			fillerUsed = true;
		}
		else
		{
			//-			Encryption Method 2 does not support filler.
			fillerUsed = false;
		}

		long relativeJump = (valueUnion.wcharValue[2] & 0x0800) >> 11;
		relativeJump += ((valueUnion.wcharValue[2] & 0x0100) >> 7);
		relativeJump += ((valueUnion.wcharValue[2] & 0x0400) >> 8);

		//-		Prepare packets of 4 for unencrypted destination.

		wchar_t decryptedCharacterArray[4]{L'\0', L'\0', L'\0', L'\0'};

		if (false == fillerUsed)
		{
			switch (encryptionMethod)
			{
				case 1:
				{
					result = Method1Decryption(checksumSeed,
											embeddedSeed,
											destinationIndex,
											&valueUnion,
											decryptedCharacterArray);
					break;
				}
				case 2:
				{
					//-					Method 2 jumps through some hoops here because the string 
					//-					is encrypted backwards (in reverse order).

					if ((sourceIndex == 0) && (destinationIndex == 0))
					{
						//-						This means that we haven't processed anything for this 
						//-						method yet.  Nothing will be processed yet, but values
						//-						will be re-initialized.

						//-						This will force then next source to be set to the next
						//-						available.
						relativeJump = c_JumpingOffset;

						firstTrackingUnused = maxTrackingLength - 1;
					}
					else
					{
						result = Method2Decryption(&valueUnion, decryptedCharacterArray);

						//-						Relative Jumping is not used by this method, though the 
						//-						bits are entered.  We must fudge a relative jump value.
						relativeJump = c_JumpingOffset - 1;

						if (sourceIndex == (maxTrackingLength - 1))
						{
							//-							This should reset not correctly processing the 
							//-							"first" time through.  This should always set 
							//-							l_lCurrentDestinationIndex to 0.
							destinationIndex = destinationIndex - 1;
							firstTrackingUnused = 0;
							trackJumping[0] = 1;
						}

					}
					break;
				}
				default:
				{
					result = -1933;
					break;
				}
			}


			if (SEV_SUCCESS != SV_SEVERITY(result))
			{
				break;
			}

			for (int i = 0; i < 4; ++i)
			{
				decryptedVector[(int)((destinationIndex * 4) + i)] = decryptedCharacterArray[i];
			}

			++destinationIndex;

		}

		trackJumping[sourceIndex] = -1;
		///First check index before accessing
		while ((firstTrackingUnused < maxTrackingLength) && (trackJumping[firstTrackingUnused] == -1))
		{
			firstTrackingUnused++;
		}

		if ((relativeJump - c_JumpingOffset) == 0)
		{
			sourceIndex = firstTrackingUnused;
		}
		else
		{
			//-		We're setting up where the NEXT packet will come from.
			sourceIndex = sourceIndex + relativeJump - c_JumpingOffset;
		}
	}
	long length = static_cast<long> (wcslen((&decryptedVector.at(0))));
	///If the last wchar is < 4 then it is a padding value and needs to be removed
	if(static_cast<long> (decryptedVector.at(length-1)) < 4)
	{
		decryptedVector.at(length - 1) = L'\0';
	}
	rDecryptedString = &decryptedVector.at(0);

	return result;
}


HRESULT SVXMLEncryptionClass::Method1Decryption(unsigned long		p_ulChecksumSeed,
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

		if ((p_opSourceValueUnion->wcharValue[2] & 0x2000) == 0x2000)
		{
			l_lCharacterSeed = 1;
		}
		else
		{
			l_lCharacterSeed = 0;
		}

		if (l_lCharacterSeed == 1)
		{
			l_oTempHoldingUnion.longValue[0] = p_opSourceValueUnion->longValue[0] >> 1;
			l_oTempHoldingUnion.longValue[0] = l_oTempHoldingUnion.longValue[0] |
				(((unsigned long)(p_opSourceValueUnion->wcharValue[2] & 0x0001)) << 31);
		}
		else
		{
			l_oTempHoldingUnion.longValue[0] = p_opSourceValueUnion->longValue[0];
		}

		l_oTempHoldingUnion.longValue[0] = l_oTempHoldingUnion.longValue[0] -
			((p_ulChecksumSeed & 0x0000000f) | (p_ulEmbeddedSeed & 0x55555555));

		for (l_lCount = 0; l_lCount < 4; l_lCount++)
		{
			p_wcpDecryptedCharacters[l_lCount] = (WCHAR)m_oRevLookUpTable1Map[(unsigned char)(l_oTempHoldingUnion.charValue[l_lCount])];

			p_wcpDecryptedCharacters[l_lCount] = (WCHAR)((p_wcpDecryptedCharacters[l_lCount] - ((p_lCurrentDestinationIndex * 4) + l_lCount)) &
														 0x00ff);
		}

		break;
	}


	return hr;
}



HRESULT SVXMLEncryptionClass::Method2Decryption(SVValue64Union*	p_opSourceValueUnion,
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

		l_oTempHoldingUnion.longValue[0] = ~(p_opSourceValueUnion->longValue[0] - m_ulNameSeed);

		for (l_lCount = 0; l_lCount < 4; l_lCount++)
		{
			p_wcpDecryptedCharacters[l_lCount] = (WCHAR)m_oRevLookUpTable1Map[(unsigned char)(l_oTempHoldingUnion.charValue[l_lCount])];

			p_wcpDecryptedCharacters[l_lCount] = (WCHAR)((p_wcpDecryptedCharacters[l_lCount] - (l_lCount + 8)) &
														 0x00ff);
		}

		break;
	}


	return hr;
}

long SVXMLEncryptionClass::CalculateEncryptionChecksum(const _bstr_t& rString, unsigned long mask)
{
	long result{0L};

	long length = rString.length();
	wchar_t*  pChar = rString;

	for (long i = 0; i < length; ++i)
	{
		//-		Each wide character (16 bits) will be shifted left 
		//-		(l_lLoop + 2 % 3).  Basically from 0 to 2 bits.
		//-      
		//-		Each wide character will also be shifted right 4 minus the left 
		//-		shift.  Basically 4 to 2 bits.  
		//-
		//-		The purpose of this algorithm is to allow shifting of the bits, but
		//-		to preserve all bits.
		long shiftLeft = (i + 2) % 3;
		long shiftRight = 4 - shiftLeft;

		long part1 = ((pChar[i] & 0x000f) << shiftLeft) & 0x0000000f;
		long part2 = ((pChar[i] & 0x000f) >> shiftRight) & 0x0000000f;

		result = result + part1 + part2;
	}
	result &= mask;

	return result;
}


long SVXMLEncryptionClass::FindChecksumParity (const bstr_t& rEncryptedString, unsigned long	checksumSeed)
{
	long result{0UL};

	//-	The passed-in encrypted string does not include the checksum parity.
	long encryptionChecksum = CalculateEncryptionChecksum(rEncryptedString, 0x0000000f);

	short checksumDifference = static_cast<short>(checksumSeed - encryptionChecksum);

	long length = rEncryptedString.length();

	long shiftRight = (length + 2) % 3;
	long shiftLeft = 4 - shiftRight;

	long part1 = (checksumDifference << shiftLeft) & 0x000f;
	long part2 = (checksumDifference & 0x000f) >> shiftRight;
	result = part1 + part2;
	return result;
}


HRESULT SVXMLEncryptionClass::CreateNameSeed(unsigned long* p_ulpSeedValue)
{
	HRESULT				hr{S_OK};

	unsigned long		l_ulMethod{0UL};

	while (1)
	{

		long l_ulSeedValue = CreateRandomLong();

		l_ulSeedValue = l_ulSeedValue & 0x3fffffff;

		//-	23 is maximum number of Name Attribute encryption algorithms that we 
		//-	will support.  The actual encryption method will be embedded within 
		//-	the seed.  

		long l_ulRand1 = CreateRandomLong();

		double l_fRandPercent = static_cast<double> (l_ulRand1) / static_cast<double> (0xffffffff);

		if (l_fRandPercent <= 1)
		{
			l_ulMethod = 0UL;
		}

		//-	Only one of the 23 Name Attribute encryption algorithms that are 
		//-	permitted, are currently implemented.

		*p_ulpSeedValue = (l_ulSeedValue / 23) * 23 + l_ulMethod; // Name attribute encryption method 0


		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::SetFiller(bool filler, long option, SVValue64Union* pValueUnion)
{
	//-	Option -1 means hard coded first format.
	//-	Option -2 means hard coded second format.
	unsigned long rand1 = (option > 0) ? CreateRandomLong() : 2;

	if (filler)
	{
		if ((rand1 % static_cast<unsigned long>(option)) == 0)
		{
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] | 0x1200;
		}
		else
		{
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] & 0xedff;
		}
	}
	else
	{
		if ((rand1 % static_cast<unsigned long>(option)) == 0)
		{
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] | 0x1000;
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] & 0xfdff;
		}
		else
		{
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] | 0x0200;
			pValueUnion->wcharValue[2] = pValueUnion->wcharValue[2] & 0xefff;
		}
	}

	return S_OK;
}


HRESULT SVXMLEncryptionClass::SetPadding(long					p_lSourceStringLength,
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
			*p_ulpSourceValue = (*p_ulpSourceValue & 0x000000ff) + 0x03000000;
		}
		else
			if ((p_lCurrentSourceIndex + 2) == p_lSourceStringLength)
			{
				//-		Pad last 2 bytes (first 2 bytes are part of string.		
				*p_ulpSourceValue = (*p_ulpSourceValue & 0x0000ffff) + 0x02000000;
			}
			else
				if ((p_lCurrentSourceIndex + 3) == p_lSourceStringLength)
				{
					//-		Pad last 1 byte (first 3 bytes are part of string.		
					*p_ulpSourceValue = (*p_ulpSourceValue & 0x00ffffff) + 0x01000000;
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



HRESULT SVXMLEncryptionClass::SetCharSeed(long					p_lValue,
										  SVValue64Union*	p_opNewValueUnion)
{
	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		if (p_lValue == TRUE)
		{
			p_opNewValueUnion->wcharValue[2] = p_opNewValueUnion->wcharValue[2] | 0x2000; // Set Seed1
		}
		else
		{
			p_opNewValueUnion->wcharValue[2] = p_opNewValueUnion->wcharValue[2] & 0xdfff; // Clear Seed1
		}

		break;
	}

	return hr;
}


HRESULT SVXMLEncryptionClass::SetRelativeJump(long				p_lRelativeJump,
											  SVValue64Union*	p_opNewValueUnion)
{

	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		p_opNewValueUnion->wcharValue[2] = (WCHAR)((p_opNewValueUnion->wcharValue[2] & 0xf7ff) |
			((p_lRelativeJump & 1) << 11)); // result bit 44.
		p_opNewValueUnion->wcharValue[2] = (WCHAR)((p_opNewValueUnion->wcharValue[2] & 0xfeff) |
			((p_lRelativeJump & 2) << 7));  // result bit 41
		p_opNewValueUnion->wcharValue[2] = (WCHAR)((p_opNewValueUnion->wcharValue[2] & 0xfbff) |
			((p_lRelativeJump & 4) << 8));  // result bit 43
		break;
	}


	return hr;
}



HRESULT SVXMLEncryptionClass::SetMethod(long				p_lEncryptionMethod,
										SVValue64Union*	p_opNewValueUnion)
{

	HRESULT	hr;

	hr = S_OK;

	while (1)
	{
		p_opNewValueUnion->wcharValue[2] = (WCHAR)((p_opNewValueUnion->wcharValue[2] & 0xff87) |
			(p_lEncryptionMethod << 3));
		break;
	}


	return hr;
}

} //namespace SvXml
