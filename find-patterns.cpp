#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iomanip>
#include <utility>
// #include <fmt/core.h>
#include <format>
#include <find-patterns.hpp>

const std::filesystem::path data_dir{DATA_DIR};
constexpr int minPoints = 4;

struct Point {
    long long x_;
    long long y_;

    Point(long long x = 0, long long y = 0) : x_{x}, y_{y} {}

    bool operator==(const Point& p) const = default;

    std::strong_ordering operator<=>(const Point& p) const {
        if (y_ != p.y_) return y_ <=> p.y_;
        return x_ <=> p.x_;
    }

    double slopeTo(const Point& other) const {
        if (y_ == other.y_ && x_ == other.x_) {
            return -std::numeric_limits<double>::infinity();
        }
        if (other.x_ == x_) return std::numeric_limits<double>::infinity();
        if (other.y_ == y_) return 0.0;
        return static_cast<double>(other.y_ - y_) / (other.x_ - x_);
    }

    std::string toString() const { return std::format("({},{})", x_, y_); }
    std::string toCoordString() const { return std::format("{} {}", x_, y_); }
};

std::vector<Point> readPoints(const std::filesystem::path& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        std::cerr << "Error: Cannot open points file: " << filePath << std::endl;
        return {};
    }

    int n;
    inFile >> n;
    std::vector<Point> points;
    points.reserve(n);
    long long x, y;

    for (int i = 0; i < n; ++i) {
        if (!(inFile >> x >> y)) {
            std::cerr << "Error reading point " << i << std::endl;
            return {};
        }
        points.emplace_back(x, y);
    }
    return points;
}

void writeOutputFiles(const std::filesystem::path& segmentsFile,
                      const std::vector<std::vector<Point>>& detailedSegments) {
    // Create output directories
    std::filesystem::create_directories(segmentsFile.parent_path());
    std::filesystem::path detailedDir = segmentsFile.parent_path() / "detailed_output";
    std::filesystem::create_directories(detailedDir);

    // Write segments file
    std::ofstream segFile(segmentsFile);
    for (const auto& seg : detailedSegments) {
        segFile << seg.front().toCoordString() << " " << seg.back().toCoordString() << "\n";
    }

    // Write detailed segments file
    std::filesystem::path detailedFile =
        detailedDir / ("detailed-" + segmentsFile.filename().string());
    std::ofstream detFile(detailedFile);
    for (const auto& seg : detailedSegments) {
        for (size_t i = 0; i < seg.size(); ++i) {
            detFile << seg[i].toString() << (i == seg.size() - 1 ? "" : "->");
        }
        detFile << "\n";
    }
}

void analyseData(const std::filesystem::path& pointsFile,
                 const std::filesystem::path& segmentsFile) {
    auto points = readPoints(pointsFile);
    if (points.empty()) return;

    std::sort(points.begin(), points.end()); // sort by coordinates (y then x)

    std::vector<std::vector<Point>> detailedSegments;

    std::vector<Point> otherPoints;
    otherPoints.reserve(points.size() - 1);

    std::vector<Point> holdSegments;
    holdSegments.reserve(points.size());

    int segmentsCounter = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        const Point& p = points[i];
        
        // Only consider points after p to avoid duplicate segments
        otherPoints.clear();
        for (size_t j = 0; j < points.size(); ++j) {
            if (points[j] != p) {
                otherPoints.push_back(points[j]);
            }
        }

        if (otherPoints.size() < minPoints - 1) continue; // continue if we can't create a line of atleast 4 points.

        // Sort all points by slope but keep the sorting by coordinates.
        std::stable_sort(otherPoints.begin(), otherPoints.end(),
            [&p](const Point& a, const Point& b) { return p.slopeTo(a) < p.slopeTo(b); });

        Point smallestPoint(std::numeric_limits<long long>::max(),
                            std::numeric_limits<long long>::max());

        for (size_t j = 0; j < otherPoints.size(); ++j) { 
            bool is_last_point = (j == otherPoints.size() - 1);
            bool slope_changed = false;

            if (!is_last_point) {
                if (p.slopeTo(otherPoints[j]) != p.slopeTo(otherPoints[j + 1])) {
                    slope_changed = true;
                }
            }

            // Determine if the current point j is the end of a segment
            // This happens if it's the last point or if the slope changed
            bool endOfCurrentSegment = is_last_point || slope_changed;

            if (smallestPoint > otherPoints[j] && !slope_changed) {
                    smallestPoint = otherPoints[j];
            }

            if (!endOfCurrentSegment) {
                // Current point is part of a continuing segment
                holdSegments.push_back(otherPoints[j]);
            } else { // will be called once if all are collinear.
                // Current point j is the end of the segment (or the only point in a segment)
                holdSegments.push_back(otherPoints[j]);  // Add the current point, which completes the segment

                // Check if segment is long enough
                if (holdSegments.size() >= minPoints - 1) {
                    holdSegments.insert(holdSegments.begin(), p);  
                    // Add the reference point p at the beginning insert will take O(amount of elements) time.
                    if (p <= smallestPoint) {
                        detailedSegments.push_back(holdSegments);
                        segmentsCounter += 1;
                    }
                }
                smallestPoint = Point(std::numeric_limits<long long>::max(),  // reset smallestPoint inorder to find new one.
                                      std::numeric_limits<long long>::max());
                holdSegments.clear();  // Clear for the next potential segment, O(amount of elements) time.
            }
        }
    }
    writeOutputFiles(segmentsFile, detailedSegments);
    std::cout << "Found " << segmentsCounter << " unique line segments." << std::endl;
}

void analyseData(const std::string& name) {
    std::filesystem::path points_name = name;
    if (points_name.has_parent_path()) {
        points_name = points_name.filename();
    }

    std::filesystem::path segments_name = "segments-" + points_name.string();
    analyseData(data_dir / points_name, data_dir / "output" / segments_name);
}

int main() {
    std::cout << "Enter the name of input points file: ";
    std::string points_file;
    std::cin >> points_file;
    analyseData(points_file);
    return 0;
}

//***********************************************************************************************************************
// when running the program enter e.g. points6.txt in both LinesDiscovery and Rendering.                                *
//                                                                                                                      *
// 1. read file and store all points in a vector points.                                                                *
// 2. sort vector points.                                                                                               *
// 3. pick a point p and add all other points from vector points to a new vector otherPoints.                           *
// 4. calculate slope of all points from vector otherPoints relativ to p and stable sort based on slope.                *
// 5. loop through otherPoints and store all points with same slope relative to p into holdSegments.                    *
// 6. when slope changes or end of points, check if line is long enough and add p.                                      *
// 7. check if p is the smallest point in the line segment, if so, add the line segment into detailedsegments.          *
// 8. clear holdSegments for the next line segment and reset smallest point in segment.                                 *
//                                                                                                                      *
//***********************************************************************************************************************