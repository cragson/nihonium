#pragma once

#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <print>
#include <vector>
#include <netinet/tcp.h>
#include <ranges>

#include "string-utils.hpp"

constexpr auto NIHONIUM_TCP_SERVER_MSG_SIZE = 1024;
constexpr auto NIHONIUM_BLOCK_READ_SIZE = 512;

class tcp_server
{
public:

    tcp_server()
    {
        this->m_port = {};
        this->m_server_fd = {};

        this->m_server_addr = std::make_unique< sockaddr_in >();
        this->m_client_addr = std::make_unique< sockaddr_in >();
    }

    ~tcp_server()
    {
        close( this->m_client_fd );

        close( this->m_server_fd );  
    }


    [[nodiscard]] inline bool setup_server( const int32_t port )
    {
        if( port <= 0 )
            return false;

        auto srv_fd = socket(AF_INET, SOCK_STREAM, 0 );

        if( srv_fd == -1 )
        {
            std::println("[!] Failed to create socket, while setting up TCP server.");
            return false;
        }
            

        int32_t opt = 1;
        if( setsockopt( srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) ) == -1 )
        {

            std::println("[!] Failed to prepare socket for quick reconnection, while setting up TCP server.");

            close( srv_fd );

            return false;
        }
        
        auto sock_addr = std::make_unique< sockaddr_in >();
        sock_addr->sin_addr.s_addr = INADDR_ANY;
        sock_addr->sin_family = AF_INET;
        sock_addr->sin_port = htons( port );

        if( bind( srv_fd, reinterpret_cast< sockaddr* >( sock_addr.get() ), sizeof( sockaddr_in ) ) == -1 ) 
        {
            std::println("[!] Failed to bind socket, while setting up TCP server.");

            close( srv_fd );

            return false;
        }

        if( listen( srv_fd, 5 ) == -1 )
        {

            std::println("[!] Failed to listen to socket, while setting up TCP server.");

            close( srv_fd );

            return false;
        }

        this->m_server_fd = srv_fd;
        this->m_port = port;
        this->m_server_addr = std::move( sock_addr );

