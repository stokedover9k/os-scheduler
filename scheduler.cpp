#include "scheduler.h"

void sch::fcfs_policy::add_process(process_queue & q, sch::process *p) const {
  q.push_back(p);
}

std::pair<prc::state_transition, int> sch::fcfs_policy::run(sch::process *p) const {
  return p->run(INT_MAX);
}

