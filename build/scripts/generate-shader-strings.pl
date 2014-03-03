#!/usr/bin/perl -w

###############################################################################

use strict;
use FileHandle;
use Getopt::Long;
use Pod::Usage;

###############################################################################

sub OpenFileForWriting
{
  my($fileName) = shift;

  my $fh = FileHandle->new;
  if (!$fh->open("> $fileName"))
  {
    die "Can't open $fileName for writing: $!\n";
  }
  return $fh;
}

###############################################################################

sub CloseFile
{
  my($fh) = shift;
  $fh->close;
}

###############################################################################

sub GenerateHeaderFileHeader
{
  my($headerFile) = shift;
  my $time=localtime();
  print $headerFile <<"H_HEADER_END";
//
// Automatically Generated on $time
//

#ifndef __DALI_GENERATED_SHADERS_H_
#define __DALI_GENERATED_SHADERS_H_

#include <string>

namespace Dali
{
namespace Internal
{

H_HEADER_END
}

###############################################################################

sub GenerateHeaderFileFooter
{
  my($headerFile) = shift;
  print $headerFile <<"H_FOOTER_END";

} // namespace Internal
} // namespace Dali

#endif // __DALI_GENERATED_SHADERS_H_
H_FOOTER_END
}

###############################################################################

sub GenerateSourceFileHeader
{
  my($sourceFile) = shift;
  my $time=localtime();
  print $sourceFile <<"CPP_HEADER_END";
//
// Automatically Generated on $time
//

#include <string>

namespace Dali
{
namespace Internal
{

CPP_HEADER_END
}

###############################################################################

sub GenerateSourceFileFooter
{
  my($sourceFile) = shift;
  print $sourceFile <<"CPP_FOOTER_END";
} // namespace Internal
} // namespace Dali
CPP_FOOTER_END
}

###############################################################################

sub GenerateStringsFromFile
{
  my($file) = shift;
  my($dir) = shift;
  my($headerFile) = shift;
  my($sourceFile) = shift;

  my $shadername = $file;
  $shadername =~ s/-([a-z])/uc($1)/eg;
  $shadername =~ s/^([a-z])/uc($1)/e;
  $shadername =~ s/\.txt$//;

  my $state = 0;

  open MEM, "$dir/$file" || die "Can't open $file for reading: $!\n";
  while(<MEM>)
  {
    chomp;
    if ($state == 0)
    {
      if (/<VertexShader>/)
      {
        $state = 1;
        print $headerFile "extern const std::string ${shadername}Vertex;\n";
        print $sourceFile "extern const std::string ${shadername}Vertex(\n";
      }
      elsif (/<FragmentShader>/)
      {
        $state = 1;
        print $headerFile "extern const std::string ${shadername}Fragment;\n";
        print $sourceFile "extern const std::string ${shadername}Fragment(\n";
      }
    }
    elsif ($state == 1)
    {
      if (m!</VertexShader>! || m!</FragmentShader>!)
      {
        $state = 0;
        print $sourceFile ");\n\n";
      }
      else
      {
        print $sourceFile "\"$_\\n\"\n";
      }
    }
  }
  close MEM;
}

###############################################################################

sub GetShaderFiles
{
  my ($dir) = shift;
  opendir (DIR, $dir) || die "Can't open $dir for reading: $!\n";
  my @shaderFiles = grep { /\.txt$/ && -f "$dir/$_" } readdir DIR;
  closedir DIR;
  return @shaderFiles;
}

###############################################################################

my($optHelp);
my($optMan);
my($shaderDir) = "";
my($filePrefix) = "";

GetOptions(
  "help"           => \$optHelp,
  "man"            => \$optMan,
  "shader-dir=s"   => \$shaderDir,
  "file-prefix=s"  => \$filePrefix
) or pod2usage(2);

pod2usage(1) if $optHelp;
pod2usage(-exitstatus => 0, -verbose => 2) if $optMan;

if ($shaderDir eq "" || $filePrefix eq "")
{
  pod2usage(1);
}
else
{
  my $dir = $shaderDir;
  my @shaderFiles = GetShaderFiles($dir);

  my $headerFile = OpenFileForWriting("$filePrefix.h");
  my $sourceFile = OpenFileForWriting("$filePrefix.cpp");

  GenerateHeaderFileHeader($headerFile);
  GenerateSourceFileHeader($sourceFile);

  my $file;
  foreach $file (@shaderFiles)
  {
    GenerateStringsFromFile($file, $dir, $headerFile, $sourceFile);
  }

  GenerateHeaderFileFooter($headerFile);
  GenerateSourceFileFooter($sourceFile);

  CloseFile($headerFile);
  CloseFile($sourceFile);
}

###############################################################################

__END__

=head1 NAME

generate-shader-strings.pl - Given a shader directory and a file prefix, this script generates a header and source file where all the shaders in the directory are stored as vertex and fragment shader std::string(s).

=head1 SYNOPSIS

generate-shader-strings.pl -s=<shader-dir> -f=<file-prefix>

generate-shader-strings.pl --shader-dir=<shader-dir> -file-prefix=<file-prefix>

=head1 DESCRIPTION

Given a shader directory and a file prefix, this script generates a header and source file where all the shaders in the directory are stored as vertex and fragment shader std::string(s).

The shader files in the specified directory should have the suffix ".txt" and the vertex and fragment shaders should be encapsulated within <VertexShader>*</VertexShader> and <FragmentShader>*</FragmentShader> respectively in this text file.

The generated source files will be called <file-prefix>.h and <file-prefix>.cpp.

=head1 OPTIONS

=over 36

=item B<-s|--shader-dir=<shader-directory>>

The directory the shader files should be loaded from.

=item B<-f|--file-prefix>

The prefix of the output files.

