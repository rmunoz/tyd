#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "managers/tasks_manager.hpp"

using namespace tyd::models;

TEST(tasks_manager, add_several_tasks)
{
  std::string name = "task@project";
  effort estimate = 12h;
  task_size size = task_size::L;

  auto task = tyd::managers::add_task(name, estimate, size);

  task_id expected_id = 1;

  EXPECT_EQ(task.id(), expected_id);
  EXPECT_EQ(task.name(), name);
  EXPECT_EQ(task.estimate(), estimate);
  EXPECT_EQ(task.size(), size);

  tyd::managers::add_task(name, estimate, size);
  auto task2 = tyd::managers::add_task(name, estimate, size);

  expected_id = 3;

  EXPECT_EQ(task2.id(), expected_id);
  EXPECT_EQ(task2.name(), name);
  EXPECT_EQ(task2.estimate(), estimate);
  EXPECT_EQ(task2.size(), size);
}

TEST(tasks_manager, find_a_valid_task)
{
  auto expected_task = tyd::managers::add_task(
      "another task@project",
      6h,
      task_size::XXL);

  auto task = tyd::managers::find_task(expected_task.id());

  EXPECT_EQ(task, expected_task);
  EXPECT_EQ(task, expected_task);
}

TEST(tasks_manager, find_an_unregisterd_task)
{
  ASSERT_THROW(
      tyd::managers::find_task(tyd::models::task_id(99999)),
      std::runtime_error
    );
}
