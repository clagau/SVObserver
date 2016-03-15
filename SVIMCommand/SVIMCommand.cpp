// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommand
// * .File Name       : $Workfile:   SVIMCommand.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:26:44  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIMCommand.h"


//##ModelId=38BAE3E10109
SVIMCommand::SVIMCommand()
{
	// Note: these strings are NOT supposed to be UNICODE

	// Command Strings
	mszCommandList.Add ("PutConfig ");
	mszCommandList.Add ("GetConfig ");
	mszCommandList.Add ("GetCurrentConfig ");
	mszCommandList.Add ("SetCurrentConfig ");
	mszCommandList.Add ("GoOffline ");
	mszCommandList.Add ("GoOnline ");
	mszCommandList.Add ("GetState ");
	mszCommandList.Add ("Cancel ");
	mszCommandList.Add ("Data ");
	mszCommandList.Add ("PutFile ");
}

//##ModelId=38BAE3ED00DA
SVIMCommand::~SVIMCommand()
{
	mszCommandList.RemoveAll ();
}

//##ModelId=38BAE3FE0138
int SVIMCommand::GetCommandToken(char * pszCommandText)
{
	int i;

	for (i = 0; i < mszCommandList.GetSize (); i++)
	{
		if (!_strnicmp (pszCommandText, mszCommandList.GetAt (i), strlen (mszCommandList.GetAt (i))))
		{
			return i;
		}
	}
	return -1;
}

//##ModelId=38BAE44001A5
char *  SVIMCommand::GetCommandText(int CommandToken )
{
	if (CommandToken > -1 && CommandToken < mszCommandList.GetSize ())
	{
		return mszCommandList.GetAt(CommandToken);
	}

	return NULL;
}

BOOL SVIMCommand::escape(char *pInput, char *pOutput, int cbOutput)
{
	char *p;
	int i;

	i = 0;
	for (p = pInput; *p && (i < cbOutput); p++)
	{
		if (isalnum (*p))
		{
			pOutput[i++] = *p;
		}
		else
		{
			if (i < (cbOutput - 3))
			{
				sprintf (&pOutput[i], "\\%02.2x", (int) *p);
				i += 3;
			}
			else
			{
				return FALSE;		// not enough space in output buffer
			}
		}
	}
	pOutput[i] = 0;
	return TRUE;
}

BOOL SVIMCommand::unescape(char *pInput, char *pOutput, int cbOutput)
{
	char *p;
	int i, j;

	i = 0;
	p = pInput;

	for (i = 0; i < cbOutput; i++)
	{
		switch (*p)
		{
			case '\\' :
				sscanf (p, "\\%2x", &j);
				pOutput[i] = (char) j;
				p += 3;
				break;

			default :
				if (isalnum (*p))
				{
					pOutput[i] = *p;
					p++;
				}
				else
				{
					pOutput[i] = 0;
					return TRUE;
				}
		}
	}
	return FALSE;
}

