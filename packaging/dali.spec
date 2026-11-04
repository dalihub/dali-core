# dali2 is the same code built a second time under the dali2 name, for
# applications that still use it. To drop it, remove the "with dali2" blocks.
%bcond_without dali2

Name:       dali3
Summary:    DALi 3D Engine
Version:    2.5.46
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

%if %{with dali2}
##############################
# dali2
##############################
%package -n dali2
Summary:    DALi 3D Engine
Group:      System/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n dali2
DALi 3D Engine - the same code as %{name}, under the dali2 name

%package -n dali2-devel
Summary:    Development components for DALi 3D Engine
Group:      Development/Building
Requires:   dali2 = %{version}-%{release}

%description -n dali2-devel
Development components for DALi 3D Engine - public headers and package config

%package -n dali2-integration-devel
Summary:    Integration development package for DALi 3D Engine
Group:      Development/Building
Requires:   dali2 = %{version}-%{release}
Requires:   dali2-devel = %{version}-%{release}

%description -n dali2-integration-devel
Integration development package for DALi 3D Engine - headers for integrating with an adaptor/platform abstraction library.
%endif

##############################
# Preparation
##############################
%prep
%setup -q

%define dev_include_path %{_includedir}
%if %{with dali2}
%define dali2_build_dir %{_builddir}/%{name}-%{version}/_build/dali2
%endif

##############################
# Build
##############################
%build
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

cmake \
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
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir}

make %{?jobs:-j%jobs}

%if %{with dali2}
# Keep the options the same as dali3 above; only DALI_API_MAJOR differs.
# -S and -B are required: a plain source path would reuse the dali3 in-source cache.
cmake -S . -B %{dali2_build_dir} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DDALI_API_MAJOR=2 \
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
      -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
      -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir}

make -C %{dali2_build_dir} %{?jobs:-j%jobs}
%endif

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

_saved_dir=$(pwd)
cd %{_builddir}/%{name}-%{version}/build/tizen
%make_install
cd "$_saved_dir"

%if %{with dali2}
cd %{dali2_build_dir}
%make_install
%endif

##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

%if %{with dali2}
%post -n dali2
/sbin/ldconfig
exit 0
%endif

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

%if %{with dali2}
%postun -n dali2
/sbin/ldconfig
exit 0
%endif

##############################
# Files in Binary Packages
##############################

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
%{_bindir}/dali-invoke-method-generator
%{_libdir}/pkgconfig/dali3-core.pc
%{dev_include_path}/dali/public-api/*
%{dev_include_path}/dali/doc/*

%files integration-devel
%defattr(-,root,root,-)
%{dev_include_path}/dali/devel-api/*
%{_includedir}/dali/integration-api/*
%{_includedir}/dali/graphics-api/*

%if %{with dali2}
# Both builds install the same headers and tools, so dali2 shares them with dali3.
%files -n dali2
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali2-core.so*
%license LICENSE

%files -n dali2-devel
%defattr(-,root,root,-)
%{_bindir}/dali-invoke-method-generator
%{_libdir}/pkgconfig/dali2-core.pc
%{dev_include_path}/dali/public-api/*
%{dev_include_path}/dali/doc/*

%files -n dali2-integration-devel
%defattr(-,root,root,-)
%{dev_include_path}/dali/devel-api/*
%{_includedir}/dali/integration-api/*
%{_includedir}/dali/graphics-api/*
%endif
