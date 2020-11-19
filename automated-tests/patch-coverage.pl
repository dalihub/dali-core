#!/usr/bin/perl
#
# Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
use Data::Dumper;

# Dali specific program to run lcov and parse output for files in patch

# A) Generate lcov output from lib & test cases
# B) Get patch using git diff
# C) Generate patch output with covered/uncovered lines marked in green/red
# D) Generate coverage data for changed lines
# E) Exit status should be 0 for high coverage (90% line coverage for all new/changed lines)
#    or 1 for low coverage

# Sources for conversion of gcno/gcda files:
# /usr/bin/lcov & genhtml
# Python git-coverage (From http://stef.thewalter.net/git-coverage-useful-code-coverage.html)

# From genhtml:
sub read_info_file($);
sub get_info_entry($);
sub set_info_entry($$$$$$$$$;$$$$$$);
sub combine_info_entries($$$);
sub combine_info_files($$);
sub compress_brcount($);
sub brcount_to_db($);
sub db_to_brcount($;$);
sub brcount_db_combine($$$);
sub add_counts($$);
sub info(@);

our $repo = Git->repository();
our $debug=0;
our $pd_debug=0;
our $root;
our %info_data; # Hash containing all data from .info files

# Settings from genhtml:
our $func_coverage;     # If set, generate function coverage statistics
our $no_func_coverage;  # Disable func_coverage
our $br_coverage;       # If set, generate branch coverage statistics
our $no_br_coverage;    # Disable br_coverage
our $sort = 1;          # If set, provide directory listings with sorted entries
our $no_sort;           # Disable sort

# Branch data combination types
our $BR_SUB = 0;
our $BR_ADD = 1;

# Command line options
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


