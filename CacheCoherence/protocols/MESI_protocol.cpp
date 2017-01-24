#include "MESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MESI_protocol::MESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MESI_CACHE_I;
}

MESI_protocol::~MESI_protocol ()
{    
}

void MESI_protocol::dump (void)
{
    const char *block_states[8] = {"X","I","S","E","M","IM","IS","SM"};
    fprintf (stderr, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
        case MESI_CACHE_I:  do_cache_I (request); break;
        case MESI_CACHE_S:  do_cache_S (request); break;
        case MESI_CACHE_E:  do_cache_E (request); break;
        case MESI_CACHE_M:  do_cache_M (request); break;
       
    default:
        fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

void MESI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
        case MESI_CACHE_I:  do_snoop_I (request); break;
        case MESI_CACHE_S:  do_snoop_S (request); break;
        case MESI_CACHE_E:  do_snoop_E (request); break;
        case MESI_CACHE_M:  do_snoop_M (request); break;
        case MESI_CACHE_IM:  do_snoop_IM (request); break;
        case MESI_CACHE_IS:  do_snoop_IS (request); break;
        case MESI_CACHE_SM:  do_snoop_SM (request); break;

    default:
    	fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

inline void MESI_protocol::do_cache_I (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_GETS(request->addr);
            state = MESI_CACHE_IS;
            Sim->cache_misses++;
            //printf("###I read\n");
            break;
        case STORE:
            send_GETM(request->addr);
            state = MESI_CACHE_IM;
            Sim->cache_misses++;
            //printf("###I write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_DATA_to_proc(request->addr);
            //printf("###S read\n");
            break;
        case STORE:
            send_GETM(request->addr);
            state = MESI_CACHE_SM;
            //printf("###S write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_E (Mreq *request)
{
    switch (request->msg) {
    // If we get a request from the processor we need to get the data
        case LOAD:
            send_DATA_to_proc(request->addr);
            //printf("###E read\n");
            break;
        case STORE:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M;
            Sim->silent_upgrades++;
            // send_GETM(request->addr);
            // state = MESI_CACHE_SM;
            //printf("###E write\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }

}

inline void MESI_protocol::do_cache_M (Mreq *request)
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

inline void MESI_protocol::do_snoop_I (Mreq *request)
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

inline void MESI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            set_shared_line();
            //printf("$$$S snoop read\n");
            break;
        case GETM:
            state = MESI_CACHE_I;
            //printf("$$$S snoop write\n");
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }

}

inline void MESI_protocol::do_snoop_E (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state = MESI_CACHE_S;
            //printf("$$$E snoop read\n");
            break;
        case GETM:
            state = MESI_CACHE_I;
            send_DATA_on_bus(request->addr,request->src_mid);
            //printf("$$$E snoop write\n");
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            // printf("$$$M snoop read\n");
            // printf("on bus\n");
            state = MESI_CACHE_S;
            break;
        case GETM:
            send_DATA_on_bus(request->addr,request->src_mid);
            // printf("$$$M snoop write\n");
            // printf("on bus\n");
            state = MESI_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IM (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M;
            //printf("$$$IM snoop data\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IS (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            //printf("$$$IS snoop data\n");
            send_DATA_to_proc(request->addr);
            if (get_shared_line()){
                state = MESI_CACHE_S;
            }else{
                state = MESI_CACHE_E;
            }    
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_SM (Mreq *request)
{
    switch (request->msg) {
    
        case GETS:
            set_shared_line();
            break;
        case GETM:
            state = MESI_CACHE_IM;
            Sim->cache_misses++;
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M;
            //printf("$$$SM snoop data\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}


