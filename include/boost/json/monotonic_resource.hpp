//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2020 Krystian Stasiowski (sdkrystian@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/json
//

#ifndef BOOST_JSON_MONOTONIC_RESOURCE_HPP
#define BOOST_JSON_MONOTONIC_RESOURCE_HPP

#include <boost/json/detail/config.hpp>
#include <boost/json/memory_resource.hpp>
#include <boost/json/detail/align.hpp>

BOOST_JSON_NS_BEGIN

/** A resource with a trivial deallocate.

    This memory resource is designed to...

    @par Thread Safety
    Members of the same instance may not be
    called concurrently.
*/
class monotonic_resource final
    : public memory_resource
{   
#ifdef BOOST_JSON_DOCS
    // VFALCO doc toolchain screws up on alignas
    struct block
#else
    struct alignas(alignof(detail::max_align_t)) block
#endif
    {
        void* p;
        std::size_t n;
        std::size_t size;
        block* next;
    };

    block buffer_;
    block* head_ = &buffer_;
    std::size_t next_size_ = 1024;

    static constexpr std::size_t min_size_ = 1024;
    inline static constexpr std::size_t max_size();
    inline static std::size_t round_pow2(
        std::size_t n) noexcept;
    inline static std::size_t next_pow2(
        std::size_t n) noexcept;

public:
    /// Copy constructor (deleted)
    monotonic_resource(
        monotonic_resource const&) = delete;

    /// Copy assignment (deleted)
    monotonic_resource& operator=(
        const monotonic_resource&) = delete;

    /** Destructor

        Deallocates all the memory owned by this resource.

        @par Effects
        @code
        this->release();
        @endcode

        @par Complexity

        Linear in the number of deallocations performed.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    ~monotonic_resource() noexcept;

    /** Constructor

        This constructs the resource and indicates
        that the first internal dynamic allocation
        shall be at least `initial_size` bytes.
    \n
        This constructor is guaranteed not to perform
        any dynamic allocations.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param initial_size The size of the first
        internal dynamic allocation. If this is lower
        than the implementation-defined lower limit, then
        the lower limit is used instead.
    */
    BOOST_JSON_DECL
    monotonic_resource(
        std::size_t initial_size = 1024) noexcept;

    /** Constructor

        This constructs the resource and indicates that
        subsequent allocations should use the specified
        caller-owned buffer. When this buffer is exhausted,
        dynamic allocations from the heap are made.
    \n
        This constructor is guaranteed not to perform
        any dynamic allocations.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

        @param buffer A pointer to valid memory of
        at least `size` bytes. Ownership is not
        transferred; the caller is responsible for
        ensuring that the lifetime of the buffer
        extends until the resource is destroyed.

        @param size The number of valid bytes
        pointed to by `buffer`.
    */
    BOOST_JSON_DECL
    monotonic_resource(
        void* buffer,
        std::size_t size) noexcept;

    /** Release all allocated memory.

        This function deallocates all allocated memory.
        If an initial buffer was provided upon construction,
        then all of the bytes will be available again for
        allocation. Allocated memory is deallocated even
        if @ref deallocate has not been called for some
        of the allocated blocks.

        @par Complexity
        Linear in the number of deallocations performed.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_JSON_DECL
    void
    release() noexcept;

protected:
    BOOST_JSON_DECL
    void*
    do_allocate(
        std::size_t n,
        std::size_t align) override;

    BOOST_JSON_DECL
    void
    do_deallocate(
        void* p,
        std::size_t n,
        std::size_t align) override;

    BOOST_JSON_DECL
    bool
    do_is_equal(
        memory_resource const& mr) const noexcept override;
};

template<>
struct is_deallocate_trivial<
    monotonic_resource>
{
    static constexpr bool value = true;
};

BOOST_JSON_NS_END

#ifdef BOOST_JSON_HEADER_ONLY
#include <boost/json/impl/monotonic_resource.ipp>
#endif

#endif
