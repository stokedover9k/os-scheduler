#ifndef __EVENT_SIMULATION_H__
#define __EVENT_SIMULATION_H__

#include "process.h"
#include "events_queue.h"
#include "scheduler.h"
#include "loglib.h"

#include <exception>

extern random_generator *rgen;
extern general_clock & timer;
extern controlled_clock & timer_control;

namespace des
{

  //========== event_simulation =========//
  struct event_simulation {              //
  //=====================================//
    event_simulation(events_queue const& initial_events, sch::scheduler *);
    bool done() const;
    bool run();

  protected:
    void handle_preempt(process *p);
    void handle_block(process *p);
    void handle_unblock(process *p);
    void handle_arrive(process *p);
    void handle_finish(process *p);

  private:
    events_queue events;
    sch::scheduler * sched;
    int busy_until;
  }; //endl: event_simulation -----------//

};

#endif //__EVENT_SIMULATION_H__
