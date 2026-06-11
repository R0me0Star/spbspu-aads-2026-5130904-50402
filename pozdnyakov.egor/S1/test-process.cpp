#include <cstddef>
#include <limits>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "list.hpp"
#include "process.hpp"

using namespace pozdnyakov;

List< ValueType > makeList(const ValueType *values, std::size_t size)
{
  List< ValueType > list;
  LIter< ValueType > tail = list.end();
  for (std::size_t i = 0; i < size; ++i) {
    if (list.empty()) {
      list.pushFront(values[i]);
      tail = list.begin();
    } else {
      list.insertAfter(tail, values[i]);
      ++tail;
    }
  }
  return list;
}

BOOST_AUTO_TEST_SUITE(ProcessTests)

BOOST_AUTO_TEST_CASE(testEmptySequences)
{
  List< NamedSequence > sequences;

  const List< List< ValueType > > rows = buildInterleavedRows(sequences);
  const List< ValueType > sums = calculateSums(rows);

  BOOST_CHECK(rows.empty());
  BOOST_CHECK(sums.empty());
}

BOOST_AUTO_TEST_CASE(testSequencesWithoutNumbers)
{
  List< NamedSequence > sequences;
  sequences.pushFront({"second", List< ValueType >()});
  sequences.pushFront({"first", List< ValueType >()});

  const List< List< ValueType > > rows = buildInterleavedRows(sequences);
  const List< ValueType > sums = calculateSums(rows);

  BOOST_CHECK(rows.empty());
  BOOST_CHECK(sums.empty());
}

BOOST_AUTO_TEST_CASE(testSingleSequence)
{
  List< NamedSequence > sequences;
  const ValueType values[] = {10, 20};
  sequences.pushFront({"single", makeList(values, 2)});

  const List< List< ValueType > > rows = buildInterleavedRows(sequences);
  const List< ValueType > sums = calculateSums(rows);

  const ValueType expectedSums[] = {10, 20};
  int i = 0;
  for (auto it = sums.cbegin(); it != sums.cend(); ++it) {
    BOOST_CHECK_EQUAL(*it, expectedSums[i++]);
  }
  BOOST_CHECK_EQUAL(i, 2);
}

BOOST_AUTO_TEST_CASE(testProcessLogicOverflow)
{
  List< NamedSequence > sequences;

  const ValueType value2[] = {2};
  sequences.pushFront({"list2", makeList(value2, 1)});

  const ValueType value1[] = {std::numeric_limits< ValueType >::max()};
  sequences.pushFront({"list1", makeList(value1, 1)});

  const List< List< ValueType > > rows = buildInterleavedRows(sequences);
  BOOST_CHECK_THROW(calculateSums(rows), std::overflow_error);
}

BOOST_AUTO_TEST_SUITE_END()
