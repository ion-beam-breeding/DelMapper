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

## Check input
if (! defined ( $ARGV[3] ) ) {
  print 'Usege : perl makeeachcluster.pl [-d] cluster.csv input.csv out.csv outdirectory',"\n";
  print 'cluster.csv: file path for infile.cluster.csv (output from clusterinput.pl)',"\n";
  print 'input.csv: file path for infile.input.csv (output from clusterinput.pl)',"\n";
  print 'out.csv: file path for infile.out.csv (output from the main program)',"\n";
  print 'outdirectory: name used for output directory',"\n";
  print '-d : switches on the Del option',"\n";
  exit(1);
}
my $sFileMarkers = $ARGV[0];
if (! -f $sFileMarkers) {
  print $sFileMarkers,' not exist.',"\n";
  exit (1);
}
my $sFileClusters = $ARGV[1];
if (! -f $sFileClusters) {
  print $sFileClusters,' not exist.',"\n";
  exit (1);
}
my $sFileResult = $ARGV[2];
if (! -f $sFileResult) {
  print $sFileResult,' not exist.',"\n";
  exit (1);
}
my $sOutDirectory = $ARGV[3];

## Read result of DelMapper
my @aResults = ();
open (IN, "<$sFileResult");
while (<IN>) {
  s/[\r\n]*$//;
  if ( /N/ ) {
    @aResults = ($_);
  } else {
    push (@aResults, $_);
  }
}
close(IN);
for (my $i = 0; $i <= $#aResults; $i++) {
  print $i,' : ',$aResults[$i],"\n";
}
print 'Select the number on the left of the result to be used.',"\n";
my $iInput = <STDIN>;
chomp ($iInput);

## Read clustered matrix (input for DelMapper)
my $cluster;
my $iCluster;
my $line = 0;
open (IN, "<$sFileClusters");
while (<IN>) {
  s/[\r\n]*$//;
  my @r = split(/\,/, $_);
  if ($line == 0) {
    @{$cluster->{'name'}} = @r;
  } elsif ($line == 1) {
    @{$cluster->{'nmarker'}} = @r;
  } else {
    $iCluster = 0;
    foreach my $a (@r) {
      $cluster->{'matrix'}->{$iCluster} .= ($a."\,");
      $iCluster++;
    }
  }
  $line++;
}
for (my $i = 0; $i < $iCluster; $i++) {
  chop ($cluster->{'matrix'}->{$i});
}

## Read input matrix
my $marker;
my $iMarker;
open (IN, "<$sFileMarkers");
$line = 0;
while (<IN>) {
  s/[\r\n]*$//;
  my @r = split(/\,/, $_);
  if ($line == 0) {
    @{$marker->{'cluster'}} = @r;
  } elsif ($line == 1) {
    @{$marker->{'name'}} = @r;
  } else {
    $iMarker = 0;
    foreach my $a (@r) {
      $marker->{'matrix'}->{$iMarker} .= ($a."\,");
      $iMarker++;
    }
  }
  $line++;
}
for (my $i = 0; $i < $iMarker; $i++) {
  chop ($marker->{'matrix'}->{$i});
}

my @order = split(/\,/, $aResults[$iInput]);
for (my $i = 1; $i < $#order; $i++) {
  my $nmarker = 0;
  my $fixleft = 1;
  my $fixright = 1;
  my $output;
  for (my $j = 1; $j <= $#{$marker->{'cluster'}}; $j++) {
    if ( $marker->{'cluster'}->[$j] eq $order[$i]+1) {         ### cluster : output+1=input
      push (@{$output}, $marker->{'name'}->[$j]."\,1\,".$marker->{'matrix'}->{$j});
      $nmarker++;
    }
  }
  if ($i == 1) {
    $fixleft = 0;    #leftside
  } else {
    for (my $j = 0; $j < $#{$cluster->{'name'}}; $j++) {
      if ( $cluster->{'name'}->[$j] eq $order[$i-1]+1) {
        push (@{$output}, $cluster->{'name'}->[$j]."\,".$cluster->{'nmarker'}->[$j]."\,".$cluster->{'matrix'}->{$j});
      }
    }
  }
  if ($i == $#order-1) {
    $fixright = 0;    #rightside
  } else {
    for (my $j = 0; $j < $#{$cluster->{'name'}}; $j++) {
      if ( $cluster->{'name'}->[$j] eq $order[$i+1]+1) {
        push (@{$output}, $cluster->{'name'}->[$j]."\,".$cluster->{'nmarker'}->[$j]."\,".$cluster->{'matrix'}->{$j});
      }
    }
  }
  push (@{$output}, $cluster->{'name'}->[$#{$cluster->{'name'}}]."\,".$cluster->{'nmarker'}->[$#{$cluster->{'name'}}]."\,".$cluster->{'matrix'}->{$#{$cluster->{'name'}}});

  my $tOutput;
  for (my $j = 0; $j <= $#{$output}; $j++) {
    my @line = split (/\,/, $output->[$j]);
    for (my $k = 0; $k <= $#line; $k++) {
      $tOutput->[$k] .= $line[$k]."\,";
    }
  }

  ## Make directory
  my $sFileNameOut = $sOutDirectory.'-c'.$i;
  if (!-d "./$sFileNameOut") {
    mkdir "./$sFileNameOut";
  }

  ## Make input file for DelMapper
  my $nplant = -1;
  open (OUT, ">./$sFileNameOut/$sFileNameOut.input.csv");
  for (my $j = 0; $j <= $#{$tOutput}; $j++) {
    print OUT $tOutput->[$j],"\n";
    $nplant++;
  }
  close (OUT);

  ## Edit header file
  my @variable = (
    $sFileNameOut.'.input.csv', #IN_NAME
    $sFileNameOut.'.out.csv',   #OUT_NAME
    $nmarker,                #N_MARKER
    $nplant,                 #N_PLANT
    $fixleft,                       #FIX_LEFT
    $fixright,                       #FIX_RIGHT
    $delOption);               #DEL_OPTION
  open(OUT, ">./$sFileNameOut/Configure.h");
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
  
  `cp ./src/Marker.cpp ./$sFileNameOut`;
  `cp ./src/Marker.h ./$sFileNameOut`;
  `cp ./src/Utility.cpp ./$sFileNameOut`;
  `cp ./src/Utility.h ./$sFileNameOut`;
  `cp ./src/Main.cpp ./$sFileNameOut`;
  
}
