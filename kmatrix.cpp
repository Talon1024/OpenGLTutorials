#include "kmatrix.h"
#include <cstdarg>
#include <cmath>
#include <vector>

KMatrix::KMatrix(unsigned int rows, unsigned int cols) : rows(rows), cols(cols)
{
    entries = std::vector<float>(rows * cols, 0.0);
}

// Same as above but allows varargs
KMatrix::KMatrix(unsigned int rows, unsigned int cols, unsigned int givenEntries...) : rows(rows), cols(cols)
{
    entries = std::vector<float>(rows * cols, 0.0);
    va_list entryvalues;
    unsigned int vaIndex = 0;
    va_start(entryvalues, givenEntries);
    for (vaIndex = 0; vaIndex < givenEntries; vaIndex++)
    {
        entries[vaIndex] = va_arg(entryvalues, double);
    }
    va_end(entryvalues);
}

const float KMatrix::GetEntry(unsigned int at) const
{
    return entries[at];
}

const float KMatrix::GetEntry(unsigned int row, unsigned int col) const
{
    unsigned int at = row * cols + col;
    return entries[at];
}

void KMatrix::SetEntry(unsigned int at, float value)
{
    entries[at] = value;
}

void KMatrix::SetEntry(unsigned int row, unsigned int col, float value)
{
    unsigned int at = row * cols + col;
    entries[at] = value;
}

KMatrix KMatrix::operator* (const KMatrix &other) const
{
    if (cols == other.GetRows())
    {
        unsigned int resultRows = rows;
        unsigned int resultCols = other.GetCols();
        KMatrix result(resultRows, resultCols);
        for (unsigned int row = 0; row < resultRows; row++)
        {
            for (unsigned int col = 0; col < resultCols; col++)
            {
                float curEntry = 0;
                for (unsigned int idx = 0; idx < cols; idx++)
                {
                    curEntry += GetEntry(row, idx) * other.GetEntry(idx, col);
                }
                result.SetEntry(row, col, curEntry);
            }
        }
        return result;
    }
    return KMatrix(0, 0);
}

KMatrix KMatrix::Transpose()
{
    // This class uses row-major ordering, OpenGL uses column-major ordering
    KMatrix result(cols, rows);
    for (unsigned int entry = 0; entry < GetSize(); entry++)
    {
        // Example for 4x4 matrix
        // [  0  1  2  3 ]    [ 0 4  8 12 ]
        // [  4  5  6  7 ] -> [ 1 5  9 13 ]
        // [  8  9 10 11 ]    [ 2 6 10 14 ]
        // [ 12 13 14 15 ]    [ 3 7 11 15 ]
        // Example for 4x2 matrix
        // [ 0 1 ]    [ 0 2 4 6 ]
        // [ 2 3 ] -> [ 1 3 5 7 ]
        // [ 4 5 ]
        // [ 6 7 ]
        unsigned int resultEntry = (entry * cols + entry / rows) % GetSize();
        result.SetEntry(resultEntry, entries[entry]);
    }
    return result;
}

KMatrix KMatrix::Identity()
{
    return KMatrix(4, 4, 16,
                   1., 0., 0., 0.,
                   0., 1., 0., 0.,
                   0., 0., 1., 0.,
                   0., 0., 0., 1.);
}

KMatrix KMatrix::Scale(float x, float y, float z)
{
    return KMatrix(4, 4, 16,
                   x, 0., 0., 0.,
                   0., y, 0., 0.,
                   0., 0., z, 0.,
                   0., 0., 0., 1.);
}

KMatrix KMatrix::Translation(float x, float y, float z)
{
    return KMatrix(4, 4, 16,
                   1., 0., 0., x,
                   0., 1., 0., y,
                   0., 0., 1., z,
                   0., 0., 0., 1.);
}

KMatrix KMatrix::Rotation(float x, float y, float z)
{
    KMatrix xRotation = Identity();
    xRotation.SetEntry(1, 1, std::cos(x));
    xRotation.SetEntry(1, 2, -std::sin(x));
    xRotation.SetEntry(2, 1, std::sin(x));
    xRotation.SetEntry(2, 2, std::cos(x));
    KMatrix yRotation = Identity();
    xRotation.SetEntry(0, 0, std::cos(y));
    xRotation.SetEntry(0, 2, std::sin(y));
    xRotation.SetEntry(2, 0, -std::sin(y));
    xRotation.SetEntry(2, 2, std::cos(y));
    KMatrix zRotation = Identity();
    xRotation.SetEntry(0, 0, std::cos(x));
    xRotation.SetEntry(0, 1, -std::sin(x));
    xRotation.SetEntry(1, 0, std::sin(x));
    xRotation.SetEntry(1, 1, std::cos(x));
    KMatrix abc = xRotation * yRotation * zRotation;
    return abc;
}