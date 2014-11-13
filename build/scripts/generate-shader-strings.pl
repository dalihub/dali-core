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

my %shaders=();

sub GenerateStringsFromFile
{
  my($file) = shift;
  my($dir) = shift;

  my $shadername = $file;
  $shadername =~ s/-([a-z])/uc($1)/eg;
  $shadername =~ s/^([a-z])/uc($1)/e;
  $shadername =~ s/\.txt$//;

  my $state = 0;

  my $shader="";
  open MEM, "$dir/$file" || die "Can't open $file for reading: $!\n";
  while(<MEM>)
  {
    chomp;
    if ($state == 0)
    {
      if (/<VertexShader>/)
      {
        $state = 1;
        $shader = "";
      }
      elsif (/<FragmentShader>/)
      {
        $state = 1;
        $shader = "";
      }
    }
    elsif ($state == 1)
    {
      if (m!</VertexShader>!)
      {
        $state = 0;
        $shaders{$shadername}->{"vertex"} = $shader;
      }
      elsif( m!</FragmentShader>!)
      {
        $state = 0;
        $shaders{$shadername}->{"fragment"} = $shader;
      }
      else
      {
        ## Accumulate
        $shader .= "$_\n";
#       print $sourceFile "\"$_\\n\"\n";
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

sub PrintSourceLine
{
    my $sourceFile=shift;
    my $line=shift;
    chomp $line;
    $line =~ s!//.*$!!; # Strip out comments
    $line =~ s!\s*$!!; # Strip out trailing space
    if( $line !~ m!^\s*$! )
    {
        print $sourceFile "\"$line\\n\"\n";
    }
}

sub PrintMacroLine
{
    my $sourceFile=shift;
    my $line=shift;
    chomp $line;
    $line =~ s!//.*$!!; # Strip out comments
    $line =~ s!\s*$!!; # Strip out trailing space
    if( $line !~ m!^\s*$! )
    {
        print $sourceFile "\"$line\\n\" \\\n";
    }
}


sub PrintShaderProgramWithMacros
{
    my $sourceFile=shift;
    my $shadername=shift;
    my $SHADERNAME=$shadername;
    $SHADERNAME =~ s/([A-Z])/_$1/g;
    substr($SHADERNAME,0,1)="";
    $SHADERNAME = uc($SHADERNAME);

    my $program_type=shift;
    my $ProgramType=ucfirst($program_type);
    my $PROGRAM_TYPE=uc($program_type);
    my $custom=shift;

    my @lines=split(/\n/, $shaders{$shadername}->{$program_type} );

    print $sourceFile "#define ${SHADERNAME}_PREFIX_${PROGRAM_TYPE} \\\n";
    LINE: while( scalar(@lines) )
    {
        last LINE if $lines[0] =~ /main()/;
        PrintMacroLine($sourceFile, shift(@lines));
    }
    print $sourceFile "\n\n";

    print $sourceFile "#define ${SHADERNAME}_POSTFIX_${PROGRAM_TYPE} \\\n";
    LINE: while( scalar(@lines) )
    {
        PrintMacroLine($sourceFile, shift(@lines));
    }
    print $sourceFile "\n\n";

    if($custom)
    {
        print $sourceFile "extern const char* const Custom${shadername}Prefix${ProgramType};\n";
        print $sourceFile "const char* const Custom${shadername}Prefix${ProgramType}(\n";
        print $sourceFile "  ${SHADERNAME}_PREFIX_${PROGRAM_TYPE}\n";
        print $sourceFile ");\n\n";

        print $sourceFile "extern const char* const Custom${shadername}Postfix${ProgramType};\n";
        print $sourceFile "const char* const Custom${shadername}Postfix${ProgramType}(\n";
        print $sourceFile "  ${SHADERNAME}_POSTFIX_${PROGRAM_TYPE}\n";
        print $sourceFile ");\n\n";
    }
    print $sourceFile "extern const char* const ${shadername}${ProgramType};\n";
    print $sourceFile "const char* const ${shadername}${ProgramType}(\n";
    print $sourceFile "  ${SHADERNAME}_PREFIX_${PROGRAM_TYPE}\n";
    print $sourceFile "  ${SHADERNAME}_POSTFIX_${PROGRAM_TYPE}\n";
    print $sourceFile ");\n\n";

}

sub PrintShaderProgram
{
    my $sourceFile=shift;
    my $shadername=shift;
    my $program_type=shift;
    my $custom=shift;
    my $ProgramType=ucfirst($program_type);

    my @lines=split(/\n/, $shaders{$shadername}->{$program_type} );

    print $sourceFile "const char* const ${shadername}${ProgramType}(\n";
    for my $i (0..scalar(@lines)-1)
    {
        PrintSourceLine($sourceFile, $lines[$i]);
    }
    print $sourceFile ");\n\n";

    if( $custom )
    {
        print $sourceFile "const char* const Custom${shadername}Prefix${ProgramType}(\n";
      LINE:
        while( scalar(@lines) )
        {
            last LINE if $lines[0] =~ /main()/;
            PrintSourceLine($sourceFile, shift(@lines));
        }
        print $sourceFile ");\n\n";

        print $sourceFile "const char* const Custom${shadername}Postfix${ProgramType}(\n";

        while( scalar(@lines) )
        {
            PrintSourceLine($sourceFile, shift(@lines));
        }
        print $sourceFile ");\n\n";
    }
}


sub PrintShaderSources
{
  my($headerFile) = shift;
  my($sourceFile) = shift;
  my $shadername;

  # Strings are now in memory. Dump them back out again:
  foreach $shadername (sort(keys(%shaders)))
  {
    print $headerFile "extern const char* const ${shadername}Vertex;\n";
    print $headerFile "extern const char* const ${shadername}Fragment;\n";

    my $custom = 0;
    if( $shadername !~ /TextDistanceField/ || $shadername eq "TextDistanceField" )
    {
        print $headerFile "extern const char* const Custom${shadername}PrefixVertex;\n";
        print $headerFile "extern const char* const Custom${shadername}PostfixVertex;\n";
        print $headerFile "extern const char* const Custom${shadername}PrefixFragment;\n";
        print $headerFile "extern const char* const Custom${shadername}PostfixFragment;\n";
        $custom = 1;
    }
    PrintShaderProgramWithMacros($sourceFile, $shadername, "vertex", $custom);
    PrintShaderProgramWithMacros($sourceFile, $shadername, "fragment", $custom);
  }
}

###############################################################################

my($optHelp);
my($optMan);
my($shaderDir) = "";
my($fileName) = "";

GetOptions(
  "help"           => \$optHelp,
  "man"            => \$optMan,
  "shader-dir=s"   => \$shaderDir,
  "file-name=s"  => \$fileName
) or pod2usage(2);

pod2usage(1) if $optHelp;
pod2usage(-exitstatus => 0, -verbose => 2) if $optMan;

if ($shaderDir eq "" || $fileName eq "")
{
  pod2usage(1);
}
else
{
  my $dir = $shaderDir;
  my @shaderFiles = GetShaderFiles($dir);

  my $headerFile = OpenFileForWriting("$fileName.h");
  my $sourceFile = OpenFileForWriting("$fileName.cpp");

  GenerateHeaderFileHeader($headerFile);
  GenerateSourceFileHeader($sourceFile);

  my $file;
  foreach $file (@shaderFiles)
  {
    GenerateStringsFromFile($file, $dir);
  }

  PrintShaderSources($headerFile, $sourceFile);

  GenerateHeaderFileFooter($headerFile);
  GenerateSourceFileFooter($sourceFile);

  CloseFile($headerFile);
  CloseFile($sourceFile);
}

###############################################################################

__END__

=head1 NAME

generate-shader-strings.pl - Given a shader directory and a file name, this script generates a header and source file where all the shaders in the directory are stored as vertex and fragment shader const char arrays.

=head1 SYNOPSIS

generate-shader-strings.pl -s=<shader-dir> -f=<file-name>

generate-shader-strings.pl -shader-dir=<shader-dir> -file-name=<file-name>

=head1 DESCRIPTION

Given a shader directory and a file name, this script generates a header and source file where all the shaders in the directory are stored as vertex and fragment shader const char arrays.

The shader files in the specified directory should have the suffix ".txt" and the vertex and fragment shaders should be encapsulated within <VertexShader>*</VertexShader> and <FragmentShader>*</FragmentShader> respectively in this text file.

The generated source files will be called <file-name>.h and <file-name>.cpp.

=head1 OPTIONS

=over 36

=item B<-s|--shader-dir=<shader-directory>>

The directory the shader files should be loaded from.

=item B<-f|--file-name>

The name of the output files.
