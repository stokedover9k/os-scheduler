#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdexcept>
#include <sstream>

namespace prc
{
  enum state { 
    READY, EXECUTING, BLOCKED, ABSTRACT, FINISHED };

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

  //====== process_core =======//
  struct process_core {        //
  //===========================//

    state get_state() const;

    void transition( state_transition t );

  private:
    state state_;

  }; //end: process_core ------//

};

#endif //__PROCESS_H__
