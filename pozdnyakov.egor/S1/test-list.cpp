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

BOOST_AUTO_TEST_SUITE_END()
