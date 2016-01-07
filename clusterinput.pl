#!/usr/local/bin/perl

#Copyright (c) 2015 Kotaro Ishii, Yusuke Kazama, Tokihiro Ikeda
#
#This file is part of DelMapper.
#
#DelMapper is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#any later version.
#
#DelMapper is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with DelMapper.  If not, see <http://www.gnu.org/licenses/>.

use warnings;
use strict;
use Getopt::Long qw(:config posix_default no_ignore_case gnu_compat);

my $delOption = 0;
GetOptions('d' => \$delOption);
my $nCluster = $ARGV[1];
my $threshold = 4;
#for noise reduction: single "absent" element in a cluster consisting more than "threshold" number of markers will be ignored.

## Check input
if ((!defined ($ARGV[1])) || (! -f $ARGV[0]) || $ARGV[1] > 20) {
  print 'Usege : perl clusterinput.pl [-d] infile.csv nCluster',"\n";
  print 'ncluster: number of clusters (less than 21).',"\n";
  print '-d : switches on the Del option',"\n";
  exit(1);
}

## Make directory
my $sFileName = $ARGV[0];
my $sFileNameNoExt;
if ($sFileName =~ /\./) {
  $sFileNameNoExt = $`;
} else {
  $sFileNameNoExt = $sFileName;
}
if (!-d "./$sFileNameNoExt") {
  mkdir "./$sFileNameNoExt";
}

## Invert state for calculation by R
open(IN, "<$sFileName");
open(OUT, ">rwrap$$.input");
while (<IN>) {
  my $line = $_;
  $line =~ s/[\r\n]*$//;
  $line =~ s/\,0/\,999/g;
  $line =~ s/\,1/\,0/g;
  $line =~ s/\,999/\,1/g;
  print OUT $line,"\n";
}
close (IN);
close (OUT);

## Cluster input matrix by R

open(SCRIPT, ">rwrap$$.R");
print SCRIPT <<EOF;
  data <- read.table("rwrap$$.input", header=T, row.name=1, sep=",")
  datat <- t(data)
  hc <- hclust(dist(datat, method = "binary"), method="ward.D2")
  sink ("$sFileNameNoExt.cluster", split=T)
  cutree (hc, k=$nCluster)
  sink ()
EOF
close(SCRIPT);

my $Rstat;
system("R --vanilla --slave < rwrap$$.R");
($Rstat = $?/256) && die "Aborted in R with status $Rstat.?n";
unlink "rwrap$$.R";
unlink "rwrap$$.input";

## Merge input matrix and cluster information

my $array; # $array->{'Marker'} = [(plant, plant,,)]
           #         {marker} = [(0, 1,,,)]

# Read cluster information

my @marker;
my $cluster;
open(IN, "$sFileNameNoExt.cluster");
my $i = 1;
while(<IN>) {
  my $line = $_;
  $line =~ s/[\r\n]*$//;
  if ($line =~ m/^\s+(.*)/) {  # Delete blank at beginning
    $line = $1;
  }
  my @line = split(/\s+/, $line); # Separate by blank
  if ($i % 2 == 1) { # odd line -> marker name
    @marker = @line;
  } else { # even line -> cluster number
    for (my $i = 0; $i <= $#line; $i++) {
      push (@{$cluster->{$line[$i]}}, $marker[$i]);
    }
  }
  $i++;
}
close(IN);
unlink "$sFileNameNoExt.cluster";

# Read input matrix
my @header;
my $nPlant = 0;
undef (@header);
open (IN, "<$sFileName");
while(<IN>) {
  my $line = $_;
  $line =~ s/[\r\n]*$//;
  my @line = split(/\,/, $line);

# Transpose matrix
  if ( !(@header) ) {
    @header = @line;
  } else {
    for (my $i = 0; $i <= $#line; $i++) {
      push (@{$array->{$header[$i]}}, $line[$i]);
    }
  }
  $nPlant++;
}
close(IN);

# Output intermediate file

my $tArray;
push (@{$tArray}, [('Cluster')]);
push (@{$tArray}, [($header[0])]);
foreach my $element (@{$array->{$header[0]}}) {
  push (@{$tArray}, [($element)]);
}

for (my $i = 1; $i <= $nCluster; $i++) {
  foreach my $element (@{$cluster->{"$i"}}) {
    push (@{$tArray->[0]}, $i);
    push (@{$tArray->[1]}, $element);
    my $j = 2;
    foreach my $status (@{$array->{$element}}) {
      push (@{$tArray->[$j]}, $status);
      $j++;
    }
  }
}

open(OUT, ">./$sFileNameNoExt/$sFileNameNoExt.cluster.csv");
for (my $i = 0; $i <= $#{$tArray}; $i++) {
  my $str = $tArray->[$i]->[0];
  for (my $j = 1; $j <= $#{$tArray->[$i]}; $j++) {
    $str .= "\,".$tArray->[$i]->[$j];
  }
  print OUT $str,"\n";
}
close(OUT);

### Compress markers in each cluster into virtual marker

my $cArray; # $cArray->[cluster]->[(nMarker, status, status,,)]
for (my $i = 1; $i <= $nCluster; $i++) {
  $cArray->[$i]->[0] = 0;
  foreach my $element (@{$cluster->{"$i"}}) {
    $cArray->[$i]->[0]++;
    for (my $j = 0; $j <= $#{$array->{$element}}; $j++) {
      $cArray->[$i]->[1+$j] += $array->{$element}->[$j];
    }
  }
}

# Filter out single "absent" element in each cluster

for (my $i = 1; $i <= $nCluster; $i++) {
  my $nMarker = $cArray->[$i]->[0];
  foreach my $element (@{$cArray->[$i]}) {
    if ( $nMarker >= $threshold && $element == $nMarker - 1) {
      $element = $nMarker;
    }
  }
}

# Transpose matrix

my $tcArray;
for (my $i = 1; $i <= $nCluster; $i++) {
  push (@{$tcArray->[0]}, $i);
}
for (my $i = 0; $i <= $#{$array->{$header[0]}}+1; $i++) {
  for (my $j = 1; $j <= $nCluster; $j++) {
    push (@{$tcArray->[$i+1]}, $cArray->[$j]->[$i]);
  }
}
push (@{$tcArray->[0]}, 'Cluster');
push (@{$tcArray->[1]}, 'Nmarker');
for (my $i = 0; $i <= $#{$array->{$header[0]}}; $i++) {
  push (@{$tcArray->[$i+2]}, $array->{$header[0]}->[$i]);
}

open(OUT, ">./$sFileNameNoExt/$sFileNameNoExt.input.csv");
for (my $i = 0; $i <= $#{$tcArray}; $i++) {
  my $str = $tcArray->[$i]->[0];
  for (my $j = 1; $j <= $#{$tcArray->[$i]}; $j++) {
    $str .= "\,".$tcArray->[$i]->[$j];
  }
  print OUT $str,"\n";
}
close (OUT);

### Edit header file

my @variable = (
  $sFileNameNoExt.'.input.csv', #IN_NAME
  $sFileNameNoExt.'.out.csv',   #OUT_NAME
  $ARGV[1],                #N_MARKER
  $nPlant,                 #N_PLANT
  0,                       #FIX_LEFT
  0,                       #FIX_RIGHT
  $delOption);               #DEL_OPTION
open(OUT, ">./$sFileNameNoExt/Configure.h");
open(IN, "<./src/Configure.org");
my $count = 0;
while(<IN>) {
  if (/REPLACE/) {
    s/REPLACE/$variable[$count]/;
    $count++;
  }
  print OUT;
}
close(IN);
close(OUT);

##Copy other sources
  
`cp ./src/Marker.cpp ./$sFileNameNoExt`;
`cp ./src/Marker.h ./$sFileNameNoExt`;
`cp ./src/Utility.cpp ./$sFileNameNoExt`;
`cp ./src/Utility.h ./$sFileNameNoExt`;
`cp ./src/Main.cpp ./$sFileNameNoExt`;
