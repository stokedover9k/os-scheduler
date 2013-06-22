#include "timer.h"

//===== controlled_clock =====//

controlled_clock::controlled_clock(int start) :
  current_time_(start) { }

void controlled_clock::add_time(int t) {
  current_time_ += t; }

void controlled_clock::set_time(int t) {
  current_time_ = t; }

int controlled_clock::get_time() const {
  return current_time_; }