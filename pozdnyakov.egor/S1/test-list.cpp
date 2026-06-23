#include <boost/test/unit_test.hpp>
#include "list.hpp"

using namespace pozdnyakov;

BOOST_AUTO_TEST_SUITE(ListTests)

BOOST_AUTO_TEST_CASE(testListInsertAndIterators)
{
  List< int > list;
  BOOST_CHECK(list.empty());

  list.pushFront(10);
  BOOST_CHECK_EQUAL(list.front(), 10);

  list.insertAfter(list.begin(), 20);
  list.pushBack(30);

  const int expected[] = {10, 20, 30};
  size_t i = 0;
  for (auto it = list.cbegin(); it != list.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i]);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(testListCopyExceptionSafety)
{
  List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);

  List< int > list2 = list1;
  BOOST_CHECK(!list2.empty());
  BOOST_CHECK_EQUAL(list2.front(), 1);

  list2.insertAfter(list2.begin(), 3);

  auto it = list1.cbegin();
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
}

BOOST_AUTO_TEST_CASE(testListSplice)
{
  List< int > list1;
  list1.pushBack(1);
  list1.pushBack(2);

  List< int > list2;
  list2.pushBack(3);
  list2.pushBack(4);

  list1.splice(list1.cend(), list2);

  BOOST_CHECK(list2.empty());
  const int expected[] = {1, 2, 3, 4};
  size_t i = 0;
  for (auto it = list1.cbegin(); it != list1.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListSpliceSingle)
{
  List< int > list1;
  list1.pushBack(1);
  list1.pushBack(2);

  List< int > list2;
  list2.pushBack(3);
  list2.pushBack(4);

  list1.splice(list1.begin(), list2, list2.begin());

  const int expected1[] = {3, 1, 2};
  size_t i = 0;
  for (auto it = list1.cbegin(); it != list1.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected1[i++]);
  }

  const int expected2[] = {4};
  size_t j = 0;
  for (auto it = list2.cbegin(); it != list2.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected2[j++]);
  }
}

BOOST_AUTO_TEST_CASE(testListMerge)
{
  List< int > list1;
  list1.pushBack(1);
  list1.pushBack(3);
  list1.pushBack(5);

  List< int > list2;
  list2.pushBack(2);
  list2.pushBack(4);
  list2.pushBack(6);

  list1.merge(list2);

  BOOST_CHECK(list2.empty());
  const int expected[] = {1, 2, 3, 4, 5, 6};
  size_t i = 0;
  for (auto it = list1.cbegin(); it != list1.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListSort)
{
  List< int > list;
  list.pushBack(4);
  list.pushBack(2);
  list.pushBack(5);
  list.pushBack(1);
  list.pushBack(3);

  list.sort();

  const int expected[] = {1, 2, 3, 4, 5};
  size_t i = 0;
  for (auto it = list.cbegin(); it != list.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_CASE(testListPartition)
{
  List< int > list;
  list.pushBack(1);
  list.pushBack(2);
  list.pushBack(3);
  list.pushBack(4);
  list.pushBack(5);

  list.partition([](const int &val) {
    return val % 2 == 0;
  });

  const int expected[] = {2, 4, 1, 3, 5};
  size_t i = 0;
  for (auto it = list.cbegin(); it != list.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
}

BOOST_AUTO_TEST_SUITE_END()
