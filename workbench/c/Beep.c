/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Beep
    Lang: English
*/

/*****************************************************************************

    NAME
	Beep

    FORMAT
	Beep

    SYNOPSIS

    LOCATION
	Workbench:C

    FUNCTION
	BEEP produces a beep via Intuition DisplayBeep(NULL).

    EXAMPLE

    SEE ALSO

******************************************************************************/

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <proto/intuition.h>
#include <intuition/screens.h>

#include <stdio.h>

static const char version[] = "$VER: Beep 41.0 (30.12.2000)";

int IntuitionBase_version = 0;

int main()
{
    DisplayBeep( NULL );
    return 0;
}

