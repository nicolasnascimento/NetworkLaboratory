#ifndef IPPOOL_H
#define IPPOOL_H

#include <sys/types.h>

/// Bounds this transaction id to the corresponding ip
void set_ip_for_trs_id(in_addr_t ip, uint32_t trs_id);

/// Returns the ip corresponding to the transaction id
/// Returns 0 if no ip was found
in_addr_t get_ip_for_trs_id(uint32_t trs_id);

#endif // IPPOOL_H
