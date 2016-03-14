//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Err200_249
//* .File Name       : $Workfile:   ec200249.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:40:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVErrorClass.h"

//
// Next error number to use 200003  - 16 Nov 1999 - frb.
//
int SVErrorClass::Err200_249 (int *found)
{

    *found = TRUE;
    
    switch (msvlLastProgramCd)
    {
		//
        case 20000:
            //switch (lastErrorCd & 0x0000ffff)
            //{
            //    case 20000:

                    msvlLastErrorAction = DISPLAY +
                                          ACKNOWLEDGE +
                                          LOG + 
									               TERMINATE +
                                          BRK;

                    msvlLastFacillity = SVR;
                    msvlLastMessageNbr = 0; //?????? - Message Text handle.
                    break;

            //    default:
            //        break;
            //}
            //break;

        default:

            *found = FALSE;
            break;
    }


    return 0;
}

