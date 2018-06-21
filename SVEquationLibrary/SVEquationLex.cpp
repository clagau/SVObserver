//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationLexClass
//* .File Name       : $Workfile:   SVEquationLex.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:22:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVEquationLex.h"
#include "SVEquationBase.h"

//Moved to precompiled header: #include <iostream>
// Using namespace is required for the lex.yy.cc
using namespace std;
#pragma warning( push )
#pragma warning( disable : 4102 4013 4244 ) 
#include "SVParserScripts/lex.yy.cc"

SVEquationLexClass::SVEquationLexClass()
{
	lex_err = 0;
	currentPos = 0;
	position = 0;
	syntax_check = 1;
	paren = 0;
}

SVEquationLexClass::~SVEquationLexClass()
{
}

/**
@SVOperationName Scan Buffer

@SVOperationDescription This operator allocates the new scan buffer and initializes the Buffer State data.  If an error occues, an error status will be returned.

*/
struct yy_buffer_state* SVEquationLexClass::yy_scan_buffer( char *base, yy_size_t size )
{
	struct yy_buffer_state* b;

	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		/* They forgot to leave room for the EOB's. */
		return 0;

	b = (struct yy_buffer_state*) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = size - 2;	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = 0;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer( b );

	return b;
}

/**
@SVOperationName Scan Bytes

@SVOperationDescription This operator creates a buffer for scanning the provided byte block.  The provided data is copied into the created buffer.  The Scan Buffer operator is executed.  If an error occurs, an error status is returned.

*/
struct yy_buffer_state* SVEquationLexClass::yy_scan_bytes( const char *bytes, int len )
{
	struct yy_buffer_state* b;
	char *buf;
	yy_size_t n;
	int i;

	/* Get memory for full buffer, including space for trailing EOB's. */
	n = len + 2;
	buf = (char *) yy_flex_alloc( n );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < len; ++i )
		buf[i] = bytes[i];

	buf[len] = buf[len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer( buf, n );
	if ( ! b )
		YY_FATAL_ERROR( "bad buffer in yy_scan_bytes()" );

	/* It's okay to grow etc. this buffer, and we should throw it
	 * away when we're done.
	 */
	b->yy_is_our_buffer = 1;

	return b;
}

/**
@SVOperationName Scan String

@SVOperationDescription This operator walks the character string until it encounters a NULL character or the length of the provided buffer.  After the length of the string is determined, the Scan Bytes operator is executed with the provided buffer and the caculated length.  If an error occues, an error status is returned.

*/
struct yy_buffer_state* SVEquationLexClass::yy_scan_string( const char *yy_str )
{
	int len;
	for ( len = 0; yy_str[len]; ++len )
		;

	return yy_scan_bytes( yy_str, len );
}

void SVEquationLexClass::LexerError( const char* msg )
{
	lex_err = 1;
	position = currentPos;
}

int SVEquationLexClass::addSymbol( const char* name )
{
	// call the Equation Class to add this symbol
	return pEquation->AddSymbol( name );
}

#pragma warning( pop )

