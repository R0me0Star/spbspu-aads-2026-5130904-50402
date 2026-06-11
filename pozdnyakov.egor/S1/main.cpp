#include <exception>
#include <iostream>
#include <string>
#include "list.hpp"
#include "process.hpp"

int main()
{
  try {
    pozdnyakov::List< pozdnyakov::NamedSequence > sequences;
    pozdnyakov::LIter< pozdnyakov::NamedSequence > sequenceTail = sequences.end();

    std::string name;

    while (std::cin >> name) {
      if (sequences.empty()) {
        sequences.pushFront({name, pozdnyakov::List< pozdnyakov::ValueType >()});
        sequenceTail = sequences.begin();
      } else {
        sequences.insertAfter(sequenceTail, {name, pozdnyakov::List< pozdnyakov::ValueType >()});
        ++sequenceTail;
      }

      pozdnyakov::LIter< pozdnyakov::ValueType > valueTail = sequenceTail->second.end();
      pozdnyakov::ValueType value = 0;

      while (std::cin >> value) {
        if (sequenceTail->second.empty()) {
          sequenceTail->second.pushFront(value);
          valueTail = sequenceTail->second.begin();
        } else {
          sequenceTail->second.insertAfter(valueTail, value);
          ++valueTail;
        }
      }

      if (std::cin.eof()) {
        break;
      }

      std::cin.clear();
    }

    if (sequences.empty()) {
      std::cout << 0 << "\n";
      return 0;
    }

    bool firstName = true;
    for (auto it = sequences.cbegin(); it != sequences.cend(); ++it) {
      if (!firstName) {
        std::cout << " ";
      }
      std::cout << it->first;
      firstName = false;
    }
    std::cout << "\n";

    const pozdnyakov::List< pozdnyakov::List< pozdnyakov::ValueType > > rows =
        pozdnyakov::buildInterleavedRows(sequences);

    for (auto rowIterator = rows.cbegin(); rowIterator != rows.cend(); ++rowIterator) {
      bool firstElement = true;
      for (auto elementIterator = rowIterator->cbegin(); elementIterator != rowIterator->cend(); ++elementIterator) {
        if (!firstElement) {
          std::cout << " ";
        }
        std::cout << *elementIterator;
        firstElement = false;
      }
      std::cout << "\n";
    }

    const pozdnyakov::List< pozdnyakov::ValueType > sums = pozdnyakov::calculateSums(rows);

    if (sums.empty()) {
      std::cout << 0 << "\n";
    } else {
      bool firstSum = true;
      for (auto sumIterator = sums.cbegin(); sumIterator != sums.cend(); ++sumIterator) {
        if (!firstSum) {
          std::cout << " ";
        }
        std::cout << *sumIterator;
        firstSum = false;
      }
      std::cout << "\n";
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
