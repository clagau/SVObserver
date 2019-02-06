//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEquationLexClass
//* .File Name       : $Workfile:   SVEquationLex.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:23:00  $
//******************************************************************************

#pragma once

#include "SVParserScripts/FlexLexer.h"
#ifndef Y_TAB_H
#define Y_TAB_H
	#include "SVParserScripts/y.tab.h"
#endif // #ifndef Y_TAB_H

class SVEquationBase;

/**
@SVObjectName Equation LEX

@SVObjectOverview This object performs the scanning for the lexical grammar of the supplied buffer to extract the symbolic information to generate the associated token values.

@SVObjectOperations This object presents all of the public functionality of the Flex Lexer and provides the necessary operations to interface into the main application data.

The Scan Buffer operator creates and initializes an internal scan buffer and validates the provided buffer size.

The Scan Bytes operator creates an internal scan buffer and copies the provided data to that buffer.

The Scan String operator validates the provided buffer and determines the string length.

The Lexical Error operator updates the error status and error position information.

The Lexical Scan operator performs the scanning for the lexical grammar of the provided buffer.

The Add Symbol operator notifies the application of the desired symbol and responds based on symbol availability.

*/
class SVEquationLexClass : public yyFlexLexer
{
public:
	SVEquationLexClass();
	virtual ~SVEquationLexClass();

	struct yy_buffer_state* yy_scan_buffer( char *base, yy_size_t size );
	struct yy_buffer_state* yy_scan_bytes( const char *bytes, int len );
	struct yy_buffer_state* yy_scan_string( const char *yy_str );

	virtual void LexerError( const char *msg ) override;

	virtual int yylex() override;

	void setEquation(SVEquationBase* pEquation){ m_pEquation = pEquation; }

protected:
	int addSymbol( const char* name );

public:
	int lex_err;
	size_t currentPos;
	size_t position;
	int syntax_check;
	int paren;
	YYSTYPE yylval;

protected:
	SVEquationBase* m_pEquation;
};

