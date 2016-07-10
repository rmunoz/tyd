#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "models/task_record.hpp"

#include <ctime>

using namespace std::chrono_literals;

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

TEST(task_records, build_a_task_record_full_ctor)
{
  auto timestamp = std::chrono::system_clock::now();
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);
  auto end = make_time_point(2016, 6, 1, 14, 0, 0);
  auto estimate = 12h;

  task_record trecord = { timestamp, start, end, estimate };

  EXPECT_EQ(trecord.timestamp(), timestamp);
  EXPECT_EQ(trecord.start(), start);
  EXPECT_EQ(trecord.end(), end);
  EXPECT_EQ(trecord.estimate(), estimate);
}

TEST(task_records, build_a_task_record_partial_ctor)
{
  auto timestamp = std::chrono::system_clock::now();
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);

  task_record trecord = { timestamp, start };

  EXPECT_EQ(trecord.timestamp(), timestamp);
  EXPECT_EQ(trecord.start(), start);
  EXPECT_FALSE(trecord.end());
  EXPECT_FALSE(trecord.estimate());
}

TEST(task_records, set_end)
{
  auto timestamp = std::chrono::system_clock::now();
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);
  auto end = make_time_point(2016, 6, 1, 14, 0, 0);

  task_record trecord = { timestamp, start };

  EXPECT_EQ(trecord.timestamp(), timestamp);
  EXPECT_EQ(trecord.start(), start);
  EXPECT_FALSE(trecord.end());
  EXPECT_FALSE(trecord.estimate());

  trecord.end(end);

  EXPECT_TRUE(trecord.end());
  EXPECT_EQ(trecord.end(), end);
}

TEST(task_records, set_estimate)
{
  auto timestamp = std::chrono::system_clock::now();
  auto start = make_time_point(2016, 6, 1, 12, 0, 0);
  auto estimate = 12h;

  task_record trecord = { timestamp, start };

  EXPECT_EQ(trecord.timestamp(), timestamp);
  EXPECT_EQ(trecord.start(), start);
  EXPECT_FALSE(trecord.end());
  EXPECT_FALSE(trecord.estimate());

  trecord.estimate(estimate);

  EXPECT_TRUE(trecord.estimate());
  EXPECT_EQ(trecord.estimate(), estimate);
}
