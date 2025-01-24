---
author:
- Lily Tsuru
date: 'January 24, 2025'
title: eupak
---

# NAME

eupak - create, extract, list Europa packfiles

# SYNOPSIS

**eupak** *SUBCOMMAND* [*OPTIONS*...]

*ARCHIVE-VERSION* := { **europa-prerelease** | **starfighter** | **jedistarfighter** }

**eupak** **create** **-V|--archive-version** *ARCHIVE-VERSION* **-d|--directory** *DIRECTORY* [**--verbose**] [**-s|--sector-aligned**] *PACKFILE*

**eupak** **extract** **-d|--directory** *PACKFILE*

**eupak** **list** *PACKFILE*

# DESCRIPTION

Eupak is a tool which allows extracting and creating new Europa packfiles for the LEC Europa games.

# COMMAND SYNTAX

## CREATE

**-V** must be one of the following:

- **europa-prerelease** to make a version 3 package
- **starfighter** to make a version 4 package
- **jedistarfighter** to make a version 5 package

The default when it is unspecified is **starfighter**.

**-d** must be specified and is the directory to create the packfile from.

**-s** builds a sector-aligned package, and is only valid for **-V jedistarfighter**.

*PACKFILE* must be specified as a non-positional argument. It is the path of the output packfile.

## EXTRACT

**-d** is optional, and will default to the basename of the packfile name.

*PACKFILE* must be specified as a non-positional argument. It is the path of the packfile to extract.

## INFO

*PACKFILE* must be specified as a non-positional argument. It is the path of the packfile to list information and files from.