# From genhtml:
#
# read_info_file(info_filename)
#
# Read in the contents of the .info file specified by INFO_FILENAME. Data will
# be returned as a reference to a hash containing the following mappings:
#
# %result: for each filename found in file -> \%data
#
# %data: "test"    -> \%testdata
#        "sum"     -> \%sumcount
#        "func"    -> \%funcdata
#        "found"   -> $lines_found (number of instrumented lines found in file)
#        "hit"     -> $lines_hit (number of executed lines in file)
#        "f_found" -> $fn_found (number of instrumented functions found in file)
#        "f_hit"   -> $fn_hit (number of executed functions in file)
#        "b_found" -> $br_found (number of instrumented branches found in file)
#        "b_hit"   -> $br_hit (number of executed branches in file)
#        "check"   -> \%checkdata
#        "testfnc" -> \%testfncdata
#        "sumfnc"  -> \%sumfnccount
#        "testbr"  -> \%testbrdata
#        "sumbr"   -> \%sumbrcount
#
# %testdata   : name of test affecting this file -> \%testcount
# %testfncdata: name of test affecting this file -> \%testfnccount
# %testbrdata:  name of test affecting this file -> \%testbrcount
#
# %testcount   : line number   -> execution count for a single test
# %testfnccount: function name -> execution count for a single test
# %testbrcount : line number   -> branch coverage data for a single test
# %sumcount    : line number   -> execution count for all tests
# %sumfnccount : function name -> execution count for all tests
# %sumbrcount  : line number   -> branch coverage data for all tests
# %funcdata    : function name -> line number
# %checkdata   : line number   -> checksum of source code line
# $brdata      : vector of items: block, branch, taken
#
# Note that .info file sections referring to the same file and test name
# will automatically be combined by adding all execution counts.
#
# Note that if INFO_FILENAME ends with ".gz", it is assumed that the file
# is compressed using GZIP. If available, GUNZIP will be used to decompress
# this file.
#
# Die on error.
#
sub read_info_file($)
{
    my $tracefile = $_[0];      # Name of tracefile
    my %result;             # Resulting hash: file -> data
    my $data;           # Data handle for current entry
    my $testdata;           #       "             "
    my $testcount;          #       "             "
    my $sumcount;           #       "             "
    my $funcdata;           #       "             "
    my $checkdata;          #       "             "
    my $testfncdata;
    my $testfnccount;
    my $sumfnccount;
    my $testbrdata;
    my $testbrcount;
    my $sumbrcount;
    my $line;           # Current line read from .info file
    my $testname;           # Current test name
    my $filename;           # Current filename
    my $hitcount;           # Count for lines hit
    my $count;          # Execution count of current line
    my $negative;           # If set, warn about negative counts
    my $changed_testname;       # If set, warn about changed testname
    my $line_checksum;      # Checksum of current line
    my $notified_about_relative_paths;
    local *INFO_HANDLE;         # Filehandle for .info file

    info("Reading data file $tracefile\n");

    # Check if file exists and is readable
    stat($tracefile);
    if (!(-r _))
    {
        die("ERROR: cannot read file $tracefile!\n");
    }

    # Check if this is really a plain file
    if (!(-f _))
    {
        die("ERROR: not a plain file: $tracefile!\n");
    }

    # Check for .gz extension
    if ($tracefile =~ /\.gz$/)
    {
        # Check for availability of GZIP tool
        system_no_output(1, "gunzip" ,"-h")
            and die("ERROR: gunzip command not available!\n");

        # Check integrity of compressed file
        system_no_output(1, "gunzip", "-t", $tracefile)
            and die("ERROR: integrity check failed for ".
                    "compressed file $tracefile!\n");

        # Open compressed file
        open(INFO_HANDLE, "-|", "gunzip -c '$tracefile'")
            or die("ERROR: cannot start gunzip to decompress ".
                   "file $tracefile!\n");
    }
    else
    {
        # Open decompressed file
        open(INFO_HANDLE, "<", $tracefile)
            or die("ERROR: cannot read file $tracefile!\n");
    }

    $testname = "";
    while (<INFO_HANDLE>)
    {
        chomp($_);
        $line = $_;

        # Switch statement
        foreach ($line)
        {
            /^TN:([^,]*)(,diff)?/ && do
            {
                # Test name information found
                $testname = defined($1) ? $1 : "";
                if ($testname =~ s/\W/_/g)
                {
                    $changed_testname = 1;
                }
                $testname .= $2 if (defined($2));
                last;
            };

            /^[SK]F:(.*)/ && do
            {
                # Filename information found
                # Retrieve data for new entry
                $filename = File::Spec->rel2abs($1, $root);

                if (!File::Spec->file_name_is_absolute($1) &&
                    !$notified_about_relative_paths)
                {
                    info("Resolved relative source file ".
                         "path \"$1\" with CWD to ".
                         "\"$filename\".\n");
                    $notified_about_relative_paths = 1;
                }

                $data = $result{$filename};
                ($testdata, $sumcount, $funcdata, $checkdata,
                 $testfncdata, $sumfnccount, $testbrdata,
                 $sumbrcount) =
                    get_info_entry($data);

                if (defined($testname))
                {
                    $testcount = $testdata->{$testname};
                    $testfnccount = $testfncdata->{$testname};
                    $testbrcount = $testbrdata->{$testname};
                }
                else
                {
                    $testcount = {};
                    $testfnccount = {};
                    $testbrcount = {};
                }
                last;
            };

            /^DA:(\d+),(-?\d+)(,[^,\s]+)?/ && do
            {
                # Fix negative counts
                $count = $2 < 0 ? 0 : $2;
                if ($2 < 0)
                {
                    $negative = 1;
                }
                # Execution count found, add to structure
                # Add summary counts
                $sumcount->{$1} += $count;

                # Add test-specific counts
                if (defined($testname))
                {
                    $testcount->{$1} += $count;
                }

                # Store line checksum if available
                if (defined($3))
                {
                    $line_checksum = substr($3, 1);

                    # Does it match a previous definition
                    if (defined($checkdata->{$1}) &&
                        ($checkdata->{$1} ne
                         $line_checksum))
                    {
                        die("ERROR: checksum mismatch ".
                            "at $filename:$1\n");
                    }

                    $checkdata->{$1} = $line_checksum;
                }
                last;
            };

            /^FN:(\d+),([^,]+)/ && do
            {
                last if (!$func_coverage);

                # Function data found, add to structure
                $funcdata->{$2} = $1;

                # Also initialize function call data
                if (!defined($sumfnccount->{$2})) {
                    $sumfnccount->{$2} = 0;
                }
                if (defined($testname))
                {
                    if (!defined($testfnccount->{$2})) {
                        $testfnccount->{$2} = 0;
                    }
                }
                last;
            };

            /^FNDA:(\d+),([^,]+)/ && do
            {
                last if (!$func_coverage);
                # Function call count found, add to structure
                # Add summary counts
                $sumfnccount->{$2} += $1;

                # Add test-specific counts
                if (defined($testname))
                {
                    $testfnccount->{$2} += $1;
                }
                last;
            };

            /^BRDA:(\d+),(\d+),(\d+),(\d+|-)/ && do {
                # Branch coverage data found
                my ($line, $block, $branch, $taken) =
                    ($1, $2, $3, $4);

                last if (!$br_coverage);
                $sumbrcount->{$line} .=
                    "$block,$branch,$taken:";

                # Add test-specific counts
                if (defined($testname)) {
                    $testbrcount->{$line} .=
                        "$block,$branch,$taken:";
                }
                last;
            };

            /^end_of_record/ && do
            {
                # Found end of section marker
                if ($filename)
                {
                    # Store current section data
                    if (defined($testname))
                    {
                        $testdata->{$testname} =
                            $testcount;
                        $testfncdata->{$testname} =
                            $testfnccount;
                        $testbrdata->{$testname} =
                            $testbrcount;
                    }

                    set_info_entry($data, $testdata,
                                   $sumcount, $funcdata,
                                   $checkdata, $testfncdata,
                                   $sumfnccount,
                                   $testbrdata,
                                   $sumbrcount);
                    $result{$filename} = $data;
                    last;
                }
            };

            # default
            last;
        }
    }
    close(INFO_HANDLE);

    # Calculate lines_found and lines_hit for each file
    foreach $filename (keys(%result))
    {
        $data = $result{$filename};

        ($testdata, $sumcount, undef, undef, $testfncdata,
         $sumfnccount, $testbrdata, $sumbrcount) =
            get_info_entry($data);

        # Filter out empty files
        if (scalar(keys(%{$sumcount})) == 0)
        {
            delete($result{$filename});
            next;
        }
        # Filter out empty test cases
        foreach $testname (keys(%{$testdata}))
        {
            if (!defined($testdata->{$testname}) ||
                scalar(keys(%{$testdata->{$testname}})) == 0)
            {
                delete($testdata->{$testname});
                delete($testfncdata->{$testname});
            }
        }

        $data->{"found"} = scalar(keys(%{$sumcount}));
        $hitcount = 0;

        foreach (keys(%{$sumcount}))
        {
            if ($sumcount->{$_} > 0) { $hitcount++; }
        }

        $data->{"hit"} = $hitcount;

        # Get found/hit values for function call data
        $data->{"f_found"} = scalar(keys(%{$sumfnccount}));
        $hitcount = 0;

        foreach (keys(%{$sumfnccount})) {
            if ($sumfnccount->{$_} > 0) {
                $hitcount++;
            }
        }
        $data->{"f_hit"} = $hitcount;

        # Combine branch data for the same branches
        (undef, $data->{"b_found"}, $data->{"b_hit"}) = compress_brcount($sumbrcount);
        foreach $testname (keys(%{$testbrdata})) {
            compress_brcount($testbrdata->{$testname});
        }
    }

    if (scalar(keys(%result)) == 0)
    {
        die("ERROR: no valid records found in tracefile $tracefile\n");
    }
    if ($negative)
    {
        warn("WARNING: negative counts found in tracefile ".
             "$tracefile\n");
    }
    if ($changed_testname)
    {
        warn("WARNING: invalid characters removed from testname in ".
             "tracefile $tracefile\n");
    }

    return(\%result);
}

