// Guid.h
#pragma once
#ifndef GUID_H
#define GUID_H

bool operator < (const GUID& guid1, const GUID& guid2)
{
	// this does not implement REAL ordering, but enough for our map's purposes
	const BYTE* pBytes1 = reinterpret_cast<const BYTE*> (&guid1);
	const BYTE* pBytes2 = reinterpret_cast<const BYTE*> (&guid2);
	for ( int i=0; i < sizeof(GUID); i++)
	{
		if ( pBytes1[i] == pBytes2[i] )
			continue;
		else if ( pBytes1[i] < pBytes2[i] )
			return true;
		else
			return false;
	}
	return false;
}
#endif  //#ifndef GUID_H