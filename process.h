#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdexcept>
#include <sstream>

#ifndef PER_PROC_STATS
#define PER_PROC_STATS (true)
#endif

namespace prc
{
  int get_time();

  enum state { 
    INVALID, READY, EXECUTING, BLOCKED, FINISHED };

  enum state_transition {
    DISPATCH,   // ready -> executing
    PREEMPT,    // executing -> ready
    BLOCK,      // executing -> blocked
    UNBLOCK,    // blocked -> ready
    ARRIVE,     // abstract -> ready
    FINISH };   // executing -> finish

  // return state associated with given transition
  state transition_from( state_transition t );  // state from which process transitions
  state transition_to( state_transition t );    // state into which process transitions

  //====== process_state ======//
  struct process_state {       //
  //===========================//
    state get_state() const;
    void transition( state_transition t );
  private:
    state state_;
    #if PER_PROC_STATS
    int blocked_time_, ready_time_;
    int last_blocked_, last_ready_;
    #endif

  protected: process_state(state s = INVALID);
  }; //end: process_state -----//

  //====== process_core =======================//
  struct process_core : public process_state { //
  //===========================================//
    int get_pid() const;
    int run(int max_time);
  protected:
    virtual int __execute__(int max_time) = 0;
  private: 
    int pid_;
    int cpu_used_;

  protected: process_core(int pid);
  }; //end: process_core ----------------------//

  //===== process =======================//
  struct process : public process_core { //
  //=====================================//

  private:
  }; // end: process --------------------//
};

#endif //__PROCESS_H__
