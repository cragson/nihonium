/* Copyright (C) 2025 cragson */

#include <string>
#include <memory>
#include <print>
#include <format>

#include "ILibkernel.hpp"
#include "kernel_memory.hpp"
#include "tcp-server.hpp"



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

    std::println("[+] Kernel Text Base: 0x{:X}", kernel_txt);
    std::println("[+] Kernel Text Base: {}", kernel_txt);

    hexdump( kernel_txt, 32 );

    const auto kernel_data = KernelMemory::get_kernel_data_base();

    std::println("[+] Kernel Data Base: {:X}", kernel_data);

    hexdump( kernel_data, 32 );
}

void demo_current_process()
{
    const auto _current_process = KernelMemory::get_current_process();

    std::println("[+] Current process: {:X}", _current_process);

    std::string _process_name = KernelMemory::read_string_from_memory( _current_process + 0x59C );
    //_process_name.resize(32);

    //_process_name = KernelMemory::read_kernel< char[32] >( _current_process + 0x59C);
    //kernel_copyout( _current_process + 0x59C, _process_name.data(), 32);


    std::println("[-] Process name: {}", _process_name);

    const auto _process_vmspace = KernelMemory::read_kernel< std::intptr_t >( _current_process + 0x200 );

    std::println("[+] Process vmspace: {:X}", _process_vmspace);

    const auto _process_vmspace_num_entries = KernelMemory::read_kernel< uint32_t >( _process_vmspace + 0x1A8 );

    std::println("[+] vmspace num entries: {}", _process_vmspace_num_entries);


    auto _process = KernelMemory::read_kernel< std::intptr_t >( KERNEL_ADDRESS_ALLPROC );

    uint32_t counter_process = 0;

    do
    {
        kernel_copyout( _process + 0x59C, _process_name.data(), 32);

        const auto _process_pid = KernelMemory::read_kernel< int32_t >( _process + KERNEL_OFFSET_PROC_P_PID );

        const auto _process_path = KernelMemory::read_string_from_memory( _process + 0x5BC );

        std::println( "[{}]: pid: {} - name: {} [{}]", ++counter_process, _process_pid, _process_name, _process_path );

        _process_name.clear();
        _process_name.resize( 32 );

        _process = KernelMemory::read_kernel< std::intptr_t >( _process );

    } while ( _process );
    
}

void demo_tcp()
{
    const auto srv = std::make_unique< tcp_server >();

    std::print("[-] Trying to setup tcp server..");

    constexpr auto SERVER_PORT = 60002;

    if( srv->setup_server( SERVER_PORT ) )
        std::println("done!");
    else
    {
        std::println("failed!");

        return;
    }

    ILibkernel::send_kernel_notitifcation( std::format("nihonium\nserver listening now on: {}", SERVER_PORT ) );

    srv->run();
}


int main() 
{
   demo_kmem();

   demo_tcp();

   return 0;
}
