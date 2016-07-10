#ifndef TYD_MODELS_TASK_RECORD_HPP
#define TYD_MODELS_TASK_RECORD_HPP

#include <chrono>
#include <iosfwd>
#include <experimental/optional>

namespace tyd::models {

  using task_record_time = std::chrono::system_clock::time_point;
  using effort = std::chrono::hours;

  class task_record
  {
    public:
      task_record(
          const task_record_time& timestamp,
          const task_record_time& start,
          const task_record_time& end,
          const effort& estimate
        );

      task_record(
          const task_record_time& timestamp,
          const task_record_time& start
        );

      task_record_time timestamp() const;
      task_record_time start() const;
      std::experimental::optional<task_record_time> end() const;
      std::experimental::optional<effort> estimate() const;

      void end(const task_record_time& end);
      void estimate(const effort& estimate);

    private:
      task_record_time timestamp_;
      task_record_time start_;
      std::experimental::optional<task_record_time> end_;
      std::experimental::optional<effort> estimate_;

  };

  std::ostream& operator<<(std::ostream& os, const task_record& trecord);

} // tyd::models

#endif
