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

#include "Marker.h"
#include "stdio.h"
#include "Utility.h"
#include "Configure.h"
#include <string.h>
#include <stdlib.h>

CMarker::CMarker(void)
{
}


CMarker::~CMarker(void)
{
}

int CMarker::ShowMarker(char** marker)
{
	for( int i = 0 ; i < N_MARKER ; i++ )
	{
		printf("%2d [%s]\n", i, marker[i]);
	}

	return 1;
}

int CMarker::ShowSideMarker(char** marker)
{
	for( int i = N_MARKER ; i < N_MARKER+FIX_LEFT+FIX_RIGHT ; i++ )
	{
		printf("%2d [%s]\n", i, marker[i]);
	}

	return 1;
}

int CMarker::ReleaseMarker(char** marker)
{
	for( int i = 0 ; i < N_MARKER ; i++ )
	{
		delete [] marker[i];
	}

	return 1;
}

int CMarker::GetMatrix(char** marker, short* matrix)
{
	printf ("Input file : %s\n", IN_NAME);
	FILE *fp_in;
	char *filename = IN_NAME;
	if( NULL == (fp_in = fopen(filename, "r")))
	{
		printf ("Error! : %s cannot open.\n", filename);
		exit(1);
	}

	char cbuff[1024];
	char cbuff0[1024];
	int i_line = 0;
	int i_matrix = 0;
	int length_max = -1;

	while( i_line < N_PLANT+1)
	{
		fgets(cbuff0,1023,fp_in);
		int len0 = (int)strlen(cbuff0);
		if( cbuff0[len0-1] == '\n' ) len0-= 2;
		strcpy(cbuff, cbuff0);

		if(feof(fp_in)) break;

		if(strstr(cbuff, ",")!=NULL)
		{
			char *token;
			char *next_token = NULL;
			token = strtok_r( cbuff, ",", &next_token );

			if( i_line > 0 )
			{
				*(matrix + i_matrix ) = atoi(token);
				i_matrix++;
			}
			else // marker names @ i_line==0
			{
				while( *token == ' ' )token++;
				
				int length = (int) strlen(token) +1;
				if(length_max<length)length_max=length;
				marker[0] = new char[length];
				strncpy(marker[0], token, length);
			}

			for( int i = 1 ; i < N_MARKER ; i++)
			{
				if(strcmp(token,"")==0) break;

		        token = strtok_r( NULL, ",", &next_token );

				if( i_line > 0 )
				{
					*(matrix + i_matrix ) = atoi(token);
					i_matrix++;
				}
				else // marker names @ i_line==0
				{
					while( *token == ' ' )token++;
					int length = (int) strlen(token) +1;
					if(length_max<length)length_max=length;
					marker[i] = new char[length];
					strncpy(marker[i], token, length);
				}
			}
		}

		i_line ++;

	}

	fclose(fp_in);

	printf ("Input matrix : \n");
	for(int i_length = 0 ; i_length < length_max-1 ; i_length++)
	{
		printf("     ");
		for( int i_m = 0 ; i_m < N_MARKER ; i_m++)
		{
			int k = (int)strlen(marker[i_m]) - length_max + i_length +1;
			if( k >= 0 )
			{
				printf("%3c", marker[i_m][k]);
			}
			else
			{
				printf("   ");
			}
		}
		printf("\n");
	}

	for( int i_plant = 0 ; i_plant < N_PLANT ; i_plant++)
	{
		printf("[%2d] ", i_plant);
		for( int i_m = 0 ; i_m < N_MARKER ; i_m++)
		{
			printf("%3d", *(matrix +i_plant*N_MARKER+ i_m) );
		}
		printf("\n");
	}

	return 1;
}

