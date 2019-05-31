#pragma once
#include <vector>

class KMatrix
{
protected:
    unsigned int rows;
    unsigned int cols;
    std::vector<float> entries;
public:
    KMatrix(unsigned int rows, unsigned int cols);
    KMatrix(unsigned int rows, unsigned int cols, unsigned int givenEntries...);
    const float GetEntry(unsigned int at) const;
    const float GetEntry(unsigned int row, unsigned int col) const;
    void SetEntry(unsigned int at, float value);
    void SetEntry(unsigned int row, unsigned int col, float value);
    const unsigned int GetRows() const { return rows; }
    const unsigned int GetCols() const { return cols; }
    const unsigned int GetSize() const { return rows * cols; }
    const bool IsNull() const { return rows > 0 && cols > 0; }
    const float* GetEntryPtr() { return entries.data(); }
    KMatrix operator* (const KMatrix &other) const;
    KMatrix Transpose();
    static KMatrix Identity();
    static KMatrix Scale(float x, float y, float z);
    static KMatrix Translation(float x, float y, float z);
    static KMatrix Rotation(float x, float y, float z);
};

