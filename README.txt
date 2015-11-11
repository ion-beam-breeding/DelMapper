README for DelMapper

Authors: Kotaro Ishii (kotaro@riken.jp),
	 Yusuke Kazama (ykaze@riken.jp),
	 and Tokihiro Ikeda (tokihiro@riken.jp)
Date:	22/7/2015

(1) INTRODUCTION
DelMapper is a program for deletion mapping. When PCR data showing the presence or absence of marker sets in a chromosomal region of partial deletion mutants are input by a user, an optimized solution of marker orders is obtained. DelMapper is especially useful for mapping marker sets in a non-recombining region.

DelMapper clusters the input markers into n clusters (n: user input), which are treated as virtual markers in the following process. DelMapper determines the optimal permutation of virtual markers. Then, DelMapper provides the optimal permutation of the input markers in each virtual marker, resulting in the best permutation of all input markers.

DelMapper is a software package consisting of three components: (a) The Perl script clusterinput.pl converts an input CSV file, consisting of a table of PCR results in which every mutant was inspected for the existence of every marker (e.g., see infile.csv.), into another CSV file in which input markers are clustered into n virtual markers (n: user input) by an externally called R script. (b) The main program DelMapper is a C++ program consisting of a sequence of source files. DelMapper calculates the best permutation(s) of virtual markers, as it is reduced to a travelling salesman problem (TSP) from the output file of clusterinput.pl. (c) For the best permutation chosen by the user, the Perl script makeeachcluster.pl creates input CSV files for mapping input markers in each virtual marker by the main program of DelMapper.

The Perl scripts of DelMapper have been tested using Cygwin (version 1.7.31-3). The C++ program was compiled with a C++ language processing system, Parallelnavi Language Package for Linux V3.0L30 (Fujitsu), based on the C++ language standard, Programming languages -- C++(ISO/IEC 144882:1998(E)), and was tested using the RIKEN Integrated Cluster of Clusters (RICC) supercomputer system.

(2) REQUIRED SYSTEMS
-perl (an interpreter for the perl language, see https://www.perl.org/)
-R (an environment for statistical computing and graphics, see https://www.r-project.org/)
-C++ compiler (DelMapper was tested with the Parallelnavi Language Package for Linux V3.0L30 (Fujitsu) in RICC.)

(3) PACKAGE FILES
The following files are included in the DelMapper package.

   clusterinput.pl	# Perl script to cluster input data and makes input files for DelMapper
   src	# folder containing DelMapper source files
   # The following six files are components of the main program of DelMapper.
   src/Configure.org
   src/Main.cpp
   src/Marker.cpp
   src/Marker.h
   src/Utility.cpp
   src/Utility.h
   makeeachcluster.pl	# Perl script to make input files for DelMapper to map markers in each virtual marker (cluster)
   infile.csv	# example input for clusterinput.pl (to be replaced by user data)
   README.txt	# this file

(4) USAGE
The input file must be a comma-delimited CSV file, in which the first row indicates the names of the markers, the first column indicates the names of the mutants, and the other elements in the table indicate PCR results (see the example infile.csv). The input file is processed by the following four steps:

(a) The Perl script clusterinput.pl classifies the markers into n virtual markers (n: user input) based on PCR results, and outputs files are used as input files for the next step.

Input file:
   infile.csv	# a comma-delimited CSV file

Syntax:
   perl clusterinput.pl [-d] infile.csv ncluster
where infile.csv indicates the input file in CSV format and ncluster indicates the number of virtual markers into which the input markers were classified.

Option:
   -d	# switches on the Del option
In all mutants, one end of the mapping region is always considered Ågpresent.Åh When the Del option is on, the opposite end of the region is considered Ågdeleted,Åh otherwise, this opposite end would be considered in the same state as the neighboring markers (regarded as selecting the Any option).

Output files:
clusterinput.pl generates a folder named ÅginfileÅh (depending on the name of the input file), in which the following files are generated.
   infile/infile.cluster.csv	# input file in which the information for correspondence relations between  markers and virtual markers is added
   infile/infile.input.csv	# input file for DelMapper
   # The following six files are components of the main program of DelMapper.
   infile/Configure.h
   infile/Main.cpp
   infile/Marker.cpp
   infile/Marker.h
   infile/Utility.cpp
   infile/Utility.h

(b) All cpp files are compiled into an executable file as the main program of DelMapper. The execution of the program does not need any parameters. DelMapper calculates the Ågtotal costÅh values for all permutations of the virtual markers. When Ågtotal costÅh for one permutation is equal to or less than the previous one, DelMapper adds a line including the permutation and its Ågtotal costÅh as a temporary best score to the output file.

Input files:
   infile.input.csv
   Configure.h
   Main.cpp
   Marker.cpp
   Marker.h
   Utility.cpp
   Utility.h

Syntax:
   cd infile
   c++ Main.cpp Marker.cpp Utility.cpp -o a.out	# This is an example, variable by the compiler.
   ./a.out

Output files:
   infile.out.csv

DelMapper outputs a CSV file, in which each line records the marker permutations with the temporarybest scores. The first column is ÅgNÅh if the temporary best score is renewed; otherwise, it is ÅgE.Åh The remaining columns indicate permutations of the virtual markers. Note that the numbers in the permutations do not indicate the names of virtual markers, but rather the order of the virtual markers in the input file (infile.cluster.csv). The last column represents the temporarybest score of the permutations in that row.

(c) The Perl script makeeachcluster.pl makes input files for DelMapper to map the input markers in each virtual marker. The script searches infile.out.csv for the permutations with the best score and ask users to choose a permutation on which the script works.

Input files:
   infile.out.csv	# output file from the main program of DelMapper
   infile.cluster.csv	# output files from clusterinput.pl
   infile.input.csv	# output files from clusterinput.pl


Syntax:
   perl makeeachcluster.pl [-d] cluster.csv input.csv out.csv outdirectory
where cluster.csv and input.csv are file paths for infile.cluster.csv and infile.input.csv (outputs from clusterinput.pl), respectively. out.csv is a file path for infile.output.csv that is an output file from the main program. outdirectory is a name used for the output directory.

Option:
   -d	# switches on the Del option as is described in step (a)

Output files:
The script makes n folders Ågoutdirectory-cnÅh and the following files appear in each folder:
   outdirectory-cn/outdirectory-cn.input.csv	# input file for DelMapper
   outdirectory-cn/Configure.h
   outdirectory-cn/Main.cpp
   outdirectory-cn/Marker.cpp
   outdirectory-cn/Marker.h
   outdirectory-cn/Utility.cpp
   outdirectory-cn/Utility.h

(d) An executable file of the main program of DelMapper is compiled and executed in each Åginfile-cnÅh folders in the same manner as that described in step (b) except that both ends of the mapping region are the virtual markers that neighbor the virtual marker in which the order of the markers is determined. DelMapper provides the best permutation of the input markers in each virtual marker. According to the results in all virtual markers,  the order of the input markers is manually rearranged in Åginfile.cluster.csv,Åh and then the mapping is completed.

(5) LICENSE 
Copyright (c) 2015 Kotaro Ishii, Yusuke Kazama, and Tokihiro Ikeda

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
