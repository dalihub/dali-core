#!/bin/bash

# check if there is 'realpath' tool installed
if [ "$(which realpath)" == "" ] ; then
  echo "You need 'realpath' to run this script."
  echo "to install: apt-get install realpath"
  exit
fi

# test whether script is running in the right directory
if [[ $(realpath $(pwd)) != $(realpath $(dirname $0)) ]] ; then
  echo "Error! You must launch this script from tizen-cmake directory!"
  exit 0
fi

function find_file_lists_dirs()
{
  for f in $(find ../../ -name 'file.list') ; do
    dirname $(realpath $f);
  done
}

# now prepare file-lists
# read line by line
# 1. line with '=' defines variable name
# 2. line with '#' defines comment
function write_cmake_file_lists()
{
  doxy=

  function update_list()
  {
    if [[ "$2" != "" ]] ; then
      # update sources, headers, doxy
      echo "  *** Updating $1 list..." >&2
      echo ""
      echo "SET( $1 \${$1}"
      for var in $2 ; do
        echo "  \${${var}}"
      done
      echo ")"
    fi
  }

  for file_list_dir in ${file_cmake_lists} ; do
    sources=
    public_headers=
    devel_headers=
    integration_headers=
    outfile=$file_list_dir/file-list.cmake
    echo " * Writing $(realpath --relative-base=../../ $file_list_dir)/file-list.cmake..."
    echo "# This file is auto-generated!" > $outfile
    echo "#" >> $outfile

    # look for *_src_dir variables ( should be just one ) and
    # substitute it with current path
    for dirpath in $(grep -oE '\(.*_src_dir\)' $file_list_dir/file.list | sort -u) ; do
      src_dir=${dirpath:1:-1}
      new_path=$(realpath --relative-base=../../ $file_list_dir)
      echo -e "\n# Set the source directory\n" >> $outfile
      echo "SET( $src_dir \${ROOT_SRC_DIR}/$new_path )" >> $outfile
      echo "" >>$outfile
    done
    variable=
    comment=
    lines=$(cat $file_list_dir/file.list | sed -e 's/ /|/g;')
    for l in $lines ; do
      is_var=$(echo $l | grep '=')
      is_comment=$(echo $l | grep '#')
      if [ "$is_comment" ] ; then
        comment="$(echo "$l" | sed -e 's/|/ /g;')"
      elif [ "$is_var" ] ; then
        if [ "$variable" ] ; then
          echo ")"  >> $outfile
          echo "" >> $outfile
        fi
        echo $comment >> $outfile
        comment=
        variable=$(echo $is_var | sed -e 's/|//g;' | awk -F '=' '{print $1}')
        echo "SET( $variable " >> $outfile

        # collect sources, headers and doxy to compile final
        # set of vars to be included
        if [[ ${variable:${#variable}-9:9} = src_files ]] ; then
          sources="${sources} $variable"
        elif [[ ${variable:${#variable}-12:12} = header_files ]] ; then
          if [[ ${variable} = devel_api* ]] ; then
            devel_headers="${devel_headers} $variable"
          elif [[ ${variable} = public_api* ]] ; then
            public_headers="${public_headers} $variable"
          elif [[ ${variable} = platform_abstraction_* ]] ; then
            integration_headers="${integration_headers} $variable"
          else
            echo "ERROR! Variable $variable in $file_list_dir unrecoginzed!"
            exit 0
          fi
        elif [[ ${variable:${#variable}-10:10} = doxy_files ]] ; then
          doxy="${doxy} $variable"
        fi
      else
        echo "$(echo "$l" | sed -e 's/(/{/g;s/)/}/g;s/|/ /g;s/\\//g;')" >> $outfile
      fi
    done
    if [ "$variable" ] ; then
      echo ")" >> $outfile
      echo "" >> $outfile
    fi

    update_list SOURCES "$sources" >> $outfile

    update_list PUBLIC_API_HEADERS "$public_headers" >>$outfile

    update_list DEVEL_API_HEADERS "$devel_headers" >>$outfile

    update_list INTEGRATION_API_HEADERS "$integration_headers" >>$outfile

    # Remove trailing whitespaces
    cat $outfile | sed -e 's/[[:space:]]*$//' > /tmp/file-list.cmake
    mv /tmp/file-list.cmake $outfile
    echo "  * Done"
  done

}

export file_cmake_lists=$(find_file_lists_dirs)

write_cmake_file_lists

