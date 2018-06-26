#include <stdio.h>
#include <string.h>

#include <msg.h>
#include <shell.h>

#include "random.h"
#include "lib/network.c"
#include "lib/pump.c"
#include "lib/sensor.h"
#include "lib/sensor_thread.c"
#include "lib/util.c"
#include "lib/global.c"

/* stack containing all started stacks */
char thread_stack[THREAD_STACKSIZE_MAIN];

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { "light", "read light data", read_light_shell },
    { "humidity", "read humidity data", read_humidity_shell },
    { "h2od", "start h2o server", h2o_server },
    { "h2od_debug", "turn h2od debug prints on and off", shell_h2od_debug },
    { "pump_set_data", "Send data to the pump controller", shell_pump_set_data },
    { "h2o_send_data", "send data using the h2o protocol", h2o_send_data_shell },
    { NULL, NULL, NULL },
};


int main(void)
{
    // NODE_ID declared in global.c
#ifdef NODE_ID_RANDOM
    // We reserve 0xff.. for special addresses
    NODE_ID = (nodeid_t) random_uint32_range(1, 0xff00);
#else
    NODE_ID = (NODE_ID_);
#endif /* NODE_ID_RANDOM */

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    printf("[Pflanzen 1] Welcome! I am a %s, my node ID is %04X.\n",
           NODE_ROLE, NODE_ID);

    add_public_address(NULL);

#ifdef UPSTREAM_NODE
    h2op_add_receive_hook(h2op_forward_data_hook);
#endif
#ifdef NODE_ROLE_COLLECTOR
    h2op_add_receive_hook(h2op_pump_set_data_hook);
#endif

    initialize_sensors();

#ifdef NODE_ROLE_SENSOR
    thread_create(thread_stack, sizeof(thread_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  sensor_thread, NULL, "sensor_thread");
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
