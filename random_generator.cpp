#include "random_generator.h"

#include "loglib.h"

int looping_random_generator::get_random(int cap) {
  if (current == nums.end())
    current = nums.begin();
  return 1 + *(current++) % cap;
}