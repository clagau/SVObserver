//******************************************************************************
//* .Module Name     : Regexp
//* .File Name       : $Workfile:   Regexp.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:57:18  $
//******************************************************************************

#ifndef __REGEXP_H__
#define __REGEXP_H__

// http://www.codeguru.com/Cpp/Cpp/string/regex/article.php/c2779
//
//class CString;
class regexp;

class /*AFX_EXT_CLASS*/ Regexp
{
public:
	enum { NSUBEXP = 10 };

	Regexp();
	Regexp( LPCTSTR exp, BOOL iCase = 0 );
	Regexp( const Regexp &r );
	~Regexp();
	const Regexp & operator=( const Regexp & r );

	bool Match( const TCHAR * s );
	int SubStrings() const;
	
	const CString operator[]( unsigned int i ) const;
	int SubStart( unsigned int i ) const;
	int SubLength( unsigned int i ) const;

	CString GetReplaceString( LPCTSTR source ) const;

	CString GetErrorString() const;
	bool CompiledOK() const;

#if defined( _RE_DEBUG )
	void Dump();
#endif
private:
	const TCHAR * string;	/* used to return substring offsets only */
	mutable CString m_szError;
	regexp * rc;

	void ClearErrorString() const;
	int safeIndex( unsigned int i ) const;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\Regexp.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:57:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Mar 2006 13:42:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/