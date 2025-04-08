#pragma once

#include <ps5/kernel.h>
#include <unistd.h>

#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>

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

    [[nodiscard]] std::vector< uint8_t > read_block_kernel(const std::uintptr_t address, const size_t size )
    {
        if( !address )
            return {};

        std::vector< uint8_t > ret = {};   

        const auto make_u64_to_u8s = [&]( const uint64_t buffer )
        {
            ret.push_back( static_cast< uint8_t >( buffer >> 0 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 8 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 16 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 24 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 32 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 40 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 48 & 0xFF ) );
            ret.push_back( static_cast< uint8_t >( buffer >> 56 & 0xFF ) );
        };


        for( auto idx = 0; idx < size; idx += 8 )
        {
            const auto buffer = read_kernel< uint64_t >( address + idx * sizeof( uint8_t ) );

            make_u64_to_u8s( buffer );
        }

        return ret;
    }

    template< typename T >
    void write_kernel( const std::uintptr_t address, const T& value )
    {
        kernel_copyin(  &value, address, sizeof( T ) );
    }

    [[nodiscard]] const inline auto get_kernel_text_base() noexcept
    {
        return static_cast< std::uintptr_t >( KERNEL_ADDRESS_TEXT_BASE );
    }

    [[nodiscard]] const inline auto get_kernel_data_base() noexcept
    {
        return static_cast< std::uintptr_t >( KERNEL_ADDRESS_DATA_BASE );
    }

    [[nodiscard]] const inline auto change_memory_protection( const int32_t pid, const std::uintptr_t address, const size_t size, const int64_t protection )
    {
        return kernel_mprotect( pid, address, size, protection ) == 0;
    }

    [[nodiscard]] const inline auto get_process_by_pid( const int64_t pid )
    {
        return static_cast< std::uintptr_t >( kernel_get_proc( pid ) );
    }

    [[nodiscard]] const inline auto get_current_process()
    {
        return get_process_by_pid( getpid() );
    }

    [[nodiscard]] const inline auto read_string_from_memory( const std::uintptr_t addr )
    {
        std::string temp = {};
        uint32_t offset = {};
        char ch = read_kernel< char >( addr + ++offset );

        while( ch != 0x00 )
        {
            temp += ch;

            ch = read_kernel< char >( addr + ++offset );
        }
            
        return temp;
    }
}