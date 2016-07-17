#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "models/task.hpp"

using namespace tyd::models;

std::chrono::system_clock::time_point make_time_point(
    unsigned int year,
    unsigned int month,
    unsigned int day,
    unsigned int hours,
    unsigned int minutes,
    unsigned int seconds
    )
{
  std::tm time_struct;

  time_struct.tm_year = year - 1900;
  time_struct.tm_mon = month - 1;
  time_struct.tm_mday = day;
  time_struct.tm_hour = hours;
  time_struct.tm_min = minutes;
  time_struct.tm_sec = seconds;
  time_struct.tm_isdst = -1; // unknown

  return std::chrono::system_clock::from_time_t(std::mktime(&time_struct));
}

TEST(tasks, build_a_task_without_task_records)
{
  task_id id = 42;
  std::string name = "task@project";
  effort estimate = 12h;
  task_size size = task_size::L;

  task task = { id, name, estimate, size };

  EXPECT_EQ(task.id(), id);
  EXPECT_EQ(task.name(), name);
  EXPECT_EQ(task.estimate(), estimate);
  EXPECT_EQ(task.size(), size);
  EXPECT_TRUE(task.records().empty());
}

TEST(tasks, open_record)
{
  task task = { 42, "task@project", 12h, task_size::XL };
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);

  EXPECT_TRUE(task.records().empty());

  task.open_record(start);

  EXPECT_FALSE(task.records().empty());

  auto last_task_record = task.records().back();
  EXPECT_EQ(last_task_record.start(), start);
}

TEST(tasks, close_record)
{
  task task = { 42, "task@project", 12h, task_size::XL };
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);
  auto end = make_time_point(2016, 6, 1, 14, 0, 0);
  effort reestimate = 10h;

  EXPECT_TRUE(task.records().empty());

  task.open_record(start);
  task.close_record(end, reestimate);

  EXPECT_FALSE(task.records().empty());

  auto last_task_record = task.records().back();
  EXPECT_EQ(last_task_record.start(), start);
  EXPECT_EQ(last_task_record.end(), end);
  EXPECT_EQ(last_task_record.estimate(), reestimate);
}

TEST(tasks, add_three_task_records)
{
  task task = { 42, "task@project", 12h, task_size::XL };

  EXPECT_TRUE(task.records().empty());

  task.open_record(make_time_point(2016, 6, 1, 12, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 14, 0, 0), 10h);

  task.open_record(make_time_point(2016, 6, 1, 14, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 16, 0, 0), 8h);

  task.open_record(make_time_point(2016, 6, 1, 16, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 18, 0, 0), 6h);

  size_t expected_size = 3;
  EXPECT_EQ(task.records().size(), expected_size);
}

TEST(tasks, ostream_operator_task_without_task_records)
{
  std::string expected_output =
    "\n\tTask"
    "\n\t\tid: 42"
    "\n\t\tname: task@project"
    "\n\t\testimate: 12 hours"
    "\n\t\tsize: XL"
    "\n\t\ttask records: \n"
    "";

  std::ostringstream oss;

  task task = { 42, "task@project", 12h, task_size::XL };

  oss << task;

  EXPECT_EQ(oss.str(), expected_output);
}

TEST(tasks, task_size_to_string)
{
  using namespace tyd::models;

  EXPECT_EQ("XS", to_string(task_size::XS));
  EXPECT_EQ("S", to_string(task_size::S));
  EXPECT_EQ("M", to_string(task_size::M));
  EXPECT_EQ("L", to_string(task_size::L));
  EXPECT_EQ("XL", to_string(task_size::XL));
  EXPECT_EQ("XXL", to_string(task_size::XXL));
}

TEST(tasks, task_size_to_task_size)
{
  using namespace tyd::models;

  EXPECT_EQ(task_size::XS, to_task_size(to_string(task_size::XS)));
  EXPECT_EQ(task_size::S, to_task_size(to_string(task_size::S)));
  EXPECT_EQ(task_size::M, to_task_size(to_string(task_size::M)));
  EXPECT_EQ(task_size::L, to_task_size(to_string(task_size::L)));
  EXPECT_EQ(task_size::XL, to_task_size(to_string(task_size::XL)));
  EXPECT_EQ(task_size::XXL, to_task_size(to_string(task_size::XXL)));

  ASSERT_THROW(to_task_size("foobar"), std::out_of_range);
}

TEST(tasks, get_invested_effort)
{
  task task = { 42, "task@project", 12h, task_size::XL };

  EXPECT_TRUE(task.records().empty());

  task.open_record(make_time_point(2016, 6, 1, 12, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 14, 0, 0), 10h);

  task.open_record(make_time_point(2016, 6, 1, 15, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 18, 30, 0), 8h);

  task.open_record(make_time_point(2016, 6, 1, 19, 0, 0));
  task.close_record(make_time_point(2016, 6, 1, 20, 30, 0), 6h);

  effort expected_effort = 7h;
  EXPECT_EQ(tyd::models::get_invested_effort(task), expected_effort);
}
