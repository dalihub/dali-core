%define MODULE_NAME dali-unmanaged
%define MODULE_LIBNAME dali-unmanaged
Name:       core-%{MODULE_NAME}-tests
Summary:    Core API unit TC (%{name})
Version:    0.1
Release:    0
Group:      Development/Tools
License:    Apache License, Version 2.0, Samsung Properietary
Source0:    %{name}-%{version}.tar.gz
Requires: dali
BuildRequires:  dali-devel
BuildRequires:  dali-integration-devel
BuildRequires:  pkgconfig(dali-core)
BuildRequires:  pkgconfig(dali)
BuildRequires:  libxml2-devel
BuildRequires:  cmake

%description
Core API unit TC (%{name})

%prep
%setup -q

%build

%define PREFIX "%{_libdir}/%{name}"

export LDFLAGS+="-Wl,--rpath=%{PREFIX} -Wl,--as-needed"
cd automated-tests
cmake . -DMODULE="%{MODULE_NAME}" -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd automated-tests
%make_install
mkdir -p %{buildroot}/opt/usr/share/license
cp %{_builddir}/%{name}-%{version}/LICENSE %{buildroot}/opt/usr/share/license/%{name}
mkdir -p %{buildroot}/tmp/
cp %{_builddir}/%{name}-%{version}/automated-tests/scripts/add_all_smack_rule.sh %{buildroot}/tmp/
cp %{_builddir}/%{name}-%{version}/automated-tests/scripts/all_smack.rule %{buildroot}/tmp/

%post

%postun


%files
/opt/usr/bin/*
/opt/usr/share/license/%{name}
/tmp/add_all_smack_rule.sh
/tmp/all_smack.rule