        return true;
    }

    inline void send_msg_to_client( const std::string& msg )
    {   
        // I always send first the length of the incoming message to the client
        // The message containing the length of the incoming message is always 8 byte long!

        const auto msg_length = msg.size();

        send( this->m_client_fd, &msg_length, sizeof( msg_length ), 0 );

        // after sending now the length of the next message, I can send the message
        send( this->m_client_fd, msg.c_str(), msg_length, 0 );

        //write( this->m_client_fd, msg.c_str(), msg.size() );
    }

    [[nodiscard]] inline bool handle_command( std::string& command )
    {
        StringUtils::removeSpaces( command );

        const auto cmd_data = StringUtils::split_string( command );

        const auto& cmd = cmd_data.front();

        if( cmd == "quit" || cmd == "exit" ) 
        {
            std::println( "[!] Received 'quit' command. Terminating server." );
            
            return false;
        } 
        else if( cmd == "print" ) 
        {
            for( const auto& elem : cmd_data )
                std::print("{} ", elem );

            std::println("");

            return true;
        }
        else if( cmd == "kread8")
        {
            if( cmd_data.size() != 2 )
            {
                std::println("[!] Invalid kread8 command with arg size: {}", cmd_data.size());;

                return true;
            }

            const auto addr = StringUtils::str_to_ul( cmd_data.back() );

            if( addr <= 0 )
            {
                std::println("[kread8] Invalid address");
                
                this->send_msg_to_client("NIHONIUM-INVALID-ADDRESS");

                return true;
            }

            const auto client_ret = KernelMemory::read_kernel< uint8_t >( addr );

            std::println( "[kread8] Read 1 byte from: {:X} => {:X}", addr, client_ret );

            this->send_msg_to_client( std::format( "{}", client_ret ) );

            return true;
        }
        else if( cmd == "kread16")
        {
            if( cmd_data.size() != 2 )
            {
                std::println("[!] Invalid kread16 command with arg size: {}", cmd_data.size());;

                return true;
            }

            const auto addr = StringUtils::str_to_ul( cmd_data.back() );

            if( addr <= 0 )
            {
                std::println("[kread16] Invalid address");
                
                this->send_msg_to_client("NIHONIUM-INVALID-ADDRESS");

                return true;
            }

            const auto client_ret = KernelMemory::read_kernel< uint16_t >( addr );

            std::println( "[kread16] Read 2 bytes from: {:X} => {:X}", addr, client_ret );

            this->send_msg_to_client( std::format( "{}", client_ret ) );

            return true;
        }
        else if( cmd == "kread32")
        {
            if( cmd_data.size() != 2 )
            {
                std::println("[!] Invalid kread32 command with arg size: {}", cmd_data.size());;

                return true;
            }

            const auto addr = StringUtils::str_to_ul( cmd_data.back() );

            if( addr <= 0 )
            {
                std::println("[kread32] Invalid address");
                
                this->send_msg_to_client("NIHONIUM-INVALID-ADDRESS");

                return true;
            }

            const auto client_ret = KernelMemory::read_kernel< uint32_t >( addr );

            std::println( "[kread32] Read 4 bytes from: {:X} => {:X}", addr, client_ret );

            this->send_msg_to_client( std::format( "{}", client_ret ) );

            return true;
        }
        else if( cmd == "kread64")
        {
            if( cmd_data.size() != 2 )
            {
                std::println("[!] Invalid kread64 command with arg size: {}", cmd_data.size());;

                return true;
            }

            const auto addr = StringUtils::str_to_ul( cmd_data.back() );

            if( addr <= 0 )
            {
                std::println("[kread64] Invalid address");
                
                this->send_msg_to_client("NIHONIUM-INVALID-ADDRESS");

                return true;
            }

            const auto client_ret = KernelMemory::read_kernel< std::uintptr_t >( addr );

            std::println( "[kread64] Read 8 bytes from: {:X} => {:X}", addr, client_ret );

            this->send_msg_to_client( std::format( "{}", client_ret ) );

            return true;
        }
        else if( cmd == "kreadblock")
        {
            if( cmd_data.size() != 2 )
            {
                std::println( "[!] Invalid kreadblock command with arg size: {}", cmd_data.size() );

                return true;
            }

            const auto addr = StringUtils::str_to_ul( cmd_data.back() );

            if( addr <= 0 )
            {
                std::println( "[kreadblock] Invalid address" );
                
                this->send_msg_to_client("NIHONIUM-INVALID-ADDRESS");

                return true;
            }

            const auto bytes = KernelMemory::read_block_kernel( addr, NIHONIUM_BLOCK_READ_SIZE );

            std::println( "[kreadblock] Read {} bytes from: {:X}", bytes.size(), addr );

            if( bytes.size() != NIHONIUM_BLOCK_READ_SIZE )
                std::println( "[!] Failed to read the full block of {} bytes", NIHONIUM_BLOCK_READ_SIZE );

            auto client_msg = std::string();

            // convert byte stream to formatted hex string, fucking love modern cpp features    
            std::ranges::for_each( bytes, [&]( const auto& elem ){ client_msg += std::format( "{:02X} ", elem ); } );
            
            // remove last space
            client_msg.pop_back();

            this->send_msg_to_client( client_msg );

            return true;
        }
        else 
        {
            std::println( "Unknown command: {}", command );
            return true;
        }
    }

    inline void run()
    {
        if( this->m_port <= 0 || !this->m_server_fd || !this->m_server_addr )
        {
            std::println("[!] TCP server was not setup correctly, can not run.");

            return;
        }

        std::println( "[+] TCP server is running on port: {}", this->m_port );

        bool running = true;

        while( running ) 
        {
            auto client_address = std::make_unique< sockaddr_in >();
            socklen_t client_addr_len = sizeof( client_address );

            int client_fd = accept( this->m_server_fd, reinterpret_cast< sockaddr* >( client_address.get() ), &client_addr_len );

            if( client_fd == -1 ) 
            {
                std::println("[!] Client accept failed!");
                
                // Continue accepting new connections
                continue; 
            }
            
            this->m_client_fd = client_fd;

            const auto acpt_client_ip = inet_ntoa( client_address->sin_addr );
            std::println( "[-] Accepted client: {}", acpt_client_ip );
            ILibkernel::send_kernel_notitifcation( std::format( "nihonium:\naccepted client: {}", acpt_client_ip ) );  
            
            int32_t delay_flag = 1;

            if( setsockopt( client_fd, IPPROTO_TCP, TCP_NODELAY, &delay_flag, sizeof( delay_flag ) ) == -1 )
            {
                std::println("[!] Failed to remove the delay of the client connection");

                continue;
            }

            // Process multiple commands from the same client connection.
            bool client_connected = true;
            while( client_connected ) 
            {
                auto buffer = std::vector< char >();
                buffer.resize( NIHONIUM_TCP_SERVER_MSG_SIZE );

                ssize_t bytes_received = read( client_fd, buffer.data(), buffer.size() );
                if( bytes_received <= 0 ) 
                {
                    std::println("[!] Client disconnected or error reading from client." );
                    
                    this->m_client_fd = -1;

                    // Exit loop if client disconnects or error occurs
                    break; 
                }

                auto recv_cmd = std::string( buffer.data(), bytes_received + 1 );

                std::println( "[-] Received cmd: {} ({} bytes)", recv_cmd, bytes_received );

                bool should_continue = handle_command( recv_cmd );

                if( !should_continue ) 
                {
                    // If "quit" was received, notify the client and exit loops.
                    //const std::string quitResponse = "NIHONIUM-TERMINATED";

                    //write( client_fd, quitResponse.c_str(), quitResponse.size() );

                    running = false;
                    client_connected = false;
                    this->m_client_fd = -1;

                    break;
                } 
            }

            this->m_client_fd = -1;

            // Close the connection with the client
            close( client_fd );
        }

        std::println( "[-] Stopping and terminating TCP server, goodbye! :-)" );

        ILibkernel::send_kernel_notitifcation( "nihonium:\nserver says goodbye!" );

        return;
    }

private:
    int32_t m_port;
    int32_t m_server_fd;
    int32_t m_client_fd;
    std::unique_ptr< sockaddr_in > m_server_addr;
    std::unique_ptr< sockaddr_in > m_client_addr;
};