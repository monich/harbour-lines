Name:       harbour-lines
Summary:    Lines game
Version:    1.0.2
Release:    1
Group:      Amusements/Games
License:    BSD
URL:        http://github.com/monich/harbour-lines
Source0:    %{name}-%{version}.tar.bz2

Requires:      sailfishsilica-qt5 >= 0.10.9
Requires:      qt5-qtsvg-plugin-imageformat-svg
BuildRequires: pkgconfig(glib-2.0) >= 2.32
BuildRequires: pkgconfig(sailfishapp) >= 1.0.2
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(mlite5)
BuildRequires: desktop-file-utils
BuildRequires: qt5-qttools-linguist

%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

%description
Lines game for Sailfish

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5 harbour-lines.pro
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%exclude %{_datadir}/%{name}/qml/meego

%changelog
* Sat Feb 21 2015 Slava Monich <slava.monich@jolla.com> 1.0.0
- Initial version
