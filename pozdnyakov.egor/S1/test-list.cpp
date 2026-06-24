#include <cstddef>
#include <boost/test/unit_test.hpp>
#include "list.hpp"

using namespace pozdnyakov;

BOOST_AUTO_TEST_SUITE(ListTests)

BOOST_AUTO_TEST_CASE(testListPushFrontAndIterators)
{
  List< int > list;
  BOOST_CHECK(list.empty());

  list.pushFront(30);
  list.pushFront(20);
  list.pushFront(10);

  BOOST_CHECK(!list.empty());
  BOOST_CHECK_EQUAL(list.front(), 10);

  const int expected[] = {10, 20, 30};
  size_t i = 0;
  for (auto it = list.begin(); it != list.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i]);
    ++i;
  }
  BOOST_CHECK_EQUAL(i, 3);
}

BOOST_AUTO_TEST_CASE(testListCopyExceptionSafety)
{
  List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);

  List< int > list2 = list1;
  BOOST_CHECK(!list2.empty());
  BOOST_CHECK_EQUAL(list2.front(), 1);

  list1.pushFront(0);

  auto it = list2.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
}

BOOST_AUTO_TEST_CASE(testListSplice)
{
  List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);

  List< int > list2;
  list2.pushFront(4);
  list2.pushFront(3);

  list1.splice(list1.end(), list2);

  BOOST_CHECK(list2.empty());
  const int expected[] = {1, 2, 3, 4};
  size_t i = 0;
  for (auto it = list1.begin(); it != list1.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListSpliceSingle)
{
  List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);

  List< int > list2;
  list2.pushFront(4);
  list2.pushFront(3);

  list1.splice(list1.begin(), list2, list2.begin());

  const int expected1[] = {3, 1, 2};
  size_t i = 0;
  for (auto it = list1.begin(); it != list1.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected1[i++]);
  }

  const int expected2[] = {4};
  size_t j = 0;
  for (auto it = list2.begin(); it != list2.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected2[j++]);
  }
}

BOOST_AUTO_TEST_CASE(testListMerge)
{
  List< int > list1;
  list1.pushFront(5);
  list1.pushFront(3);
  list1.pushFront(1);

  List< int > list2;
  list2.pushFront(6);
  list2.pushFront(4);
  list2.pushFront(2);

  list1.merge(list2);

  BOOST_CHECK(list2.empty());
  const int expected[] = {1, 2, 3, 4, 5, 6};
  size_t i = 0;
  for (auto it = list1.begin(); it != list1.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListSort)
{
  List< int > list;
  list.pushFront(4);
  list.pushFront(2);
  list.pushFront(5);
  list.pushFront(1);
  list.pushFront(3);

  list.sort();

  const int expected[] = {1, 2, 3, 4, 5};
  size_t i = 0;
  for (auto it = list.begin(); it != list.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListPartition)
{
  List< int > list;
  list.pushFront(1);
  list.pushFront(2);
  list.pushFront(3);
  list.pushFront(4);
  list.pushFront(5);

  list.partition([](const int &val) {
    return val % 2 == 0;
  });

  const int expected[] = {4, 2, 5, 3, 1};
  size_t i = 0;
  for (auto it = list.begin(); it != list.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
  BOOST_CHECK_EQUAL(i, 5);
}

BOOST_AUTO_TEST_SUITE_END()
