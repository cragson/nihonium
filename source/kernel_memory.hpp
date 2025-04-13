#pragma once

#include <ps5/kernel.h>
#include <unistd.h>

#include <cstdint>
#include <type_traits>
#include <string>
#include <vector>
#include <print>

#include "ps5_structs.hpp"

#include <sys/types.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/sysctl.h>

class kernel_memory
{
public:
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

    [[nodiscard]] std::vector< uint8_t > read_block_kernel( const std::uintptr_t address, const size_t size )
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
            const auto buffer = this->read_kernel< uint64_t >( address + idx * sizeof( uint8_t ) );

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

    [[nodiscard]] const auto change_memory_protection( const int32_t pid, const std::uintptr_t address, const size_t size, const int64_t protection )
    {
        return kernel_mprotect( pid, address, size, protection ) == 0;
    }

    [[nodiscard]] const auto get_process_by_pid( const int64_t pid )
    {
        return static_cast< std::uintptr_t >( kernel_get_proc( pid ) );
    }

    [[nodiscard]] const auto get_current_process()
    {
        return get_process_by_pid( getpid() );
    }

    [[nodiscard]] const auto read_string_from_memory( const std::uintptr_t addr )
    {
        std::string temp = {};
        uint32_t offset = {};
        char ch = this->read_kernel< char >( addr + ++offset );

        while( ch != 0x00 )
        {
            temp += ch;

            ch = this->read_kernel< char >( addr + ++offset );
        }
            
        return temp;
    }

    [[nodiscard]] auto get_process_list()
    {
        // reference: https://github.com/ps5-payload-dev/sdk/blob/17b1d592d4aa095f6cc52e33c8a26fd6f898d65c/samples/ps/main.c#L49
        // I just rewrote it in C++ and adapted it for myself

        std::vector< std::unique_ptr< orbisview_process > > ret = {};

        int mib[ 4 ] = { CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0 };
        size_t buf_size;

        // determine size of query response
        if( sysctl( mib, 4, NULL, &buf_size, NULL, 0 ) ) 
        {
            perror( "sysctl" );
            return ret;
        }

        //auto buf = std::make_unique< uint8_t[] >( buf_size );
        auto buf = std::vector< uint8_t >();
        buf.resize( buf_size );

        // query the kernel for proc info
        if( sysctl( mib, 4, buf.data(), &buf_size, NULL, 0 ) ) 
        {
            perror( "sysctl" );
            return ret;
        }

        for( auto idx = 0; idx < buf.size(); )
        {
            const auto ki = reinterpret_cast< kinfo_proc* >( &buf[ idx ] );

            if( !ki->ki_structsize )
                break;

            const auto name = std::string( ki->ki_comm );

            const auto state = PROC_STATES.at( ki->ki_stat );

            const auto proc_ptr = reinterpret_cast< std::uintptr_t >( ki->ki_paddr );

            const auto vmspace_ptr = reinterpret_cast< std::uintptr_t >( ki->ki_vmspace );

            auto _process = std::make_unique< orbisview_process >( name, ki->ki_pid, state, proc_ptr, vmspace_ptr, ki );

            ret.push_back( std::move( _process ) );
            
            idx += ki->ki_structsize;
        }    
        return ret;
    }
};