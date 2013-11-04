//******************************************************************************
//* .Module Name     : Convert
//* .File Name       : $Workfile:   convert.c  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   29 Apr 2013 10:07:24  $
//******************************************************************************

//* This file include changes of original package code to allow this driver
//* to function in the SVObserver enviroment.

////////////////////////////////////////////////////////////////////////////////
// Module for converting videodata.
// c. kuehnel, intek, 12.5.2001
////////////////////////////////////////////////////////////////////////////////

#include <convert.h>
#include <string.h>

typedef struct
{
  union
  {
    SINT16    Vr;
    SINT16    Ug;
  };

  union
  {
    SINT16    Vg;
    SINT16    Ub;
  };
}TBLENTRY;

TBLENTRY VTable[256];
TBLENTRY UTable[256];

UINT8  TablesInitialized=FALSE;

////////////////////////////////////////////////////////////////////////////////
// Init lookup-tables.
// We use the following formulas:
// R = Y + 1.4022V
// G = Y - 0.3457U - 0.7144V
// B = Y + 1.771U
////////////////////////////////////////////////////////////////////////////////

void InitTables()
{
  SINT16  Vr,Vg,Ug,Ub;
  UINT32  i;
  SINT16  U,V;

  if(TablesInitialized)
   return;

  // Initialize Assemblertables
  for(i=0;i<256;i++)
  {
    U=(SINT16)i-(SINT16)128;
    V=(SINT16)i-(SINT16)128;

    Vr=(SINT16)(1.4022*V);
    Vg=(SINT16)(-0.7144*V);
    Ug=(SINT16)(-0.3457*U);
    Ub=(SINT16)(1.7710*U);

    VTable[i].Vr=Vr;
    VTable[i].Vg=Vg;

    UTable[i].Ug=Ug;
    UTable[i].Ub=Ub;
  }

  TablesInitialized=TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Convert RGB to BGR in mode RGB.
// Is: R G B R G B ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void ConvertRGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
	UINT32 StepSize( BitCnt / 8 );
	UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );

	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize; ++x )
		{
			pBGR[2]=pBuf[0];
			pBGR[1]=pBuf[1];
			pBGR[0]=pBuf[2];

			pBuf+=3;
			pBGR+=StepSize;
		}

		pBGR+=LinePadding;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert RGB16 to BGR in mode RGB.
