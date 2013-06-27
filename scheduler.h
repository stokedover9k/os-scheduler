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

    // returns a pair: (dispatched process, time in execution)
    // if no process is dispatched, pair.first is NULL
    virtual std::pair<process *, int> dispatch() = 0;
  }; //end: scheduler -//



  typedef std::list<process *> process_queue;

  //============ policy_scheduler ==============//
  template <typename schedule_policy>           //
  struct policy_scheduler : public scheduler {  //
  //============================================//
    policy_scheduler(schedule_policy policy);
    virtual void add_process( process * );
    virtual std::pair<process *, int> dispatch();
  private:
    schedule_policy policy;
    process_queue processes;
  }; //end: policy_scheduler -------------------//



  //======== FCFS_policy ==========//  first come, first serve
  struct fcfs_policy {             //
  //===============================//
    void add_process(process_queue & q, process *p) const;
    int run(process *p) const;
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
std::pair<sch::process *, int> sch::policy_scheduler<schedule_policy>::dispatch() {
  if( processes.size() == 0 )
    return std::make_pair(NULL, 0);
  process * p = processes.front();
  processes.pop_front();
  int ran_for = policy.run(p);
  return std::make_pair(p, ran_for);
}

#endif //__SCHEDULER_H__
