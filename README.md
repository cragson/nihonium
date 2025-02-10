# Nihonium - A C++ framework for memory manipulation on the Playstation 5 (PS5).
**Note: Currently heavily work in progress**

# **Table of Contents**
1. **[Requirements](#requirements)**
2. **[Setup & Build](#setup--build)**
3. **[Framework features](#framework-features)**
4. **[Examples](#examples)**
    - **[How to retreive kernel text base](#how-to-retreive-the-kernel-text-base)**
    - **[How to retreive kernel data base](#how-to-retreive-the-kernel-data-base)**
    - **[How to read kernel memory](#how-to-read-kernel-memory)**
    - **[How to write to kernel memory](#how-to-write-to-kernel-memory)**
    - **[How to change the protection of a memory region in the kernel](#how-to-change-the-memory-protection-of-a-memory-region-in-the-kernel)**
    - **[How to retreive an process by its pid](#how-to-retreive-an-process-by-its-pid)**

## **Requirements**
1. Currently only tested on Firmware 2.00 but should work on all supported firmwares by the SDK.
2. C++23 is the highest language revision of C++ used.
3. Relies on SDK from [John Törnblom](https://github.com/john-tornblom), thanks for the great work!

## **Setup & Build**
1. Clone and build the [SDK from John Törnblom](https://github.com/ps5-payload-dev/sdk) (GNU/Linux based)
    - ``wget https://github.com/ps5-payload-dev/sdk/releases/latest/download/ps5-payload-sdk.zip``
    - ``sudo unzip -d /opt ps5-payload-sdk.zip``
2. Clone Nihonium
    - ``git clone git@github.com:cragson/nihonium``
3. **(Optional)** Jailbreak your PS5 to send the payload directly after building
4. Set the correct IP address of your PS5 inside of ``nihonium/debug.sh``
5. Run ``nihonium/debug.sh`` to build the payload and sent it to your PS5

## **Framework features**
Here are all the available features of this framework listed, explained and shown with an example.

### **KernelMemory**
This should give you access to functions, which you can use for manipulation of the Kernel (e.g. reading and writing to kernel memory).

### **ILibKernel - Interface for Libkernel**
Currently the following functions are supported:
- sceKernelSendNotificationRequest
- sceKernelGetHwModelName
- sceKernelGetHwSerialNumber
- sceKernelGetCpuFrequency
- sceKernelGetCpuTemperature
- sceKernelGetSocSensorTemperature

## Examples
Here are and will be a lot of small examples on how to use different parts of this framework, so you can easily try it out yourself.

### How to retreive the kernel text base
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto kernel_txt = KernelMemory::get_kernel_text_base();

    std::println( "[+] kernel text: {:X}", kernel_txt );

    return 0;
}
```
### How to retreive the kernel data base
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto kernel_data = KernelMemory::get_kernel_data_base();

    std::println( "[+] kernel data: {:X}", kernel_data );

    return 0;
}
```
### How to read kernel memory
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto kernel_txt = KernelMemory::get_kernel_text_base();

    if(!kernel_txt)
        return 0;

    const auto buffer = KernelMemory::read_kernel< uint64_t >( kernel_txt );

    std::println( "[+] First 8 bytes from kernel text: {:X}", buffer );

    return 0;
}
```
### How to write to kernel memory
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto kernel_data = KernelMemory::get_kernel_data_base();

    if(!kernel_data)
        return 0;

    if( KernelMemory::write_kernel< uint64_t >( kernel_data + 0x1337, 0xDEADAFFE ) )
        std::println( "[+] Successfully wrote to memory!" );
    else
        std::println( "[!] Failed to write to memory!" );

    return 0;
}
```
### How to change the memory protection of a memory region in the kernel
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto pid = int32_t( 0x1337);
    const auto address = std::uintptr_t( 0xDEADBEEF );
    constexpr auto PROT_READ = 0x01;
    constexpr auto PROT_WRITE = 0x02;
    constexpr auto PROT_EXEC = 0x04;

    if( KernelMemory::change_memory_protection( pid, address, 0x420, PROT_READ | PROT_WRITE | PROT_EXEC ) )
        std::println( "[+] Successfully changed memory protection!" );
    else
        std::println( "[!] Failed change memory protection!" );

    return 0;
}
```
### How to retreive an process by its pid
```cpp
#include "kernel_memory.hpp"
#include <print>

int main()
{
    const auto process = KernelMemory::get_process( 0x1337 );

    std::println( "[+] process: {:X}", process );

    return 0;
}
```