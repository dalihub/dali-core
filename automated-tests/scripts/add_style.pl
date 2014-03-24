#!/usr/bin/perl

use strict;
use Encode;
use Getopt::Long;
use Cwd;

my $pwd = getcwd;
my $MOD_NAME = $ARGV[0];
my $results_xml = "tct-$MOD_NAME-core-tests.xml";
my $results_out = "results_xml.$$";

# Copy $results_xml, writing new stylesheet line:
# Write <?xml-stylesheet type="text/xsl" href="./style/testresult.xsl"?> as second line
open RESULTS, "<$results_xml" || die "Can't open $results_xml for reading:$!\n";
open RESULTS_OUT, ">$results_out" || die "Can't open $results_out for writing:$!\n";
my $fline = readline RESULTS;
print RESULTS_OUT $fline;
print RESULTS_OUT "<?xml-stylesheet type=\"text/xsl\" href=\"./style/testresult.xsl\"?>\n";
while(<RESULTS>)
{
    print RESULTS_OUT $_;
}
close RESULTS_OUT;
close RESULTS;
unlink $results_xml;
print `mv $results_out $results_xml`;
