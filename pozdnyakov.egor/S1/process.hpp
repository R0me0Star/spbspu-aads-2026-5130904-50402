#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <cstddef>
#include <string>
#include <utility>
#include "list.hpp"

namespace pozdnyakov
{

  using ValueType = std::size_t;
  using NamedSequence = std::pair< std::string, List< ValueType > >;

  List< List< ValueType > > buildInterleavedRows(const List< NamedSequence > &sequences);
  List< ValueType > calculateSums(const List< List< ValueType > > &rows);

}

#endif
