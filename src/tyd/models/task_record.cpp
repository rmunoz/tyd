#include "models/task_record.hpp"

#include <ostream>
#include <iomanip>

namespace tyd::models {

  task_record::task_record(
      const task_record_time& timestamp,
      const task_record_time& start,
      const task_record_time& end,
      const effort& estimate
    ) :
      timestamp_(timestamp),
      start_(start),
      end_(end),
      estimate_(estimate)
  {
  }

  task_record::task_record(
      const task_record_time& timestamp,
      const task_record_time& start
    ) :
      timestamp_(timestamp),
      start_(start),
      end_(),
      estimate_()
  {
  }

  task_record_time task_record::timestamp() const
  {
    return timestamp_;
  }

  task_record_time task_record::start() const
  {
    return start_;
  }

  std::experimental::optional<task_record_time> task_record::end() const
  {
    return end_;
  }

  std::experimental::optional<effort> task_record::estimate() const
  {
    return estimate_;
  }

  void task_record::end(const task_record_time& end)
  {
    end_ = end;
  }

  void task_record::estimate(const effort& estimate)
  {
    estimate_ = estimate;
  }

  std::ostream& operator<<(std::ostream& os, const task_record& trecord)
  {
    auto timestamp = std::chrono::system_clock::to_time_t(trecord.timestamp());
    os << "\n\t\t\ttimestamp: ";
    os << std::put_time(std::localtime(&timestamp), "%F %T");

    auto start = std::chrono::system_clock::to_time_t(trecord.start());
    os << "\n\t\t\tstart: ";
    os << std::put_time(std::localtime(&start), "%F %T");

    os << "\n\t\t\tend: ";
    if(trecord.end())
    {
      auto end = std::chrono::system_clock::to_time_t(trecord.end().value());
      os << std::put_time(std::localtime(&end), "%F %T");
    }
    else
    {
      os << "current";
    }

    os << "\n\t\t\testimate: ";
    if(trecord.estimate())
    {
      os << trecord.estimate().value().count() << "hours";
    }
    else
    {
      os << "not set";
    }

    return os;
  }

} // namespace tyd::models
