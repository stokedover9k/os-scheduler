#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <list>
#include <utility>
#include <climits>

#include "process.h"

namespace sch
{
  typedef prc::process_core process;

  //==== scheduler ====//
  struct scheduler {   //
  //===================//
    virtual void add_process( process * ) = 0;

    // returns a pair: (dispatched process, (transition upon end of execution, time in execution))
    // if no process is dispatched, pair.first is NULL
    virtual std::pair<sch::process *, std::pair<prc::state_transition, int> > dispatch() = 0;
  }; //end: scheduler -//



  typedef std::list<process *> process_queue;

  //============ policy_scheduler ==============//
  template <typename schedule_policy>           //
  struct policy_scheduler : public scheduler {  //
  //============================================//
    policy_scheduler(schedule_policy policy);
    virtual void add_process( process * );
    virtual std::pair<sch::process *, std::pair<prc::state_transition, int> > dispatch();
  private:
    schedule_policy policy;
    process_queue processes;
  }; //end: policy_scheduler -------------------//



  // templated factory function for creating policy schedulers
  template <typename T>
  policy_scheduler<T> * create_policy_scheduler(T const &policy);



  //======== FCFS_policy ==========//  first come, first serve
  struct fcfs_policy {             //
  //===============================//
    void add_process(process_queue & q, process *p) const;
    std::pair<prc::state_transition, int> run(process *p) const;
  }; //end: fcfs_policy -----------//
};

//================ TEMPLATE IMPLEMENTATION ===================//

template <typename schedule_policy>
sch::policy_scheduler<schedule_policy>::policy_scheduler( schedule_policy sp )
  : policy(sp) { }

template <typename schedule_policy>
void sch::policy_scheduler<schedule_policy>::add_process( process * p ) {
  policy.add_process( processes, p );
}

template <typename schedule_policy>
std::pair<sch::process *, std::pair<prc::state_transition, int> > sch::policy_scheduler<schedule_policy>::dispatch() {
  if( processes.size() == 0 )
    return std::pair<sch::process *, std::pair<prc::state_transition, int> >(NULL, std::make_pair(prc::ARRIVE, 0));
  process * p = processes.front();
  processes.pop_front();
  auto res = policy.run(p);
  return std::make_pair(p, res);
}

template <typename T>
sch::policy_scheduler<T> * sch::create_policy_scheduler(T const &policy) {
  return new policy_scheduler<T>(policy);
}

#endif //__SCHEDULER_H__
