#pragma once

#include <cstdint>
#include <sys/types.h>
#include <unordered_map>
#include <string>

#include <sys/user.h>

// ref: https://github.com/ps5-payload-dev/sdk/blob/17b1d592d4aa095f6cc52e33c8a26fd6f898d65c/include/freebsd/vm/vm_map.h#L99
class _200_vm_map_entry 
{
public:
    _200_vm_map_entry* prev;	        /* previous entry */
    _200_vm_map_entry* next;	        /* next entry */
    _200_vm_map_entry* left;	        /* left child in binary search tree */
    _200_vm_map_entry* right;	        /* right child in binary search tree */
	std::uintptr_t start;		        /* start address */
	std::uintptr_t end;		            /* end address */
	std::uintptr_t pad0;
	std::uintptr_t next_read;		    /* vaddr of the next sequential read */
	size_t adj_free;		        /* amount of adjacent free space */
	size_t max_free;		        /* max free space in subtree */
	std::uintptr_t object;	            /* object I point to */
	int64_t offset;		                /* offset into object */
	unsigned int eflags;		        /* map entry flags */
	unsigned char protection;		    /* protection code */
	unsigned char max_protection;	    /* maximum protection */
	char inheritance;	                /* inheritance */
	uint8_t read_ahead;		            /* pages in the read-ahead window */
	int wired_count;		            /* can be paged if = 0 */
	std::uintptr_t cred;		        /* tmp storage for creator ref */
	std::uintptr_t wiring_thread;
};

class _200_process
{
public:
    _200_process* m_pForward;            // 0x0
    uint8_t m_pad1[56];                  // 0x8
    std::uintptr_t m_pUcred;             // 0x40
    uint8_t m_pad2[115];                 // 0x48
    uint8_t m_state;                     // 0xBB
    int32_t m_pid;                       // 0xBC
    std::uintptr_t m_pVmspace;           // 0xC4
    uint8_t m_pad3[1180];                // 0xCC
    char m_comm[32];                     // 0x564
    char m_path[32];                     // 0x584
};

// ref: https://github.com/ps5-payload-dev/sdk/blob/17b1d592d4aa095f6cc52e33c8a26fd6f898d65c/include/freebsd/sys/proc.h#L761C1-L767C42
inline std::unordered_map< int32_t, std::string > PROC_STATES = {
    { 1, "BeingCreated" }, { 2, "Runnable/Running" }, { 3, "Sleeping" }, { 4, "Stopped" }, { 5, "Zombie" }, { 6, "Waiting" }, { 7, "Lock" }
};

class orbisview_process
{
public:
    orbisview_process(
        const std::string& _name, 
        const int32_t _pid, 
        const std::string& _state,
        const std::uintptr_t _vmspace, 
        const std::uintptr_t _proc,
        const kinfo_proc* _ki
    ) : 
    m_name(_name), 
    m_pid(_pid), 
    m_state(_state ), 
    m_vmspace( _vmspace ),
    m_proc( _proc )
    { 
        this->m_ki = std::make_unique< kinfo_proc >();

        this->fill_ki( _ki );
    }

    void fill_ki( const kinfo_proc* _ki )
    {
        if( !_ki )
            return;
        
        memcpy( this->m_ki.get(), _ki, sizeof( kinfo_proc ) );
    }

    std::string m_name;
    int32_t m_pid;
    std::string m_state;
    std::uintptr_t m_vmspace;
    std::uintptr_t m_proc;
    std::unique_ptr< kinfo_proc > m_ki;
};