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

#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "Configure.h"
#include "Utility.h"
#include "Marker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef  unsigned long long int UL64;

int main()
{
	void Permutation_main();

	Permutation_main();
}

void Permutation_main()
{
	CUtility cut;
	CMarker cma;

	void Iteration(CUtility*, char**, char**, unsigned short*, unsigned short*);

	char* marker[N_MARKER];
	char* sidemarker[N_MARKER + 2]; // Used when side markers are fixed.
	unsigned short curperm[N_MARKER];    // AABBCCDD...
	unsigned short bestperm[N_MARKER];    // AABBCCDD...
	
	Iteration(&cut, marker, sidemarker, curperm, bestperm);
	cma.ReleaseMarker(marker);
}

void Iteration(CUtility* p_cut, char** marker, char** sidemarker, unsigned short* curperm, unsigned short* bestperm)
{
	CMarker cma;
	UL64 iULPerm = 0; //focused permutation
	UL64 iULPermSave = 0; //permutation with best score
	UL64 iULdx = 1; //increment value

    static UL64 factorial[] = 
	{
		1, //0
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
		2432902008176640000 //20
	};

// -----------------------------------------------------------
//
// initial iPerm value
//
	iULPerm = PERM_START_FROM * ( factorial[N_MARKER] / PERM_DIVIDE_BY );
    printf ("N of markers = %d\nN of plants = %d\nPermutation from %llu, to %llu will be processed, totally %llu calculations.\nDel option : %d\n",
		N_MARKER, N_PLANT - 1,
		PERM_START_FROM * ( factorial[N_MARKER] / PERM_DIVIDE_BY ),
		PERM_END_BY * ( factorial[N_MARKER] / PERM_DIVIDE_BY ) - 1,
		( PERM_END_BY - PERM_START_FROM ) * (factorial[N_MARKER] / PERM_DIVIDE_BY),
		DEL_OPTION);

// -----------------------------------------------------------

	int sw_file_use = 1; // Switch for file output
	FILE *fp_out;
	if( sw_file_use )
	{
		printf ("Output file : %s\n", OUT_NAME);
		char* filename = OUT_NAME;
   		if( NULL == (fp_out = fopen(filename, "w")))
		{
			printf("Error! : %s cannot open.\n", filename);
			return;
		}
	}

// ----------------------------------------------------------- 

	short matrix[N_PLANT][N_MARKER]; // Read matrix data from input file
	cma.GetMatrix(marker, matrix[0]);
	cma.ShowMarker(marker);

	short sidematrix[N_PLANT][N_MARKER+FIX_LEFT+FIX_RIGHT];
	cma.GetSideMatrix(sidemarker, sidematrix[0]);
	cma.ShowSideMarker(sidemarker);

	int ibreakage_best = 2000000000; // Least number of breakages : the lower, the better

// -----------------------------------------------------------

	int phase = 0; // Switch for pruning

	while( iULPerm < PERM_END_BY * ( factorial[N_MARKER] / PERM_DIVIDE_BY ) )
	{
		p_cut->MakeCurPerm(curperm, iULPerm);

// -----------------------------------------------------------
//
// energy function calculation
//

		int ibreakage= 0; // Number of breakages
		int ielement;
		unsigned short iclusternum;
		int node_order = 0; // Order of focused marker in the permutation, used for pruning
        int ele_stat[N_PLANT]; // Status of previous element in focused plant, 0: absent; 1: present

		for (int i_plant = 1; i_plant < N_PLANT; i_plant++)
		{
			ele_stat[i_plant] = 1;
		}

		if ( FIX_LEFT ) // If marker on left side is fixed
		{
			iclusternum = sidematrix[0][N_MARKER]; // Number of markers included in focused element
			for ( int i_plant = 1; i_plant < N_PLANT; i_plant++)
			{
				ielement = sidematrix[i_plant][N_MARKER]; // Status of focused element
				if (ielement == 0) // If all markers absent in focused element
				{
					ele_stat[i_plant] = 0; // Set for calculation of next order
				}
				else if (ielement == iclusternum) // If all markers present
				{
					ele_stat[i_plant] = 1;
				}
				else // Some markers absent, the others present
				{
					ibreakage++;
					ele_stat[i_plant] = 0; // All present markers gathered on the left side
				}
			}
		}

		for( unsigned short* p_curperm = curperm;
			p_curperm < curperm + N_MARKER;
			p_curperm++, node_order++)
		{
			for( int i_plant = 0 ; i_plant < N_PLANT ; i_plant++)
			{
				short* p_mtrx = matrix[i_plant];
				ielement = *(p_mtrx + *p_curperm);

				if (i_plant == 0)
				{
					iclusternum = ielement;
				}
				else
				{
					if ( ele_stat[i_plant] == 1 )
					{
						if ( ielement < iclusternum )
						{
							ibreakage++;
							ele_stat[i_plant] = 0;
						}
						else
						{
							ele_stat[i_plant] = 1;
						}
					}
					else if ( ele_stat[i_plant] == 0 )
					{
						if ( ielement == 0 )
						{
							ele_stat[i_plant] = 0;
						}
						else
						{
							ibreakage++;
							ele_stat[i_plant] = 1;
						}
					}
				}
			} // i_plant loop

			if ( ibreakage > ibreakage_best )
			{
			    phase = 1;
			    goto _pruning;
			}
		} // p_curperm loop

		if ( FIX_RIGHT )
		{
			for ( int i_plant = 0; i_plant < N_PLANT; i_plant++)
			{
				if ( i_plant == 0 )
				{
					iclusternum = sidematrix[0][N_MARKER+FIX_LEFT];
				}
				else
				{
					ielement = sidematrix[i_plant][N_MARKER+FIX_LEFT];
					if ( ele_stat[i_plant] == 1 )
					{
						if ( ielement < iclusternum )
						{
							ibreakage++;
						}
					}
					else if ( ele_stat[i_plant] == 0 )
					{
						if ( ielement > 0 )
						{
							ibreakage++;
						}
					}
				}
			}
		}
		else
		{
			for ( int i_plant = 1; i_plant < N_PLANT; i_plant++)
			{
				if ( ele_stat[i_plant] == 1 && DEL_OPTION )
				{
					ibreakage++;
				}
			}
		}

// -----------------------------------------------------------
//
// update the best ibreakage
//

		if(ibreakage <= ibreakage_best)
		{
			if ( ibreakage < ibreakage_best )
			{
				printf("New record   : ");
			}
			else
			{
				printf("Equal record : ");
			}

			for( int jj = 0 ; jj<N_MARKER ; jj++)
	        {
		        printf("%2d,", curperm[jj]);
	        }
			printf(" ; Score = %d ; Perm. = %llu\n", ibreakage, iULPerm);
			if( sw_file_use )
			{
				if ( ibreakage < ibreakage_best )
				{
					fprintf( fp_out, "N");
				}
				else if ( ibreakage == ibreakage_best)
				{
					fprintf( fp_out, "E");
				}
				
				for( int jj = 0 ; jj<N_MARKER ; jj++)
				{
					fprintf( fp_out, ",%d", curperm[jj]);
				}
				fprintf( fp_out, ",%d\n", ibreakage);
			}
		

			iULPermSave = iULPerm;
			ibreakage_best = ibreakage;
			for( int jj = 0 ; jj<N_MARKER ; jj++) bestperm[jj] = curperm[jj];
		}


		

// -----------------------------------------------------------
		_pruning:;

		if( phase == 1)
		{
			iULPerm += (factorial[(N_MARKER - node_order - 1)]);

			phase = 0;
			continue;
		}

		iULPerm += iULdx;
	} // iULPerm loop

	printf("Iteration():END\n");

	fclose(fp_out);
	cma.ShowCurMatrix(matrix[0], bestperm, marker );
	cma.ShowMarker(marker);
	printf ("(Perm : %llu)\n", iULPermSave);
}
