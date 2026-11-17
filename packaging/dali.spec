# This source builds DALi 3 (dali3). On Tizen 10.1 and 11, where applications
# are still moving from DALi 2.x, it also produces the DALi 2.x package names
# (dali2, dali2-devel, dali2-integration-devel) as alias packages: links to
# the dali3 files plus the Provides that 2.x binaries and specs depend on. A
# binary built against 2.x loads the dali3 library through the link, and a
# project linked against the alias records the dali3 SONAME, so it becomes a
# dali3 binary without a source change. On Tizen 10.0 and below the real DALi
# 2.x is installed from its own source, so the aliases must never be built
# there. --with/--without dali2_aliases overrides the default for a local
# build.
#
# Nested %if, not %elif: Tizen's rpm (4.14) predates %elif and silently
# skips such a branch.
%define legacy_tizen (0%{?tizen_version_major} < 10 || (0%{?tizen_version_major} == 10 && 0%{?tizen_version_minor} == 0))
%if %{legacy_tizen}
%bcond_with    dali2_aliases
%else
%bcond_without dali2_aliases
%endif
%if %{with dali2_aliases} && %{legacy_tizen}
%{error:Tizen <= 10.0 ships the real DALi 2.x; dali2 aliases must not be built there}
%endif

# Temporary, for the Tizen Studio rootstrap. Its generator copies only the
# files that the packages it names list themselves, and reaches a directory
# link as an empty directory, so while the public rootstrap still names the
# DALi 2.x packages and include paths the alias devel packages install real
# copies of the headers under include/dali in place of the link; the adaptor
# and ui specs carry the same switch. Remove this once the rootstrap names the
# dali3 packages and include/dali3.
%if %{with dali2_aliases}
%bcond_without dali2_include_copy
%endif

# rpm derives no Provides from a symbolic link, so the alias packages declare
# the ELF ones themselves. 64-bit ELF provides carry a suffix.
%if "%{__isa_bits}" == "64"
%define elf_suffix ()(64bit)
%else
%define elf_suffix %{nil}
%endif

Name:       dali3
Summary:    DALi 3D Engine
Version:    3.0.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-core.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  gawk
BuildRequires:  python3

%if 0%{?tizen_version_major} >= 3
BuildRequires:  pkgconfig(libtzplatform-config)
%endif

# For ASAN test
%if "%{vd_asan}" == "1" || "%{asan}" == "1"
BuildRequires: asan-force-options
BuildRequires: asan-build-env
BuildRequires: libasan
%endif

%description
DALi 3D Engine

##############################
# devel
##############################
%package devel
Summary:    Development components for DALi 3D Engine
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description devel
Development components for DALi 3D Engine - public headers and package config

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for DALi 3D Engine
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-devel = %{version}-%{release}

%description integration-devel
Integration development package for DALi 3D Engine - headers for integrating with an adaptor/platform abstraction library.

%if %{with dali2_aliases}
##############################
# DALi 2.x names (aliases)
##############################
%package -n dali2
Summary:    DALi 2.x library name for the DALi 3D Engine
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides:   libdali2-core.so.2%{elf_suffix}

%description -n dali2
libdali2-core.so.2 as a link to the dali3 library, for binaries built against DALi 2.x. Goes away once the applications have moved to dali3.

%package -n dali2-devel
Summary:    DALi 2.x development names for the DALi 3D Engine
Group:      Development/Building
Requires:   dali2 = %{version}-%{release}
Requires:   %{name}-devel = %{version}-%{release}

%description -n dali2-devel
include/dali, dali2-core.pc and dali-invoke-method-generator as links to their dali3 counterparts, so that a project written for DALi 2.x builds unchanged and links against dali3.

%package -n dali2-integration-devel
Summary:    DALi 2.x integration development name for the DALi 3D Engine
Group:      Development/Building
Requires:   dali2-devel = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}

%description -n dali2-integration-devel
Name-only package: the integration headers are reached through the include/dali link of dali2-devel.
%endif

##############################
# Preparation
##############################
%prep
%setup -q

##############################
# Build
##############################
%build
# Keep shadow diagnostics enabled independently of the build server policy.
export CFLAGS="${CFLAGS:-%optflags} -Werror=shadow"
export CXXFLAGS="${CXXFLAGS:-%optflags} -Werror=shadow"

