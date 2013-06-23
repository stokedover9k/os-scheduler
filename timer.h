#ifndef __TIMER_H__
#define __TIMER_H__

//======= general_clock =======//
struct general_clock {         //
//=============================//
  virtual int operator()() const = 0;  // returns current time
}; //end: general_clock -------//

//============ controlled_clock ===================//
struct controlled_clock : public general_clock {   //
//=================================================//
  controlled_clock(int start_time = 0);

  void add(int t);
  void set(int t);
  virtual int operator()() const;

private:
  int current_time_;
}; //end: controlled_clock ------------------------//

#endif //__TIMER_H__
