#ifndef __PROTOS_CLIENT_UTIL__
#define __PROTOS_CLIENT_UTIL__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#define ADD_USER "add-user"
#define ADD_USER_NO 1
#define DEL_USER "del-user"
#define DEL_USER_NO 2
#define LIST_USER "list-users"
#define LIST_USERS_NO 3
#define GET_METRIC "get-metric"
#define GET_METRIC_NO 4
#define GET_CONFIG "get-config"
#define GET_CONFIG_NO 5
#define SET_CONFIG "set-config"
#define SET_CONFIG_NO 6

#define MAX_DATA_LEN    514
#define READBUFFER_LEN  257
#define MAX_VAL_BYTES   sizeof(uint64_t)



uint8_t 
valid_args(int argc, char * const *argv);
int get_next_command(int argc, char * const *argv, int * cmdStartIndex, uint8_t * data, int * datalen);
int handle_response(int sockfd,int cmd, uint8_t *readBuffer);
void recv_wrapper(int sockfd,void *buffer, size_t len, int flags);


enum response_errors {
    error_inv_command = 0x01,
    error_inv_ulen = 0x02,
    error_inv_utype = 0x03,
    error_inv_metric = 0x04,
    error_inv_config = 0x05,
    error_inv_value = 0x06,
    error_max_ucount = 0x07,

    error_server_fail = 0xFF,
    error_none = 0x00,
};

/* Possible commands */
enum commands {
    command_add_user = 0x01,
    command_del_user = 0x02,
    command_list_user = 0x03,
    command_get_metric = 0x04,
    command_get_config = 0x05,
    command_set_config = 0x06,

    command_none = 0xFF,
};

/* Possible metrics */
enum metric_options {
    metric_hist_conn = 0x00,
    metric_conc_conn = 0x01,
    metric_hist_btransf = 0x02,

    metric_none = 0xFF,
};

/* Possible configurations */
enum config_options {
    config_buff_read_size = 0x00,
    config_buff_write_size = 0x01,
    config_sel_tout = 0x02,

    config_none = 0xFF,
};


#endif