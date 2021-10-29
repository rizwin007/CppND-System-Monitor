#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

 private:
  long m_idleJiffies;
  long m_totalJiffies;
};

#endif