#ifndef __EVENTS_QUEUE_H__
#define __EVENTS_QUEUE_H__

#include <set>

#include "process.h"

namespace des
{

  typedef prc::process_core process;



  //==== event ===================//
  struct event {                  //
  //==============================//
    event(int time, process *, prc::state_transition);

    int get_time() const;
    process * get_process() const;
    prc::state_transition get_transition() const;

  private:
    int time;
    process * proc;
    prc::state_transition transition;
  }; //end: event ----------------//



  //==== time_oreder =====//
  struct time_order {     //
  //======================//
    bool operator()(event const & a, event const & b);
  }; //end: time_order ---//






  //======== events_queue ========//
  struct events_queue {           //
  //==============================//
    void push(event const& e);
    void push(int time, process * p, prc::state_transition);

    event pop();
    event const& peek() const;

    size_t size() const;

  protected:
    typedef std::multiset<
      event,
      time_order
    > container_type;

  private:
    container_type contents;
  }; //end: events_queue ---------//
};

#endif //__EVENTS_QUEUE_H__
