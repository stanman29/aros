/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Init of workbench.library
    Lang: english
*/

#include <aros/symbolsets.h>

#include "lowlevel_intern.h"
#include LC_LIBDEFS_FILE

AROS_SET_LIBFUNC(Init, LIBBASETYPE, lh)
{
    AROS_SET_LIBFUNC_INIT;
    
    InitSemaphore(&lh->ll_Lock);
    lh->ll_VBlank.is_Data = NULL;
    lh->ll_VBlank.is_Code = NULL;

    return TRUE;
    
    AROS_SET_LIBFUNC_EXIT;
}

ADD2INITLIB(Init, 0);
