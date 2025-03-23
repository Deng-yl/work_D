#ifndef __STOPWATCH__
#define __STOPWATCH__
#include <any>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <thread>
#include <sys/time.h>

class StopWatch
{
public:
  // timer for benchmark time useage
  void tic()//记录当前时间
  {
    struct timeval tp;
    gettimeofday(&tp, NULL);//获取当前的系统时间，并将结果存储在tp中，NULL表示不使用时区信息
    _sec = tp.tv_sec;       //存储当前时间秒数
    _usec = tp.tv_usec;     //存储当前时间微秒数
  }

  double toc()//返回当前时间与tic()记录的时间的时间差
  { // return ms
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (((double)(tp.tv_sec - _sec)) * 1000.0 + (tp.tv_usec - _usec) / 1000.0);//总的经过时间
  }

private:
  double _sec, _usec;
};
#endif
