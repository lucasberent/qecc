//
// Created by lucas on 13/06/22.
//
// to keep 0/1 in boolean areas without clang-tidy warnings:
// NOLINTBEGIN(readability-implicit-bool-conversion,modernize-use-bool-literals)

#include "Codes.hpp"
#include "UFDecoder.hpp"

#include <gtest/gtest.h>

class OriginalUFDtest : public testing::TestWithParam<std::vector<bool>> {};
class UniquelyCorrectableErrTest_original : public OriginalUFDtest {};
class InCorrectableErrTest_original : public OriginalUFDtest {};
class UpToStabCorrectableErrTest_original : public OriginalUFDtest {};

INSTANTIATE_TEST_SUITE_P(CorrectableSingleBitErrsSteane, UniquelyCorrectableErrTest_original,
                         testing::Values(
                                 std::vector<bool>{0, 0, 0, 0, 0, 0, 0},
                                 std::vector<bool>{1, 0, 0, 0, 0, 0, 0},
                                 std::vector<bool>{0, 1, 0, 0, 0, 0, 0},
                                 std::vector<bool>{0, 0, 1, 0, 0, 0, 0}));

INSTANTIATE_TEST_SUITE_P(UptoStabCorrSteane, InCorrectableErrTest_original,
                         testing::Values(
                                 std::vector<bool>{0, 0, 0, 1, 0, 0, 0},
                                 std::vector<bool>{0, 0, 0, 0, 1, 0, 0},
                                 std::vector<bool>{0, 0, 0, 0, 0, 1, 0}));

INSTANTIATE_TEST_SUITE_P(UptoStabCorrSteane, UpToStabCorrectableErrTest_original,
                         testing::Values(
                                 std::vector<bool>{0, 0, 0, 0, 0, 0, 1},
                                 std::vector<bool>{1, 1, 0, 0, 0, 0, 0},
                                 std::vector<bool>{0, 0, 0, 0, 1, 1, 0},
                                 std::vector<bool>{1, 0, 0, 0, 0, 0, 1}));

/**
 * Tests for unambigous syndromes, estimates must be computed exactly
 */
TEST_P(UniquelyCorrectableErrTest_original, SteaneCodeDecodingTestEstim) {
    auto      code = SteaneXCode();
    UFDecoder decoder;
    decoder.setCode(code);
    std::cout << "code: " << std::endl
              << code << std::endl;
    std::vector<bool> err = GetParam();

    auto syndr = code.getXSyndrome(err);
    std::cout << "syndrome: ";
    Utils::printGF2vector(syndr);
    decoder.decode(syndr);
    const auto& decodingResult = decoder.result;
    const auto& estim          = decodingResult.estimBoolVector;
    const auto& estimIdx       = decodingResult.estimNodeIdxVector;
    gf2Vec      estim2(err.size());
    std::cout << "estiIdxs: ";
    for (unsigned long idx : estimIdx) {
        estim2.at(idx) = true;
        std::cout << idx;
    }
    std::cout << std::endl;
    gf2Vec sol = GetParam();

    std::cout << "Estim: " << std::endl;
    Utils::printGF2vector(estim);
    std::cout << "EstimIdx: " << std::endl;
    Utils::printGF2vector(estim2);
    std::cout << "Sol: " << std::endl;
    Utils::printGF2vector(sol);
    EXPECT_TRUE(sol == estim);
    EXPECT_TRUE(sol == estim2);
}

/**
 * Tests for ambigous errors that cannot be corrected
 */
TEST_P(InCorrectableErrTest_original, SteaneCodeDecodingTestEstim) {
    auto      code = SteaneXCode();
    UFDecoder decoder;
    decoder.setCode(code);
    std::cout << "code: " << std::endl
              << code << std::endl;
    std::vector<bool> err = GetParam();

    auto syndr = code.getXSyndrome(err);
    decoder.decode(syndr);
    const auto& decodingResult = decoder.result;
    const auto& estim          = decodingResult.estimBoolVector;
    const auto& estimIdx       = decodingResult.estimNodeIdxVector;
    gf2Vec      estim2(err.size());
    std::cout << "estiIdxs: ";
    for (unsigned long idx : estimIdx) {
        estim2.at(idx) = true;
        std::cout << idx;
    }
    std::vector<bool> residualErr(err.size());
    for (size_t i = 0; i < err.size(); i++) {
        residualErr.at(i) = (err[i] != estim[i]);
    }

    gf2Vec sol = GetParam();

    std::cout << "Estim: " << std::endl;
    Utils::printGF2vector(estim);
    std::cout << "EstimIdx: " << std::endl;
    Utils::printGF2vector(estim2);
    std::cout << "Sol: " << std::endl;
    Utils::printGF2vector(sol);
    EXPECT_FALSE(sol == estim);
    EXPECT_FALSE(sol == estim2);
}

/**
 * Tests for errors that are correctable up to stabilizer
 */
TEST_P(UpToStabCorrectableErrTest_original, SteaneCodeDecodingTest) {
    auto      code = SteaneCode();
    UFDecoder decoder;
    decoder.setCode(code);
    std::cout << "code: " << std::endl
              << code << std::endl;
    std::vector<bool> err = GetParam();
    std::cout << "err :" << std::endl;
    Utils::printGF2vector(err);
    auto syndr = code.getXSyndrome(err);
    decoder.decode(syndr);
    const auto& decodingResult = decoder.result;
    const auto& estim          = decodingResult.estimBoolVector;
    const auto& estimIdx       = decodingResult.estimNodeIdxVector;
    gf2Vec      estim2(err.size());
    std::cout << "estiIdxs: ";
    for (unsigned long idx : estimIdx) {
        estim2.at(idx) = true;
        std::cout << idx;
    }
    std::cout << std::endl;
    std::vector<bool> residualErr(err.size());
    for (size_t i = 0; i < err.size(); i++) {
        residualErr.at(i) = (err[i] != estim[i]);
    }
    std::vector<bool> residualErr2(err.size());
    for (size_t i = 0; i < err.size(); i++) {
        residualErr2.at(i) = (err[i] != estim2[i]);
    }

    EXPECT_TRUE(Utils::isVectorInRowspace(*code.getHx()->pcm, residualErr));
    EXPECT_TRUE(Utils::isVectorInRowspace(*code.getHx()->pcm, residualErr2));
}
// NOLINTEND(readability-implicit-bool-conversion,modernize-use-bool-literals)
