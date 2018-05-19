/**
 * Copyright (c) 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifdef MBPOLL_GPIO_RTS
// -----------------------------------------------------------------------------
#include <piduino/gpio.h>
#include "mbpoll-gpio-rts.h"

using namespace Piduino;

// -----------------------------------------------------------------------------
namespace MbPoll {

  class RtsPin {
    public:
      RtsPin() : _num (-1), _rev (false) {}

      bool begin (int num, int act);
      void write (bool value) const;

    private:
      int _num;
      bool _rev;
  };
  
  // ---------------------------------------------------------------------------
  bool 
  RtsPin::begin (int num, int act) {
    _num = num;
    _rev = !act;

    if (gpio.open()) {

      gpio.pin (_num).setMode (Pin::ModeOutput);
      this->write (false);
      return true;
    }
    return false;
  }

  // ---------------------------------------------------------------------------
  void 
  RtsPin::write (bool value) const {

    gpio.pin (_num).write (value ^ _rev);
  }

  RtsPin rts;
}

// -----------------------------------------------------------------------------
int
init_gpio_rts (int num, int act) {

  if (MbPoll::rts.begin (num, act)) {
    
    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
void
set_gpio_rts (modbus_t *ctx, int on) {

  MbPoll::rts.write (on != 0);
}
// -----------------------------------------------------------------------------
#endif /* MBPOLL_GPIO_RTS defined */
/* ========================================================================== */
