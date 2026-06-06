#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "bstree.hpp"

namespace pozdnyakov
{
  BSTree< int, std::string > createTestTree()
  {
    BSTree< int, std::string > tree;
    tree.push(3, "three");
    tree.push(1, "one");
    tree.push(2, "two");
    tree.push(4, "four");
    return tree;
  }
}

BOOST_AUTO_TEST_SUITE(BSTreeTests)

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
  pozdnyakov::BSTree< int, std::string > tree;
  BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(test_push)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.get(1), "one");

  tree.push(2, "two");
  tree.push(3, "three");
  BOOST_CHECK_EQUAL(tree.get(2), "two");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
}

BOOST_AUTO_TEST_CASE(test_get_const)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");

  const pozdnyakov::BSTree< int, std::string > &ctree = tree;
  BOOST_CHECK_EQUAL(ctree.get(1), "one");
  BOOST_CHECK_EQUAL(ctree.get(2), "two");
}

BOOST_AUTO_TEST_CASE(test_get_non_const)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.get(1) = "ONE";
  BOOST_CHECK_EQUAL(tree.get(1), "ONE");
}

BOOST_AUTO_TEST_CASE(test_get_not_found)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");

  BOOST_CHECK_THROW(tree.get(2), std::out_of_range);
  const pozdnyakov::BSTree< int, std::string > &ctree = tree;
  BOOST_CHECK_THROW(ctree.get(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_push_update_existing)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(1, "ONE");

  BOOST_CHECK_EQUAL(tree.get(1), "ONE");
}

BOOST_AUTO_TEST_CASE(test_drop)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");
  tree.push(3, "three");

  std::string val = tree.drop(2);
  BOOST_CHECK_EQUAL(val, "two");
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
  BOOST_CHECK_THROW(tree.get(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_drop_root_with_two_children)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  tree.drop(2);
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
  BOOST_CHECK_THROW(tree.get(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_copy_constructor)
{
  pozdnyakov::BSTree< int, std::string > tree1;
  tree1.push(1, "one");
  tree1.push(2, "two");
  tree1.push(3, "three");

  pozdnyakov::BSTree< int, std::string > tree2(tree1);

  BOOST_CHECK_EQUAL(tree2.get(1), "one");
  BOOST_CHECK_EQUAL(tree2.get(2), "two");
  BOOST_CHECK_EQUAL(tree2.get(3), "three");

  tree2.get(1) = "ONE";
  BOOST_CHECK_EQUAL(tree1.get(1), "one");
  BOOST_CHECK_EQUAL(tree2.get(1), "ONE");
}

BOOST_AUTO_TEST_CASE(test_copy_assignment)
{
  pozdnyakov::BSTree< int, std::string > tree1;
  tree1.push(1, "one");

  pozdnyakov::BSTree< int, std::string > tree2;
  tree2.push(99, "ninety-nine");

  tree2 = tree1;

  BOOST_CHECK_EQUAL(tree2.get(1), "one");
  BOOST_CHECK_THROW(tree2.get(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_move_constructor)
{
  pozdnyakov::BSTree< int, std::string > tree1;
  tree1.push(1, "one");
  tree1.push(2, "two");

  pozdnyakov::BSTree< int, std::string > tree2(std::move(tree1));

  BOOST_CHECK_EQUAL(tree2.get(1), "one");
  BOOST_CHECK_EQUAL(tree2.get(2), "two");
  BOOST_CHECK_THROW(tree1.get(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_move_assignment)
{
  pozdnyakov::BSTree< int, std::string > tree1;
  tree1.push(1, "one");

  pozdnyakov::BSTree< int, std::string > tree2;
  tree2.push(99, "ninety-nine");

  tree2 = std::move(tree1);

  BOOST_CHECK_EQUAL(tree2.get(1), "one");
  BOOST_CHECK_THROW(tree1.get(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_iterator_traversal)
{
  pozdnyakov::BSTree< int, std::string > tree = pozdnyakov::createTestTree();

  pozdnyakov::BSTIterator< int, std::string > it = tree.begin();

  BOOST_REQUIRE(it != tree.end());
  BOOST_CHECK_EQUAL((*it).first, 1);

  ++it;
  BOOST_REQUIRE(it != tree.end());
  BOOST_CHECK_EQUAL((*it).first, 2);

  ++it;
  BOOST_REQUIRE(it != tree.end());
  BOOST_CHECK_EQUAL((*it).first, 3);

  ++it;
  BOOST_REQUIRE(it != tree.end());
  BOOST_CHECK_EQUAL((*it).first, 4);

  ++it;
  BOOST_CHECK(it == tree.end());
}

BOOST_AUTO_TEST_CASE(test_const_iterator)
{
  pozdnyakov::BSTree< int, std::string > tree = pozdnyakov::createTestTree();

  const pozdnyakov::BSTree< int, std::string > &ctree = tree;
  pozdnyakov::BSTConstIterator< int, std::string > it = ctree.begin();

  BOOST_CHECK(it != ctree.end());
  BOOST_CHECK_EQUAL((*it).first, 1);
  BOOST_CHECK_EQUAL((*it).second, "one");
}

BOOST_AUTO_TEST_CASE(test_iterator_modification)
{
  pozdnyakov::BSTree< int, std::string > tree = pozdnyakov::createTestTree();

  for (pozdnyakov::BSTIterator< int, std::string > it = tree.begin(); it != tree.end(); ++it) {
    if ((*it).first == 1) {
      (*it).second = "ONE";
    }
  }
  BOOST_CHECK_EQUAL(tree.get(1), "ONE");
}

BOOST_AUTO_TEST_CASE(test_height_empty)
{
  pozdnyakov::BSTree< int, std::string > tree;
  BOOST_CHECK_EQUAL(tree.height(), 0);
}

BOOST_AUTO_TEST_CASE(test_height_single)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  BOOST_CHECK_EQUAL(tree.height(), 1);
}

BOOST_AUTO_TEST_CASE(test_height_balanced)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");
  BOOST_CHECK_EQUAL(tree.height(), 2);
}

BOOST_AUTO_TEST_CASE(test_height_degenerate)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");
  tree.push(3, "three");
  tree.push(4, "four");
  BOOST_CHECK_EQUAL(tree.height(), 4);
}

BOOST_AUTO_TEST_CASE(test_height_iterator)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  auto it = tree.begin();
  ++it;
  BOOST_CHECK_EQUAL(tree.height(it), 2);
}

BOOST_AUTO_TEST_CASE(test_rotate_left)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");
  tree.push(3, "three");

  BOOST_CHECK_EQUAL(tree.height(), 3);

  auto it = tree.begin();
  ++it;
  ++it;
  tree.rotateLeft(it);

  BOOST_CHECK_EQUAL(tree.height(), 2);
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(2), "two");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
}

BOOST_AUTO_TEST_CASE(test_rotate_right)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(3, "three");
  tree.push(2, "two");
  tree.push(1, "one");

  BOOST_CHECK_EQUAL(tree.height(), 3);

  auto it = tree.begin();
  ++it;
  ++it;
  tree.rotateRight(it);

  BOOST_CHECK_EQUAL(tree.height(), 2);
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(2), "two");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
}

