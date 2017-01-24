#include "MOSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOSI_protocol::MOSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MOSI_CACHE_I;
}

MOSI_protocol::~MOSI_protocol ()
{    
}

void MOSI_protocol::dump (void)
{
    const char *block_states[9] = {"X","I","S","O","M","IM","IS","SM","OM"};
    fprintf (stderr, "MOSI_protocol - state: %s\n", block_states[state]);
}

void MOSI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
        case MOSI_CACHE_I:  do_cache_I (request); break;
        case MOSI_CACHE_S:  do_cache_S (request); break;
        case MOSI_CACHE_O:  do_cache_O (request); break;
        case MOSI_CACHE_M:  do_cache_M (request); break;
    default:
        fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

void MOSI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
        case MOSI_CACHE_I:  do_snoop_I (request); break;
        case MOSI_CACHE_S:  do_snoop_S (request); break;
        case MOSI_CACHE_O:  do_snoop_O (request); break;
        case MOSI_CACHE_M:  do_snoop_M (request); break;
        case MOSI_CACHE_IM:  do_snoop_IM (request); break;
        case MOSI_CACHE_IS:  do_snoop_IS (request); break;
        case MOSI_CACHE_SM:  do_snoop_SM (request); break;
        case MOSI_CACHE_OM:  do_snoop_OM (request); break;
    default:
    	fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

inline void MOSI_protocol::do_cache_I (Mreq *request)
{
    switch (request->msg) {
   
        case LOAD:
            send_GETS(request->addr);
            state = MOSI_CACHE_IS;
            Sim->cache_misses++;
            //printf("###I read\n");
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_IM;
            Sim->cache_misses++;
            //printf("###I write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_DATA_to_proc(request->addr);
            //printf("###S read\n");
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_SM;
            //printf("###S write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_O (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_DATA_to_proc(request->addr);
            //printf("###S read\n");
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_OM;
            //printf("###S write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_M (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_DATA_to_proc(request->addr);
            //printf("###M read\n");
            break;
        case STORE:
            send_DATA_to_proc(request->addr);
            //printf("###M write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            //set_shared_line();
            //printf("$$$S snoop read\n");
            break;
        case GETM:
            state = MOSI_CACHE_I;
            //printf("$$$S snoop write\n");
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_O (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            //set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            //printf("$$$O snoop read\n");
            break;
        case GETM:
            send_DATA_on_bus(request->addr,request->src_mid);
            state = MOSI_CACHE_I;
            //printf("$$$O snoop write\n");
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            //set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state = MOSI_CACHE_O;
            //printf("$$$M snoop read\n");
            break;
        case GETM:
            send_DATA_on_bus(request->addr,request->src_mid);
            state = MOSI_CACHE_I;
            //printf("$$$M snoop write\n");
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_IM (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_IS (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_S;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_SM (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:

            break;
        case GETM:
            state = MOSI_CACHE_IM;
            Sim->cache_misses++;
            //printf("$$$SM snoop write\n");
            break;
        case DATA:
            // send_DATA_to_proc(request->addr);
            // state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_OM (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            send_DATA_on_bus(request->addr,request->src_mid);
            break;
        case GETM:
            state = MOSI_CACHE_IM;
            Sim->cache_misses++;
            send_DATA_on_bus(request->addr,request->src_mid);
            //printf("$$$SM snoop write\n");
            break;
        case DATA:
            // send_DATA_to_proc(request->addr);
            // state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}



