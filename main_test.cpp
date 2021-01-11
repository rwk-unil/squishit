#include <gtest/gtest.h>
#include "pbwt_exp.hpp"

namespace {
    TEST(PBWT, empty) {
    }

    template <typename T>
    class SparseRLE : public testing::Test {
    public:
        std::vector<T> non_sparse = {0,1,1,0,0,0,0,1,0,0,
                                     0,0,1,0,0,0,0,1,1,1};
        Pbwt<>::SparseRLE::encoding_t expected_encoding = {
            {.position =  1, .length = 2},
            {.position =  7, .length = 1},
            {.position = 12, .length = 1},
            {.position = 17, .length = 3}
        };
    };

    using MyTypes = ::testing::Types<bool, char, int, unsigned int>;
    TYPED_TEST_SUITE(SparseRLE, MyTypes);

    TYPED_TEST(SparseRLE, Construct) {
        Pbwt<>::SparseRLE sprle(this->non_sparse);

        ASSERT_EQ(sprle.encoding.size(), this->expected_encoding.size()) << "Number of encodings is wrong";
        for (size_t _ = 0; _ < this->expected_encoding.size(); ++_) {
            ASSERT_EQ(sprle.encoding.at(_).position, this->expected_encoding.at(_).position) << "Position of encoding " << _ << " is wrong";
            ASSERT_EQ(sprle.encoding.at(_).length, this->expected_encoding.at(_).length) << "Length of encoding " << _ << " is wrong";
        }
    }

    class SparseRLEBool : public SparseRLE<bool> {
    public:
        SparseRLEBool(): SparseRLE<bool>(), sprle(this->non_sparse) {}
        Pbwt<>::SparseRLE sprle;
    };

    TEST_F(SparseRLEBool, Access) {
        Pbwt<>::SparseRLEAccessor access(this->sprle);

        for (size_t _ = 0; _ < this->non_sparse.size(); ++_) {
            EXPECT_EQ(access.at(_), this->non_sparse.at(_)) << "Accessor is messed up for position " << _;
        }
    }

    TEST_F(SparseRLEBool, NextAccess) {
        Pbwt<>::SparseRLEAccessor access(this->sprle);

        for (size_t _ = 0; _ < this->non_sparse.size(); ++_) {
            EXPECT_EQ(access.next(), this->non_sparse.at(_)) << "Accessor is messed up for position " << _;
        }
    }

    TEST_F(SparseRLEBool, NextAccessTwice) {
        Pbwt<>::SparseRLEAccessor access(this->sprle);

        for (size_t _ = 0; _ < this->non_sparse.size(); ++_) {
            EXPECT_EQ(access.next(), this->non_sparse.at(_)) << "Accessor is messed up for position " << _;
        }
        access.reset();
        for (size_t _ = 0; _ < this->non_sparse.size(); ++_) {
            EXPECT_EQ(access.next(), this->non_sparse.at(_)) << "Accessor is messed up for position " << _;
        }
    }

    /**
     * @brief Example hap map for unit tests
     */
    const std::vector<std::vector<bool> > hap_map_original = {
        // ------------> Indivs                        Markers (sites)
        {1, 0, 1, 0, 0, 1, 1, 1,  0, 0, 1, 0, 0, 0, 1, 1}, // | site 0
        {1, 1, 0, 1, 1, 1, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0}, // | site 1
        {1, 0, 1, 1, 1, 1, 0, 1,  0, 0, 1, 0, 1, 0, 0, 1}, // | ...
        {0, 0, 1, 1, 0, 1, 0, 1,  0, 0, 1, 0, 0, 1, 1, 1}, // v
        {0, 0, 1, 0, 0, 0, 1, 0,  0, 0, 1, 1, 0, 1, 1, 1}, //
        {0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0}
    //   0  1  2  3  4  5  6  7   8  9 10 11 12 13 14 15
    }; 

    /**
     * @brief Final order when sorted by reverse prefix
     */
    const std::vector<size_t> expected_final_ppa = {9,1,8,12,4,0,7,3,5,11,6,13,14,2,10,15};

    /**
     * @brief Final div array
     */
    const std::vector<size_t> expected_final_div = {8,2,0,3,2,1,4,2,1,5,1,4,1,3,0,0};

#if 0
    TEST(PBWT, Sort) {
        auto hap_map = hap_map_original;

        pbwt_sort(hap_map);
    }
#endif