sub print_simplified_info
{
    for my $key (keys(%info_data))
    {
        print "K $key: \n";
        my $sumcountref = $info_data{$key}->{"sum"};
        for my $line (sort{$a<=>$b}(keys(%$sumcountref)))
        {
            print "L  $line: $sumcountref->{$line}\n";
        }
    }
}

# From genhtml:
#
# get_info_entry(hash_ref)
#
# Retrieve data from an entry of the structure generated by read_info_file().
# Return a list of references to hashes:
# (test data hash ref, sum count hash ref, funcdata hash ref, checkdata hash
#  ref, testfncdata hash ref, sumfnccount hash ref, lines found, lines hit,
#  functions found, functions hit)
#

sub get_info_entry($)
{
    my $testdata_ref = $_[0]->{"test"};
    my $sumcount_ref = $_[0]->{"sum"};
    my $funcdata_ref = $_[0]->{"func"};
    my $checkdata_ref = $_[0]->{"check"};
    my $testfncdata = $_[0]->{"testfnc"};
    my $sumfnccount = $_[0]->{"sumfnc"};
    my $testbrdata = $_[0]->{"testbr"};
    my $sumbrcount = $_[0]->{"sumbr"};
    my $lines_found = $_[0]->{"found"};
    my $lines_hit = $_[0]->{"hit"};
    my $fn_found = $_[0]->{"f_found"};
    my $fn_hit = $_[0]->{"f_hit"};
    my $br_found = $_[0]->{"b_found"};
    my $br_hit = $_[0]->{"b_hit"};

    return ($testdata_ref, $sumcount_ref, $funcdata_ref, $checkdata_ref,
            $testfncdata, $sumfnccount, $testbrdata, $sumbrcount,
            $lines_found, $lines_hit, $fn_found, $fn_hit,
            $br_found, $br_hit);
}


