#pragma once

#include <ps5/kernel.h>

#include <cstdint>
#include <type_traits>
#include <string>

namespace KernelMemory
{
    template< typename T >
    [[nodiscard]] auto read_kernel( const std::uintptr_t address )
    {
        if constexpr( std::is_arithmetic_v< T > )
        {
            T buffer = {};

            kernel_copyout( address, &buffer, sizeof( buffer) );

            return buffer;
        }

        throw std::string("Can't read to non arithmetic type from kernel!");
    }

    template< typename T >
    void write_kernel( const std::uintptr_t address, const T& value )
    {
        kernel_copyin(  &value, address, sizeof( T ) );
    }

    [[nodiscard]] const inline auto get_kernel_text_base() noexcept
    {
        return reinterpret_cast< std::uintptr_t >( KERNEL_ADDRESS_TEXT_BASE );
    }

    [[nodiscard]] const inline auto get_kernel_data_base() noexcept
    {
        return reinterpret_cast< std::uintptr_t >( KERNEL_ADDRESS_DATA_BASE );
    }

    [[nodiscard]] const inline auto change_memory_protection( const int32_t pid, const std::uintptr_t address, const size_t size, const int64_t protection )
    {
        return kernel_mprotect( pid, address, size, protection ) == 0;
    }

    [[nodiscard]] const inline auto get_process( const int64_t pid )
    {
        return reinterpret_cast< std::uintptr_t >( kernel_get_proc( pid ) );
    }
}