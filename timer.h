#ifndef __TIMER_H__
#define __TIMER_H__

//======== timer ==============//
struct timer {                 //
//=============================//
  virtual int get_time() const = 0;
}; //end: timer ---------------//

//============ controlled_clock==============//
struct controlled_clock : public timer {     //
//===========================================//
  controlled_clock(int start_time = 0);

  void add_time(int t);
  void set_time(int t);
  virtual int get_time() const;

private:
  int current_time_;
}; //end: controlled_clock ------------------//

#endif //__TIMER_H__
