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

#pragma once
class CUtility
{
public:
	CUtility(void);
	~CUtility(void);
  	int MakeCurPerm(unsigned short* curperm, unsigned long long int iULPerm);
};
