#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;

#define MAX_READINGS 10
#define SEND_INTERVAL (60 * CLOCK_SECOND)
#define FAKE_TEMPS 5

static struct simple_udp_connection udp_conn;

static unsigned sum_during_disconn = 0;
static unsigned num_readings = 0;
static unsigned disconnected = 0;
static unsigned i;

static unsigned readings[MAX_READINGS];
static unsigned next_reading=0;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static unsigned
get_temperature()
{
  static unsigned fake_temps [FAKE_TEMPS] = {30, 25, 20, 15, 10};
  return fake_temps[random_rand() % FAKE_TEMPS];
  
}
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  // ...
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static float temperature_to_send;
  static float average_batched;
  static uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /*for (i=0; i<next_reading; i++) {
    readings[i] = 0;
  }*/

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    temperature_to_send = (float) get_temperature();
    LOG_INFO("[CLIENT] Temperature reading: %f.\n", temperature_to_send);
    if (NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      if (disconnected == 1) {
        // Compute average on the batched value
        LOG_INFO("[CLIENT] Reconnection: computing average.\n");
        for (i = 0; i < num_readings && i < MAX_READINGS; i++) {
          sum_during_disconn += readings[i];
        }
        average_batched = ((float) sum_during_disconn)/i;
        simple_udp_sendto(&udp_conn, &average_batched, sizeof(average_batched), &dest_ipaddr);
        etimer_set(&periodic_timer, SEND_INTERVAL);
      }
      // Server is reachable: send current temperature
      LOG_INFO("[CLIENT] Sending temperature %f to ", temperature_to_send);
      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO("\n");
      simple_udp_sendto(&udp_conn, &temperature_to_send, sizeof(temperature_to_send), &dest_ipaddr);
      // Reset measures read during disconnection
      num_readings = 0;
      disconnected = 0;
    } else {
      LOG_INFO("[CLIENT] Server not reachable yet.\n");
      disconnected = 1;

      // Server not reachable: batches the current temp.
      LOG_INFO("[CLIENT] Value read: %f, index: %u.\n", temperature_to_send, next_reading);
      readings[next_reading++] = temperature_to_send;
      num_readings++;
      if (next_reading == MAX_READINGS) {
        next_reading = 0;
      }
    }
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
