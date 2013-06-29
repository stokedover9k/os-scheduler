#include "event_simulation.h"

//================ process_statistician ======================//

std::list<des::process *> const & des::process_statistician::get_processes() const {
  return processes; }

std::map<des::process *, int> const & des::process_statistician::get_arrival_times() const {
  return arrival_times; }

std::map<des::process *, int> const & des::process_statistician::get_finish_times() const {
  return finish_times; }

int des::process_statistician::get_total_cpu_time() const {
  return total_cpu_on; }

int des::process_statistician::get_total_io_time() const {
  return total_io_on; }

float des::process_statistician::get_average_turnaround() const {
  int total;
  for( auto i = get_finish_times().begin(); 
    i != get_finish_times().end(); ++i )
    total += i->second;
  return (float) total / get_finish_times().size(); }

//====================== event_simulation ====================//

des::event_simulation::event_simulation(des::events_queue const& initial_events, sch::scheduler * s)
  : events(initial_events), sched(s), busy_until(timer()) 
{
  if( !sched )
    throw std::logic_error("Simulation created with a NULL scheduler.");

  num_using_io = last_io_on = total_io_on = total_cpu_on = last_finish = 0;
}

bool des::event_simulation::done() const {
  return events.size() == 0; }

bool des::event_simulation::run() {
  if( events.size() == 0 ) return false;

  timer_control.set( events.peek().get_time() );

  while( // handle all events at the next time stamp
    events.size() > 0 &&
    events.peek().get_time() == timer() )
  {
    event e = events.pop();
    e.get_process()->transition( e.get_transition() );
    switch( e.get_transition() ) {
      case prc::DISPATCH: throw std::logic_error("DISPATCH found in event queue");
      case prc::PREEMPT:  handle_preempt(e.get_process()); break;
      case prc::BLOCK:    handle_block(e.get_process());   break;
      case prc::UNBLOCK:  handle_unblock(e.get_process()); break;
      case prc::ARRIVE:   handle_arrive(e.get_process());  break;
      case prc::FINISH:   handle_finish(e.get_process());  break;
    }
  }

  // if scheduler not busy, dispatch a process and create an event upon its return
  if( timer() >= busy_until )
  {
    auto res = sched->dispatch();
    process *p = res.first;
    if( p )
    {
      busy_until = timer() + res.second.second;
      total_cpu_on += res.second.second;
      prc::state_transition upon_end_of_execution = res.second.first;

      switch( upon_end_of_execution ) {
        case prc::FINISH:
        case prc::BLOCK:
        case prc::PREEMPT:
          events.push(busy_until, p, upon_end_of_execution);
          break;
        default:
          throw std::logic_error("Dispatched process returned in an unexpected state");
      }
    }
  }

  return true;
}

void des::event_simulation::handle_arrive(des::process * p) {
  processes.push_back( p );
  arrival_times[p] = timer();
  sched->add_process( p );
}

void des::event_simulation::handle_preempt(des::process * p) {
  sched->add_process( p );
}

void des::event_simulation::handle_block(des::process * p) {
  int io_burst = p->io();
  events.push(timer() + io_burst, p, prc::UNBLOCK);

  if( num_using_io++ == 0 ) 
    last_io_on = timer();

  TLOG << timer() << ": " << *p << " BLOCKED at " << timer() << " for " << io_burst;
}

void des::event_simulation::handle_unblock(des::process * p) {
  sched->add_process( p );

  if( --num_using_io == 0 )
    total_io_on += timer() - last_io_on;
}

void des::event_simulation::handle_finish(des::process * p) {
  finish_times[p] = timer();
  last_finish = std::max(timer(), last_finish);
}