BOOST_AUTO_TEST_CASE(test_rotate_large_left)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(2, "two");

  BOOST_CHECK_EQUAL(tree.height(), 3);

  auto it = tree.begin();
  ++it;
  ++it;
  tree.rotateLargeLeft(it);

  BOOST_CHECK_EQUAL(tree.height(), 2);
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(2), "two");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
}

BOOST_AUTO_TEST_CASE(test_rotate_large_right)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(3, "three");
  tree.push(1, "one");
  tree.push(2, "two");

  BOOST_CHECK_EQUAL(tree.height(), 3);

  auto it = tree.begin();
  tree.rotateLargeRight(it);

  BOOST_CHECK_EQUAL(tree.height(), 2);
  BOOST_CHECK_EQUAL(tree.get(1), "one");
  BOOST_CHECK_EQUAL(tree.get(2), "two");
  BOOST_CHECK_EQUAL(tree.get(3), "three");
}

BOOST_AUTO_TEST_CASE(test_push_rvalue)
{
  pozdnyakov::BSTree< int, std::string > tree;
  std::string val = "rvalue";
  tree.push(1, std::move(val));
  BOOST_CHECK_EQUAL(tree.get(1), "rvalue");
}

BOOST_AUTO_TEST_CASE(test_iterator_equality)
{
  pozdnyakov::BSTree< int, std::string > tree;
  tree.push(1, "one");

  pozdnyakov::BSTIterator< int, std::string > it1 = tree.begin();
  pozdnyakov::BSTIterator< int, std::string > it2 = tree.begin();
  pozdnyakov::BSTIterator< int, std::string > it3 = tree.end();

  BOOST_CHECK(it1 == it2);
  BOOST_CHECK(it1 != it3);
  BOOST_CHECK(tree.begin() != tree.end());
}

BOOST_AUTO_TEST_SUITE_END()
