Name:       dali
Summary:    The OpenGLES Canvas Core Library
Version:    1.1.1
Release:    1
Group:      System/Libraries
License:    Apache-2.0, BSD-2.0, MIT
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-core.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig
BuildRequires:  gawk

%description
The OpenGLES Canvas Core Library provides a 3D scene graph

##############################
# devel
##############################
%package devel
Summary:    Development components for the OpenGLES Canvas Core Library
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}

%description devel
Development components for the OpenGLES Canvas Core Library - public headers and package config

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for the OpenGLES Canvas
Group:      Development/Building
Requires:   %{name} = %{version}-%{release}

%description integration-devel
Integration development package for the OpenGLES Canvas - headers for integrating with an adaptor/platform abstraction library.

##############################
# Preparation
##############################
%prep
%setup -q
%define dali_data_rw_dir /opt/usr/share/dali/
%define dali_data_ro_dir /usr/share/dali/
%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -lpthread -Wl,-Bsymbolic-functions "

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
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
      --infodir=%{_infodir}

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/tizen
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

# LICENSE
mkdir -p %{buildroot}/usr/share/license
cp -af %{_builddir}/%{name}-%{version}/LICENSE %{buildroot}/usr/share/license/%{name}


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
%{_libdir}/lib%{name}-core.so*
%defattr(-,app,app,-)
%{_datadir}/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/*.pc
%{dev_include_path}/%{name}/public-api/*
%{dev_include_path}/%{name}/devel-api/*
%{dev_include_path}/%{name}/doc/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/integration-api/*
