#include "cdc_sim.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    const char* slave = cdc_sim_start();
    if (!slave) {
        fprintf(stderr, "Failed to start CDC simulator\n");
        return 1;
    }
    printf("CDC ACM device simulator running. Connect to: %s\n", slave);
    printf("Press Enter to stop...\n");
    getchar();
    cdc_sim_stop();
    printf("Simulator stopped.\n");
    return 0;
}