int CMarker::GetSideMatrix(char** marker, short* matrix)
{
	FILE *fp_in;
	char *filename = IN_NAME;
	if( NULL == (fp_in = fopen(filename, "r")))
	{
		printf ("Error! : %s cannot open.\n", filename);
		exit(1);
	}

	char cbuff[1024];
	char cbuff0[1024];
	int i_line = 0;
	int i_matrix = 0;
	int length_max = -1;

	while( i_line < N_PLANT+1)
	{
		fgets(cbuff0,1023,fp_in);
		int len0 = (int)strlen(cbuff0);
		if( cbuff0[len0-1] == '\n' ) len0-= 2;
		strcpy(cbuff, cbuff0);
		if(feof(fp_in)) break;

		if(strstr(cbuff, ",")!=NULL)
		{
			char *token;
			char *next_token = NULL;
			token = strtok_r( cbuff, ",", &next_token );

			if( i_line > 0 )
			{
				*(matrix + i_matrix ) = atoi(token);
				i_matrix++;
			}
			else // marker names @ i_line==0
			{
				while( *token == ' ' )token++;
				int length = (int) strlen(token) +1;
				if(length_max<length)length_max=length;
				marker[0] = new char[length];
				strncpy(marker[0], token, length); 
			}

			for( int i = 1 ; i < N_MARKER+FIX_LEFT+FIX_RIGHT ; i++)
			{
				if(strcmp(token,"")==0) break;
		        token = strtok_r( NULL, ",", &next_token );
				if( i_line > 0 )
				{
					*(matrix + i_matrix ) = atoi(token);
					i_matrix++;
				}
				else // marker names @ i_line==0
				{
					while( *token == ' ' )token++;
					
					int length = (int) strlen(token) +1;
					if(length_max<length)length_max=length;
					marker[i] = new char[length];
					strncpy(marker[i], token, length);
				}
			}
		}

		i_line ++;

	}
	fclose(fp_in);
	
	if ( FIX_LEFT + FIX_RIGHT > 0 )
	{
		printf ("Matrix of side markers : \n");
		for(int i_length = 0 ; i_length < length_max-1 ; i_length++)
		{
			printf("     ");
			for( int i_m = N_MARKER ; i_m < N_MARKER+FIX_LEFT+FIX_RIGHT ; i_m++)
			{
				int k = (int)strlen(marker[i_m]) - length_max + i_length +1;
				if( k >= 0 )
				{
					printf("%3c", marker[i_m][k]);
				}
				else
				{
					printf("   ");
				}

			}
			printf("\n");
		}

		for( int i_plant = 0 ; i_plant < N_PLANT ; i_plant++)
		{
			printf("[%2d] ", i_plant);
			for( int i_m = N_MARKER ; i_m < N_MARKER+FIX_LEFT+FIX_RIGHT ; i_m++)
			{
				 printf("%3d", *(matrix +i_plant*(N_MARKER+FIX_LEFT+FIX_RIGHT)+ i_m) );
			}
			printf("\n");
		}
	}
	else
	{
		printf ("Side markers are not fixed.\n");
	}

		return 1;
}

int CMarker::ShowCurMatrix(short* matrix, unsigned short* curperm, char** marker)
{
	int length_max = -1;
	for( int i_m = 0 ; i_m < N_MARKER ; i_m++)
	{
		if( length_max < (int) strlen(marker[i_m]) +1 ) length_max = (int) strlen(marker[i_m]) +1;
	}

	printf ("Matrix of best permutation :\n");
	for(int i_length = 0 ; i_length < length_max-1 ; i_length++)
	{
		printf("     ");
		for( int i_m = 0 ; i_m < N_MARKER ; i_m++)
		{
			int k = (int)strlen(marker[*(curperm+i_m)]) - length_max + i_length +1;
			if( k >= 0 )
			{
				printf("%3c", marker[*(curperm+i_m)][k]);
			}
			else
			{
				printf("   ");
			}
		}
		printf("\n");
	}

	for( int i_plant = 0 ; i_plant < N_PLANT ; i_plant++)
	{
		printf("[%2d] ", i_plant);
		for( int i_m = 0 ; i_m < N_MARKER ; i_m++)
		{
			printf("%3d", *(matrix +i_plant*N_MARKER+ *(curperm+i_m)) );
		}
		printf("\n");
	}

	return 1;
}
