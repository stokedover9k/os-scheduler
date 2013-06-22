#include "process.h"

int prc::get_time() { 
  static int t = 0;
  return t++; }

void throw_invalid_transition_value(prc::state_transition t, std::string const& function_name) {
  std::ostringstream s;
  s << "Invalid state_transition value " << t
    << " in function " << function_name;
  throw std::invalid_argument( s.str() ); }

prc::state prc::transition_from( state_transition t ) {
  switch( t ) {
    case DISPATCH:  return READY;
    case PREEMPT:   return EXECUTING;
    case BLOCK:     return EXECUTING;
    case UNBLOCK:   return BLOCKED;
    case ARRIVE:    return INVALID;
    case FINISH:    return EXECUTING; }
  throw_invalid_transition_value(t, "transition_from"); }

prc::state prc::transition_to( state_transition t ) {
  switch( t ) {
    case DISPATCH:  return EXECUTING;
    case PREEMPT:   return READY;
    case BLOCK:     return BLOCKED;
    case UNBLOCK:   return READY;
    case ARRIVE:    return READY;
    case FINISH:    return FINISHED; }
  throw_invalid_transition_value(t, "transition_to"); }

//===== process_state =====//

prc::process_state::process_state(state s)
  : state_(s) 
  #if PER_PROC_STATS
  , blocked_time_(0), ready_time_(0), last_blocked_(0), last_ready_(0)
  #endif PER_PROC_STATS
{ }

prc::state prc::process_state::get_state() const {
  return state_; }

void prc::process_state::transition( state_transition t ) {
  if( get_state() != transition_from(t) )
    throw_invalid_transition_value(t, "process_state::transition");
  state_ = transition_to(t); 

  #if PER_PROC_STATS
  state from = transition_from(t);
  state to   = transition_to(t);
  if( from == BLOCKED && to != BLOCKED )
    blocked_time_ += get_time() - last_blocked_;
  if( to == BLOCKED && from != BLOCKED )
    last_blocked_ = get_time();
  if( from == READY && to != READY )
    ready_time_ += get_time() - last_ready_;
  if( to == READY && from != READY )
    last_ready_ = get_time();
  #endif
}

//===== process_core =====//

prc::process_core::process_core(int pid)
  : process_state(INVALID), pid_(pid), cpu_used_(0) { }

int prc::process_core::get_pid() const {
  return pid_; }

int prc::process_core::run(int max_time) {
  int ran_for = __execute__(max_time);
  cpu_used_ += ran_for;
  return ran_for; }

//===== process ======//

