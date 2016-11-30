#ifndef HTTP_H
#define HTTP_H

/// Use the buffer from the get method to generate a valid http address and stores it in addr
/// Returns the length of the addr, if an erro occurred returns -1.
size_t set_addr_from_get_buf(char*, char*);

#endif // HTTP_H
