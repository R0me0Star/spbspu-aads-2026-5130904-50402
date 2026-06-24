#include "process.hpp"
#include <limits>
#include <stdexcept>

namespace pozdnyakov
{

  List< List< ValueType > > buildInterleavedRows(List< NamedSequence > &sequences)
  {
    List< LIter< ValueType > > tempIterators;
    List< LIter< ValueType > > tempEndIterators;

    for (auto it = sequences.begin(); it != sequences.end(); ++it) {
      tempIterators.pushFront(it->second.begin());
      tempEndIterators.pushFront(it->second.end());
    }

    List< LIter< ValueType > > iterators;
    List< LIter< ValueType > > endIterators;

    for (auto it = tempIterators.begin(); it != tempIterators.end(); ++it) {
      iterators.pushFront(*it);
    }
    for (auto it = tempEndIterators.begin(); it != tempEndIterators.end(); ++it) {
      endIterators.pushFront(*it);
    }

    List< List< ValueType > > tempRows;

    while (true) {
      bool elementsLeft = false;
      List< ValueType > tempRow;

      auto it = iterators.begin();
      auto endIt = endIterators.begin();

      for (; it != iterators.end() && endIt != endIterators.end(); ++it, ++endIt) {
        if (*it != *endIt) {
          elementsLeft = true;
          const ValueType value = *(*it);
          tempRow.pushFront(value);
          ++(*it);
        }
      }

      if (!elementsLeft) {
        break;
      }

      List< ValueType > row;
      for (auto rIt = tempRow.begin(); rIt != tempRow.end(); ++rIt) {
        row.pushFront(*rIt);
      }
      tempRows.pushFront(std::move(row));
    }

    List< List< ValueType > > rows;
    for (auto rIt = tempRows.begin(); rIt != tempRows.end(); ++rIt) {
      rows.pushFront(std::move(*rIt));
    }

    return rows;
  }

  List< ValueType > calculateSums(List< List< ValueType > > &rows)
  {
    List< ValueType > tempSums;

    for (auto rowIterator = rows.begin(); rowIterator != rows.end(); ++rowIterator) {
      ValueType currentRowSum = 0;

      for (auto elementIterator = rowIterator->begin(); elementIterator != rowIterator->end(); ++elementIterator) {
        const ValueType value = *elementIterator;
        if (currentRowSum > std::numeric_limits< ValueType >::max() - value) {
          throw std::overflow_error("Sum calculation overflow");
        }
        currentRowSum += value;
      }

      tempSums.pushFront(currentRowSum);
    }

    List< ValueType > sums;
    for (auto sIt = tempSums.begin(); sIt != tempSums.end(); ++sIt) {
      sums.pushFront(*sIt);
    }

    return sums;
  }

}
