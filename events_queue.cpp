#include "events_queue.h"

des::event::event(
  int _time,
  process * _proc,
  prc::state_transition _transition)
  : time(_time),
    proc(_proc),
    transition(_transition) { }

int des::event::get_time() const {
  return time; }
des::process * des::event::get_process() const {
  return proc; }
prc::state_transition des::event::get_transition() const {
  return transition; }



bool des::time_order::operator()(event const & a, event const & b) {
  return a.get_time() < b.get_time(); }



void des::events_queue::push(event const &e) {
  contents.insert(e); }
void des::events_queue::push(int time, process * p, prc::state_transition transition) {
  contents.insert( event(time, p, transition) ); }

des::event des::events_queue::pop() {
  event e(peek());
  contents.erase( contents.begin() );
  return e; }

des::event const& des::events_queue::peek() const {
  return *contents.begin(); }

size_t des::events_queue::size() const { 
  return contents.size(); }