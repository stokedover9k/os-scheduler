#include "process.h"

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
    case ARRIVE:    return ABSTRACT;
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

prc::state prc::process_core::get_state() const {
  return state_; }

void prc::process_core::transition( state_transition t ) {
  if( get_state() != transition_from(t) )
    throw_invalid_transition_value(t, "process_core::transition");
  state_ = transition_to(t); }
