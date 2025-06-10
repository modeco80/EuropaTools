//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

// Helpful marker macros for annotating how or how not
// an class/struct can be relocated.
//
// Relocation in C++ basically means one of two forms of
// relocation taking place (excluding RVO/etc, since those are Complicated):
//
// - Copying is the most obvious form of relocation, and also 
//	 the oldest (depending on who you ask). Typically this will
//	 perform a deep copy (which seems to be the intention). Some
//	 developers abuse this in wrong ways to do CoW.
//	 T(const T&) is called when an object is copied.
//
// - Move. First introduced in C++11, this allows an object which 
//	 supports movement to simply move to another memory location.
//	 This was done with the addition of rvalue references (&&),
//	 and new standard library vocabulary (`std::move`, which simply
//	 casts an existing rvalue reference to an rvalue reference.
//	 The compiler will call T(T&&) when an object is moved.
//
// The macros here are sugar over C++11 = default/= delete.
// Are they required? Not really, but having them is a bit nicer.

/// Forbids copy construction.
#define EUROPA_FORBID_COPY(T) \
	T(const T&) = delete;

/// Forbids relocation via move (std::move)
#define EUROPA_FORBID_MOVE(T) \
	T(T&&) = delete;

/// Allows relocation via move (via default. If you
/// need to do anything special, you will have to subdue this macro)
#define EUROPA_ALLOW_MOVE(T) \
	T(T&&) = default;

/// Forbids all forms of relocation.
#define EUROPA_FORBID_RELOCATION(T) \
	EUROPA_FORBID_COPY(T)           \
	EUROPA_FORBID_MOVE(T)