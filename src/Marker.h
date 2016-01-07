//Copyright (c) 2015 Kotaro Ishii, Yusuke Kazama, and Tokihiro Iked
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

#pragma once
class CMarker
{
public:
	CMarker(void);
	~CMarker(void);

	int GetMatrix(char** marker, short* matrix);
	int GetSideMatrix(char** side_marker, short* side_matrix);
	int ReleaseMarker(char** marker);
	int ShowCurMatrix(short* matrix, unsigned short* curperm, char** marker);
	int ShowMarker(char** marker);
	int ShowSideMarker(char** marker);
};
