#!/usr/bin/perl

use strict;
use XML::Parser;
use Encode;
use Getopt::Long;
use Cwd;

my $pwd = getcwd;
my $num_tests=0;
my $num_passes=0;
my $num_actual_passes=0;
my $text = "";

sub handle_start
{
    my ($p, $elt, %attrs) = @_;

    if($elt =~ /testcase/)
    {
        $num_tests++;
        if($attrs{"result"} eq "PASS")
        {
            $num_passes++;
        }
    }
    if($elt =~ /actual_result/)
    {
        $text = "";
    }
}

sub handle_end
{
  my ($p, $elt) = @_;
  if($elt =~ /actual_result/)
  {
      if($text eq "PASS")
      {
          $num_actual_passes++;
      }
      $text = "";
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


# Write summary.xml:
open SUMMARY, ">summary.xml" || die "Can't open summary.xml for writing:$!\n";
print SUMMARY << "EOS";
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="./style/summary.xsl"?>
<result_summary plan_name="Core">
  <other xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:type="xs:string" />
  <summary test_plan_name="Empty test_plan_name">
    <start_at>2014-03-21_18_52_41</start_at>
    <end_at>2014-03-21_18_57_54</end_at>
  </summary>
EOS

print "\n\nSummary of all tests:\n";
my $output_files = `ls tct*core-tests.xml`;
my $file;
foreach $file (split /\s+/, $output_files )
{
    $num_tests=0;
    $num_passes=0;
    $num_actual_passes=0;
    $text = "";

    $parser->parsefile($file);

    my $pass_rate = sprintf("%5.2f", $num_passes * 100.0 / $num_tests);
    my $num_fails = $num_tests - $num_passes;
    my $fail_rate = sprintf("%5.2f", $num_fails * 100.0 / $num_tests);

    my $suite_name = $file;
    $suite_name =~ s/\.xml$//;

    print "$suite_name: $num_passes tests passed out of $num_tests ( $pass_rate% )\n";

print SUMMARY << "EOS2";
  <suite name="$suite_name">
    <total_case>$num_tests</total_case>
    <pass_case>$num_passes</pass_case>
    <pass_rate>$pass_rate</pass_rate>
    <fail_case>$num_fails</fail_case>
    <fail_rate>$fail_rate</fail_rate>
    <block_case>0</block_case>
    <block_rate>0.00</block_rate>
    <na_case>0</na_case>
    <na_rate>0.00</na_rate>
  </suite>
EOS2
}

print SUMMARY "</result_summary>\n";
close SUMMARY;

print "Summary of results written to summary.xml\n";
