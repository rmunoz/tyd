#ifndef TYD_TASKS_MANAGER_HPP
#define TYD_TASKS_MANAGER_HPP

#include <string>
#include <chrono>

#include "models/task.hpp"

namespace tyd::managers {

  models::task& add_task(
      const std::string& name,
      const models::effort& estimate,
      const models::task_size& size
    );

  models::task& find_task(const models::task_id& id);

} // namespace tyd::managers

#endif
