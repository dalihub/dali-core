#!/usr/bin/perl
#
# Copyright (c) 2016 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

use strict;
use Git;
use Getopt::Long;
use Error qw(:try);
use Pod::Usage;
use File::Basename;
use File::stat;
use Scalar::Util qw /looks_like_number/;
use Cwd qw /getcwd/;
use Term::ANSIColor qw(:constants);

# Program to run gcov on files in patch (that are in source dirs - needs to be dali-aware).

# A) Get patch
# B) Remove uninteresting files
# C) Find matching gcno/gcda files
# D) Copy and rename them to match source prefix (i.e. strip library name off front)
# E) Generate patch output with covered/uncovered lines marked in green/red
# F) Generate coverage data for changed lines
# G) Exit status should be 0 for high coverage (90% line coverage for all new/changed lines)
#    or 1 for low coverage

# Sources for conversion of gcno/gcda files:
# ~/bin/lcov
# Python git-coverage (From http://stef.thewalter.net/git-coverage-useful-code-coverage.html)

our $repo = Git->repository();
our $debug=0;
our $pd_debug=0;
our $opt_cached;
our $opt_help;
our $opt_output;
our $opt_quiet;
our $opt_verbose;

my %options = (
    "cached"       => { "optvar"=>\$opt_cached, "desc"=>"Use index" },
    "output:s"     => { "optvar"=>\$opt_output, "desc"=>"Generate html output"},
    "help"         => { "optvar"=>\$opt_help, "desc"=>""},
    "quiet"        => { "optvar"=>\$opt_quiet, "desc"=>""},
    "verbose"      => { "optvar"=>\$opt_verbose, "desc"=>"" });

my %longOptions = map { $_ => $options{$_}->{"optvar"} } keys(%options);
GetOptions( %longOptions ) or pod2usage(2);
pod2usage(1) if $opt_help;


## Format per file, repeated, no linebreak
# <diffcmd>
# index c1..c2 c3
# --- a/<left-hand-side-file>
# +++ b/<right-hand-side-file>
# <diff hunks>

# Format of each diff hunk, repeated, no linebreak
# @@ <ranges> @@ line
# 3 lines of context
# [-|+]lines removed on left, added on right
# 3 lines of context
#
# output:
sub parse_diff
{
    my $patchref = shift;
    my $file="";
    my @checklines=();
    my %b_lines=();
    my $state = 0;
    my $store_line=-1;
    my %files=();

    print "Patch size: ".scalar(@$patchref)."\n" if $pd_debug;
    for my $line (@$patchref)
    {
        if($state == 0)
        {
            print "State: $state  $line  \n" if $pd_debug;
            # Search for a line matching "+++ b/<filename>"
            if( $line =~ m!^\+\+\+ b/([\w-_\./]*)!)
            {
                $file = $1;
                $state = 1 ;
                print "Found File: $file\n" if $pd_debug;
            }
        }
        else #elsif($state == 1)
        {
            # If we find a line starting with diff, the previous
            # file's diffs have finished, store them.
            if( $line =~ /^diff/)
            {
                print "State: $state  $line  \n" if $pd_debug;
                $state = 0;
                # if the file had changes, store the new/modified line numbers
                if( $file && scalar(@checklines))
                {
                    $files{$file}->{"patch"} = [@checklines];
                    $files{$file}->{"b_lines"} = {%b_lines};
                    @checklines=();
                    %b_lines=();
                }
                print("\n\n") if $pd_debug;
            }
            # If we find a line starting with @@, it tells us the line numbers
            # of the old file and new file for this hunk.
            elsif( $line =~ /^@@/)
            {
                print "State: $state  $line  \n" if $pd_debug;

                # Find the lines in the new file (of the form "+<start>[,<length>])
                my($start,$space,$length) = ($line =~ /\+([0-9]+)(,| )([0-9]+)?/);
                if($length || $space eq " ")
                {
                    if( $space eq " " )
                    {
                        $length=1;
                    }
                    push(@checklines, [$start, $length]);
                    $store_line=$start;
                }
                else
                {
                    $store_line = -1;
                }
                if($pd_debug)
                {
                    my $last = scalar(@checklines)-1;
                    if( $last >= 0 )
                    {
                        print "Checkline:" . $checklines[$last]->[0] . ", " . $checklines[$last]->[1] . "\n";
                    }
                }
            }
            # If we find a line starting with "+", it belongs to the new file's patch
            elsif( $line =~ /^\+/)
            {
               if($store_line >= 0)
               {
                   chomp;
                   $line = substr($line, 1); # Remove leading +
                   $b_lines{$store_line} = $line;
                   $store_line++;
               }
            }
        }
    }
    # Store the final entry
    $files{$file}->{"patch"} = [@checklines];
    $files{$file}->{"b_lines"} = {%b_lines};

    my %filter = map { $_ => $files{$_} } grep {m!^dali(-toolkit)?/!} (keys(%files));;

    if($pd_debug)
    {
        print("Filtered files:\n");
        foreach my $file (keys(%filter))
        {
            print("$file: ");
            $patchref = $filter{$file}->{"patch"};
            foreach my $lineblock (@$patchref)
            {
                print "$lineblock->[0]($lineblock->[1]) "
            }
            print ( "\n");
        }
    }

    return {%filter};
}

