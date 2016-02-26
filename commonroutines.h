/* Include file for some general common routines. v0.3 June 18 2010.
 * Copyright (C) 2010, Shouri Chatterjee
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Most probably you won't have to include this from your program.
 * This header file is called by the lxi/gpib/usb control headers.
 *
 * This header file has some functions defined that can make your
 * life easier.
*/


#ifndef COMMONROUTINES
#define COMMONROUTINES

#include <string.h>
#include <stdlib.h>

/* Now these are non essentials, and I am sure
 * there are better ways of doing these things.
 * Nevertheless, here is a function that parses
 * a comma-separated-string into an array of
 * floating point numbers. */

int ParseStringIntoFloats(char* InputString, float* DestAddress)
{
	int count = -1;
	int i = 0;
	char c = ',';
	char* floatstr = NULL;
	while( c == ',')
	{
		floatstr = calloc(20, sizeof(char));
		int k = 0;
		c = InputString[++count];
		while((c!=',') && (c!='\0'))
		{
			floatstr[k++] = c;
			c = InputString[++count];
		}
		DestAddress[i++] = atof(floatstr);
	}
	return(i);
}

/* The following function checks whether a string is terminated
 * with a '\n' character. If not, it appends the '\n' to the
 * string. */

char *FixNewlineProblemIfAny(char* suspectstring)
{
	int n, retval=0;
	char c;
	char* suspectcopy;
	n = strlen(suspectstring);
	c = suspectstring[n-1];
	if ( c != '\n' )
	{
		suspectcopy = malloc((n+1)*sizeof(char));
		suspectcopy = strcpy(suspectcopy, suspectstring);
		suspectcopy[n] = '\n';
		suspectcopy[n+1] = '\0';
	}
	else suspectcopy = suspectstring;
	return(suspectcopy);
}

#endif


