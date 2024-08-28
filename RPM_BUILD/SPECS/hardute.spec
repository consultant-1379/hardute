# SPEC file for HWUtil

%define buildroot 	/home/pkg/broot
%define _topdir	/home/pkg/pkg1
%define _bindir	/hardute/bin


BuildRoot:		%{buildroot}
Summary: 		"hardware utilisation monitor"
Name: 			hardute
Release:		Linux
License:	        CXP9019868	
Vendor:		Ericsson (c) 2020
Version: 		R3A18
Source: 		hardute.tar
Group: 		Applications/System

%description
Collects utilisation and performance statistics

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
cp hardute %{buildroot}%{_bindir}/
cp hardute.sh %{buildroot}%{_bindir}/
cp hardute_d1.sh %{buildroot}%{_bindir}/
cp hardute_d2.sh %{buildroot}%{_bindir}/
cp expire.sh %{buildroot}%{_bindir}/

%clean
rm -rf %{buildroot}

%files
%defattr(0755,root,root,-)
%{_bindir}/*

%post
mkdir -m 777 -p /var/opt/ericsson/eniqpc/
mkdir -m 755 -p /hardute/data/
echo "%{name}-%{version} has been installed successfully"

%postun
rm -r  /hardute
echo "Uninstallation Complete"