# From genhtml:
#
# set_info_entry(hash_ref, testdata_ref, sumcount_ref, funcdata_ref,
#                checkdata_ref, testfncdata_ref, sumfcncount_ref,
#                testbrdata_ref, sumbrcount_ref[,lines_found,
#                lines_hit, f_found, f_hit, $b_found, $b_hit])
#
# Update the hash referenced by HASH_REF with the provided data references.
#

sub set_info_entry($$$$$$$$$;$$$$$$)
{
    my $data_ref = $_[0];

    $data_ref->{"test"} = $_[1];
    $data_ref->{"sum"} = $_[2];
    $data_ref->{"func"} = $_[3];
    $data_ref->{"check"} = $_[4];
    $data_ref->{"testfnc"} = $_[5];
    $data_ref->{"sumfnc"} = $_[6];
    $data_ref->{"testbr"} = $_[7];
    $data_ref->{"sumbr"} = $_[8];

    if (defined($_[9])) { $data_ref->{"found"} = $_[9]; }
    if (defined($_[10])) { $data_ref->{"hit"} = $_[10]; }
    if (defined($_[11])) { $data_ref->{"f_found"} = $_[11]; }
    if (defined($_[12])) { $data_ref->{"f_hit"} = $_[12]; }
    if (defined($_[13])) { $data_ref->{"b_found"} = $_[13]; }
    if (defined($_[14])) { $data_ref->{"b_hit"} = $_[14]; }
}

# From genhtml:
#
# combine_info_entries(entry_ref1, entry_ref2, filename)
#
# Combine .info data entry hashes referenced by ENTRY_REF1 and ENTRY_REF2.
# Return reference to resulting hash.
#
sub combine_info_entries($$$)
{
    my $entry1 = $_[0];     # Reference to hash containing first entry
    my $testdata1;
    my $sumcount1;
    my $funcdata1;
    my $checkdata1;
    my $testfncdata1;
    my $sumfnccount1;
    my $testbrdata1;
    my $sumbrcount1;

    my $entry2 = $_[1];     # Reference to hash containing second entry
    my $testdata2;
    my $sumcount2;
    my $funcdata2;
    my $checkdata2;
    my $testfncdata2;
    my $sumfnccount2;
    my $testbrdata2;
    my $sumbrcount2;

    my %result;         # Hash containing combined entry
    my %result_testdata;
    my $result_sumcount = {};
    my $result_funcdata;
    my $result_testfncdata;
    my $result_sumfnccount;
    my $result_testbrdata;
    my $result_sumbrcount;
    my $lines_found;
    my $lines_hit;
    my $fn_found;
    my $fn_hit;
    my $br_found;
    my $br_hit;

    my $testname;
    my $filename = $_[2];

    # Retrieve data
    ($testdata1, $sumcount1, $funcdata1, $checkdata1, $testfncdata1,
     $sumfnccount1, $testbrdata1, $sumbrcount1) = get_info_entry($entry1);
    ($testdata2, $sumcount2, $funcdata2, $checkdata2, $testfncdata2,
     $sumfnccount2, $testbrdata2, $sumbrcount2) = get_info_entry($entry2);

#    # Merge checksums
#    $checkdata1 = merge_checksums($checkdata1, $checkdata2, $filename);
#
#    # Combine funcdata
#    $result_funcdata = merge_func_data($funcdata1, $funcdata2, $filename);
#
#    # Combine function call count data
#    $result_testfncdata = add_testfncdata($testfncdata1, $testfncdata2);
#    ($result_sumfnccount, $fn_found, $fn_hit) =
#        add_fnccount($sumfnccount1, $sumfnccount2);
#
#    # Combine branch coverage data
#    $result_testbrdata = add_testbrdata($testbrdata1, $testbrdata2);
#    ($result_sumbrcount, $br_found, $br_hit) =
#        combine_brcount($sumbrcount1, $sumbrcount2, $BR_ADD);
#
    # Combine testdata
    foreach $testname (keys(%{$testdata1}))
    {
        if (defined($testdata2->{$testname}))
        {
            # testname is present in both entries, requires
            # combination
            ($result_testdata{$testname}) =
                add_counts($testdata1->{$testname},
                           $testdata2->{$testname});
        }
        else
        {
            # testname only present in entry1, add to result
            $result_testdata{$testname} = $testdata1->{$testname};
        }

        # update sum count hash
        ($result_sumcount, $lines_found, $lines_hit) =
            add_counts($result_sumcount,
                       $result_testdata{$testname});
    }

    foreach $testname (keys(%{$testdata2}))
    {
        # Skip testnames already covered by previous iteration
        if (defined($testdata1->{$testname})) { next; }

        # testname only present in entry2, add to result hash
        $result_testdata{$testname} = $testdata2->{$testname};

        # update sum count hash
        ($result_sumcount, $lines_found, $lines_hit) =
            add_counts($result_sumcount,
                       $result_testdata{$testname});
    }

    # Calculate resulting sumcount

    # Store result
    set_info_entry(\%result, \%result_testdata, $result_sumcount,
                   $result_funcdata, $checkdata1, $result_testfncdata,
                   $result_sumfnccount, $result_testbrdata,
                   $result_sumbrcount, $lines_found, $lines_hit,
                   $fn_found, $fn_hit, $br_found, $br_hit);

    return(\%result);
}

