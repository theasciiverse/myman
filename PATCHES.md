# MyMan: upstream source and collected patches

Assembled 2026-10-10 for the Asciiverse catalog. This file records where the
source came from, every downstream patch that could be found, and what was
done with each one.

## Upstream

- Project: <https://sourceforge.net/projects/myman/>, homepage
  <https://myman.sourceforge.io/>. Author: Benjamin C. Wiley Sittler
  (`bsittler` on SourceForge). License: MIT/Expat style, see `LICENSE`.
- Version string in the tree: `0.7.1` (`VERSION`). The last formal release is
  0.7.0 (Files section, 2008); later code was only published as CVS snapshot
  tarballs, the newest being `myman-cvs-2009-10-30`.
- **History in this repository** is the full SourceForge CVS repository,
  downloaded as <https://sourceforge.net/code-snapshots/cvs/m/my/myman.zip>
  (the link on <https://sourceforge.net/p/myman/code/>; SourceForge CVS is
  read-only) and converted with `cvs-fast-export` 1.59 (Ubuntu package).
  694 commits on `main` (702 including commits reachable only from tags); the 1998 sources
  were imported 2008-08-16, the last commit is 2017-11-04.
  CVS tags were kept as git tags (`myman-1998-06-28`, `myman-0-6`,
  `myman-0-7-0`, `myman-0-7-1wip2008-08-17`, `cvs-vendor-import-1.1.1`).
  CVS has no commit ids before 2017, so commits are grouped by author, log
  message and time; many upstream log messages are `*** empty log message ***`
  or `-`.
- **CVS HEAD is newer than the newest tarball.** About 220 commits landed after
  2009-10-30 (2010, 2015, 2016, 2017): ports to more compilers and platforms
  (`inc/rawcurs.h`, Turbo C, TCC on Windows/macOS, NaCl, CP/M-86), new mazes
  (`cpm86`, `bas256`, `msquik` and others), Makefile work
  (`--with-symlinks`, silent loopback make) and gameplay logic changes in
  `src/logic.c` (2017).
- Conversion checked against two independent references:
  - Every live file at HEAD matches an RCS `co` of the same `,v` file
    (keyword expansion aside: git has `$Id$`/`$Log$` expanded, as a
    `cvs checkout` would).
  - The tree at 2009-10-30 matches `myman-wip-2009-10-30.tar.gz` from the
    Files section, except `chr/chr1.txt` (one tile glyph; the tarball used the
    vendor-branch revision 1.1.1.2, the converter used trunk 1.1 for that
    interval). The file was replaced by revision 1.2 in 2015, so HEAD is not
    affected.
- `.cvsignore` became `.gitignore` during conversion (cvs-fast-export
  prepends the CVS default ignore list).
- The imported commits are unchanged; the commits after them were added on
  2026-10-10 and are listed below.

## Patches applied

| # | Source | What it fixes | Status |
|---|--------|---------------|--------|
| 1 | Termux `packages/myman/Makefile.patch`, Henrik Grimler, 2020 | Link lines ignore `LDFLAGS` when `LD` is unset (the default) | **Applied in part**, see below |
| 2 | Fedora `myman-0.7.0-makefile_clean.patch`, Jiri Vymazal, 2016/2017 | Versioned install dirs, `update-whatis` writing into `DESTDIR` | **Applied in part (ported)**, see below |

### 1. Termux: `LDFLAGS` on the link line

- URL: <https://github.com/termux/termux-packages/blob/master/packages/myman/Makefile.patch>,
  added in commit
  <https://github.com/termux/termux-packages/commit/9fe700504a6d5a382bb071cacb7fef23539af769>
  ("myman: fix build", 2020-05-09).
- License: termux-packages states that package patches carry the license of
  the package they patch (here MIT).
- Written against the 2009-10-30 CVS tree; five of six hunks still apply to
  HEAD with an offset.
- What it does, hunk by hunk:
  1. `HOSTCC` default `gcc` -> `clang`. Android toolchain specific. **Not
     applied.**
  2. and 3. Add `$(LDFLAGS)` to `LINK` and `HOSTLINK` in the branch used when
     `LD`/`HOSTLD` are unset. Upstream drops `LDFLAGS` entirely in that case,
     so packager or hardening flags never reach the linker. **Applied** (only
     these lines). The same hunks also rewrite the explicit-`LD` branch to
     call `${CC}` with `LDFLAGS` twice; that is specific to the Termux
     toolchain and **not applied**.
  4. to 6. Remove the compiler sanity check (`compiler.ok`, `_sanity0/1`),
     which runs freshly built test programs and so fails when cross
     compiling. Hunk 5 no longer applies (upstream changed this rule to run
     through `${RUN}`, which can name an emulator). **Not applied**; the
     Windows build cross-compiles with MinGW (see "Packaging" below) and
     did not need it.
