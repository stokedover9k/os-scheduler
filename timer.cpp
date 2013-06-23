#include "timer.h"

#include <iostream>

//===== controlled_clock =====//

controlled_clock::controlled_clock(int start) :
  current_time_(start) { }

void controlled_clock::add(int t) {
  current_time_ += t; }

void controlled_clock::set(int t) {
  current_time_ = t; }

int controlled_clock::operator()() const {
  return current_time_; }