# From genhtml:
#
# combine_info_files(info_ref1, info_ref2)
#
# Combine .info data in hashes referenced by INFO_REF1 and INFO_REF2. Return
# reference to resulting hash.
#
sub combine_info_files($$)
{
    my %hash1 = %{$_[0]};
    my %hash2 = %{$_[1]};
    my $filename;

    foreach $filename (keys(%hash2))
    {
        if ($hash1{$filename})
        {
            # Entry already exists in hash1, combine them
            $hash1{$filename} =
                combine_info_entries($hash1{$filename},
                                     $hash2{$filename},
                                     $filename);
        }
        else
        {
            # Entry is unique in both hashes, simply add to
            # resulting hash
            $hash1{$filename} = $hash2{$filename};
        }
    }

    return(\%hash1);
}

# From genhtml:
#
# add_counts(data1_ref, data2_ref)
#
# DATA1_REF and DATA2_REF are references to hashes containing a mapping
#
#   line number -> execution count
#
# Return a list (RESULT_REF, LINES_FOUND, LINES_HIT) where RESULT_REF
# is a reference to a hash containing the combined mapping in which
# execution counts are added.
#
sub add_counts($$)
{
    my $data1_ref = $_[0];  # Hash 1
    my $data2_ref = $_[1];  # Hash 2
    my %result;             # Resulting hash
    my $line;               # Current line iteration scalar
    my $data1_count;        # Count of line in hash1
    my $data2_count;        # Count of line in hash2
    my $found = 0;          # Total number of lines found
    my $hit = 0;            # Number of lines with a count > 0

    foreach $line (keys(%$data1_ref))
    {
        $data1_count = $data1_ref->{$line};
        $data2_count = $data2_ref->{$line};

        # Add counts if present in both hashes
        if (defined($data2_count)) { $data1_count += $data2_count; }

        # Store sum in %result
        $result{$line} = $data1_count;

        $found++;
        if ($data1_count > 0) { $hit++; }
    }

    # Add lines unique to data2_ref
    foreach $line (keys(%$data2_ref))
    {
        # Skip lines already in data1_ref
        if (defined($data1_ref->{$line})) { next; }

        # Copy count from data2_ref
        $result{$line} = $data2_ref->{$line};

        $found++;
        if ($result{$line} > 0) { $hit++; }
    }

    return (\%result, $found, $hit);
}


# From genhtml:
sub compress_brcount($)
{
    my ($brcount) = @_;
    my $db;

    $db = brcount_to_db($brcount);
    return db_to_brcount($db, $brcount);
}

