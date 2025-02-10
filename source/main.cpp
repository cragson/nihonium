/* Copyright (C) 2025 cragson */

#include <string>
#include <memory>
#include <print>
#include <format>

#include "ILibkernel.hpp"
#include "kernel_memory.hpp"

void demo_hwinfo()
{
    const auto hw_model = ILibkernel::get_hw_model_name();
    const auto hw_serial = ILibkernel::get_hw_serial_number();

    std::println( "[+] hw serial: {}", hw_serial.c_str() );

    const auto soc_temp = std::to_string( ILibkernel::get_soc_sensor_temperature() );
    const auto cpu_temp = std::to_string( ILibkernel::get_cpu_temperature() );

    const auto cpu_freq = std::to_string( ILibkernel::get_cpu_frequency() );

    ILibkernel::send_kernel_notitifcation( hw_model );
    ILibkernel::send_kernel_notitifcation( hw_serial );
    ILibkernel::send_kernel_notitifcation( {"SOC: " + soc_temp + "°C CPU: " + cpu_temp + "°C" + " CPU Freq: " + cpu_freq + " MHz"});
}

void demo_kmem()
{
    const auto hexdump = [](const std::uintptr_t addr, const size_t size )
    {
        auto buffer = uint64_t();

        for( auto idx = 0; idx < size; idx += sizeof( uint64_t ) )
        {
            buffer = KernelMemory::read_kernel< uint64_t >( addr + idx * sizeof( uint64_t ) );

            const auto b0 = static_cast< uint8_t >( buffer >> 0 & 0xFF );
            const auto b1 = static_cast< uint8_t >( buffer >> 8 & 0xFF );
            const auto b2 = static_cast< uint8_t >( buffer >> 16 & 0xFF );
            const auto b3 = static_cast< uint8_t >( buffer >> 24 & 0xFF );
            const auto b4 = static_cast< uint8_t >( buffer >> 32 & 0xFF );
            const auto b5 = static_cast< uint8_t >( buffer >> 40 & 0xFF );
            const auto b6 = static_cast< uint8_t >( buffer >> 48 & 0xFF );
            const auto b7 = static_cast< uint8_t >( buffer >> 56 & 0xFF );

            const auto dump = std::vformat( 
                "[{:X}] {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {}{}{}{}{}{}{}{}",
                std::make_format_args( addr + idx * sizeof( uint64_t ),
                b0,
                b1,
                b2,
                b3,
                b4, 
                b5,
                b6,
                b7,
                std::isprint( b0 ) ? static_cast< char >( b0 ) : '.',
                std::isprint( b1 ) ? static_cast< char >( b1 ) : '.',
                std::isprint( b2 ) ? static_cast< char >( b2 ) : '.',
                std::isprint( b3 ) ? static_cast< char >( b3 ) : '.',
                std::isprint( b4 ) ? static_cast< char >( b4 ) : '.',
                std::isprint( b5 ) ? static_cast< char >( b5 ) : '.',
                std::isprint( b6 ) ? static_cast< char >( b6 ) : '.',
                std::isprint( b7 ) ? static_cast< char >( b7 ) : '.'
                )
            );

            std::println("{}", dump );
        }

        std::println("");
    };

    const auto kernel_txt = KernelMemory::get_kernel_text_base();

    std::println("[+] Kernel Text Base: {:X}", kernel_txt);

    hexdump( kernel_txt, 0x80 );

    const auto kernel_data = KernelMemory::get_kernel_data_base();

    std::println("[+] Kernel Data Base: {:X}", kernel_data);

    hexdump( kernel_data, 0x80 );
}

int main() 
{
    demo_hwinfo();

    demo_kmem();
  
    return 0;
}