sub show_patch_lines
{
    my $filesref = shift;
    print "\nNumber of files: " . scalar(keys(%$filesref)) . "\n";
    for my $file (keys(%$filesref))
    {
        print("$file:");
        my $clref = $filesref->{$file}->{"patch"};
        for my $cl (@$clref)
        {
            print("($cl->[0],$cl->[1]) ");
        }
        print("\n");
    }
}

sub get_gcno_file
{
    # Assumes test cases have been run, and "make rename_cov_data" has been executed

    my $file = shift;
    my ($name, $path, $suffix) = fileparse($file, (".c", ".cpp", ".h"));
    my $gcno_file = $repo->wc_path() . "/build/tizen/.cov/$name.gcno";

    # Note, will translate headers to their source's object, which
    # may miss execution code in the headers (e.g. inlines are usually
    # not all used in the implementation, and require getting coverage
    # from test cases.

    if( -f $gcno_file )
    {
        my $gcno_st = stat($gcno_file);
        my $fq_file = $repo->wc_path() . $file;
        my $src_st = stat($fq_file);
        if($gcno_st->ctime < $src_st->mtime)
        {
            print "WARNING: GCNO $gcno_file older than SRC $fq_file\n";
            $gcno_file="";
        }

    }
    else
    {
        print("WARNING: No equivalent gcno file for $file\n");
    }
    return $gcno_file;
}

