#include "dataParser.h"

#include <chrono>
#include <cmath>
#include <deque>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

bool expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
    }
    return condition;
}

size_t append(std::deque<char>& destination, const std::string& text)
{
    destination.insert(destination.end(), text.begin(), text.end());
    return text.size();
}

} // namespace

int main()
{
    AppConfig config;
    dataParser parser(config);
    parser.setTimestampClockRunning(true);
    std::deque<char> receiveHistory;
    std::vector<ParsedSample> samples;
    bool passed = true;

    parser.parse(receiveHistory, append(receiveHistory, "1.5,2"), samples);
    passed &= expect(samples.empty(), "an incomplete line must not produce a sample");

    parser.parse(receiveHistory, append(receiveHistory, ",3\n4,5"), samples);
    passed &= expect(samples.size() == 1, "a completed line must produce exactly one sample");
    if (samples.size() == 1) {
        passed &= expect(samples[0].values.size() == 3, "the sample must contain all configured channels");
        passed &= expect(std::abs(samples[0].values[0] - 1.5) < 1e-12, "the first float must parse correctly");
        passed &= expect(samples[0].sampleNumber == 0, "sample numbering must begin at zero");
    }
    const double firstTimestamp = samples.empty() ? 0.0 : samples[0].timestampSeconds;

    parser.setTimestampClockRunning(false);
    const double pausedTimestamp = parser.currentTimestampSeconds();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    passed &= expect(parser.currentTimestampSeconds() == pausedTimestamp,
        "the timestamp clock must remain frozen while capture is paused");
    parser.setTimestampClockRunning(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    passed &= expect(parser.currentTimestampSeconds() > pausedTimestamp,
        "the timestamp clock must continue from its frozen value after resume");

    parser.parse(receiveHistory, 0, samples);
    passed &= expect(samples.empty(), "a frame with no new bytes must not reparse receive history");

    parser.parse(receiveHistory, append(receiveHistory, ",6\n"), samples);
    passed &= expect(samples.size() == 1, "a partial line must carry across parse calls");
    if (samples.size() == 1) {
        passed &= expect(samples[0].sampleNumber == 1, "sample numbers must increase monotonically");
        passed &= expect(samples[0].timestampSeconds >= firstTimestamp, "timestamps must not move backward");
    }

    parser.format = "%i,%x,%u";
    parser.compile();
    passed &= expect(parser.currentTimestampSeconds() == 0.0,
        "compiling a format must reset the capture clock in a paused state");
    parser.setTimestampClockRunning(true);
    receiveHistory.clear();
    parser.parse(receiveHistory, append(receiveHistory, "-3,ff,7\n"), samples);
    passed &= expect(samples.size() == 1, "integer and hexadecimal formats must parse");
    if (samples.size() == 1) {
        passed &= expect(samples[0].values[0] == -3.0, "signed integer parsing must be exact");
        passed &= expect(samples[0].values[1] == 255.0, "hexadecimal parsing must be exact");
        passed &= expect(samples[0].values[2] == 7.0, "unsigned integer parsing must be exact");
        passed &= expect(samples[0].sampleNumber == 0, "compiling a format must start a new sample sequence");
    }

    receiveHistory.clear();
    std::string malformedLine(9000, 'x');
    malformedLine += "\n8,9,a\n";
    parser.parse(receiveHistory, append(receiveHistory, malformedLine), samples);
    passed &= expect(samples.size() == 1, "an overlong malformed line must be discarded without losing the next line");

    if (passed) {
        std::cout << "All incremental data parser tests passed.\n";
        return 0;
    }
    return 1;
}
