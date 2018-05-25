#!/bin/bash

if [ ! -e 'dali-core' ] ; then

    echo "Error: This script must be executed from the top-level directory where dali-core can be found!"
    exit 0

fi

d=dali-core/dali/graphics/vulkan/shaders
outdir=dali-core/dali/graphics/vulkan/generated

cd $d
all_shaders=$(ls -1 *.{vert,frag})
cd $OLDPWD

for name in $all_shaders ; do
  echo "Compiling $name..."
  varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')  
  glslangValidator -V1.0 $d/$name --vn $varname -o /tmp/spv.h

  newname=$(echo ${name} | sed -e 's/\./-/;')".h"
  echo Writing $newname

  cat /tmp/spv.h | sed -e 's/const uint32_t /std::vector<uint32_t> /g;s/\[\]//g;' | tail -n +3 > $outdir/$newname
  echo >> $outdir/$newname
done

echo "Generating cpp file..."
echo "#include <cstdint>" > $outdir/builtin-shader-gen.cpp
echo "#include <vector>" >> $outdir/builtin-shader-gen.cpp
echo "#include <string>" >> $outdir/builtin-shader-gen.cpp
echo -e "#include <map>\n" >> $outdir/builtin-shader-gen.cpp

varnames=
for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    varnames="${varnames} $varname"
    echo "#include \"$newname\"" >> $outdir/builtin-shader-gen.cpp
done

echo "static std::map<std::string, std::vector<uint32_t>> gGraphicsBuiltinShader = {" >> $outdir/builtin-shader-gen.cpp
for var in ${varnames} ; do
    echo "  { \"$var\", $var }," >> $outdir/builtin-shader-gen.cpp
done
echo -e "};\n" >> $outdir/builtin-shader-gen.cpp

# generate graphics shader getter function
cat >> $outdir/builtin-shader-gen.cpp << EOF
extern "C" {

#define IMPORT_API __attribute__ ((visibility ("default")))

IMPORT_API std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );

std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag )
{
    auto iter = gGraphicsBuiltinShader.find( tag );
    if( iter != gGraphicsBuiltinShader.end() )
    {
        return iter->second;
    }
    return {};
}

}
EOF

echo "Generating extern header file ( for external use )..."
echo "#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" > $outdir/builtin-shader-extern-gen.h
echo "#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" >> $outdir/builtin-shader-extern-gen.h
echo "#include <cstdint>" >> $outdir/builtin-shader-extern-gen.h
echo "#include <vector>" >> $outdir/builtin-shader-extern-gen.h
echo "#include <string>" >> $outdir/builtin-shader-extern-gen.h
echo "" >> $outdir/builtin-shader-extern-gen.h

for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    echo "extern std::vector<uint32_t> $varname;" >> $outdir/builtin-shader-extern-gen.h
done
cat >> $outdir/builtin-shader-extern-gen.h << EOF
extern "C" std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );
#define GraphicsGetBuiltinShaderId( x ) GraphicsGetBuiltinShader( #x )
EOF
echo "#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" >> $outdir/builtin-shader-extern-gen.h