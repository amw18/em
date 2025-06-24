#ifndef CDC_SIM_H
#define CDC_SIM_H

// Start the CDC ACM device simulator on a pseudo terminal.
// Returns the slave PTY path for the simulated device.
const char* cdc_sim_start();

// Stop the simulator and clean up resources.
void cdc_sim_stop();

#endif // CDC_SIM_H