- Note: in this Makefile `CC`/`LINK` build the build-machine tools and
  `HOSTCC`/`HOSTLINK` build the game itself, so it is the `HOSTLINK` line that
  makes `LDFLAGS=... ./configure` reach the `myman` binary.

### 2. Fedora: install paths and `update-whatis`

- URL: <https://src.fedoraproject.org/rpms/myman/blob/f37/f/myman-0.7.0-makefile_clean.patch>
  (spec: `patches/collected/fedora/myman.spec`). Package created 2017-03-02
  by Jiri Vymazal, orphaned and retired 2023-01-09 (last branch f37).
- License: Fedora dist-git content without an explicit license is MIT under
  the Fedora Project Contributor Agreement; the patched code is MIT.
- Written against the 0.7.0 release Makefile. Only 1 of 13 hunks applies to
  HEAD (upstream later rewrote the Makefile from `$(VAR)` to `${VAR}` and
  moved most of it), so the useful parts were **ported by hand**:
  - `privatedatadir` and `privatedocdir` use `${MYMAN}` instead of
    `${DIST}` (`myman-0.7.1`), giving `share/myman` and `share/doc/myman`
    rather than versioned directories. **Ported.** This also changes the
    compiled-in `PRIVATEDATADIR` search path.
  - Remove the `update-whatis` target and the `MANDB`/`MAKEWHATIS`
    variables, so `make install`/`uninstall` no longer run `mandb` against
    `${DESTDIR}${mandir}` (the AUR and AOSC recipes delete the stray
    `index.db` this leaves behind). **Ported.**
  - Remove the plain-text and DVI manual formats (`myman.txt`, `myman.dvi`).
    **Not ported**: it removes documentation, and upstream already ignores
    failures from these rules when `groff`/`col` lack a device.

## Our own changes

Added 2026-10-18, after the imported commits and the two patches above.

- **Snapshots in the data folder.** `T` saved `snap####.html` and
  `snap####.txt` in the current directory, which for a game started by
  av is its install folder, replaced on update. They now go to `myman/`
  under `$XDG_DATA_HOME`, else `~/.local/share`, else `%APPDATA%` on
  Windows, made as needed (`snapshot_dir()` in `src/myman.c`); with none
  of those set, or on a system without `mkdir`, the current directory as
  before. The key help (`src/utils.c`) and the man page say where. MyMan
  writes nothing else: high scores are kept only for the session, and
  `~/.mymanrc` is read, never written.
- **The intro no longer leaves text behind.** With the small tiles the
  maze is 56 columns wide, and the ghosts' names and nicknames on the
  attract screen (CHASER, "SCARLET") run past its right
  edge, where redrawing the maze never erases them, so their ends stayed
  on screen for the demo and the game. The screen is now cleared once
  when the intro ends (`src/utils.c`).

## Packaging (new files)

- `asciiverse/build.sh` runs upstream's own `configure` and Makefile and
  copies the binary to `out/`; the environment picks the target (see its
  header). Make runs with `-j2` by default (`JOBS`).
- `asciiverse/package.sh` packs `asciiverse/archive/myman.txt` (the
  binary, `myman.6`, `LICENSE`, `README`) into `dist/myman-<target>.tar.gz`,
  `.zip` on Windows; the same script as the NetBSD games'.
