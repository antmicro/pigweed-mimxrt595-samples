#pragma once


namespace sample {

class SimpleCounter {
 public:
  SimpleCounter();
  SimpleCounter(int number);

  // Returns the current count value;
  int GetCount() const;

  // Adds number to the count.
  void Add(int number);

 private:
  int count_;
};
}  // namespace sample
