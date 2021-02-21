Name:       harbour-lines
Summary:    Lines game
Version:    1.0.9
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
Lines game for Sailfish OS

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
* Sun Feb 21 2021 Slava Monich <slava.monich@jolla.com> 1.0.9
- Chinese translation

* Tue Feb 16 2021 Slava Monich <slava.monich@jolla.com> 1.0.8
- Hungarian translation
- Minor UI tweaks

* Sun Feb 14 2021 Slava Monich <slava.monich@jolla.com> 1.0.7
- Added sounds
- Freshened up the UI
- Redesigned cover page
- Updated Russian translation
- Integration with harbour-backup

* Sun Mar 1 2020 Slava Monich <slava.monich@jolla.com> 1.0.6
- Swedish translation

* Sun Mar 1 2020 Slava Monich <slava.monich@jolla.com> 1.0.5
- Changed score animations (again)
- Update high scores when game is restarted

* Sat Feb 29 2020 Slava Monich <slava.monich@jolla.com> 1.0.4
- Added score table

* Sun Jan 13 2019 Slava Monich <slava.monich@jolla.com> 1.0.3
- Compatibility with light ambiences
- Added hi-res icons
- Dutch translation

* Sun Apr 3 2016 Slava Monich <slava.monich@jolla.com> 1.0.2
- Added option to use different shapes for different colors

* Sat Feb 28 2015 Slava Monich <slava.monich@jolla.com> 1.0.1
- Tweaked colors, particularly yellow/green and red/brown
- Stop jump animation when the screen is locked
- Added game options: next balls, ball move animation

* Sat Feb 21 2015 Slava Monich <slava.monich@jolla.com> 1.0.0
- Initial version