#
# brcount_to_db(brcount)
#
# Convert brcount data to the following format:
#
# db:          line number    -> block hash
# block hash:  block number   -> branch hash
# branch hash: branch number  -> taken value
#

sub brcount_to_db($)
{
    my ($brcount) = @_;
    my $line;
    my $db;

    # Add branches to database
    foreach $line (keys(%{$brcount})) {
        my $brdata = $brcount->{$line};

        foreach my $entry (split(/:/, $brdata)) {
            my ($block, $branch, $taken) = split(/,/, $entry);
            my $old = $db->{$line}->{$block}->{$branch};

            if (!defined($old) || $old eq "-") {
                $old = $taken;
            } elsif ($taken ne "-") {
                $old += $taken;
            }

            $db->{$line}->{$block}->{$branch} = $old;
        }
    }

    return $db;
}


#
# db_to_brcount(db[, brcount])
#
# Convert branch coverage data back to brcount format. If brcount is specified,
# the converted data is directly inserted in brcount.
#

sub db_to_brcount($;$)
{
    my ($db, $brcount) = @_;
    my $line;
    my $br_found = 0;
    my $br_hit = 0;

    # Convert database back to brcount format
    foreach $line (sort({$a <=> $b} keys(%{$db}))) {
        my $ldata = $db->{$line};
        my $brdata;
        my $block;

        foreach $block (sort({$a <=> $b} keys(%{$ldata}))) {
            my $bdata = $ldata->{$block};
            my $branch;

            foreach $branch (sort({$a <=> $b} keys(%{$bdata}))) {
                my $taken = $bdata->{$branch};

                $br_found++;
                $br_hit++ if ($taken ne "-" && $taken > 0);
                $brdata .= "$block,$branch,$taken:";
            }
        }
        $brcount->{$line} = $brdata;
    }

    return ($brcount, $br_found, $br_hit);
}


#
# brcount_db_combine(db1, db2, op)
#
# db1 := db1 op db2, where
#   db1, db2: brcount data as returned by brcount_to_db
#   op:       one of $BR_ADD and BR_SUB
#
sub brcount_db_combine($$$)
{
    my ($db1, $db2, $op) = @_;

    foreach my $line (keys(%{$db2})) {
        my $ldata = $db2->{$line};

        foreach my $block (keys(%{$ldata})) {
            my $bdata = $ldata->{$block};

            foreach my $branch (keys(%{$bdata})) {
                my $taken = $bdata->{$branch};
                my $new = $db1->{$line}->{$block}->{$branch};

                if (!defined($new) || $new eq "-") {
                    $new = $taken;
                } elsif ($taken ne "-") {
                    if ($op == $BR_ADD) {
                        $new += $taken;
                    } elsif ($op == $BR_SUB) {
                        $new -= $taken;
                        $new = 0 if ($new < 0);
                    }
                }

                $db1->{$line}->{$block}->{$branch} = $new;
            }
        }
    }
}

# From genhtml
sub info(@)
{
    if($debug)
    {
        # Print info string
        printf(@_);
    }
}

# NEW STUFF

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
# <dali-source-file>: source / header files in dali/dali-toolkit
# \%filter: <dali-source-file> -> \%filedata
# %filedata: "patch"   -> \@checklines
#            "b_lines" -> \%b_lines
# @checklines: vector of \[start, length] # line numbers of new/modified lines
# %b_lines: <line-number> -> patch line in b-file.
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

    return {%filter};#copy? - test and fixme
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


# Run the git diff command to get the patch
# Output - see parse_diff
sub run_diff
{
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
    show_patch_lines($filesref) if $debug>1;

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

        my $abs_filename = File::Spec->rel2abs($file, $root);
        my $sumcountref = $info_data{$abs_filename}->{"sum"};

        if( $sumcountref )
        {
            for my $patch (@$patchref)
            {
                for(my $i = 0; $i < $patch->[1]; $i++ )
                {
                    my $line = $i + $patch->[0];
                    if(exists($sumcountref->{$line}))
                    {
                        if( $sumcountref->{$line} > 0 )
                        {
                            $covered_lines++;
                            $total_covered_lines++;
                        }
                        else
                        {
                            $uncovered_lines++;
                            $total_uncovered_lines++;
                        }
                    }
                }
            }
            $filesref->{$file}->{"covered_lines"} = $covered_lines;
            $filesref->{$file}->{"uncovered_lines"} = $uncovered_lines;
            my $total = $covered_lines + $uncovered_lines;
            my $percent = 0;
            if($total > 0)
            {
                $percent = $covered_lines / $total;
            }
            $filesref->{$file}->{"percent_covered"} = 100 * $percent;
        }
        else
        {
            print "Can't find coverage data for $abs_filename\n";
        }
    }
    my $total_exec = $total_covered_lines + $total_uncovered_lines;
    my $percent = 0;
    if($total_exec > 0) { $percent = 100 * $total_covered_lines / $total_exec; }

    return [ $total_exec, $percent, $total_covered_lines ];
}

