/* Exercise the real option parsing and initialization, stopping at connect. */
#define main mbpoll_main
#include "../src/mbpoll.c"
#undef main

static uint32_t expected_sec;
static uint32_t expected_usec;

int modbus_connect (modbus_t *bus) {
  uint32_t sec = 0, usec = 0;

  if (modbus_get_response_timeout (bus, &sec, &usec) != 0 ||
      sec != expected_sec || usec != expected_usec) {
    fprintf (stderr, "Timeout at connect: %"PRIu32".%06"PRIu32
             ", expected %"PRIu32".%06"PRIu32"\n",
             sec, usec, expected_sec, expected_usec);
    exit (EXIT_FAILURE);
  }
  modbus_free (bus);
  exit (EXIT_SUCCESS);
}

int main (int argc, char **argv) {
  if (argc < 3) {
    return EXIT_FAILURE;
  }
  expected_sec = (uint32_t) strtoul (argv[1], NULL, 10);
  expected_usec = (uint32_t) strtoul (argv[2], NULL, 10);
  return mbpoll_main (argc - 2, argv + 2);
}