// Is: R16 G16 B16 R16 G16 B16 ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void ConvertRGB16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT8 Swap,UINT16 BitCnt)
{
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  if(!Swap)
  {
		for( UINT32 y = 0; y < YSize; ++y )
		{
			for( UINT32 x = 0; x < XSize; ++x )
			{
				pBGR[2]=pBuf[0];
				pBGR[1]=pBuf[2];
				pBGR[0]=pBuf[4];

				pBuf+=6;
				pBGR+=StepSize;
			}

			pBGR+=LinePadding;
		}
	}
	else
	{
		for( UINT32 y = 0; y < YSize; ++y )
		{
			for( UINT32 x = 0; x < XSize; ++x )
			{
				pBGR[2]=pBuf[1];
				pBGR[1]=pBuf[3];
				pBGR[0]=pBuf[5];

				pBuf+=6;
				pBGR+=StepSize;
			}

			pBGR+=LinePadding;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:4:4.
// Is: U Y V U Y V U Y V ...
// Windows BITMAP-Format is B G R B G R ...
////////////////////////////////////////////////////////////////////////////////

void Convert444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  TBLENTRY *pU,*pV;
  UINT16    Y;
  UINT16    Res;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
	UINT32 PixelPadding( BitCnt / 8 - 3 );

	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize; ++x )
		{
			pU=UTable+*pBuf++;
			Y=(UINT16)*pBuf++;
			pV=VTable+*pBuf++;

			// Blue
			if((Res=(UINT16)((SINT16)Y+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green
			if((Res=((UINT16)(SINT16)Y+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red
			if((Res=((UINT16)(SINT16)Y+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;
		}

		pBGR+=LinePadding;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:2:2.
// Is: U Y V Y U Y V Y ...
////////////////////////////////////////////////////////////////////////////////

void Convert422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2;
  UINT16    Res;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );

	UINT32 PixelPadding( BitCnt / 8 - 3 );
	
	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize / 2; ++x )
		{
			pU=UTable+*pBuf++;
			Y1=(UINT16)*pBuf++;
			pV=VTable+*pBuf++;
			Y2=(UINT16)*pBuf++;

			// Blue0
			if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green0
			if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red0
			if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;

			// Blue1
			if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green1
			if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red1
			if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;
		}

		pBGR+=LinePadding;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert YUV to RGB in mode 4:1:1.
// Is: U Y Y V Y Y U Y Y V Y Y ...
////////////////////////////////////////////////////////////////////////////////

void Convert411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2,Y3,Y4;
  UINT16    Res;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );

	UINT32 PixelPadding( BitCnt / 8 - 3 );
	
	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize / 4; ++x )
		{
			pU=UTable+*pBuf++;
			Y1=(UINT16)*pBuf++;
			Y2=(UINT16)*pBuf++;
			pV=VTable+*pBuf++;
			Y3=(UINT16)*pBuf++;
			Y4=(UINT16)*pBuf++;

			// Blue0
			if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green0
			if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red0
			if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;

			// Blue1
			if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green1
			if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red1
			if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;

			// Blue2
			if((Res=(UINT16)((SINT16)Y3+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green2
			if((Res=((UINT16)(SINT16)Y3+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red2
			if((Res=((UINT16)(SINT16)Y3+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;

			// Blue3
			if((Res=(UINT16)((SINT16)Y4+pU->Ub))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Green3
			if((Res=((UINT16)(SINT16)Y4+pU->Ug+pV->Vg))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			// Red3
			if((Res=((UINT16)(SINT16)Y4+pV->Vr))>255)
			 Res=-(Res>>8);
			*pBGR++=(UINT8)Res;

			pBGR+=PixelPadding;
		}

		pBGR+=LinePadding;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y16 to RGB (big endian).
////////////////////////////////////////////////////////////////////////////////

void ConvertY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8  Val;

  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
	UINT32 PixelPadding( ( 32 == BitCnt ) ? 1 : 0 );
	UINT32 ElementCount( BitCnt / 8 - PixelPadding );
	
	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize; ++x )
		{
			Val=*pBuf;

			for( UINT32 i = 0; i < ElementCount; ++i )
			{
				*pBGR++=Val;
			}

			pBGR+=PixelPadding;

			pBuf+=2;
		}

		pBGR+=LinePadding;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y8 to BGR or Mono8
// Windows BITMAP-Format is B G R B G R ... or Y Y Y Y ...
////////////////////////////////////////////////////////////////////////////////

void ConvertY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
	UINT32 StepSize( BitCnt / 8 );
	UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );

	if( BitCnt == 8 )
	{
		if( LinePadding == 0 )
		{
			memcpy( (void *)pBGR, pBuf, XSize * YSize );
		}
		else
		{
		  for( UINT32 y = 0; y < YSize; ++y )
		  {
			memcpy( (void *)pBGR, pBuf, XSize );

			pBGR += XSize + LinePadding;
			pBuf += XSize;
		  }
		}
	}
	else
	{
	  for( UINT32 y = 0; y < YSize; ++y )
	  {
		for( UINT32 x = 0; x < XSize; ++x )
		{
			pBGR[2]=*pBuf;
			pBGR[1]=*pBuf;
			pBGR[0]=*pBuf;

			pBuf+=1;
			pBGR+=StepSize;
		}

		pBGR+=LinePadding;
	  }
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert Y16 to RGB byte reversed order (little endian).
////////////////////////////////////////////////////////////////////////////////

void ConvertY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8  Val;
	
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
	UINT32 PixelPadding( ( 32 == BitCnt ) ? 1 : 0 );
	UINT32 ElementCount( BitCnt / 8 - PixelPadding );
	
  pBuf++;
	for( UINT32 y = 0; y < YSize; ++y )
	{
		for( UINT32 x = 0; x < XSize; ++x )
		{
			Val=*pBuf;

			for( UINT32 i = 0; i < ElementCount; ++i )
			{
				*pBGR++=Val;
			}

			pBGR+=PixelPadding;

			pBuf+=2;
		}

		pBGR+=LinePadding;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8RGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;

  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  UINT8* pBGRBuf;

  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pBuf+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pBuf+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8GRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;

  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pBuf+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pBuf+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

      pBGR+=StepSize;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8BGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pBuf+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pBuf+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

	    pBGR+=StepSize;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8GBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT8 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pDst=(struct pdst*)pBGR;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pBuf+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pBuf+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      pDst->B=*pB;
      pDst->G=(UINT8)((*pG0+*pG1)/2);
      pDst->R=*pR;

	    pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 8 bit values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY8(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                  UINT8 BayerPattern,UINT16 BitCnt)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY8RGGB(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 1: ConvertRawY8GRBG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 2: ConvertRawY8BGGR(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 3: ConvertRawY8GBRG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16RGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pMem+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one byte
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

	    pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }

		pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16GRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pMem+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

	    pBGR+=StepSize;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16BGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pMem+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

	    pBGR+=StepSize;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16GBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pMem+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one MSB (it the lower one)
      pDst->B=(UINT8)(*(UINT8*)pB);
      pDst->G=(UINT8)(((*(UINT8*)pG0+*(UINT8*)pG1)/2));
      pDst->R=(UINT8)(*(UINT8*)pR);

	    pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                   UINT8 BayerPattern,UINT16 BitCnt)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY16RGGB(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 1: ConvertRawY16GRBG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 2: ConvertRawY16BGGR(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 3: ConvertRawY16GBRG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern RG GB.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aRGGB(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pB=pG1+1;
      pR=pG1+XSize;
      pG0=pR+1;
    }
    else
    {
      pR=pMem+i*XSize;
      pG0=pR+1;
      pG1=pR+XSize;
      pB=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

	    pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG0+=2;
      }
      else
      {
        pR+=2;
        pG1+=2;
      }
    }
	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern GR BG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aGRBG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pB=pMem+i*XSize;
      pG1=pB+1;
      pG0=pB+XSize;
      pR=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pR=pG0+1;
      pB=pG0+XSize;
      pG1=pB+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

	    pBGR+=StepSize;

      if(j&1)
      {
        pR+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pB+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern BG GR.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aBGGR(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pG1=pMem+i*XSize;
      pR=pG1+1;
      pB=pG1+XSize;
      pG0=pB+1;
    }
    else
    {
      pB=pMem+i*XSize;
      pG0=pB+1;
      pG1=pB+XSize;
      pR=pG1+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

	    pBGR+=StepSize;

      if(j&1)
      {
        pG0+=2;
        pR+=2;
      }
      else
      {
        pB+=2;
        pG1+=2;
      }
    }
	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit invers values pattern GB RG.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16aGBRG(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,UINT16 BitCnt)
{
  UINT16 *pR,*pB,*pG0,*pG1;
  UINT32 i,j;
  UINT16 *pMem;
  struct pdst
  {
    UINT8 B;
    UINT8 G;
    UINT8 R;
  }*pDst;
  UINT8* pBGRBuf;
  UINT32 StepSize( BitCnt / 8 );
  UINT32 LinePadding( ( ( 4 - ( ( 4 - StepSize ) * XSize ) % 4 ) ) % 4 );
	
  pBGRBuf = pBGR;

  pDst=(struct pdst*)pBGR;
  pMem=(UINT16*)pBuf;

  for(i=0;i<YSize-1;i++)
  {
    if(i&1)
    {
      pR=pMem+i*XSize;
      pG1=pR+1;
      pG0=pR+XSize;
      pB=pG0+1;
    }
    else
    {
      pG0=pMem+i*XSize;
      pB=pG0+1;
      pR=pG0+XSize;
      pG1=pR+1;
    }

    // Go through all pixels
    for(j=0;j<XSize-1;j++)
    {
		  pDst=(struct pdst*)pBGR;

      // We only use one byte
      pDst->B=(UINT8)(*((UINT8*)pB+1));
      pDst->G=(UINT8)(((*((UINT8*)pG0+1)+*((UINT8*)pG1+1))/2));
      pDst->R=(UINT8)(*((UINT8*)pR+1));

	    pBGR+=StepSize;

      if(j&1)
      {
        pB+=2;
        pG1+=2;
      }
      else
      {
        pG0+=2;
        pR+=2;
      }
    }

	  pDst=(struct pdst*)pBGR;

    pDst->B=0;
    pDst->G=0;
    pDst->R=0;

    pBGR+=StepSize;

		pBGR+=LinePadding;
  }

  memset(pBGRBuf+(XSize*(YSize-1))*StepSize,0,XSize*StepSize);
}

////////////////////////////////////////////////////////////////////////////////
// Convert raw mode with 16 bit inverted values.
////////////////////////////////////////////////////////////////////////////////

void ConvertRawY16a(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pBGR,
                    UINT8 BayerPattern,UINT16 BitCnt)
{
  switch(BayerPattern)
  {
    case 0: ConvertRawY16aRGGB(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 1: ConvertRawY16aGRBG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 2: ConvertRawY16aBGGR(XSize,YSize,pBuf,pBGR,BitCnt);
            break;

    case 3: ConvertRawY16aGBRG(XSize,YSize,pBuf,pBGR,BitCnt);
            break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Plane-Convert YUV to RGB in mode 4:4:4.
// Is: U Y V U Y V U Y V ...
////////////////////////////////////////////////////////////////////////////////

void XPlane444(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y;
  UINT16    Res;

  Cnt=(XSize*YSize);
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;

    // Blue
    if((Res=(UINT16)((SINT16)Y+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green
    if((Res=((UINT16)(SINT16)Y+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red
    if((Res=((UINT16)(SINT16)Y+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Plane Convert YUV to RGB in mode 4:2:2.
// Is: U Y V Y U Y V Y ...
// R* = Y            + 1.4022 V
// G* = Y - 0.3457 U - 0.7144 V
// B* = Y + 1.7710 U
////////////////////////////////////////////////////////////////////////////////

void XPlane422(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2;
  UINT16    Res;

  Cnt=(XSize*YSize)/2;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y2=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Plane Convert YUV to RGB in mode 4:1:1.
// Is: U Y Y V Y Y U Y Y V Y Y ...
////////////////////////////////////////////////////////////////////////////////

void XPlane411(UINT32 XSize,UINT32 YSize,UINT8 *pBuf,UINT8 *pR,UINT8 *pG,UINT8 *pB)
{
  UINT32    Cnt;
  TBLENTRY *pU,*pV;
  UINT16    Y1,Y2,Y3,Y4;
  UINT16    Res;

  Cnt=(XSize*YSize)/4;
  while(Cnt--)
  {
    pU=UTable+*pBuf++;
    Y1=(UINT16)*pBuf++;
    Y2=(UINT16)*pBuf++;
    pV=VTable+*pBuf++;
    Y3=(UINT16)*pBuf++;
    Y4=(UINT16)*pBuf++;

    // Blue0
    if((Res=(UINT16)((SINT16)Y1+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green0
    if((Res=((UINT16)(SINT16)Y1+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red0
    if((Res=((UINT16)(SINT16)Y1+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue1
    if((Res=(UINT16)((SINT16)Y2+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green1
    if((Res=((UINT16)(SINT16)Y2+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red1
    if((Res=((UINT16)(SINT16)Y2+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue2
    if((Res=(UINT16)((SINT16)Y3+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green2
    if((Res=((UINT16)(SINT16)Y3+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red2
    if((Res=((UINT16)(SINT16)Y3+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;

    // Blue3
    if((Res=(UINT16)((SINT16)Y4+pU->Ub))>255)
     Res=-(Res>>8);
    *pB++=(UINT8)Res;

    // Green3
    if((Res=((UINT16)(SINT16)Y4+pU->Ug+pV->Vg))>255)
     Res=-(Res>>8);
    *pG++=(UINT8)Res;

    // Red3
    if((Res=((UINT16)(SINT16)Y4+pV->Vr))>255)
     Res=-(Res>>8);
    *pR++=(UINT8)Res;
  }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\Intek\convert.c_v  $
 * 
 *    Rev 1.0   29 Apr 2013 10:07:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Oct 2009 18:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.29
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated files based on changes from the Intek provider from 2v15 to 2v30.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Aug 2006 10:56:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Y8 conversion routine.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Jul 2006 15:24:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  569
 * SCR Title:  Fix Format 7 Color Issue with Intek Acquisition Sub-system
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added to new files to handle converting a flat image block to a Windows Bitmap DIB block.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
