/**
* Simulate average runtime for codes with growing nr of N for several physical err rates (err rates should only increase slope of curve)
*/
#include "Code.hpp"
#include "DecodingRunInformation.hpp"
#include "ImprovedUFD.hpp"
#include "OriginalUFD.hpp"
#include "Utils.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void runtime(const std::string& codeName) {
    /**
     * ***************** Comment out accordingly *****************
     */
    //**** server:
    const std::string rootPath = "/home/berent/ufpaper/simulations/montecarlo/final/";
    const std::string outPath  = rootPath + "out/";
    const std::string inPath   = rootPath + "in/toricCodes/";
    //**** local:
   // const std::string outPath = "/home/luca/Documents/uf-simulations/runtime/original/";
   // const std::string inPath  = "/home/luca/Documents/codeRepos/qecc/examples/toricCodes/";
    // ***************** config end *****************

    const std::string outFile         = outPath + "info_" + codeName;
    const std::string runningDataFile = outPath + "running_" + codeName;
    const std::string finalDataFile   = outPath + "final_" + codeName;
    std::cout << "writing results to " << outPath << std::endl;
    auto               t  = std::time(nullptr);
    auto               tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y");
    auto          timestamp = oss.str();
    std::ofstream dataOutStream(outFile + timestamp + ".json");
    std::ofstream intermediateRawOut(runningDataFile + timestamp + ".json"); // appends data after each step for intermediate results
    std::ofstream finalRawOut(finalDataFile + timestamp + ".json");          // single, final data dump at end
    intermediateRawOut.rdbuf()->pubsetbuf(0, 0);                             // force flush
    finalRawOut.rdbuf()->pubsetbuf(0, 0);

    /**
     * ***************** Basic parameters, comment out accordingly *****************
     */
    //**** paper eval:
    const std::size_t         nrOfDecodingRuns = 50;
    const std::size_t         nrSamples        = 2;
    const std::vector<double> physErrRates     = {0.05};
    //**** tests:
    //const std::size_t         nrOfDecodingRuns = 100;
    //const std::size_t nrSamples  = 5;
    //const std::vector<double> physErrRates = {0.03};
    // ***************** configure end *****************

    std::size_t                                                                    trialsTimeSum = 0U;
    std::size_t                                                                    sumAllSamples = 0U;
    std::map<std::string, std::map<std::string, double, std::less<>>, std::less<>> dataPerRate;
    std::map<std::string, double, std::less<>>                                     dataPerCode;
    std::vector<std::string>                                                       codePaths;
    std::map<std::string, std::size_t, std::less<>>                                avgSampleRuns;

    //Utils::readInFilePathsFromDirectory(inPath, codePaths);
    codePaths.emplace_back(codeName);
    std::cout << "coden" << codeName << std::endl;
    auto decoder = OriginalUFD();
    for (auto currRate: physErrRates) {
        //std::cout << "Simulating physical err rate " << currRate << std::endl;
        try {
            for (const auto& currPath: codePaths) {
                std::cout << "next code : " << currPath << std::endl;
                trialsTimeSum    = 0U;
                sumAllSamples    = 0U;
                auto       code  = Code(currPath);
                const auto codeN = code.getN();
               // std::cout << "set code" << std::endl;
                decoder.setCode(code);
                for (std::size_t j = 0; j < nrSamples; j++) {            // #sample runs to compute average
                    for (std::size_t i = 0; i < nrOfDecodingRuns; i++) { // nr of monte carlo samples
                        //std::cout << "sample err" << std::endl;
                        auto error = Utils::sampleErrorIidPauliNoise(codeN, currRate);
                        //std::cout << "getting syndr" << std::endl;
                        auto syndrome = code.getSyndrome(error);
                        //std::cout << "decoding" << std::endl;
                        decoder.decode(syndrome);
                        auto const& decodingResult = decoder.result;
                        trialsTimeSum += decodingResult.decodingTime;
                        nlohmann::json json = DecodingRunInformation(
                                                      currRate,
                                                      codeN,
                                                      error,
                                                      syndrome,
                                                      decoder.result)
                                                      .to_json();
                        dataOutStream << json.dump(2U);
                        dataOutStream.flush();
                        //std::cout << "resetting" << std::endl;
                        decoder.reset();
                    }
                    avgSampleRuns.try_emplace(std::to_string(j), trialsTimeSum); // time to decode #monte carlo trials (string for nice json output)
                    //std::cout << "sample: timesum = " << j << ":" << trialsTimeSum << std::endl;
                    sumAllSamples += trialsTimeSum;
                    trialsTimeSum = 0U;
                }
                Utils::printTimePerSampleRun(avgSampleRuns);
                dataPerCode.try_emplace(std::to_string(codeN), static_cast<double>(static_cast<double>(sumAllSamples) / nrSamples));
                std::cout << "codeN: avg = " << codeN << ":" << sumAllSamples / nrSamples << std::endl;
                avgSampleRuns.clear();
                sumAllSamples = 0U;
                trialsTimeSum = 0U;
            }
        } catch (std::exception& e) {
            std::cerr << "Exception occurred " << e.what() << std::endl;
            throw QeccException("Exception caught, simulation failed");
        }
        dataPerRate.try_emplace(std::to_string(currRate), dataPerCode);
        dataPerCode.clear();
    }
    json fr = dataPerRate;
    finalRawOut << fr.dump(2U);
    finalRawOut.flush();
    finalRawOut.close();
    intermediateRawOut.close();
    dataOutStream.close();
    flint_cleanup();
}

