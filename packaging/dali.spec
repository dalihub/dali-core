Name:       dali
Summary:    The OpenGLES Canvas Core Library
Version:    1.0.15
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://review.tizen.org/git/?p=platform/core/uifw/dali-core.git;a=summary
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:       boost-thread
BuildRequires:  pkgconfig
BuildRequires:  gawk
BuildRequires:  boost-devel
BuildRequires:  boost-thread

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
%define shader_bin_dir   %{dali_data_rw_dir}/core/shaderbin
%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -Os -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -lpthread"

%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

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
chown 5000:5000 %{shader_bin_dir}
rm -rf %{shader_bin_dir}/*
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
rm -rf %{shader_bin_dir}
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
%dir %{shader_bin_dir}
%{_datadir}/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/*.pc
%{dev_include_path}/%{name}/public-api/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/integration-api/*
