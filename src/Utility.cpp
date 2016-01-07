//Copyright (c) 2015 Kotaro Ishii, Yusuke Kazama, and Tokihiro Ikeda
//
//This file is part of DelMapper.
//
//DelMapper is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//any later version.
//
//DelMapper is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with DelMapper.  If not, see <http://www.gnu.org/licenses/>.

#include "Utility.h"
#include "stdio.h"
#include "Configure.h"

typedef unsigned long long int UL64;
typedef unsigned long ULONG;

CUtility::CUtility(void)
{
}

CUtility::~CUtility(void)
{
}

int CUtility::MakeCurPerm(unsigned short* curperm, UL64 iULPerm)
{
	unsigned short card[N_MARKER];
	ULONG icurperm;
	unsigned short *pcard = card;
	for( int i = 0 ; i < N_MARKER ; i++)
	{
		*pcard = i;
		pcard++;
	}
	static UL64 ULFa[]={1, 
		1, //1 
		2, //2
		6, //3
		24, //4
		120, //5
		720, //6
		5040, //7
		40320, //8
		362880, //9
		3628800, //10
		39916800, //11
		479001600, //12
		6227020800, //13
		87178291200, //14
		1307674368000, //15
	    20922789888000, //16
		355687428096000, //17
		6402373705728000, //18
		121645100408832000, //19
		2432902008176640000}; //20

	UL64 ULres_quotient = 0;
	UL64 ULres_residue = 0;
	int i_marker = 0;

	for( i_marker = 0 ; i_marker < 1 ; i_marker++ ) // only for the first (left edge) marker
	{
		int icard;
		UL64 t_iULPerm = iULPerm;
		int ic;

		for( ic = 0 ; ic < N_MARKER ; ic++ )
		{
			if( t_iULPerm > ULFa[N_MARKER-i_marker-1])
			{
				t_iULPerm -= ULFa[N_MARKER-i_marker-1];
			}
			else if ( t_iULPerm < ULFa[N_MARKER-i_marker-1]) break;
			else
			{
				ic++;
				break;
			}
		}

		icard = ic;

		*(curperm + i_marker) = *(card + icard);

		icurperm = *(curperm + i_marker);

		for( int i = 0 ; i < N_MARKER ; i++)
		{
			if( icurperm == *(card + i ))
			{
				for( int k = i ; k < N_MARKER-i_marker-1 ; k++)
				{
					*(card+k) = *(card+k+1);
				}
				break;
			}
		}
	}
// ---------------------------------------------------------------

	UL64 ULmod = 0;
	UL64 ULsave = 0;

	for( i_marker = 1 ; i_marker < N_MARKER-2 ; i_marker++ ) //  for the other markers
	{
		int icard = -1;

		UL64* ULnomi;

		ULnomi = ULFa+N_MARKER-i_marker-1;

		ULres_quotient = iULPerm / *ULnomi;
		ULres_residue = iULPerm % *ULnomi;

		ULmod = (unsigned int)(N_MARKER-i_marker);

		ULsave = ULres_quotient;

		ULres_quotient = ULsave / ULmod;
		ULres_residue  = ULsave % ULmod;
	
		icard= ULres_residue;

		*(curperm + i_marker) = *(card + icard);
		icurperm = *(curperm + i_marker);

		for( int i = 0 ; i < N_MARKER ; i++)
		{
			if( icurperm == *(card + i ))
			{
				for( int k = i ; k < N_MARKER-i_marker-1 ; k++)
				{
					*(card+k) = *(card+k+1);
				}
				break;
			}
		}
	}
	
	*(curperm + i_marker)   = card[ iULPerm    & 1];
	*(curperm + i_marker+1) = card[(iULPerm+1) & 1];

	return 0;
}
