#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include "list.hpp"
#include "process.hpp"

int main()
{
  try {
    pozdnyakov::List< pozdnyakov::NamedSequence > tempSequences;
    std::string name;

    while (std::cin >> name) {
      pozdnyakov::List< pozdnyakov::ValueType > tempValues;
      pozdnyakov::ValueType value = 0;

      while (std::cin >> value) {
        tempValues.pushFront(value);
      }

      if (!std::cin.eof()) {
        std::cin.clear();
      }

      pozdnyakov::List< pozdnyakov::ValueType > values;
      for (auto it = tempValues.begin(); it != tempValues.end(); ++it) {
        values.pushFront(*it);
      }

      tempSequences.pushFront({name, std::move(values)});

      if (std::cin.eof()) {
        break;
      }
    }

    pozdnyakov::List< pozdnyakov::NamedSequence > sequences;
    for (auto it = tempSequences.begin(); it != tempSequences.end(); ++it) {
      sequences.pushFront(std::move(*it));
    }

    if (sequences.empty()) {
      std::cout << 0 << "\n";
      return 0;
    }

    bool firstName = true;
    for (auto it = sequences.begin(); it != sequences.end(); ++it) {
      if (!firstName) {
        std::cout << " ";
      }
      std::cout << it->first;
      firstName = false;
    }
    std::cout << "\n";

    pozdnyakov::List< pozdnyakov::List< pozdnyakov::ValueType > > rows = pozdnyakov::buildInterleavedRows(sequences);

    for (auto rowIterator = rows.begin(); rowIterator != rows.end(); ++rowIterator) {
      bool firstElement = true;
      for (auto elementIterator = rowIterator->begin(); elementIterator != rowIterator->end(); ++elementIterator) {
        if (!firstElement) {
          std::cout << " ";
        }
        std::cout << *elementIterator;
        firstElement = false;
      }
      std::cout << "\n";
    }

    pozdnyakov::List< pozdnyakov::ValueType > sums = pozdnyakov::calculateSums(rows);

    if (sums.empty()) {
      std::cout << 0 << "\n";
    } else {
      bool firstSum = true;
      for (auto sumIterator = sums.begin(); sumIterator != sums.end(); ++sumIterator) {
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
