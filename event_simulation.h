#ifndef __EVENT_SIMULATION_H__
#define __EVENT_SIMULATION_H__

#include "process.h"
#include "events_queue.h"
#include "scheduler.h"
#include "loglib.h"

#include <exception>
#include <map>

extern random_generator *rgen;
extern general_clock & timer;
extern controlled_clock & timer_control;

namespace des
{
  struct process_statistician {
    std::list<process *> const & get_processes() const;
    std::map<process *, int> const & get_arrival_times() const;
    std::map<process *, int> const & get_finish_times() const;

    int get_total_cpu_time() const;
    int get_total_io_time() const;
    float get_average_turnaround() const;
  protected:
    std::list<process *> processes;
    std::map<process *, int> arrival_times;
    std::map<process *, int> finish_times;
    int num_using_io;
    int last_io_on, total_io_on;
    int total_cpu_on;
    int last_finish;
  };

  //========== event_simulation =========//
  struct event_simulation                //
    : public process_statistician {      //
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
