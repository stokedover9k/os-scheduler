#include "scheduler.h"

void sch::fcfs_policy::add_process(process_queue & q, sch::process *p) const {
  q.push_back(p);
}

std::pair<prc::state_transition, int> sch::fcfs_policy::run(sch::process *p) const {
  return p->run(INT_MAX);
}

void sch::lcfs_policy::add_process(process_queue & q, sch::process *p) const {
  q.push_front(p);
}

sch::rr_policy::rr_policy(int q) : quantum(q) { }

std::pair<prc::state_transition, int> sch::rr_policy::run(sch::process *p) const {
  return p->run(quantum);
}

void sch::sjf_policy::add_process(process_queue & q, sch::process *p) const {
  auto i = q.begin();
  for( ; i != q.end() && (*i)->estimate_cpu_time() <= p->estimate_cpu_time(); ++i )
    ;
  q.insert(i, p);
}
