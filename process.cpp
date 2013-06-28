#include "process.h"

char const * const transition_string(prc::state_transition t) {
  static char const * const tstrings[] = {
    "DISPATCH",
    "PREEMPT",
    "BLOCK",
    "UNBLOCK",
    "ARRIVE",
    "FINISH" };
  return tstrings[t]; }

char const * const state_string(prc::state s) {
  static char const * const sstrings[] = {
    "INVALID", "READY", "EXECUTING", "BLOCKED", "FINISHED" };
  return sstrings[s]; }

void throw_invalid_transition_value(prc::state_transition t, std::string const& function_name) {
  std::ostringstream s;
  s << "Invalid state_transition value " << transition_string(t)
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
  TLOG << timer() << ": " << *this << " transitioning: " << transition_string(t);

  if( get_state() != transition_from(t) )
    throw_invalid_transition_value(t, "process_state::transition");
  state_ = transition_to(t); 

  #if PER_PROC_STATS
  state from = transition_from(t);
  state to   = transition_to(t);
  if( from == BLOCKED && to != BLOCKED )
    blocked_time_ += timer() - last_blocked_;
  if( to == BLOCKED && from != BLOCKED )
    last_blocked_ = timer();
  if( from == READY && to != READY )
    ready_time_ += timer() - last_ready_;
  if( to == READY && from != READY )
    last_ready_ = timer();
  #endif
}

//===== process_core =====//

prc::process_core::process_core(int pid)
  : process_state(INVALID), pid_(pid), cpu_used_(0) { }

int prc::process_core::get_pid() const {
  return pid_; }

int prc::process_core::get_cpu_used() const {
  return cpu_used_; }

int prc::process_core::estimate_cpu_time() const {
  return 1; }

std::pair<prc::state_transition, int> prc::process_core::run(int max_time) {
  transition( DISPATCH );
  auto res = __execute__(max_time);
  cpu_used_ += res.second;

  return res; }

//===== stochastic_process ======//

prc::stochastic_process::stochastic_process(int pid, int cpu, int cpu_burst, int io_burst) :
  process_core(pid),
  total_cpu_(cpu),
  cpu_burst_(cpu_burst),
  io_burst_(io_burst),
  current_cpu_burst_(0) { }

int prc::stochastic_process::estimate_cpu_time() const {
  return total_cpu_ - get_cpu_used(); }

int prc::stochastic_process::io() {
  return rgen->get_random(io_burst_); }

std::pair<prc::state_transition, int> prc::stochastic_process::__execute__(int max_time) {
  // if exhausted cpu burst -> new cpu burst
  if( current_cpu_burst_ == 0 ) {
    current_cpu_burst_ = rgen->get_random( cpu_burst_ );
    current_cpu_burst_ = std::min( current_cpu_burst_, total_cpu_ - get_cpu_used() );
    TLOG << timer() << ": " << *this << " generated cpu burst of " << current_cpu_burst_;
  }

  int run_for = std::min(max_time, current_cpu_burst_);
  current_cpu_burst_ -= run_for;

  TLOG << timer() << ": " << *this << " runs for " << run_for << " (until " << timer() + run_for << ')';

  state_transition t;
  if( run_for == total_cpu_ - get_cpu_used() )
    t = FINISH;
  else if( 0 == current_cpu_burst_ )
    t = BLOCK;
  else
    t = PREEMPT;

  return std::pair<state_transition, int>(t, run_for); }

//========= io =================//

std::ostream & operator<<(std::ostream & os, prc::process_state const & p) {
  os << '[';  p.send_to_stream(os);  return os << ']'; }

void prc::process_state::send_to_stream(std::ostream &os) const {
  os << "p " << state_string(get_state()); }

void prc::process_core::send_to_stream(std::ostream &os) const {
  os << 'p' << get_pid()
     << ", " << state_string(get_state())
     << ", ctotal=" << estimate_cpu_time()
     << ", cused=" << get_cpu_used(); }

std::ostream & operator<<(std::ostream & os, prc::process_core const & p) {
  os << "[p" << p.get_pid()
     << state_string(p.get_state()) 
     << ", cused=" << p.get_cpu_used() << ']';
}
