#pragma once

#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>

class tcp_server
{
public:
    tcp_server() = default;
    virtual ~tcp_server() {}

    virtual bool setup_server( const int32_t port ) = 0;

    virtual void send_msg_to_client( const std::string& msg ) = 0;

    virtual bool handle_command( std::string& command ) = 0;

    virtual void run() = 0;

protected:
    int32_t m_port;
    int32_t m_server_fd;
    int32_t m_client_fd;
    std::unique_ptr< sockaddr_in > m_server_addr;
    std::unique_ptr< sockaddr_in > m_client_addr;
};