//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLEncryptionClass
//* .File Name       : $Workfile:   SVXMLEncryptionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:51:22  $
//******************************************************************************

#pragma once
#pragma warning (disable: 4786)	// identifier truncation 255 chars

//Moved to precompiled header: #include <map>
#include "SVLibrary/SVBStr.h" 					// SVBStr

namespace  SvXml
{
	union SVValue64Union
	{
		char					charValue [8];
		WCHAR					wcharValue [4];
		unsigned long		longValue [2];
		unsigned __int64	int64Value;
	};

	// SVXMLEncryptionClass -----------------------------------------------------
	// In general I would like to keep this class interfacing with the 
	// SVDOMClass.  However, I have previously kept all schema definition stuff
	// out of the SVDOMClass under the pretext that would be application specific
	// stuff.  In order to keep the encryption stuff together, I have decided to 
	// violate my concept of good structure, and include the encryption schema 
	// stuff in the SVCMLEncryptionClass.
	// 
	// To me, this means that the SVXMLEncryptionClass should provide two, 
	// conceptually different and separate interfaces; one to the SVDOMClass,
	// and one to the SVXMLClass.
	// 
	// Schema functionallity will be assumed to be at the SVXMLClass level and
	// encryption functionallity will be assumed to be at the SVDOMClass level.
	// 
	// For now the SVDOMClass will "own" the SVXMLEncryptionClass, and the 
	// SVXMLClass will gain its access through the DOM.
	// JAB032405
	//
	class SVXMLEncryptionClass
	{
	public:
		SVXMLEncryptionClass();
		virtual ~SVXMLEncryptionClass();


		//************************************
		//! Set the parameter for decryption 
		//! \param lpName [in]
		//! \param lpContent [in]
		//! \returns HRESULT
		//************************************
		bool LoadEncryptionFromSaxData(LPCWSTR lpName, LPCWSTR lpContent);

		unsigned long GetNameSeed();

	// SetEncryption () ---------------------------------------------------------
	// This function sets the encryption element information in the schema.
		HRESULT	SetEncryption();

		void  SetIsEncryption(BOOL isEncrypted){ m_lIsEncrypted = isEncrypted;};
		BOOL GetIsEncrypted() {return m_lIsEncrypted; } ;
		void CreateNameSeed() {CreateNameSeed(&m_ulNameSeed);};


		HRESULT	GetIsEncrypted (long*		p_lpIsEncrypted);

		HRESULT	SetEncryptionMethod (long	p_lEncryptionMethod);

	// DOM Interface ----------------------------------------------------------\/
		//HRESULT	InitializeDOMInterface (SVDOMClass*		p_opParentDOM);

		HRESULT	EncryptNameAttribute (BSTR		p_bstrSourceString, 
											   SVBStr*	p_bstrEncryptedString);

		HRESULT  DecryptNameAttribute (BSTR		p_bstrEncryptedString, 
												 SVBStr*	p_bstrpDecryptedString);


		HRESULT	EncryptString (long		p_lNameEncryptionMethod, 
										BSTR		p_bstrElementName, 
										SVBStr*	p_bstrElementTag);

		HRESULT  DecryptString (BSTR		p_bstrEncryptedString, 
									  SVBStr*	p_bstrpDecryptedString);


	// END OF DOM Interface----------------------------------------------------/\
	//

		long				m_lEncryptionMethod;

	protected:
		HRESULT	Init ();

	// CreateRandomLong () ------------------------------------------------------
	// This function will generate a 32 bit random value (even though the rand 
	//	function only provides a 31 bit random value.
		HRESULT			CreateRandomLong (unsigned long* p_lSeedValue);

		HRESULT			CreateNameSeed (unsigned long* p_ulpSeedValue);


		HRESULT			Method1Encryption (BSTR		p_bstrSourceString, 
													SVBStr*	p_bstrEncryptedString);

		HRESULT			Method1Encryption (
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
				SVValue64Union*	p_opNewValueUnion );


		HRESULT			Method2Encryption (
				long					p_lSourceStringLength,
				long					p_lChecksumSeed,
				char*					p_cpSourceChars,
				long*					p_lpCurrentSourceIndex,
				long*					p_lpCurrentDestinationIndex,
				SVValue64Union*	p_opNewValueUnion );


