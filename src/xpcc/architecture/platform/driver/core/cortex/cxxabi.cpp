// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/**
 * \file	cxxabi.cpp
 * \brief	Minimal C++ support, no exception handling, no RTTI
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>                   // for prototypes of malloc() and free()
#include <new>
#include <xpcc/architecture/interface/memory.hpp>
#include <xpcc/architecture/interface/assert.hpp>

extern "C"
{
	// ------------------------------------------------------------------------
	void* __dso_handle = (void *) &__dso_handle;
	
	void
	__cxa_pure_virtual()
	{
		xpcc_assert_debug(0, "core", "cxa", "purevirtual");
	}
	
	// ------------------------------------------------------------------------
	__extension__ typedef int __guard __attribute__((mode (__DI__)));
	
	int
	__cxa_guard_acquire(__guard *g)
	{
		return !*(char *)(g);
	}
	
	void
	__cxa_guard_release (__guard *g)
	{
		*(char *) g = 1;
	}
	
	void
	__cxa_guard_abort (__guard *)
	{
	}
	
	// ------------------------------------------------------------------------
	int
	__aeabi_atexit(void */*object*/, void (*/*destructor*/)(void *), void */*dso_handle*/)
	{
		return 0;
	}

	extern void * malloc_tr(size_t, uint32_t);
}

// ----------------------------------------------------------------------------
void *
operator new(size_t size) throw ()
{
	void * ptr = malloc(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size) throw ()
{
	void * ptr = malloc(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new(size_t size, std::nothrow_t) noexcept
{
	return malloc(size);
}

void *
operator new[](size_t size, std::nothrow_t) noexcept
{
	return malloc(size);
}

void *
operator new(size_t size, xpcc::MemoryTraits traits) noexcept
{
	void * ptr = malloc_tr(size, traits.value);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size, xpcc::MemoryTraits traits) noexcept
{
	void * ptr = malloc_tr(size, traits.value);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void
operator delete(void *ptr) noexcept
{
	free(ptr);
}

void
operator delete(void* ptr, size_t) noexcept
{
	free(ptr);
}

void
operator delete[](void* ptr) noexcept
{
	free(ptr);
}

void
operator delete[](void* ptr, size_t) noexcept
{
	free(ptr);
}
