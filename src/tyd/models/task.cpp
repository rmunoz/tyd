#include "models/task.hpp"

#include <algorithm>
#include <numeric>
#include <map>
#include <ostream>

namespace tyd::models {

  std::string to_string(const tyd::models::task_size& size)
  {
    static std::map<tyd::models::task_size, std::string>
      task_sizes_strings = {
          { tyd::models::task_size::XS, "XS" },
          { tyd::models::task_size::S, "S" },
          { tyd::models::task_size::M, "M" },
          { tyd::models::task_size::L, "L" },
          { tyd::models::task_size::XL, "XL" },
          { tyd::models::task_size::XXL, "XXL" },
        };

    return task_sizes_strings.at(size);
  }

  tyd::models::task_size to_task_size(const std::string& size_string)
  {
    static std::map<std::string, tyd::models::task_size>
      task_sizes = {
          { "XS", tyd::models::task_size::XS },
          { "S", tyd::models::task_size::S },
          { "M", tyd::models::task_size::M },
          { "L", tyd::models::task_size::L },
          { "XL", tyd::models::task_size::XL },
          { "XXL", tyd::models::task_size::XXL },
        };

    return task_sizes.at(size_string);
  }

  task::task(
      const task_id& id,
      const std::string& name,
      const effort& estimate,
      const task_size& size
    ) :
      id_(id),
      name_(name),
      estimate_(estimate),
      size_(size),
      records_()
  {
  }

  task_id task::id() const
  {
    return id_;
  }

  std::string task::name() const
  {
    return name_;
  }

  effort task::estimate() const
  {
    return estimate_;
  }

  task_size task::size() const
  {
    return size_;
  }

  const task_records& task::records() const
  {
    return records_;
  }

  task_record& task::open_record(const task_record_time& start)
  {
    records_.emplace_back(std::chrono::system_clock::now(), start);

    return records_.back();
  }

  task_record& task::close_record(
      const task_record_time& end,
      const effort& reestimate
    )
  {
    auto& current = records_.back();
    current.end(end);
    current.estimate(reestimate);

    return current;
  }

  std::ostream& operator<<(std::ostream& os, const task& task)
  {
    os << "\n\tTask";
    os << "\n\t\tid: " << task.id();
    os << "\n\t\tname: " << task.name();
    os << "\n\t\testimate: "
      << std::chrono::duration_cast<std::chrono::hours>(task.estimate()).count()
      << " hours";
    os << "\n\t\tsize: " << to_string(task.size());

    os << "\n\t\ttask records: \n";
    std::for_each(
        std::begin(task.records()),
        std::end(task.records()),
        [&](const task_record& trecord)
        {
          os << trecord;
        }
      );

    return os;
  }

  bool operator==(const task& lhs, const task& rhs)
  {
    return (lhs.id() == rhs.id());
  }

  bool operator<(const task& lhs, const task& rhs)
  {
    return (lhs.id() < rhs.id());
  }

  effort get_invested_effort(const task& task)
  {
    return std::accumulate(
        std::begin(task.records()),
        std::end(task.records()),
        0min,
        [](const effort& invested, const tyd::models::task_record& elem)
        {
          auto end = elem.end().value_or(std::chrono::system_clock::now());
          return invested + std::chrono::duration_cast<effort>(
              end - elem.start()
            );
        }
      );
  };

} // tyd::models