- `.github/workflows/release.yml`: on a tag, static Linux in Alpine
  (x86_64, aarch64; checked locally for x86_64), macOS (x86_64, arm64;
  not run yet: links the system ncurses and builds with GNU sed from
  Homebrew, as Homebrew's own formula does) and Windows x86_64, plus
  SHA256SUMS. Windows is cross-built with MinGW on upstream's Win32
  console driver (`--with-raw --without-win-icon`), so it needs no
  curses library; `-lwinpthread` supplies `nanosleep`, and `-static`
  leaves only system DLLs (`KERNEL32`, `msvcrt`, `USER32`). Checked
  under Wine: it draws the maze and plays.
- `asciiverse/record/`: `rec.py` (from the NetBSD games', taught the
  scroll sequences `CSI S` and `CSI T` that MyMan scrolls the maze with)
  and `myman.py`, a small bot that plays the first maze for the preview,
  `asciiverse/casts/myman.cast` (100x30, 44 s).
- `.gitignore`: build output, and exceptions so the preview and the
  archive list (whose `.cast` and `.txt` names the CVS-era rules ignore)
  are tracked.

## Patches collected but not applied

### github.com/michaelopdenacker/myman (8 commits, 2025)

- File: `patches/collected/github-michaelopdenacker/kragen-myman-master...michaelopdenacker-myman-master.patch`
  (mbox of the 8 commits, from
  <https://github.com/kragen/myman/compare/master...michaelopdenacker:myman:master.patch>).
- Author: Michael Opdenacker <michael.opdenacker@rootcommit.com>,
  2025-03-28 to 2025-04-17, with `Signed-off-by`. No license file in the
  fork; the code it modifies is MIT.
- Base: `kragen/myman` (Kragen Sitaker's 2009-05-24 git clone of the CVS
  repository, itself unchanged since 2009; its other 7 forks have no commits
  of their own).
- What it does: replaces upstream's hand-written `configure` and 5400-line
  `Makefile` with a small autotools build, deletes `src/main.c` and
  `src/mycurses.c`, and adds a `chdir()` to the installed data directory in
  `src/myman.c` so a non-monolithic build finds its maze and tile files.
- **Not applied**: it removes the upstream build system and builds from a
  2009 base. The data-directory problem it solves does not arise with
  upstream's default monolithic build, which compiles all mazes, tiles and
  sprites into the binary. Kept for reference.

### Upstream's own `patches/` directory

`patches/bitmap.diff` and `patches/bitmap_libcaca.diff` (April 2009) are part
of upstream CVS: experimental double-height "bitmap" rendering for
`src/myman.c` and the libcaca driver. They were never merged upstream and are
left as upstream had them.

## Packagers checked that carry no patches

Build recipes kept under `patches/collected/recipes-without-patches/` for
reference (configure flags, install fixups):

| Source | URL | Version packaged | Notes |
|--------|-----|------------------|-------|
| Arch AUR | <https://aur.archlinux.org/packages/myman> | 0.7.0 | `--with-xterm --with-rxvt --with-kterm`; deletes `man/cat6`, `index.db` |
| Homebrew | <https://github.com/Homebrew/homebrew-core/blob/HEAD/Formula/m/myman.rb> | wip 2009-10-30 | No patch in its history either; needs GNU sed/coreutils on macOS, util-linux `col` on Linux |
| nixpkgs | <https://github.com/NixOS/nixpkgs/blob/master/pkgs/by-name/my/myman/package.nix> | 0.7.0 | Added 2026-02-13 |
| openSUSE (games, Factory) | <https://api.opensuse.org/public/source/openSUSE:Factory/myman> | wip 2009-10-30 | `--with-ncursesw`; sets `date` from ChangeLog for reproducible builds |
| AOSC OS | <https://github.com/AOSC-Dev/aosc-os-abbs/tree/stable/app-games/myman> | 0.7.0 | Same as AUR |
| SlackBuilds.org | <https://slackbuilds.org/repository/15.0/games/myman/> | 0.7.0 | `--with-xterm --with-sdl-mixer` |
| T2 SDE | <https://svn.exactcode.de/t2/trunk/package/games/myman/> | 0.7.0 | Metadata only |
| Termux | `patches/collected/termux/build.sh` | CVS 2009-10-30 | Checks out the CVS module from the repository tarball; the patch is #1 above |

## Searched, MyMan not packaged

Debian and Ubuntu (no source package, nothing in Launchpad), Fedora RPM
Fusion, FreeBSD ports, NetBSD pkgsrc (main tree and wip), OpenBSD ports,
MacPorts, Gentoo (main tree; no overlay hit on gpo.zugaina.org), Alpine
aports, Void, MSYS2, Fink's current distribution (upstream ships its own
`fink/myman.info`). GitHub repository search: apart from `kragen/myman` and
its forks, no MyMan mirrors or forks with fixes. SourceForge trackers: one
feature request ("Hi-scores"), no bug reports or patches.

Also found, not a source patch: an AmigaOS 4 binary port of 0.6 (2007,
"Spot / PFP") on OS4Depot and Aminet.

## Ours

- **How to play it, at the top of README.** A short note on installing and
  starting it with av, or from a release archive; the original text follows
  unchanged.
