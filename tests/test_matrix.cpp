#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "Matrix.h"

#include <array>
#include <algorithm>
#include <utility>

TEST_CASE("Matrix") {
  SECTION("Constructors") {
    SECTION("Default") {
      Matrix<int> m;
      REQUIRE(m.size() == 0);
      REQUIRE(m.empty());
      REQUIRE(m.rows() == 0);
      REQUIRE(m.cols() == 0);
      REQUIRE(m.begin() == m.end());
    }

    SECTION("Fill ctor") {
      const size_t R = 2, C = 3;
      const int v = 7;
      Matrix<int> m(R, C, v);
      REQUIRE(!m.empty());
      REQUIRE(m.rows() == R);
      REQUIRE(m.cols() == C);
      REQUIRE(m.size() == R * C);
      for (size_t r = 0; r < R; ++r)
        for (size_t c = 0; c < C; ++c)
          REQUIRE(m(r, c) == v);
    }

    SECTION("Construct and fill via data()") {
      std::array<int, 6> ref {1,2,3,4,5,6};
      Matrix<int> m(2,3);
      std::copy(ref.begin(), ref.end(), m.data());
      REQUIRE(std::equal(m.cbegin(), m.cend(), ref.cbegin(), ref.cend()));
    }
  }

  SECTION("Accessors and operator[]") {
    std::array<int, 6> ref {1,2,3,4,5,6};
    Matrix<int> m(2,3);
    std::copy(ref.begin(), ref.end(), m.data());

    SECTION("operator()") {
      REQUIRE(m(0,0) == 1);
      REQUIRE(m(0,2) == 3);
      REQUIRE(m(1,0) == 4);
      REQUIRE(m(1,2) == 6);
    }

    SECTION("operator[] row access (matrix[i][j])") {
      REQUIRE(m[0][0] == 1);
      REQUIRE(m[1][2] == 6);
      m[1][1] = 42;
      REQUIRE(m(1,1) == 42);
    }

    SECTION("at() bounds checking") {
      REQUIRE_NOTHROW(m.at(0,0));
      REQUIRE_THROWS_AS(m.at(2,0), std::out_of_range);
      REQUIRE_THROWS_AS(m.at(0,3), std::out_of_range);
    }
  }

  SECTION("Iterators") {
    std::array<int, 6> ref {1,2,3,4,5,6};
    Matrix<int> m(2,3);
    std::copy(ref.begin(), ref.end(), m.data());

    SECTION("iterate row-major with iterator") {
      REQUIRE(std::equal(m.cbegin(), m.cend(), ref.cbegin(), ref.cend()));
      REQUIRE(std::distance(m.begin(), m.end()) == static_cast<std::ptrdiff_t>(m.size()));
    }

    SECTION("iterator default construct/copy/assign/deref/inc/dec/compare") {
      Matrix<int>::iterator it;
      it = m.begin();
      Matrix<int>::iterator it2(it);
      REQUIRE(it == it2);
      REQUIRE(*it == ref[0]);
      ++it;
      REQUIRE(*it == ref[1]);
      it++;
      REQUIRE(*it == ref[2]);
      --it;
      REQUIRE(*it == ref[1]);
      it--;
      REQUIRE(*it == ref[0]);
      REQUIRE(it != m.end());
    }

    SECTION("const_iterator works and compares to iterator") {
      Matrix<int>::const_iterator cit = m.cbegin();
      Matrix<int>::iterator it = m.begin();
      REQUIRE(*cit == *it);
      ++cit; ++it;
      REQUIRE(cit == it);
    }

    SECTION("random access arithmetic") {
      auto it = m.begin();
      auto it2 = it + 4;
      REQUIRE(*(it2) == ref[4]);
      it2 -= 2;
      REQUIRE(*it2 == ref[2]);
      REQUIRE((it2 - it) == 2);
      REQUIRE(it2[1] == ref[3]);
    }
  }

  SECTION("Copy and Move semantics") {
    std::array<int, 6> ref {1,2,3,4,5,6};
    Matrix<int> a(2,3);
    std::copy(ref.begin(), ref.end(), a.data());
    Matrix<int> b = a; // copy
    REQUIRE(b.rows() == a.rows());
    REQUIRE(b.cols() == a.cols());
    REQUIRE(std::equal(b.cbegin(), b.cend(), ref.cbegin(), ref.cend()));

    Matrix<int> c = std::move(a); // move
    REQUIRE(c.rows() == 2);
    REQUIRE(c.cols() == 3);
    REQUIRE(std::equal(c.cbegin(), c.cend(), ref.cbegin(), ref.cend()));
    REQUIRE(a.size() == 0);
  }

  SECTION("Assignment operators") {
    std::array<int, 6> ref {1,2,3,4,5,6};
    Matrix<int> a(2,3);
    std::copy(ref.begin(), ref.end(), a.data());
    Matrix<int> b;
    b = a; // copy assign
    REQUIRE(std::equal(b.cbegin(), b.cend(), ref.cbegin(), ref.cend()));

    Matrix<int> c;
    c = std::move(b); // move assign
    REQUIRE(std::equal(c.cbegin(), c.cend(), ref.cbegin(), ref.cend()));
  }

  SECTION("resize preserves top-left block and default-inits new elements") {
    Matrix<int> m(2,2);
    m(0,0) = 1; m(0,1) = 2;
    m(1,0) = 3; m(1,1) = 4;

    m.resize(3,4);
    REQUIRE(m.rows() == 3);
    REQUIRE(m.cols() == 4);
    REQUIRE(m(0,0) == 1);
    REQUIRE(m(0,1) == 2);
    REQUIRE(m(1,0) == 3);
    REQUIRE(m(1,1) == 4);
    REQUIRE(m(2,0) == 0);
    REQUIRE(m(0,3) == 0);

    m.resize(1,1);
    REQUIRE(m.rows() == 1);
    REQUIRE(m.cols() == 1);
    REQUIRE(m(0,0) == 1);
  }

  SECTION("swap") {
    Matrix<int> m1(1,2), m2(2,1);
    m1(0,0) = 7; m1(0,1) = 8;
    m2(0,0) = 1; m2(1,0) = 2;
    std::swap(m1, m2);
    REQUIRE(m1.rows() == 2);
    REQUIRE(m1.cols() == 1);
    REQUIRE(m2.rows() == 1);
    REQUIRE(m2.cols() == 2);
    REQUIRE(m1(0,0) == 1);
    REQUIRE(m1(1,0) == 2);
    REQUIRE(m2(0,0) == 7);
    REQUIRE(m2(0,1) == 8);
  }
}
