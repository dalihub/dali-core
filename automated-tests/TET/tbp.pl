#!/usr/bin/perl

use strict;

my $num;
my $scen;
my $ref;
my $part;
my $timestamp;
my $testcase;
my $built=0;
my $build_failed=0;
my $build_count=0;
my $build_failure_count=0;
my $ic=0;
my $ic2=0;
my $x=0;
my $tc=0;
my $v;
my $test;
my $time;
my $date;
my %build_tests;
my %build_summary;

my $executed_testcases=0;
my $execute_no_file=0;
my $executed=0;
my %execute;
my %execute_summary;

sub parse_file
{
    while(<>)
    {
        ($num, $scen, $ref) = split(m!\|!, $_);
        chomp $ref;

        if($num == 0)
        {
            ($v, $time, $date) = split(/\s/, $scen);
        }
# Execution
        elsif($num == 10) # 10|0 /dali-test-suite/actors/utc-Dali-Actor 16:58:27|TC Start, scenario ref 2-0
        {
            ($part, $testcase, $timestamp) = split(/\s/, $scen);
            $executed_testcases++;
        }
        elsif($num == 50) # 50||(exec.c, 131): can't exec /home/SERILOCAL/david.steele/Git/HQ-Dali/dali-core/automated-tests/./tet_tmp_dir/24242aa/dali-test-suite/geometry/utc-Dali-MeshData, reply code = ER_NOENT
        {
            $execute_no_file++;
        }
        elsif($num == 80) # 80|19 0 16:58:47|TC End, scenario ref 21-0
        {
        }
        elsif($num == 400) #400|13 1 142 16:58:40|IC Start
        {
            ($test, $ic, $x, $timestamp) = split(/\s/, $scen);
        }
        elsif($num == 410) #410|19 1 9 16:58:46|IC End
        {
        }
        elsif($num == 200) #200|13 1 16:58:40|TP Start
        {
            $execute_summary{"Total"}++;
        }
        elsif($num == 220)
        {
            ($test, $tc, $ic2, $timestamp) = split(/\s/, $scen);
            $execute{$testcase}->{$ic}->{$tc} = $ref;
            $execute_summary{$ref}++;
            $executed++;
        }

# Build
        elsif($num == 110) # Build
        {
            ($part, $testcase, $timestamp) = split(/\s/, $scen);
            $build_failed=0;
            $build_count++;
            $build_summary{"Total"}++;
        }

        elsif($num == 100)
        {
            if( ( $ref =~ /utc-/ && $ref =~ m!error!i )
                ||
                ($ref =~ /^Makefile/ && $ref =~ m!Stop!i )
                ||
                ($ref =~ /^make/ && ($ref =~ m!Stop!i || $ref =~ m!Error 1! ) ) )
            {
                $build_failed = 1;
            }
        }

        elsif($num == 130)
        {
            if($build_failed)
            {
                $build_failure_count++;
                $build_summary{"Failure"}++;
            }
            else
            {
                $build_summary{"Success"}++;
            }
            $build_tests{$testcase} = !$build_failed;
        }
    }
    $built = $build_count - $build_failure_count;
}


sub heading
{
    print <<EOH;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
"http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head>
<title>TETware Test Run Report</title>
</head>
<body bgcolor="white">
<p><table border="1" width="100%" cellpadding="3" cellspacing="0">
<tr><td bgcolor="#ccccff" class="heading1"><center><h1>
TETware Test Run Report</h1></center></td></tr></table></p>
<p><table border="0" cellpadding="2" cellspacing="1">
<tr>
<td align="left" class="noborder">Date of test run:</td>
<td align="left" class="noborder">$date</td>
</tr>
<tr>
<td align="left" class="noborder">Start time:</td>
<td align="left" class="noborder">$time</td>
</tr>
</table></p>
<hr>
EOH
}

sub summary
{
    my $heading = shift;
    my $summary_ref = shift;

    print <<EOS1;
    <p><table border="1" cellpadding="3" cellspacing="0">
        <tr><td bgcolor="#ccccff" class="heading2"><font size="+2"><b>$heading</b></font></td></tr>
        </table></p>
    <p><table border="0" cellpadding="5" cellspacing="4">
        <tr>
        <td align="center" bgcolor="#cccccc" class="neutral"><b>Result</b></td>
        <td align="center" bgcolor="#cccccc" class="neutral"><b>Count</b></td>
        </tr>
EOS1

    my ($success_string, @blah) = grep(/(Success|PASS)/, keys(%$summary_ref));
    my $successes = $summary_ref->{$success_string};
    print <<EOS2;
    <tr>
        <td align="left" bgcolor="#33cc33" class="success">$success_string</td>
        <td align="right" bgcolor="#33cc33" class="success">$successes</td>
    </tr>
EOS2

    foreach my $key (sort(grep(!/(Success|PASS|Total)/, keys(%$summary_ref))))
    {
        my $fails = $summary_ref->{$key};
        print <<EOS3;
        <tr>
            <td align="left" bgcolor="#ff5555" class="failure">$key</td>
            <td align="right" bgcolor="#ff5555" class="failure">$fails</td>
        </tr>
EOS3
    }

    my $total = $summary_ref->{"Total"};
    print <<EOS4;
    <tr>
        <td align="left" bgcolor="#cccccc" class="neutral"><b>Total</b></td>
        <td align="right" bgcolor="#cccccc" class="neutral"><b>$total</b></td>
    </tr>
    </table></p>
EOS4
}