    TEST(PBWT_EXP, alg2_exp) {
        auto result = algorithm_2_exp(hap_map_original, 0, hap_map_original.size());

        for (size_t _ = 0; _ < expected_final_ppa.size(); _++) {
            EXPECT_EQ(result.a[_], expected_final_ppa[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
            EXPECT_EQ(result.d[_], expected_final_div[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
        }
    }

#if 0
    TEST(PBWT_EXP, alg2_exp_fail) {
        const size_t offset = 1;

        auto result = algorithm_2_exp(hap_map_original, offset, hap_map_original.size()-offset);

        for (size_t _ = 0; _ < expected_final_ppa.size(); _++) {
            EXPECT_EQ(result.a[_], expected_final_ppa[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];
        }
    }

    TEST(PBWT_EXP, alg2_exp_fail_2) {
        /// @note This test relies on the fact that M is divisible by jump
        // Too lazey here to fix this edge case, and unnecessary
        const size_t jump = 4;
        const size_t M = hap_map_original.size();

        std::vector<alg2_res_t> results;

        // This whole loop can now be unrolled because there is no dependency
        for (size_t _ = 0; _ * jump < M; ++_) {
            const size_t offset = _ * jump;
            results.push_back(algorithm_2_exp(hap_map_original, offset, jump));
        }

        ASSERT_EQ(results.size(), M / jump);

        // Fix the a's and d's
        const auto& result = results.back();
        for (size_t _ = 0; _ < expected_final_ppa.size(); _++) {
            EXPECT_EQ(result.a[_], expected_final_ppa[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
            EXPECT_EQ(result.d[_], expected_final_div[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
        }
    }
#endif

    TEST(PBWT_EXP, alg2_exp_fixed) {
        /// @note This test relies on the fact that M is divisible by jump
        // Too lazey here to fix this edge case, and unnecessary
        const size_t jump = 4;
        const size_t M = hap_map_original.size();

        std::vector<alg2_res_t> results;

        // This whole loop can now be unrolled because there is no dependency
        for (size_t _ = 0; _ * jump < M; ++_) {
            const size_t offset = _ * jump;
            results.push_back(algorithm_2_exp(hap_map_original, offset, jump));
        }

        ASSERT_EQ(results.size(), M / jump);

        // Fix the a's and d's
        fix_a_d(results);

        // Check the results
        const auto& result = results.back();
        for (size_t _ = 0; _ < expected_final_ppa.size(); _++) {
            EXPECT_EQ(result.a[_], expected_final_ppa[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
            EXPECT_EQ(result.d[_], expected_final_div[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
        }
    }

    TEST(PBWT_EXP, alg2) {
        auto results = algorithm_2(hap_map_original, 0);

        // Check the results
        const auto& result = results.back();
        for (size_t _ = 0; _ < expected_final_ppa.size(); _++) {
            EXPECT_EQ(result.a[_], expected_final_ppa[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
            EXPECT_EQ(result.d[_], expected_final_div[_]) << "_ = " << _ << " a[_] = " << result.a[_] << " d[_] = " << result.d[_];;
        }
    }

    TEST(PBWT_EXP, alg2_comparison) {
        const size_t jump = 4;
        const size_t M = hap_map_original.size();

        std::vector<alg2_res_t> results;

        // This whole loop can now be unrolled because there is no dependency
        for (size_t _ = 0; _ * jump < M; ++_) {
            const size_t offset = _ * jump;
            results.push_back(algorithm_2_exp(hap_map_original, offset, jump));
        }
        // Fix the a's and d's
        fix_a_d(results);

        auto results_2 = algorithm_2(hap_map_original, jump);

        ASSERT_EQ(results.size(), results_2.size());

        for (size_t i = 0; i < results.size(); ++i) {
            ASSERT_EQ(results[i].pos, results_2[i].pos);

            ASSERT_EQ(results[i].a.size(), results_2[i].a.size());
            ASSERT_EQ(results[i].d.size(), results_2[i].d.size());
            ASSERT_EQ(results[i].a.size(), results_2[i].d.size());

            for (size_t j = 0; j < results[i].a.size(); ++j) {
                EXPECT_EQ(results[i].a[j], results_2[i].a[j]) << "for i = " << i << " j = " << j;
                EXPECT_EQ(results[i].d[j], results_2[i].d[j]) << "for i = " << i << " j = " << j;
            }
        }
    }
}