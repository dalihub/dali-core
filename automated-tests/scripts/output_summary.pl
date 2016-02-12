#!/usr/bin/perl

# Reads summary.xml and produces human readable output

use strict;
use XML::Parser;
use Encode;
use Getopt::Long;
use Cwd;

my $pwd = getcwd;
my $text = "";
my $module="";
my %modules=();

sub handle_start
{
    my ($p, $elt, %attrs) = @_;

    if($elt =~ /suite/)
    {
        $module=$attrs{"name"};
    }
    if($elt =~ /_case/)
    {
        $text = "";
    }
}

sub handle_end
{
  my ($p, $elt) = @_;
  if($elt =~ /pass_case/)
  {
      $modules{$module}->{"pass"}=$text;
      $text="";
  }
  elsif($elt =~ /fail_case/)
  {
      $modules{$module}->{"fail"}=$text;
      $text="";
  }
}

sub handle_char
{
  my ($p, $str) = @_;
  $text .= $str;
}

my($parser) = new XML::Parser(Handlers => {Start => \&handle_start,
                                           End   => \&handle_end,
                                           Char  => \&handle_char});
$parser->parsefile("summary.xml");

my $RED_COLOR="\e[1;31m";
my $GREEN_COLOR="\e[1;32m";
my $ASCII_RESET="\e[0m";
my $ASCII_BOLD="\e[1m";

print "\n";
my $totalFailures=0;
foreach $module (keys(%modules))
{
    my $result_colour = $GREEN_COLOR;
    if( $modules{$module}->{"fail"} )
    {
        $result_colour = $RED_COLOR;
    }
    my $numPasses = $modules{$module}->{"pass"};
    my $numFailures = $modules{$module}->{"fail"};
    $totalFailures += $numFailures;
    print( "$ASCII_BOLD$module results:$ASCII_RESET\n" );
    printf("Number of test passes:   %s%4d (%5.2f%%)%s\n", $ASCII_BOLD, $numPasses, 100.0 * $numPasses / ($numPasses+$numFailures),  $ASCII_RESET);
    printf("%sNumber of test failures:%s %s%4d%s\n\n", $result_colour, $ASCII_RESET, $ASCII_BOLD, $numFailures, $ASCII_RESET);
}

exit $totalFailures == 0;
