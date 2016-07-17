#ifndef TYD_MODELS_TASK_HPP
#define TYD_MODELS_TASK_HPP

#include <iosfwd>
#include <string>
#include <vector>
#include <chrono>

#include "models/task_record.hpp"

namespace tyd::models {

  using namespace std::chrono_literals;

  using task_id = unsigned int;
  using task_records = std::vector<task_record>;

  enum class task_size
  {
    XS = 1,
    S,
    M,
    L,
    XL,
    XXL
  };

  std::string to_string(const tyd::models::task_size& size);
  tyd::models::task_size to_task_size(const std::string& size_string);

  class task
  {
    public:
      task(
          const task_id& id,
          const std::string& name,
          const effort& estimate,
          const task_size& size
        );

      task_id id() const;
      std::string name() const;
      effort estimate() const;
      task_size size() const;
      const task_records& records() const;

      task_record& open_record(const task_record_time& start);
      task_record& close_record(
          const task_record_time& end,
          const effort& reestimate
        );

    protected:
      task_id id_;
      std::string name_;
      effort estimate_;
      task_size size_;
      task_records records_;

  };

  std::ostream& operator<<(std::ostream& os, const task& task);
  bool operator<(const task& lhs, const task& rhs);
  bool operator==(const task& lhs, const task& rhs);

  effort get_invested_effort(const task& task);


} // namespace tyd::models

#endif
