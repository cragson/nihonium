#pragma once

#include <string>
#include <string.h> 
#include <memory>

class notify_request 
{
public:
    notify_request( const std::string& msg )
    {
        bzero( this->useless1, sizeof( useless1 ) );
        bzero( this->message, sizeof( this->message ) );

        strncpy( this->message, msg.c_str(), msg.size() );
    }

    char useless1[45];
    char message[3075];
};

extern "C" int sceKernelSendNotificationRequest(int, notify_request*, size_t, int);
extern "C" int  sceKernelGetHwModelName(char *);
extern "C" int  sceKernelGetHwSerialNumber(char *);
extern "C" long sceKernelGetCpuFrequency(void);
extern "C" int  sceKernelGetCpuTemperature(int *);
extern "C" int  sceKernelGetSocSensorTemperature(int, int *);

class ILibkernel
{
public:
    static int send_kernel_notitifcation( const std::string& message )
    {
        auto req = std::make_unique< notify_request >( message );

        return sceKernelSendNotificationRequest( 0, req.get(), sizeof( notify_request ), 0 );
    }

    static auto get_hw_model_name()
    {
        std::string temp = {};
        temp.resize( 128 );

        sceKernelGetHwModelName( &temp[0] );

        return temp;
    }

    static auto get_hw_serial_number()
    {
        std::string temp = {};
        temp.resize( 128 );

        sceKernelGetHwSerialNumber( &temp[0] );

        return temp;
    }

    static auto get_soc_sensor_temperature()
    {
        int32_t temp = {};
        sceKernelGetSocSensorTemperature( 0, &temp );

        return temp;
    }

    static auto get_cpu_temperature()
    {
        int32_t temp = {};
        sceKernelGetCpuTemperature( &temp );

        return temp;
    }

    static auto get_cpu_frequency()
    {
        return sceKernelGetCpuFrequency() * 1.f / ( 1000.f * 1000.f );
    }

    

};