PREFIX="/usr"
CXXFLAGS="$CXXFLAGS -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS="$LDFLAGS -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -Wl,-Bsymbolic-functions "

%ifarch %{arm}
CXXFLAGS="$CXXFLAGS -D_ARCH_ARM_ -mfpu=neon"
%endif

%if 0%{?enable_coverage}
CXXFLAGS="$CXXFLAGS --coverage "
LDFLAGS="$LDFLAGS --coverage "
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/tizen

CFLAGS="${CFLAGS:-%optflags}" ;
CXXFLAGS="${CXXFLAGS:-%optflags}" ;
LDFLAGS="${LDFLAGS:-%optflags}" ;

%if "%{vd_asan}" == "1" || "%{asan}" == "1"
CFLAGS="$CFLAGS -fsanitize=address"
CXXFLAGS="$CXXFLAGS -fsanitize=address -Wno-maybe-uninitialized"
LDFLAGS="$LDFLAGS -fsanitize=address"
%endif
export CFLAGS;
export CXXFLAGS;
export LDFLAGS;

cmake . \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DENABLE_TRACE=ON \
      -DENABLE_BACKTRACE=ON \
%if 0%{?enable_low_spec_memory_management}
      -DENABLE_LOW_SPEC_MEMORY_MANAGEMENT=ON \
%endif
%if 0%{?enable_gpu_memory_profile}
      -DENABLE_GPU_MEMORY_PROFILE=ON \
%endif
%if 0%{?enable_tizen_namespace}
      -DENABLE_TIZEN_NAMESPACE=ON \
%endif
      -DDALI_LEGACY_ALIASES=%{?with_dali2_aliases:ON}%{!?with_dali2_aliases:OFF} \
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir}

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install

%if %{with dali2_include_copy}
# include/dali: a copy of the dali3 headers in place of the link (see
# dali2_include_copy above)
rm %{buildroot}%{_includedir}/dali
cp -a %{buildroot}%{_includedir}/dali3/dali %{buildroot}%{_includedir}/dali
%endif

##############################
# Scriptlets
##############################
%post
/sbin/ldconfig
exit 0

%postun
/sbin/ldconfig
exit 0

%if %{with dali2_aliases}
%post -n dali2
/sbin/ldconfig
exit 0

%postun -n dali2
/sbin/ldconfig
exit 0

%endif

##############################
# Files in Binary Packages
##############################
# The Lua scriptlet runs up to the next section, so nothing but %files may
# follow it. Every earlier package (DALi 2.x and dali3 3.0.0 alike) installed
# include/dali as a real directory; rpm cannot replace a directory with a
# link, so the alias package clears it before the transaction unpacks. With
# dali2_include_copy the package installs a directory again, and clears a
# link left by an alias package built without the copy.
%if %{with dali2_aliases}
%pretrans -n dali2-devel -p <lua>
local path = "%{_includedir}/dali"
%if %{with dali2_include_copy}
if posix.stat(path, "type") == "link" then
  os.remove(path)
end
%else
if posix.stat(path, "type") == "directory" then
  os.execute("rm -rf '" .. path .. "'")
end
%endif
%endif
%files
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali3-core.so*
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{_bindir}/dali3-invoke-method-generator
%{_libdir}/pkgconfig/dali3-core.pc
%{_includedir}/dali3/dali/public-api/*
%{_includedir}/dali3/dali/doc/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali3/dali/devel-api/*
%{_includedir}/dali3/dali/integration-api/*
%{_includedir}/dali3/dali/graphics-api/*

%if %{with dali2_aliases}
%files -n dali2
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-core.so.2
%license LICENSE

%files -n dali2-devel
%defattr(-,root,root,-)
%{_bindir}/dali-invoke-method-generator
%{_libdir}/libdali2-core.so
%{_libdir}/pkgconfig/dali2-core.pc
%if %{with dali2_include_copy}
%{_includedir}/dali/public-api/*
%{_includedir}/dali/doc/*
%else
%{_includedir}/dali
%endif

%files -n dali2-integration-devel
%defattr(-,root,root,-)
%if %{with dali2_include_copy}
%{_includedir}/dali/devel-api/*
%{_includedir}/dali/integration-api/*
%{_includedir}/dali/graphics-api/*
%endif
%endif
