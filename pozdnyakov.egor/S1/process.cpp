#include "process.hpp"
#include <limits>
#include <stdexcept>

namespace pozdnyakov
{

  List< List< ValueType > > buildInterleavedRows(const List< NamedSequence > &sequences)
  {
    List< List< ValueType > > result;

    List< LCIter< ValueType > > iterators;
    List< LCIter< ValueType > > endIterators;

    LIter< LCIter< ValueType > > iteratorsTail = iterators.end();
    LIter< LCIter< ValueType > > endIteratorsTail = endIterators.end();

    for (auto it = sequences.cbegin(); it != sequences.cend(); ++it) {
      if (iterators.empty()) {
        iterators.pushFront(it->second.cbegin());
        endIterators.pushFront(it->second.cend());
        iteratorsTail = iterators.begin();
        endIteratorsTail = endIterators.begin();
      } else {
        iterators.insertAfter(iteratorsTail, it->second.cbegin());
        endIterators.insertAfter(endIteratorsTail, it->second.cend());
        ++iteratorsTail;
        ++endIteratorsTail;
      }
    }

    LIter< List< ValueType > > rowsTail = result.end();

    while (true) {
      bool elementsLeft = false;

      List< ValueType > currentRow;
      LIter< ValueType > currentRowTail = currentRow.end();

      auto it = iterators.begin();
      auto endIt = endIterators.begin();

      for (; it != iterators.end() && endIt != endIterators.end(); ++it, ++endIt) {
        if (*it != *endIt) {
          elementsLeft = true;
          const ValueType value = *(*it);

          if (currentRow.empty()) {
            currentRow.pushFront(value);
            currentRowTail = currentRow.begin();
          } else {
            currentRow.insertAfter(currentRowTail, value);
            ++currentRowTail;
          }

          ++(*it);
        }
      }

      if (!elementsLeft) {
        break;
      }

      if (result.empty()) {
        result.pushFront(currentRow);
        rowsTail = result.begin();
      } else {
        result.insertAfter(rowsTail, currentRow);
        ++rowsTail;
      }
    }

    return result;
  }

  List< ValueType > calculateSums(const List< List< ValueType > > &rows)
  {
    List< ValueType > sums;
    LIter< ValueType > sumsTail = sums.end();

    for (auto rowIterator = rows.cbegin(); rowIterator != rows.cend(); ++rowIterator) {
      ValueType currentRowSum = 0;

      for (auto elementIterator = rowIterator->cbegin(); elementIterator != rowIterator->cend(); ++elementIterator) {
        const ValueType value = *elementIterator;
        if (currentRowSum > std::numeric_limits< ValueType >::max() - value) {
          throw std::overflow_error("Sum calculation overflow");
        }
        currentRowSum += value;
      }

      if (sums.empty()) {
        sums.pushFront(currentRowSum);
        sumsTail = sums.begin();
      } else {
        sums.insertAfter(sumsTail, currentRowSum);
        ++sumsTail;
      }
    }

    return sums;
  }

}
