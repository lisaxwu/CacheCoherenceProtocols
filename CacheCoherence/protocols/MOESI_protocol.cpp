#include "MOESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOESI_protocol::MOESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MOESI_CACHE_I;
}

MOESI_protocol::~MOESI_protocol ()
{    
}

void MOESI_protocol::dump (void)
{
    const char *block_states[9] = {"X","I","S","E","O","M","IM","IS","SM"};
    fprintf (stderr, "MOESI_protocol - state: %s\n", block_states[state]);
}

void MOESI_protocol::process_cache_request (Mreq *request)
{

}

void MOESI_protocol::process_snoop_request (Mreq *request)
{
	// switch (state) {
 //        case MOESI_CACHE_I:  do_snoop_I (request); break;
 //        case MOESI_CACHE_S:  do_snoop_S (request); break;
 //        case MOESI_CACHE_E:  do_snoop_E (request); break;
 //        case MOESI_CACHE_O:  do_snoop_O (request); break;
 //        case MOESI_CACHE_M:  do_snoop_M (request); break;
 //        // case MOESI_CACHE_IM:  do_snoop_IM (request); break;
 //        // case MOESI_CACHE_IS:  do_snoop_IS (request); break;
 //        // case MOESI_CACHE_SM:  do_snoop_SM (request); break;

 //    default:
 //    	fatal_error ("Invalid Cache State for MOESI Protocol\n");
 //    }
}

inline void MOESI_protocol::do_cache_I (Mreq *request)
{
    // switch (request->msg) {
    // // If we get a request from the processor we need to get the data
    // case GETS:
    //     send_GETS(request->addr);
    //     state = MI_CACHE_IS;
    //     Sim->cache_misses++;
    //     break;
    // case GETM:
    //     send_GETM(request->addr);
    //     state = MI_CACHE_IM;
    //     Sim->cache_misses++;
    //     break;
    // case DATA:
    //     break;
    // default:
    //     request->print_msg (my_table->moduleID, "ERROR");
    //     fatal_error ("Client: I state shouldn't see this message\n");
    // }
}

inline void MOESI_protocol::do_cache_S (Mreq *request)
{
    // switch (request->msg) {
    // // If we get a request from the processor we need to get the data
    // case LOAD:
    //     send_GETS(request->addr);
    //     Sim->cache_misses++;
    //     break;
    // case STORE:
    //     send_GETM(request->addr);
    //     state = MI_CACHE_IM;
    //     Sim->cache_misses++;
    //     break;
    // default:
    //     request->print_msg (my_table->moduleID, "ERROR");
    //     fatal_error ("Client: I state shouldn't see this message\n");
    // }

}

inline void MOESI_protocol::do_cache_E (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_O (Mreq *request)
{

}

inline void MOESI_protocol::do_cache_M (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_I (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_S (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_E (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_O (Mreq *request)
{

}

inline void MOESI_protocol::do_snoop_M (Mreq *request)
{

}

// inline void MOESI_protocol::do_snoop_IM (Mreq *request)
// {

// }

// inline void MOESI_protocol::do_snoop_IS (Mreq *request)
// {

// }

// inline void MOESI_protocol::do_snoop_SM (Mreq *request)
// {

// }