void decodingPerformance(const double per) {
    std::cout << "performance" << std::endl;
    /**
     * ***************** Comment out accordingly *****************
     */
    //****server
    //const std::string rootPath   = "/home/berent/ufpaper/simulations/decodingPerfSim/final/";
    //const std::string outpath    = rootPath + "out/";
    //const std::string inCodePath = rootPath + "source/code/hgp_(4,8)-[[5408,18,26]]_hx.txt";
    //const std::size_t code_K     = 18;
    //**** local
    const std::string outpath    = "/home/luca/Documents/uf-simulations/final/"; //TODO adapt
    const std::string inCodePath = "/home/luca/Documents/codeRepos/qecc/examples/hgp_(4,8)-[[5408,18,26]]_hx.txt"; // TODO adapt
    const std::size_t code_K     = 18;
    // ***************** configure end *****************

    const std::string outFilePath        = outpath + "results-" + std::to_string(per);
    const std::string dataFilePathInterm = outpath + "interm-" + std::to_string(per);
    const std::string dataFilePath       = outpath + "final-" + std::to_string(per);
    std::cout << "writing output to " << outpath << std::endl;
    auto               t  = std::time(nullptr);
    auto               tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y");
    auto          timestamp = oss.str();
    std::ofstream decodingResOutput(outFilePath + timestamp + ".json");
    std::ofstream rawDataOutput(dataFilePath + timestamp + ".json");
    std::ofstream rawIntermediateOut(dataFilePathInterm + timestamp + ".json");
    rawIntermediateOut.rdbuf()->pubsetbuf(0, 0);
    rawDataOutput.rdbuf()->pubsetbuf(0, 0);

    /**
     * ***************** Comment out accordingly *****************
     */
    //**** Paper eval
    double physicalErrRate = per;
    //const double      stepSize           = 0.00002;
    //const double      maxPhysicalErrRate = 0.1;
    //const std::size_t nrOfRuns           = std::floor(maxPhysicalErrRate / physicalErrRate);
    const std::size_t nrOfRunsPerRate = 1'000'000;
    //**** tests
    // double            physicalErrRate    = 0.06;
    // double            stepSize           = 0.02;
    // const double      maxPhysicalErrRate = 0.1;
    // const std::size_t nrOfRuns           = std::floor(maxPhysicalErrRate / stepSize); // to avoid float type loop increment
    // std::size_t       nrOfRunsPerRate    = 2;
    // ***************** configure end *****************

    std::map<std::string, double, std::less<>> wordErrRatePerPhysicalErrRate;
    //    decodingResOutput << "{ \"runs\" : [ ";
    //    rawIntermediateOut << "{ ";

    auto        code = HGPcode(inCodePath, code_K);
    const auto  K    = code.getK();
    const auto  N    = code.getN();
    ImprovedUFD decoder;
    decoder.setCode(code);
    // for (std::size_t i = 0; i < nrOfRuns && physicalErrRate <= maxPhysicalErrRate; i++) {
    std::size_t nrOfFailedRuns = 0U;
    //decodingResOutput << R"({ "run": { "physicalErrRate":)" << physicalErrRate << ", \"data\": [ ";
    for (size_t j = 0; j < nrOfRunsPerRate; j++) {
        auto error    = Utils::sampleErrorIidPauliNoise(N, physicalErrRate);
        auto syndrome = code.getSyndrome(error);
        decoder.decode(syndrome);
        auto const&       decodingResult = decoder.result;
        std::vector<bool> residualErr    = decodingResult.estimBoolVector;
        Utils::computeResidualErr(error, residualErr);
        auto                 success = code.isVectorStabilizer(residualErr);
        DecodingResultStatus status;
        if (success) {
            status = SUCCESS;
        } else {
            status = FAILURE;
            nrOfFailedRuns++;
        }
//        json resj = DecodingRunInformation(physicalErrRate,
//                                           code_K,
//                                           error,
//                                           syndrome,
//                                           status,
//                                           decodingResult)
//                            .to_json();
//        //decodingResOutput << resj.dump(2U);
        //            if (j != nrOfRunsPerRate - 1) {
        //                decodingResOutput << ", ";
        //            }
    }
    //compute word error rate WER
    const auto blockErrRate    = static_cast<double>(nrOfFailedRuns) / static_cast<double>(nrOfRunsPerRate);
    nrOfFailedRuns             = 0;
    const auto wordErrRate     = blockErrRate / static_cast<double>(K);                                                   // rate of codewords for decoder does not give correct answer (fails or introduces logical operator)
    const auto& [it, inserted] = wordErrRatePerPhysicalErrRate.try_emplace(std::to_string(physicalErrRate), wordErrRate); // to string for json parsing
    std::cout << "per:wer = " << it->first << ":" << it->second << std::endl;
    //        rawIntermediateOut << R"( ")" << it->first << R"(" )"
    //                           << ":" + std::to_string(it->second);
    //        rawIntermediateOut.flush();
    // only for json output
    //        if (i != nrOfRuns - 1) {
    //            decodingResOutput << "]}},";
    //            rawIntermediateOut << ",";
    //        } else {
    //            decodingResOutput << "]}}";
    //            rawIntermediateOut << "}";
    //        }
    //physicalErrRate += stepSize;
    //}
    decoder.reset();
    json dataj = wordErrRatePerPhysicalErrRate;
    rawDataOutput << dataj.dump(2U);
    decodingResOutput.close();
    rawDataOutput.close();
    rawIntermediateOut.close();
}

int main(int argc, char* argv[]) {
    std::string codeFileName = argv[1];
    //runtime("/home/luca/Documents/codeRepos/qecc/examples/toricCodes/" + codeFileName);
    //runtime("/home/berent/ufpaper/simulations/montecarlo/final/in/toricCodes/" + codeFileName);
    double per = std::stod(argv[1]);
    decodingPerformance(per);
}