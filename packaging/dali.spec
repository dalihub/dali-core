Name:       dali
Summary:    DALi 3D Engine
Version:    1.4.20
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

%description
DALi 3D Engine

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

%files
%if 0%{?enable_dali_smack_rules}
%manifest dali.manifest-smack
%else
%manifest dali.manifest
%endif
%defattr(-,root,root,-)
%{_libdir}/libdali-core-cxx11.so.*
%{_libdir}/libdali-core.so
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/dali-core.pc
%{dev_include_path}/dali/public-api/*
%{dev_include_path}/dali/devel-api/*
%{dev_include_path}/dali/doc/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/dali/integration-api/*