#
#
sub patch_output
{
    my $filesref = shift;
    foreach my $file (keys(%$filesref))
    {
        my ($name, $path, $suffix) = fileparse($file, qr{\.[^.]*$});
        next if($path !~ /^dali/);

        my $fileref = $filesref->{$file};
        my $patchref = $fileref->{"patch"};
        my $b_lines_ref = $fileref->{"b_lines"};

        my $abs_filename = File::Spec->rel2abs($file, $root);
        my $sumcountref = $info_data{$abs_filename}->{"sum"};

        print BOLD, "$file  ";

        if($fileref)
        {
            if( $fileref->{"covered_lines"} > 0
                ||
                $fileref->{"uncovered_lines"} > 0 )
            {
                print GREEN, "Covered: " . $fileref->{"covered_lines"}, RED, " Uncovered: " . $fileref->{"uncovered_lines"}, RESET;
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

                if($sumcountref)
                {
                    my $color;
                    if(exists($sumcountref->{$line}))
                    {
                        if($sumcountref->{$line} > 0)
                        {
                            $color=GREEN;
                        }
                        else
                        {
                            $color=BOLD . RED;
                        }
                    }
                    else
                    {
                        $color=CYAN;
                    }
                    my $src = $b_lines_ref->{$line};
                    print $color, "$src\n", RESET;
                }
                else
                {
                    my $src = $b_lines_ref->{$line};
                    print "$src\n";
                }
            }
        }
    }
}

#
#
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

        my $fileref = $filesref->{$file};
        my $patchref = $fileref->{"patch"};
        my $b_lines_ref = $fileref->{"b_lines"};

        my $abs_filename = File::Spec->rel2abs($file, $root);
        my $sumcountref = $info_data{$abs_filename}->{"sum"};

        print "<h2>$file</h2>\n";

        if($fileref)
        {
            if( $fileref->{"covered_lines"} > 0
                ||
                $fileref->{"uncovered_lines"} > 0 )
            {
                print "<p style=\"color:green;\">Covered: " .
                    $fileref->{"covered_lines"} . "<p>" .
                    "<p style=\"color:red;\">Uncovered: " .
                    $fileref->{"uncovered_lines"} . "</span></p>";
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

                if($sumcountref)
                {
                    my $color;
                    if(exists($sumcountref->{$line}))
                    {
                        if($sumcountref->{$line} > 0)
                        {
                            print("<span style=\"color:green;\">");
                        }
                        else
                        {
                            print("<span style=\"color:red;font-weight:bold;\">");
                        }
                    }
                    else
                    {
                        print("<span style=\"color:black;font-weight:normal;\">");
                    }
                    my $src=$b_lines_ref->{$line};
                    chomp($src);
                    print "$src</span>\n";
                }
                else
                {
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

my $cwd = getcwd(); # expect this to be automated-tests folder

# execute coverage.sh, generating build/tizen/dali.info from lib, and
# *.dir/dali.info. Don't generate html
print `./coverage.sh -n`;
chdir "..";
$root = getcwd();

our %info_data; # Hash of all data from .info files
my @info_files = split(/\n/, `find . -name dali.info`);
my %new_info;

# Read in all specified .info files
foreach (@info_files)
{
    %new_info = %{read_info_file($_)};

    # Combine %new_info with %info_data
    %info_data = %{combine_info_files(\%info_data, \%new_info)};
}


# Generate git diff command
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

# Execute diff & coverage from root directory
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

#print_simplified_info() if $debug;
#exit 0;

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

printf("Line Coverage: %d/%d\n", $percentref->[2], $percentref->[0]);
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