our %gcovfiles=();
sub get_coverage
{
    my $file = shift;
    my $filesref = shift;
    print("get_coverage($file)\n") if $debug;

    my $gcno_file = get_gcno_file($file);
    my @gcov_files = ();
    my $gcovfile;
    if( $gcno_file )
    {
        print "Running gcov on $gcno_file:\n" if $debug;
        open( my $fh,  "gcov --preserve-paths $gcno_file |") || die "Can't run gcov:$!\n";
        while( <$fh> )
        {
            print $_ if $debug>=3;
            chomp;
            if( m!'(.*\.gcov)'$! )
            {
                my $coverage_file = $1; # File has / replaced with # and .. replaced with ^
                my $source_file = $coverage_file;
                $source_file =~ s!\^!..!g;  # Change ^ to ..
                $source_file =~ s!\#!/!g;   # change #'s to /s
                $source_file =~ s!.gcov$!!; # Strip off .gcov suffix

                print "Matching $file against $source_file\n" if $debug >= 3;
                # Only want the coverage files matching source file:
                if(index( $source_file, $file ) > 0 )
                {
                    $gcovfile = $coverage_file;
                    last;
                }
            }
        }
        close($fh);

        if($gcovfile)
        {
            if($gcovfiles{$gcovfile} == undef)
            {
                # Only parse a gcov file once
                $gcovfiles{$gcovfile}->{"seen"}=1;

                print "Getting coverage data from $gcovfile\n" if $debug;

                open( FH, "< $gcovfile" ) || die "Can't open $gcovfile for reading:$!\n";
                while(<FH>)
                {
                    my ($cov, $line, @code ) = split( /:/, $_ );
                    $cov =~ s/^\s+//; # Strip leading space
                    $line =~ s/^\s+//;
                    my $code=join(":", @code);
                    if($cov =~ /\#/)
                    {
                        # There is no coverage data for these executable lines
                        $gcovfiles{$gcovfile}->{"uncovered"}->{$line}++;
                        $gcovfiles{$gcovfile}->{"src"}->{$line}=$code;
                    }
                    elsif( $cov ne "-" && looks_like_number($cov) && $cov > 0 )
                    {
                        $gcovfiles{$gcovfile}->{"covered"}->{$line}=$cov;
                        $gcovfiles{$gcovfile}->{"src"}->{$line}=$code;
                    }
                    else
                    {
                        # All other lines are not executable.
                        $gcovfiles{$gcovfile}->{"src"}->{$line}=$code;
                    }
                }
                close( FH );
            }
            $filesref->{$file}->{"coverage"} = $gcovfiles{$gcovfile}; # store hashref
        }
        else
        {
            # No gcov output - the gcno file produced no coverage of the source/header
            # Probably means that there is no coverage for the file (with the given
            # test case - there may be some somewhere, but for the sake of speed, don't
            # check (yet).
        }
    }
}

# Run the git diff command to get the patch, then check the coverage
# output for the patch.
sub run_diff
{
    #print "run_diff(" . join(" ", @_) . ")\n";
    my ($fh, $c) = $repo->command_output_pipe(@_);
    our @patch=();
    while(<$fh>)
    {
        chomp;
        push @patch, $_;
    }
    $repo->command_close_pipe($fh, $c);

    print "Patch size: " . scalar(@patch) . "\n" if $debug;

    # @patch has slurped diff for all files...
    my $filesref = parse_diff ( \@patch );
    show_patch_lines($filesref) if $debug;

    print "Checking coverage:\n" if $debug;

    my $cwd=getcwd();
    chdir ".cov" || die "Can't find $cwd/.cov:$!\n";

    for my $file (keys(%$filesref))
    {
        my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
        next if($path !~ /^dali/);
        if($suffix eq ".cpp" || $suffix eq ".c" || $suffix eq ".h")
        {
            get_coverage($file, $filesref);
        }
    }
    chdir $cwd;
    return $filesref;
}

sub calc_patch_coverage_percentage
{
    my $filesref = shift;
    my $total_covered_lines = 0;
    my $total_uncovered_lines = 0;

    foreach my $file (keys(%$filesref))
    {
        my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
        next if($path !~ /^dali/);

        my $covered_lines = 0;
        my $uncovered_lines = 0;

        my $patchref = $filesref->{$file}->{"patch"};
        my $coverage_ref = $filesref->{$file}->{"coverage"};
        if( $coverage_ref )
        {
            for my $patch (@$patchref)
            {
                for(my $i = 0; $i < $patch->[1]; $i++ )
                {
                    my $line = $i + $patch->[0];
                    if($coverage_ref->{"covered"}->{$line})
                    {
                        $covered_lines++;
                        $total_covered_lines++;
                    }
                    if($coverage_ref->{"uncovered"}->{$line})
                    {
                        $uncovered_lines++;
                        $total_uncovered_lines++;
                    }
                }
            }
            $coverage_ref->{"covered_lines"} = $covered_lines;
            $coverage_ref->{"uncovered_lines"} = $uncovered_lines;
            my $total = $covered_lines + $uncovered_lines;
            my $percent = 0;
            if($total > 0)
            {
                $percent = $covered_lines / $total;
            }
            $coverage_ref->{"percent_covered"} = 100 * $percent;
        }
    }
    my $total_exec = $total_covered_lines + $total_uncovered_lines;
    my $percent = 0;
    if($total_exec > 0) { $percent = 100 * $total_covered_lines / $total_exec; }

    return [ $total_exec, $percent ];
}

sub patch_output
{
    my $filesref = shift;
    foreach my $file (keys(%$filesref))
    {
        my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
        next if($path !~ /^dali/);

        my $patchref = $filesref->{$file}->{"patch"};
        my $b_lines_ref = $filesref->{$file}->{"b_lines"};
        my $coverage_ref = $filesref->{$file}->{"coverage"};
        print BOLD, "$file  ";

        if($coverage_ref)
        {
            if( $coverage_ref->{"covered_lines"} > 0
                ||
                $coverage_ref->{"uncovered_lines"} > 0 )
            {
                print GREEN, "Covered: " . $coverage_ref->{"covered_lines"}, RED, " Uncovered: " . $coverage_ref->{"uncovered_lines"}, RESET;
            }
        }
        else
        {
            if($suffix eq ".cpp" || $suffix eq ".c" || $suffix eq ".h")
            {
                print RED;
            }
            print "No coverage found";
        }
        print RESET "\n";

        for my $patch (@$patchref)
        {
            my $hunkstr="Hunk: " . $patch->[0];
            if( $patch->[1] > 1 )
            {
                $hunkstr .= " - " . ($patch->[0]+$patch->[1]-1);
            }
            print BOLD, "$hunkstr\n",  RESET;
            for(my $i = 0; $i < $patch->[1]; $i++ )
            {
                my $line = $i + $patch->[0];
                printf "%-6s  ", $line;

                if($coverage_ref)
                {
                    my $color;
                    if($coverage_ref->{"covered"}->{$line})
                    {
                        $color=GREEN;
                    }
                    elsif($coverage_ref->{"uncovered"}->{$line})
                    {
                        $color=BOLD . RED;
                    }
                    else
                    {
                        $color=BLACK;
                    }
                    my $src=$coverage_ref->{"src"}->{$line};
                    chomp($src);
                    print $color, "$src\n", RESET;
                }
                else
                {
                    # We don't have coverage data, so print it from the patch instead.
                    my $src = $b_lines_ref->{$line};
                    print "$src\n";
                }
            }
        }
    }
}


sub patch_html_output
{
    my $filesref = shift;

    open( my $filehandle, ">", $opt_output ) || die "Can't open $opt_output for writing:$!\n";

    my $OUTPUT_FH = select;
    select $filehandle;
    print <<EOH;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
"http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
<head>
<title>Patch Coverage</title>
</head>
<body bgcolor="white">
EOH

    foreach my $file (keys(%$filesref))
    {
        my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
        next if($path !~ /^dali/);

        my $patchref = $filesref->{$file}->{"patch"};
        my $b_lines_ref = $filesref->{$file}->{"b_lines"};
        my $coverage_ref = $filesref->{$file}->{"coverage"};
        print "<h2>$file</h2>\n";

        if($coverage_ref)
        {
            if( $coverage_ref->{"covered_lines"} > 0
                ||
                $coverage_ref->{"uncovered_lines"} > 0 )
            {
                print "<p style=\"color:green;\">Covered: " .
                    $coverage_ref->{"covered_lines"} . "<p>" .
                    "<p style=\"color:red;\">Uncovered: " .
                    $coverage_ref->{"uncovered_lines"} . "</span></p>";
            }
        }
        else
        {
            print "<p>";
            my $span=0;
            if($suffix eq ".cpp" || $suffix eq ".c" || $suffix eq ".h")
            {
                print "<span style=\"color:red;\">";
                $span=1;
            }
            print "No coverage found";
            print "</span>" if $span;
        }
        print "</p>";

        for my $patch (@$patchref)
        {
            my $hunkstr="Hunk: " . $patch->[0];
            if( $patch->[1] > 1 )
            {
                $hunkstr .= " - " . ($patch->[0]+$patch->[1]-1);
            }
            print "<p style=\"font-weight:bold;\">" . $hunkstr . "</p>";

            print "<pre>";
            for(my $i = 0; $i < $patch->[1]; $i++ )
            {
                my $line = $i + $patch->[0];
                my $num_line_digits=log($line)/log(10);
                for $i (0..(6-$num_line_digits-1))
                {
                    print " ";
                }
                print "$line  ";

                if($coverage_ref)
                {
                    my $color;
                    if($coverage_ref->{"covered"}->{$line})
                    {
                        print("<span style=\"color:green;\">");
                    }
                    elsif($coverage_ref->{"uncovered"}->{$line})
                    {
                        print("<span style=\"color:red;font-weight:bold;\">");
                    }
                    else
                    {
                        #print("<span style=\"color:black;font-weight:normal;\">");
                    }
                    my $src=$coverage_ref->{"src"}->{$line};
                    chomp($src);
                    #print $color, "$src\n", RESET;
                    print "$src</span>\n";
                }
                else
                {
                    # We don't have coverage data, so print it from the patch instead.
                    my $src = $b_lines_ref->{$line};
                    print "$src\n";
                }
            }
            print "<\pre>\n";
        }
    }

    print $filehandle "<hr>\n</body>\n</html>\n";
    close $filehandle;
    select $OUTPUT_FH;
}


################################################################################
##                                    MAIN                                    ##
################################################################################

my $cwd = getcwd();
chdir $repo->wc_path();
chdir "build/tizen";
`make rename_cov_data`;

my @cmd=('--no-pager','diff','--no-ext-diff','-U0','--no-color');

my $status = $repo->command("status", "-s");
if( $status eq "" && !scalar(@ARGV))
{
    # There are no changes in the index or working tree, and
    # no diff arguments to append. Use the last patch instead.
    push @cmd, ('HEAD~1','HEAD');
}
else
{
    # detect if there are only cached changes or only working tree changes
    my $cached = 0;
    my $working = 0;
    for my $fstat ( split(/\n/, $status) )
    {
        if(substr( $fstat, 0, 1 ) ne " "){ $cached++; }
        if(substr( $fstat, 1, 1 ) ne " "){ $working++; }
    }
    if($cached > 0 )
    {
        if($working == 0)
        {
            push @cmd, "--cached";
        }
        else
        {
            die "Both cached & working files - cannot get correct patch from git\n";
            # Would have to diff from separate clone.
        }
    }
}

push @cmd, @ARGV;
my $filesref = run_diff(@cmd);

chdir $cwd;

# Check how many actual source files there are in the patch
my $filecount = 0;
foreach my $file (keys(%$filesref))
{
    my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
    next if($path !~ /^dali/);
    next if($suffix ne ".cpp" && $suffix ne ".c" && $suffix ne ".h");
    $filecount++;
}
if( $filecount == 0 )
{
    print "No source files found\n";
    exit 0;    # Exit with no error.
}

my $percentref = calc_patch_coverage_percentage($filesref);
if($percentref->[0] == 0)
{
    print "No coverable lines found\n";
    exit 0;
}
my $percent = $percentref->[1];

my $color=BOLD RED;
if($opt_output)
{
    print "Outputing to $opt_output\n" if $debug;
    patch_html_output($filesref);
}
elsif( ! $opt_quiet )
{
    patch_output($filesref);
    if($percent>=90)
    {
        $color=GREEN;
    }
    print RESET;
}

printf("Percentage of change covered: %5.2f%\n", $percent);

exit($percent<90);


__END__

=head1 NAME

patch-coverage

=head1 SYNOPSIS

patch-coverage.pl - Determine if patch coverage is below 90%

=head1 DESCRIPTION
Calculates how well the most recent patch is covered (either the
patch that is in the index+working directory, or HEAD).

=head1 OPTIONS

=over 28

=item B<-c|--cached>
Use index files if there is nothing in the working tree

=item B<   --help>
This help

=item B<-q|--quiet>
Don't generate any output

=head1 RETURN STATUS
0 if the coverage of source files is > 90%, otherwise 1

=head1 EXAMPLES


=cut
