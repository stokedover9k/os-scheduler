#include "event_simulation.h"

des::event_simulation::event_simulation(des::events_queue const& initial_events)
  : events(initial_events), sched(NULL), busy_until(timer_control()) { }

void des::event_simulation::set_scheduler(sch::scheduler * s) {
  sched = s; }

bool des::event_simulation::done() const {
  return events.size() == 0; }

bool des::event_simulation::run() {
  if( events.size() == 0 ) return false;

  timer_control.set( events.peek().get_time() );

  while( 
    events.size() > 0 &&
    events.peek().get_time() == timer_control.get_time() )
  {
    event e = events.pop();
    switch( e.get_transition() ) {
      case prc::DISPATCH: throw std::logic_error("DISPATCH found in event queue");
      case prc::PREEMPT:  handle_preempt(e.get_process()); break;
      case prc::BLOCK:    handle_block(e.get_process());   break;
      case prc::UNBLOCK:  handle_unblock(e.get_process()); break;
      case prc::ARRIVE:   handle_arrive(e.get_process());  break;
      case prc::FINISH:   handle_finish(e.get_process());  break;
    }
  }

  if( timer_control() >= busy_until ) {
    std::pair<process *, int> res = sched->dispatch();
    if( res.first ) {
      process *p = res.first;
      int ran_until = timer_control() + res.second;

      switch( res.first.get_state() ) {
        case prc::FINISHED:
          events.push(ran_until, p, prc::FINISH);
          break;
        case prc::BLOCKED:
          events.push(ran_until, p, prc::BLOCK);
          break;
        case prc::READY:
          events.push(ran_until, p, prc::PREEMPT);
          break;
      }
    }
  }

  return true;
}

void des::event_simulation::handle_arrive(des::process * p) {
  sched->add_process( p );
}

/*
void des::event_simulation::handle_preemtn(des::process * p)
void des::event_simulation::handle_block(des::process * p)
void des::event_simulation::handle_unblock(des::process * p)
void des::event_simulation::handle_finish(des::process * p)
*/