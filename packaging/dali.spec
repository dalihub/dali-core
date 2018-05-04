Name:       dali
Summary:    DALi 3D Engine
Version:    1.3.23
Release:    1
Group:      System/Libraries
License:    Apache-2.0 and BSD-3-Clause and MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-core.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig
BuildRequires:  gawk

%if 0%{?tizen_version_major} >= 3
BuildRequires:  pkgconfig(libtzplatform-config)
%endif

%if 0%{?tizen_version_major} < 4
%define disable_cxx03_build 1
%endif

%description
DALi 3D Engine

%if !0%{?disable_cxx03_build}
##############################
# cxx03
##############################
%package cxx03
Summary:	DALi 3D Engine with cxx03 abi
Provides:	%{name}-cxx03 = %{version}-%{release}

%description cxx03
DALi 3D Engine with cxx03 abi with cxx03 abi

##############################
# devel-cxx03
##############################
%package devel-cxx03
Summary:    Development components for DALi 3D Engine with cxx03 abi
Group:      Development/Building
Requires:   %{name}-cxx03 = %{version}-%{release}
Requires:   %{name}-integration-devel-cxx03 = %{version}-%{release}

%description devel-cxx03
Development components for DALi 3D Engine - public headers and package config

##############################
# integration-devel-cxx03
##############################
%package integration-devel-cxx03
Summary:    Integration development package for DALi 3D Engine witch cxx03 abi
Group:      Development/Building
Requires:   %{name}-cxx03 = %{version}-%{release}

%description integration-devel-cxx03
Integration development package for DALi 3D Engine - headers for integrating with an adaptor/platform abstraction library.
%endif

##############################
# devel
##############################
%package devel
Summary:    Development components for DALi 3D Engine
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}

%description devel
Development components for DALi 3D Engine - public headers and package config

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for DALi 3D Engine
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description integration-devel
Integration development package for DALi 3D Engine - headers for integrating with an adaptor/platform abstraction library.

##############################
# Preparation
##############################
%prep
%setup -q

#Use TZ_PATH when tizen version is 3.x

%if "%{tizen_version_major}" == "2"
%define dali_data_rw_dir /opt/usr/share/dali/
%define dali_data_ro_dir /usr/share/dali/
%else
%define dali_data_rw_dir %TZ_SYS_SHARE/dali/
%define dali_data_ro_dir %TZ_SYS_RO_SHARE/dali/
%endif

%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -Wl,-Bsymbolic-functions "

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
%endif

%if 0%{?enable_coverage}
CXXFLAGS+=" --coverage "
LDFLAGS+=" --coverage "
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/tizen
autoreconf --install
DALI_DATA_RW_DIR="%{dali_data_rw_dir}"
DALI_DATA_RO_DIR="%{dali_data_ro_dir}"
export DALI_DATA_RW_DIR
export DALI_DATA_RO_DIR

# Default to GLES 2.0 if not specified.
%{!?target_gles_version: %define target_gles_version 20}

CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS;
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS;
LDFLAGS="${LDFLAGS:-%optflags}" ; export LDFLAGS;

./configure \
      --program-prefix=%{?_program_prefix} \
      --prefix=%{_prefix} \
      --exec-prefix=%{_exec_prefix} \
      --bindir=%{_bindir} \
      --sbindir=%{_sbindir} \
      --sysconfdir=%{_sysconfdir} \
      --datadir=%{_datadir} \
      --includedir=%{_includedir} \
      --libdir=%{_libdir} \
      --libexecdir=%{_libexecdir} \
      --localstatedir=%{_localstatedir} \
      --sharedstatedir=%{_sharedstatedir} \
      --mandir=%{_mandir} \
      --enable-gles=%{target_gles_version} \
%if 0%{?enable_debug}
      --enable-debug \
%endif
%if 0%{?enable_trace}
      --enable-trace \
%endif
      --infodir=%{_infodir} \
      --enable-rename-so=no

make %{?jobs:-j%jobs}

pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"
popd

pushd %{buildroot}%{_libdir}
for FILE in libdali-core-cxx11.so*; do mv "$FILE" "%{_builddir}/%{name}-%{version}/build/tizen/$FILE"; done
mv pkgconfig/dali-core.pc %{_builddir}/%{name}-%{version}/build/tizen/dali-core.pc
popd

%if !0%{?disable_cxx03_build}
make clean

./configure \
      --program-prefix=%{?_program_prefix} \
      --prefix=%{_prefix} \
      --exec-prefix=%{_exec_prefix} \
      --bindir=%{_bindir} \
      --sbindir=%{_sbindir} \
      --sysconfdir=%{_sysconfdir} \
      --datadir=%{_datadir} \
      --includedir=%{_includedir} \
      --libdir=%{_libdir} \
      --libexecdir=%{_libexecdir} \
      --localstatedir=%{_localstatedir} \
      --sharedstatedir=%{_sharedstatedir} \
      --mandir=%{_mandir} \
      --enable-cxx03-abi=yes  \
      --enable-gles=%{target_gles_version} \
%if 0%{?enable_debug}
      --enable-debug \
%endif
%if 0%{?enable_trace}
      --enable-trace \
%endif
      --infodir=%{_infodir} \
      --enable-rename-so=no

make %{?jobs:-j%jobs}
%endif

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen

pushd %{_builddir}/%{name}-%{version}/build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

for FILE in libdali-*.so*; do mv "$FILE" "%{buildroot}%{_libdir}/$FILE"; done
mv dali-core.pc %{buildroot}%{_libdir}/pkgconfig/dali-core.pc
popd

#############################
#rename
#############################
pushd  %{buildroot}%{_libdir}
rm -rf libdali-core.so
rm -rf libdali-core-cxx11.so
%if !0%{?disable_cxx03_build}
ln -s libdali-core.so.0.0.* libdali-core-cxx03.so
%endif
ln -s libdali-core-cxx11.so.0.0.* libdali-core.so
popd


##############################
# Post Install
##############################
%post
/sbin/ldconfig
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
exit 0

##############################
# Files in Binary Packages
##############################

%if !0%{?disable_cxx03_build}
%files cxx03
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali-core.so.*
%{_libdir}/libdali-core-cxx03.so
%defattr(-,app,app,-)
%license LICENSE
%endif

%files
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali-core-cxx11.so.*
%{_libdir}/libdali-core.so
%defattr(-,app,app,-)
%license LICENSE

%if !0%{?disable_cxx03_build}
%files devel-cxx03
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali-core-cxx03.pc
%{dev_include_path}/dali/public-api/*
%{dev_include_path}/dali/devel-api/*
%{dev_include_path}/dali/doc/*

%files integration-devel-cxx03
%defattr(-,root,root,-)
%{_includedir}/dali/integration-api/*
%endif

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali-core.pc
%{dev_include_path}/dali/public-api/*
%{dev_include_path}/dali/devel-api/*
%{dev_include_path}/dali/doc/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali/integration-api/*
