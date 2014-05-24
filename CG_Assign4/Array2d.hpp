//! A class for holding a 2D array of elements

#pragma once
#include <vector>

template<class T> 
class Array2D {
public:
    /// <summary>
    /// Initialise the array with a starting value.
    /// </summary>
    ///
    /// <param name="width">The width of the array.</param>
    /// <param name="height">The height of the array.</param>
    /// <param name="value">The initial value of the array.</param>
    Array2D(int width, int height, T value) : width_(width), height_(height) {
        raw = std::vector<T>(width * height, value);
    }

    /// <summary>
    /// Returns the value at the point specified.
    /// </summary>
    ///
    /// <param name="x">The x position</param>
    /// <param name="y">The y position</param>
    T at(int x, int y) const {
        return raw[x + y * width_];
    }

    /// <summary>
    /// Sets the value at the point specified.
    /// </summary>
    ///
    /// <param name="x">The x position</param>
    /// <param name="y">The y position</param>
    void set(int x, int y, T value) {
        raw[x + y * width_] = value;
    }

    /// <summary>
    /// Returns the width of the array.
    /// </summary>
    int width() const {
        return width_;
    }

    /// <summary>
    /// Returns the height of the array.
    /// </summary>
    int height() const {
        return height_;
    }    

private:
    std::vector<T> raw;
    int width_;
    int height_;
};