	//	SetFiller () -------------------------------------------------------------
	// This function will SET ALL affected bits.
	//   p_lOption - input, 
	//	    -1  = means hard coded first format.
	//	    -2  = means hard coded second format.
	//     > 0 = positive values represent the MOD (%) variable that will be used 
	//  	   internally to determine the format.
		HRESULT			SetFiller (long				p_lValue,
										  long				p_lOption,
										  SVValue64Union* p_opNewValueUnion);

	//	SetPadding () ------------------------------------------------------------
	// This function will SET ALL affected bits.
		HRESULT			SetPadding (long					p_lSourceStringLength, 
										   long					p_lCurrentSourceIndex, 
											unsigned long*		p_ulpSourceValue);


	//	SetCharSeed () -----------------------------------------------------------
	// Actually this is the seed for a four character packet.  This function will 
	// SET ALL affected bits.
		HRESULT			SetCharSeed (long					p_lValue, 
											 SVValue64Union*	p_opNewValueUnion);


	//	SetRelativeJump () -------------------------------------------------------
	// This assumes that the bits in question were previously set to zero.
		HRESULT			SetRelativeJump (long				p_lRelativeJump, 
												  SVValue64Union*	p_opNewValueUnion);

	// SetMethod () -------------------------------------------------------------
	// This function will SET ALL affected bits.
		HRESULT			SetMethod (long				p_lEncryptionMethod,
										  SVValue64Union*	p_opNewValueUnion);


	// JumpTracking1Initialization () -------------------------------------------
	// 
		HRESULT			JumpTracking1Initialization (long	p_lSourceStringLength, 
														long**	p_lppTrackJumping, 
																 long*	p_lpMaxTrackingLength);


	// Method1Sub1 () -----------------------------------------------------------
	// A complex subroutine call from multiple locations from within 
	//	Method1Encryption ().
		HRESULT			Method1Sub1 (
				long				p_lSourceStringLength,
				long				p_lMaxTrackingLength,
				long				p_lChecksumSeed,
				unsigned long	p_ulEmbeddedSeed,
				long*				p_lpCurrentSourceIndex,
				long*				p_lpCurrentDestinationIndex,
				long*				p_lpCurrentTrackingLengthThreshold,
				long*				p_lpTrackJumping,
				long*				p_lpFirstTrackingUnused,
				WCHAR*			p_wcpSource,
				SVBStr*			p_bstrpEncryptedString);



		HRESULT			Method1Decryption (unsigned long		p_ulChecksumSeed,
													 unsigned long		p_ulEmbeddedSeed,
													 long					p_lCurrentDestinationIndex,
													 SVValue64Union*	p_opSourceValueUnion,
													 WCHAR*				p_wcpDecryptedCharacters);



		HRESULT			Method2Decryption (SVValue64Union*	p_opSourceValueUnion,
													 WCHAR*				p_wcpDecryptedCharacters);



	// CalculateEncryptionChecksum () ------------------------------------------
	// This function will calculate a checksum value for a given string.  The 
	// intent is that this function be used on both the original (unencrypted)
	// string, and the encrypted string. The checksum for both should be the 
	// same.  
		HRESULT			CalculateEncryptionChecksum (BSTR				p_bstrString, 
																  unsigned long	p_ulMask,
													 unsigned long*	p_ulpChecksum);

	// FindChecksumParity () ---------------------------------------------------
	// This function calculates the current checksum of the string, and 
	//	calculates a word whoes appended values would make the given string 
	// calculate to the desired checksum.
		HRESULT			FindChecksumParity (SVBStr*			p_bstrEncryptedString, 
													  unsigned long	p_ulChecksumSeed, 
													  unsigned long*  p_ulpChecksumParity);


		long				m_lInitializedXMLInterface;
		long				m_lInitializedDOMInterface;

	// m_ulNameSeed -------------------------------------------------------------
	// When saving a configuration, this is set by SetEncryption ().	
		unsigned long				m_ulNameSeed;

		long							m_lIsEncrypted;

		static long					m_lLookUpTable1 [256];
		std::map <long, long>	m_oRevLookUpTable1Map;

		static unsigned char		m_cCharTable [128];
		std::map <long, long>	m_oRevCharTableMap;

	};

	extern const WCHAR* const	cNameSeed;

} //namespace SvXml