sub build_breakdown
{
    print <<EOB;
<hr>
<p><table border="1" cellpadding="3" cellspacing="0">
<tr><td bgcolor="#ccccff" class="heading2"><font size="+2"><b>
Build mode result breakdown</b></font></td></tr></table></p>
<p><table border="0" cellpadding="5" cellspacing="4">
<tr>
<td align="center" bgcolor="#cccccc" class="neutral"><b>Testcase</b></td>
<td align="center" bgcolor="#cccccc" class="neutral"><b>Result</b></td>
</tr>
EOB

    foreach my $key (sort(keys(%build_tests)))
    {
        my $success = $build_tests{$key};
        my $class = $success?"success":"failure";
        my $Class = $success?"Success":"Failure";
        my $color = $success?"#33cc33":"#ff5555";
        print("<tr><td align=\"left\" bgcolor=\"$color\" class=\"$class\">$key</td>\n");
        print("<td align=\"left\" bgcolor=\"$color\" class=\"$class\">$Class</td>\n");
    }
    print("</table><p>");
}


sub execute_breakdown
{
    print <<EOE;
<hr>
<p><table border="1" cellpadding="3" cellspacing="0">
<tr><td bgcolor="#ccccff" class="heading2"><font size="+2"><b>
Execute mode result breakdown</b></font></td></tr></table></p>
EOE

    #Constructed with:
    #$execute{$testcase}->{$ic}->{$tc} = $ref;

    my %results;
    foreach my $testcase (sort(keys(%execute)))
    {
        #print STDOUT "$testcase\n";
        foreach my $ic (sort(keys(%{$execute{$testcase}})))
        {
            my $ic_ref = $execute{$testcase}->{$ic};
            foreach my $tc (sort { $a <=> $b } (keys(%{$ic_ref})))
            {
                my $result = $execute{$testcase}->{$ic}->{$tc};
                chomp($result);
                $results{$result}->{$testcase} .= ", $ic.$tc";
                #print STDOUT "$testcase $ic.$tc $result\n";
                #print STDOUT "STRUCT:" . "\$results\{" . $result . "\}->\{$testcase\} => " . $results{$result}->{$testcase} . "\n";
            }
        }
    }

    foreach my $result ( "PASS", (sort(grep(!/PASS/, keys(%results)))))
    {
        print "<h3>$result</h3>\n";
        print <<EOE2;
        <p><table border="0" cellpadding="5" cellspacing="4">
            <tr>
            <td align="center" bgcolor="#cccccc" class="neutral"><b>Testcase</b></td>
            <td align="center" bgcolor="#cccccc" class="neutral"><b>Test purposes (IC.TP)</b></td>
       </tr>
EOE2

       my $bgcolor = "#ff5555";
       if ($result =~ /PASS/)
       {
           $bgcolor = "#33cc33";
       }

        #print STDOUT "Result: $result OUT:" . $results{$result} . "\n";
       foreach my $testcase (sort(keys(%{$results{$result}})))
       {
           #print STDOUT "$testcase\n";
           my $tests = substr($results{$result}->{$testcase}, 2);
           print <<EOE3;
               <tr>
               <td align="left" bgcolor=$bgcolor class=$result>$testcase</td>
               <td align="left" bgcolor=$bgcolor class=$result>$tests</td>
               </tr>
EOE3
       }
       print "</table>";
    }
}


sub footer
{
    print "<hr>\n</body>\n</html>\n";
}

sub report
{
    my $file = shift;
    open(my $fh, ">", $file) || die "Can't create $file";
    select $fh;
    heading;

    if($build_count)
    {
        summary("Build mode summary", \%build_summary);
        build_breakdown;
    }
    if(scalar(keys(%execute)))
    {
        summary("Execute mode summary", \%execute_summary);
        execute_breakdown;
    }
    footer;
    select STDOUT;
    close $fh;
}


## MAIN

die "No args"  if scalar(@ARGV) == 0;
my $htmlname = $ARGV[0];
$htmlname =~ s/.journal/.html/;

parse_file();
report($htmlname);


if($build_count)
{
    foreach my $key (sort(keys(%build_tests)))
    {
        if(!$build_tests{$key})
        {
            print STDOUT "$key: Failed\n";
        }
    }
    print STDOUT "Built $built of $build_count\n";
}

if($executed)
{
    my $num_exes=$executed_testcases-$execute_no_file;
    print STDOUT "Executed $num_exes of $executed_testcases testcases\n";
    my $passed = $execute_summary{"PASS"};
    my $total = $execute_summary{"Total"};
    my $passRate = ($num_exes / $executed_testcases ) * ( $passed / $total )*100;
    print STDOUT "Passed $passed of $total - Pass rate=" . sprintf("%4.1f%%", $passRate) ."\n";
}

print "Report output: $htmlname\n";
