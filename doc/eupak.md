---
author:
- Lily Tsuru
date: 'August 3, 2025'
title: eupak
---

# NAME

eupak - create, extract, list Europa packfiles

# SYNOPSIS

**eupak** *SUBCOMMAND* [*OPTIONS*...]

**eupak** **create** **-m|--manifest** *MANIFEST* *PACKFILE*

**eupak** **extract** **-d|--directory** *PACKFILE*

**eupak** **list** *PACKFILE*

# DESCRIPTION

Eupak is a tool which allows extracting and creating new Europa packfiles for the LEC Europa games.

# COMMAND SYNTAX

## CREATE

The **-m** option MUST be provided.

**MANIFEST** *MUST* be a path to a manifest file. The manifest file is JSON, and its syntax will be later documented.

**PACKFILE** must be specified as a non-positional argument. It is the path of the output packfile.

## EXTRACT

**-d** is optional, and will default to the basename of the packfile name.

*PACKFILE* must be specified as a non-positional argument. It is the path of the packfile to extract.

## LIST

*PACKFILE* must be specified as a non-positional argument. It is the path of the packfile to list information and files from.