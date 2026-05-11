# function

The following are the currently supported functions:

# Basic Type Functions

## exp
Computes e (Euler's number, 2.7182818...) raised to the given power num. 

    f64 exp(i32)
    f64 exp(i64)
    f64 exp(u32)
    f64 exp(u64)
    f32 exp(f32)
    f64 exp(f64)

## log
Computes the natural (base-e) logarithm of num.

    f64 log(i32)
    f64 log(i64)
    f64 log(u32)
    f64 log(u64)
    f32 log(f32)
    f64 log(f64)

## log2
Computes the binary (base-2) logarithm of num.

    f64 log2(i32)
    f64 log2(i64)
    f64 log2(u32)
    f64 log2(u64)
    f32 log2(f32)
    f64 log2(f64)

## log10
Computes the common (base-10) logarithm of num. 

    f64 log10(i32)
    f64 log10(i64)
    f64 log10(u32)
    f64 log10(u64)
    f32 log10(f32)
    f64 log10(f64)

## frac
Fractional portion of num.

    f32 frac(f32)
    f64 frac(f64)

## trunc
Computes the nearest integer not greater in magnitude than num.

    f32 trunc(f32)
    f64 trunc(f64)

## pow
Computes the value of base raised to the power exp.

    f32 pow(f32 base, i32 exp)
    f64 pow(f64 base, i32 exp)
    f64 pow(i32 base, i32 exp)
    f64 pow(i64 base, i32 exp)
    f32 pow(f32 base, f32 exp)
    f64 pow(f64 base, f64 exp)

## sin
Computes the sine of num (measured in radians).

    f32 sin(f32)
    f64 sin(f64)

## cos
Computes the cosine of num (measured in radians).

    f32 cos(f32)
    f64 cos(f64)

## tan
Computes the tangent of num (measured in radians). 

    f32 tan(f32)
    f64 tan(f64)

## sqrt
Computes the square root of num. 

    f32 sqrt(f32)
    f64 sqrt(f64)

## abs
Computes the absolute value of the num.

    i32 abs(i32)
    i64 abs(i64)
    f32 abs(f32)
    f64 abs(f64)

## ceil
Computes the least integer value not less than num. 

    f32 ceil(f32)
    f64 ceil(f64)

## floor
Computes the largest integer value not greater than num. 

    f32 floor(f32)
    f64 floor(f64)

## round
Computes the nearest integer value to num (in floating-point format), rounding halfway cases away from zero, regardless of the current rounding mode.

    f32 round(f32)
    f64 round(f64)

## min
Returns the smaller of the given values.

    u8  min(u8, u8)
    u16 min(u16, u16)
    u32 min(u32, u32)
    u64 min(u64, u64)
    i8  min(i8, i8)
    i16 min(i16, i16)
    i32 min(i32, i32)
    i64 min(i64, i64)
    f32 min(f32, f32)
    f64 min(f64, f64)

## max
Returns the greater of the given values. 

    u8  max(u8, u8)
    u16 max(u16, u16)
    u32 max(u32, u32)
    u64 max(u64, u64)
    i8  max(i8, i8)
    i16 max(i16, i16)
    i32 max(i32, i32)
    i64 max(i64, i64)
    f32 max(f32, f32)
    f64 max(f64, f64)

## Cast
Type Conversion Functions

    u8  CastU8(u8)
    u8  CastU8(i8)
    u8  CastU8(u16)
    u8  CastU8(i16)
    u8  CastU8(u32)
    u8  CastU8(i32)
    u8  CastU8(u64)
    u8  CastU8(i64)
    u8  CastU8(f32)
    u8  CastU8(f64)
    u8  CastU8(string)

    i8  CastI8(u8)
    i8  CastI8(i8)
    i8  CastI8(u16)
    i8  CastI8(i16)
    i8  CastI8(u32)
    i8  CastI8(i32)
    i8  CastI8(u64)
    i8  CastI8(i64)
    i8  CastI8(f32)
    i8  CastI8(f64)
    i8  CastI8(string)

    u16 CastU16(u8)
    u16 CastU16(i8)
    u16 CastU16(u16)
    u16 CastU16(i16)
    u16 CastU16(u32)
    u16 CastU16(i32)
    u16 CastU16(u64)
    u16 CastU16(i64)
    u16 CastU16(f32)
    u16 CastU16(f64)
    u16 CastU16(string)

    i16 CastI16(u8)
    i16 CastI16(i8)
    i16 CastI16(u16)
    i16 CastI16(i16)
    i16 CastI16(u32)
    i16 CastI16(i32)
    i16 CastI16(u64)
    i16 CastI16(i64)
    i16 CastI16(f32)
    i16 CastI16(f64)
    i16 CastI16(string)

    u32 CastU32(u8)
    u32 CastU32(i8)
    u32 CastU32(u16)
    u32 CastU32(i16)
    u32 CastU32(u32)
    u32 CastU32(i32)
    u32 CastU32(u64)
    u32 CastU32(i64)
    u32 CastU32(f32)
    u32 CastU32(f64)
    u32 CastU32(string)

    i32 CastI32(u8)
    i32 CastI32(i8)
    i32 CastI32(u16)
    i32 CastI32(i16)
    i32 CastI32(u32)
    i32 CastI32(i32)
    i32 CastI32(u64)
    i32 CastI32(i64)
    i32 CastI32(f32)
    i32 CastI32(f64)
    i32 CastI32(string)

    u64 CastU64(u8)
    u64 CastU64(i8)
    u64 CastU64(u16)
    u64 CastU64(i16)
    u64 CastU64(u32)
    u64 CastU64(i32)
    u64 CastU64(u64)
    u64 CastU64(i64)
    u64 CastU64(f32)
    u64 CastU64(f64)
    u64 CastU64(string)

    i64 CastI64(u8)
    i64 CastI64(i8)
    i64 CastI64(u16)
    i64 CastI64(i16)
    i64 CastI64(u32)
    i64 CastI64(i32)
    i64 CastI64(u64)
    i64 CastI64(i64)
    i64 CastI64(f32)
    i64 CastI64(f64)
    i64 CastI64(string)

    f32 CastF32(u8)
    f32 CastF32(i8)
    f32 CastF32(u16)
    f32 CastF32(i16)
    f32 CastF32(u32)
    f32 CastF32(i32)
    f32 CastF32(u64)
    f32 CastF32(i64)
    f32 CastF32(f32)
    f32 CastF32(f64)

    f64 CastF64(u8)
    f64 CastF64(i8)
    f64 CastF64(u16)
    f64 CastF64(i16)
    f64 CastF64(u32)
    f64 CastF64(i32)
    f64 CastF64(u64)
    f64 CastF64(i64)
    f64 CastF64(f32)
    f64 CastF64(f64)

# String Functions

## StringConcat
Concatenating two strings returns a new string.

    string StringConcat(string lhs, string rhs)

## StringCmp
Compares two strings lexicographically. The sign of the result is the sign of the difference between the values of the first pair of characters (both interpreted as unsigned char) that differ in the strings being compared.

    i32 StringCmp(string lhs, string rhs)

## StringLen
Return the length of string.

    u32 StringLen(string)

## StringContains
Check if the string contains the specified substring. Returns 1 if found, 0 otherwise.

    u8 StringContains(string haystack, string needle)

## CastString
Convert numeric value to string.

    string CastString(u8)
    string CastString(i8)
    string CastString(u16)
    string CastString(i16)
    string CastString(u32)
    string CastString(i32)
    string CastString(u64)
    string CastString(i64)
    string CastString(f32)
    string CastString(f64)

# List Type Functions

## ListConcat
Concatenating two lists returns a new list.

    u8list     ListConcat(u8list lhs, u8list rhs)
    u16list    ListConcat(u16list lhs, u16list rhs)
    u32list    ListConcat(u32list lhs, u32list rhs)
    u64list    ListConcat(u64list lhs, u64list rhs)
    i8list     ListConcat(i8list lhs, i8list rhs)
    i16list    ListConcat(i16list lhs, i16list rhs)
    i32list    ListConcat(i32list lhs, i32list rhs)
    i64list    ListConcat(i64list lhs, i64list rhs)
    f32list    ListConcat(f32list lhs, f32list rhs)
    f64list    ListConcat(f64list lhs, f64list rhs)
    stringlist ListConcat(stringlist lhs, stringlist rhs)

## CrossJoin
Cartesian-product string concatenation. Given an outer list `a`, an inner list `b`
and a separator `sep`, produces a string list of length `len(a) * len(b)` where
`result[i * len(b) + j] == a[i] + sep + b[j]`. The outer list iterates slower,
the inner list iterates faster (row-major order).

Example: `CrossJoin(["1","2","3"], ["a","b","c"], "_") ==
["1_a","1_b","1_c","2_a","2_b","2_c","3_a","3_b","3_c"]`.

If either input list is empty, the result is empty. Numeric lists are not
accepted directly — convert them to `stringlist` first via `CastStringList`.

    stringlist CrossJoin(stringlist a, stringlist b, string sep)

## ZipConcat
Per-position string concatenation. Pairs up `a[i]` with `b[i]` and produces a
string list of the same length where `result[i] == a[i] + sep + b[i]`. This is
the "zip" pattern from Python / Rust / Haskell.

Example: `ZipConcat(["a","b","c"], ["6","7","8"], ":") == ["a:6","b:7","c:8"]`.

The two input lists must have equal length; a length mismatch is reported via
`ExecContext::AddError` and surfaces as a non-OK Status from `Execute`.
Numeric lists are not accepted directly — convert them to `stringlist` first
via `CastStringList`.

    stringlist ZipConcat(stringlist a, stringlist b, string sep)

## in
Check if it is in the list.

    u8 in(u8, u8list)
    u8 in(u16, u16list)
    u8 in(u32, u32list)
    u8 in(u64, u64list)
    u8 in(i8, i8list)
    u8 in(i16, i16list)
    u8 in(i32, i32list)
    u8 in(i64, i64list)
    u8 in(f32, f32list)
    u8 in(f64, f64list)
    u8 in(string, stringlist)

## Len
Return the length of list.

    u32 Len(u8list)
    u32 Len(u16list)
    u32 Len(u32list)
    u32 Len(u64list)
    u32 Len(i8list)
    u32 Len(i16list)
    u32 Len(i32list)
    u32 Len(i64list)
    u32 Len(f32list)
    u32 Len(f64list)
    u32 Len(stringlist)

## Sum
Calculate the sum of all the numbers.

    u64 Sum(u8list)
    i64 Sum(i8list)
    u64 Sum(u16list)
    i64 Sum(i16list)
    u64 Sum(u32list)
    i64 Sum(i32list)
    u64 Sum(u64list)
    i64 Sum(i64list)
    f64 Sum(f32list)
    f64 Sum(f64list)

## Max
Find the maximum value of all the numbers.

    u8  Max(u8list)
    i8  Max(i8list)
    u16 Max(u16list)
    i16 Max(i16list)
    u32 Max(u32list)
    i32 Max(i32list)
    u64 Max(u64list)
    i64 Max(i64list)
    f32 Max(f32list)
    f64 Max(f64list)

## Min
Find the minimum value of all the numbers.

    u8  Min(u8list)
    i8  Min(i8list)
    u16 Min(u16list)
    i16 Min(i16list)
    u32 Min(u32list)
    i32 Min(i32list)
    u64 Min(u64list)
    i64 Min(i64list)
    f32 Min(f32list)
    f64 Min(f64list)

## CountDistinct
Find the count of unique elements in the list.

    u32 CountDistinct(u8list)
    u32 CountDistinct(i8list)
    u32 CountDistinct(u16list)
    u32 CountDistinct(i16list)
    u32 CountDistinct(u32list)
    u32 CountDistinct(i32list)
    u32 CountDistinct(u64list)
    u32 CountDistinct(i64list)
    u32 CountDistinct(f32list)
    u32 CountDistinct(f64list)

## Avg
Calculate the average of all the numbers. Returns 0.0 for empty lists.

    f64 Avg(u8list)
    f64 Avg(i8list)
    f64 Avg(u16list)
    f64 Avg(i16list)
    f64 Avg(u32list)
    f64 Avg(i32list)
    f64 Avg(u64list)
    f64 Avg(i64list)
    f64 Avg(f32list)
    f64 Avg(f64list)

## Median
Calculate the median of all the numbers. For even-length lists, returns the average of the two middle values. Uses nth_element internally, average O(n) time complexity.

    f64 Median(u8list)
    f64 Median(i8list)
    f64 Median(u16list)
    f64 Median(i16list)
    f64 Median(u32list)
    f64 Median(i32list)
    f64 Median(u64list)
    f64 Median(i64list)
    f64 Median(f32list)
    f64 Median(f64list)

## SortedMedian
Calculate the median of a pre-sorted list. For even-length lists, returns the average of the two middle values. O(1) time complexity. The input list must be sorted in ascending order.

    f64 SortedMedian(u8list)
    f64 SortedMedian(i8list)
    f64 SortedMedian(u16list)
    f64 SortedMedian(i16list)
    f64 SortedMedian(u32list)
    f64 SortedMedian(i32list)
    f64 SortedMedian(u64list)
    f64 SortedMedian(i64list)
    f64 SortedMedian(f32list)
    f64 SortedMedian(f64list)

## GroupIndex
Assign each element in `keys` a dense group id in first-appearance order, starting from 0. Duplicate keys get the same id, so the resulting `u32list` has the same length as `keys` and is the shared input of every other `Group*` function below.

    u32list GroupIndex(u8list keys)
    u32list GroupIndex(i8list keys)
    u32list GroupIndex(u16list keys)
    u32list GroupIndex(i16list keys)
    u32list GroupIndex(u32list keys)
    u32list GroupIndex(i32list keys)
    u32list GroupIndex(u64list keys)
    u32list GroupIndex(i64list keys)
    u32list GroupIndex(f32list keys)
    u32list GroupIndex(f64list keys)
    u32list GroupIndex(stringlist keys)

## GroupCount
Return the number of distinct groups encoded in a `group_index` produced by `GroupIndex`. Returns 0 for empty input.

    u32 GroupCount(u32list group_index)

## GroupKeys
Return the distinct keys in first-appearance order, one per group. The output length equals the number of distinct groups. `keys` and `group_index` must have the same length; `group_index` is typically the result of `GroupIndex(keys)`.

    u8list     GroupKeys(u8list keys, u32list group_index)
    i8list     GroupKeys(i8list keys, u32list group_index)
    u16list    GroupKeys(u16list keys, u32list group_index)
    i16list    GroupKeys(i16list keys, u32list group_index)
    u32list    GroupKeys(u32list keys, u32list group_index)
    i32list    GroupKeys(i32list keys, u32list group_index)
    u64list    GroupKeys(u64list keys, u32list group_index)
    i64list    GroupKeys(i64list keys, u32list group_index)
    f32list    GroupKeys(f32list keys, u32list group_index)
    f64list    GroupKeys(f64list keys, u32list group_index)
    stringlist GroupKeys(stringlist keys, u32list group_index)

## GroupSum
Per-group sum of `values`. The output has one entry per distinct group, in the same order as `GroupKeys`. Signed integers promote to `i64`, unsigned to `u64`, floats to `f64` — matching the ungrouped `Sum`. `values.len` must equal `group_index.len`.

    i64list GroupSum(i8list values, u32list group_index)
    i64list GroupSum(i16list values, u32list group_index)
    i64list GroupSum(i32list values, u32list group_index)
    i64list GroupSum(i64list values, u32list group_index)
    u64list GroupSum(u8list values, u32list group_index)
    u64list GroupSum(u16list values, u32list group_index)
    u64list GroupSum(u32list values, u32list group_index)
    u64list GroupSum(u64list values, u32list group_index)
    f64list GroupSum(f32list values, u32list group_index)
    f64list GroupSum(f64list values, u32list group_index)

## GroupMax
Per-group maximum of `values`. Output type matches the input element type (no promotion, same rule as the ungrouped `Max`). `values.len` must equal `group_index.len`.

    u8list  GroupMax(u8list values, u32list group_index)
    i8list  GroupMax(i8list values, u32list group_index)
    u16list GroupMax(u16list values, u32list group_index)
    i16list GroupMax(i16list values, u32list group_index)
    u32list GroupMax(u32list values, u32list group_index)
    i32list GroupMax(i32list values, u32list group_index)
    u64list GroupMax(u64list values, u32list group_index)
    i64list GroupMax(i64list values, u32list group_index)
    f32list GroupMax(f32list values, u32list group_index)
    f64list GroupMax(f64list values, u32list group_index)

## GroupMin
Per-group minimum of `values`. Output type matches the input element type (no promotion, same rule as the ungrouped `Min`). `values.len` must equal `group_index.len`.

    u8list  GroupMin(u8list values, u32list group_index)
    i8list  GroupMin(i8list values, u32list group_index)
    u16list GroupMin(u16list values, u32list group_index)
    i16list GroupMin(i16list values, u32list group_index)
    u32list GroupMin(u32list values, u32list group_index)
    i32list GroupMin(i32list values, u32list group_index)
    u64list GroupMin(u64list values, u32list group_index)
    i64list GroupMin(i64list values, u32list group_index)
    f32list GroupMin(f32list values, u32list group_index)
    f64list GroupMin(f64list values, u32list group_index)

## GroupAvg
Per-group average of `values`. Output is always `f64list` with one entry per distinct group, matching the ungrouped `Avg`. `values.len` must equal `group_index.len`.

    f64list GroupAvg(u8list values, u32list group_index)
    f64list GroupAvg(i8list values, u32list group_index)
    f64list GroupAvg(u16list values, u32list group_index)
    f64list GroupAvg(i16list values, u32list group_index)
    f64list GroupAvg(u32list values, u32list group_index)
    f64list GroupAvg(i32list values, u32list group_index)
    f64list GroupAvg(u64list values, u32list group_index)
    f64list GroupAvg(i64list values, u32list group_index)
    f64list GroupAvg(f32list values, u32list group_index)
    f64list GroupAvg(f64list values, u32list group_index)

## GetAt
Get the element at the specified index from the list. Returns the default value (0 for numeric types) if the index is out of bounds.

    u8  GetAt(u8list, u32 index)
    i8  GetAt(i8list, u32 index)
    u16 GetAt(u16list, u32 index)
    i16 GetAt(i16list, u32 index)
    u32 GetAt(u32list, u32 index)
    i32 GetAt(i32list, u32 index)
    u64 GetAt(u64list, u32 index)
    i64 GetAt(i64list, u32 index)
    f32 GetAt(f32list, u32 index)
    f64 GetAt(f64list, u32 index)

## Unique
Remove duplicate elements from the list while preserving the original order. Returns a new list containing only the first occurrence of each element.

    u8list     Unique(u8list src)
    u16list    Unique(u16list src)
    u32list    Unique(u32list src)
    u64list    Unique(u64list src)
    i8list     Unique(i8list src)
    i16list    Unique(i16list src)
    i32list    Unique(i32list src)
    i64list    Unique(i64list src)
    f32list    Unique(f32list src)
    f64list    Unique(f64list src)
    stringlist Unique(stringlist src)

## SortAsc
Return the sorted list in ascending order.

    u8list  SortAsc(u8list unsorted)
    u16list SortAsc(u16list unsorted)
    u32list SortAsc(u32list unsorted)
    u64list SortAsc(u64list unsorted)
    i8list  SortAsc(i8list unsorted)
    i16list SortAsc(i16list unsorted)
    i32list SortAsc(i32list unsorted)
    i64list SortAsc(i64list unsorted)
    f32list SortAsc(f32list unsorted)
    f64list SortAsc(f64list unsorted)

## SortDesc
Return the sorted list in descending order.

    u8list  SortDesc(u8list unsorted)
    u16list SortDesc(u16list unsorted)
    u32list SortDesc(u32list unsorted)
    u64list SortDesc(u64list unsorted)
    i8list  SortDesc(i8list unsorted)
    i16list SortDesc(i16list unsorted)
    i32list SortDesc(i32list unsorted)
    i64list SortDesc(i64list unsorted)
    f32list SortDesc(f32list unsorted)
    f64list SortDesc(f64list unsorted)

## Truncate
Return the truncated list.

    u8list     Truncate(u8list src, u32 trunc_num)
    u16list    Truncate(u16list src, u32 trunc_num)
    u32list    Truncate(u32list src, u32 trunc_num)
    u64list    Truncate(u64list src, u32 trunc_num)
    i8list     Truncate(i8list src, u32 trunc_num)
    i16list    Truncate(i16list src, u32 trunc_num)
    i32list    Truncate(i32list src, u32 trunc_num) 
    i64list    Truncate(i64list src, u32 trunc_num)
    f32list    Truncate(f32list src, u32 trunc_num)
    f64list    Truncate(f64list src, u32 trunc_num)  
    stringlist Truncate(stringlist src, u32 trunc_num)  

## ListAdd
Return a new list after adding a number to all elements in the list.

    u8list  ListAdd(u8list src, u8 add)
    u16list ListAdd(u16list src, u16 add)
    u32list ListAdd(u32list src, u32 add)
    u64list ListAdd(u64list src, u64 add)
    i8list  ListAdd(i8list src, i8 add)
    i16list ListAdd(i16list src, i16 add)
    i32list ListAdd(i32list src, i32 add)
    i64list ListAdd(i64list src, i64 add)
    f32list ListAdd(f32list src, f32 add)
    f64list ListAdd(f64list src, f64 add)

Add two lists element-wise and return a new list. Both lists must have equal length.

    u8list  ListAdd(u8list lhs, u8list rhs)
    u16list ListAdd(u16list lhs, u16list rhs)
    u32list ListAdd(u32list lhs, u32list rhs)
    u64list ListAdd(u64list lhs, u64list rhs)
    i8list  ListAdd(i8list lhs, i8list rhs)
    i16list ListAdd(i16list lhs, i16list rhs)
    i32list ListAdd(i32list lhs, i32list rhs)
    i64list ListAdd(i64list lhs, i64list rhs)
    f32list ListAdd(f32list lhs, f32list rhs)
    f64list ListAdd(f64list lhs, f64list rhs)

## ListSub
Return a new list after subtracting a number to all elements in the list.

    u8list  ListSub(u8list lhs, u8 rhs)
    u16list ListSub(u16list lhs, u16 rhs)
    u32list ListSub(u32list lhs, u32 rhs)
    u64list ListSub(u64list lhs, u64 rhs)
    i8list  ListSub(i8list lhs, i8 rhs)
    i16list ListSub(i16list lhs, i16 rhs)
    i32list ListSub(i32list lhs, i32 rhs)
    i64list ListSub(i64list lhs, i64 rhs)
    f32list ListSub(f32list lhs, f32 rhs)
    f64list ListSub(f64list lhs, f64 rhs)

Subtract two lists element-wise and return a new list. Both lists must have equal length.

    u8list  ListSub(u8list lhs, u8list rhs)
    u16list ListSub(u16list lhs, u16list rhs)
    u32list ListSub(u32list lhs, u32list rhs)
    u64list ListSub(u64list lhs, u64list rhs)
    i8list  ListSub(i8list lhs, i8list rhs)
    i16list ListSub(i16list lhs, i16list rhs)
    i32list ListSub(i32list lhs, i32list rhs)
    i64list ListSub(i64list lhs, i64list rhs)
    f32list ListSub(f32list lhs, f32list rhs)
    f64list ListSub(f64list lhs, f64list rhs)

## ListMul
Return a new list after multiplying all elements in the list by a number.

    u8list  ListMul(u8list lhs, u8 rhs)
    u16list ListMul(u16list lhs, u16 rhs)
    u32list ListMul(u32list lhs, u32 rhs)
    u64list ListMul(u64list lhs, u64 rhs)
    i8list  ListMul(i8list lhs, i8 rhs)
    i16list ListMul(i16list lhs, i16 rhs)
    i32list ListMul(i32list lhs, i32 rhs)
    i64list ListMul(i64list lhs, i64 rhs)
    f32list ListMul(f32list lhs, f32 rhs)
    f64list ListMul(f64list lhs, f64 rhs)

Multiply two lists element-wise and return a new list. Both lists must have equal length.

    u8list  ListMul(u8list lhs, u8list rhs)
    u16list ListMul(u16list lhs, u16list rhs)
    u32list ListMul(u32list lhs, u32list rhs)
    u64list ListMul(u64list lhs, u64list rhs)
    i8list  ListMul(i8list lhs, i8list rhs)
    i16list ListMul(i16list lhs, i16list rhs)
    i32list ListMul(i32list lhs, i32list rhs)
    i64list ListMul(i64list lhs, i64list rhs)
    f32list ListMul(f32list lhs, f32list rhs)
    f64list ListMul(f64list lhs, f64list rhs)

## ListDiv
Return a new list after dividing all elements in the list by a given number.

    u8list  ListDiv(u8list lhs, u8 rhs)
    u16list ListDiv(u16list lhs, u16 rhs)
    u32list ListDiv(u32list lhs, u32 rhs)
    u64list ListDiv(u64list lhs, u64 rhs)
    i8list  ListDiv(i8list lhs, i8 rhs)
    i16list ListDiv(i16list lhs, i16 rhs)
    i32list ListDiv(i32list lhs, i32 rhs)
    i64list ListDiv(i64list lhs, i64 rhs)
    f32list ListDiv(f32list lhs, f32 rhs)
    f64list ListDiv(f64list lhs, f64 rhs)

Divide two lists element-wise and return a new list. Both lists must have equal length.

    u8list  ListDiv(u8list lhs, u8list rhs)
    u16list ListDiv(u16list lhs, u16list rhs)
    u32list ListDiv(u32list lhs, u32list rhs)
    u64list ListDiv(u64list lhs, u64list rhs)
    i8list  ListDiv(i8list lhs, i8list rhs)
    i16list ListDiv(i16list lhs, i16list rhs)
    i32list ListDiv(i32list lhs, i32list rhs)
    i64list ListDiv(i64list lhs, i64list rhs)
    f32list ListDiv(f32list lhs, f32list rhs)
    f64list ListDiv(f64list lhs, f64list rhs)

## ListMod
Return a new list after applying the modulus operation to all elements in the list with a given number.

    u8list  ListMod(u8list lhs, u8 rhs)
    u16list ListMod(u16list lhs, u16 rhs)
    u32list ListMod(u32list lhs, u32 rhs)
    u64list ListMod(u64list lhs, u64 rhs)
    i8list  ListMod(i8list lhs, i8 rhs)
    i16list ListMod(i16list lhs, i16 rhs)
    i32list ListMod(i32list lhs, i32 rhs)
    i64list ListMod(i64list lhs, i64 rhs)

Apply the modulus operation element-wise on two lists and return a new list. Both lists must have equal length.

    u8list  ListMod(u8list lhs, u8list rhs)
    u16list ListMod(u16list lhs, u16list rhs)
    u32list ListMod(u32list lhs, u32list rhs)
    u64list ListMod(u64list lhs, u64list rhs)
    i8list  ListMod(i8list lhs, i8list rhs)
    i16list ListMod(i16list lhs, i16list rhs)
    i32list ListMod(i32list lhs, i32list rhs)
    i64list ListMod(i64list lhs, i64list rhs)

## ListBitwiseAnd
Return a new list after performing a bitwise AND operation between all elements in the list and a given number.

    u8list  ListBitwiseAnd(u8list lhs, u8 rhs)
    u16list ListBitwiseAnd(u16list lhs, u16 rhs)
    u32list ListBitwiseAnd(u32list lhs, u32 rhs)
    u64list ListBitwiseAnd(u64list lhs, u64 rhs)
    i8list  ListBitwiseAnd(i8list lhs, i8 rhs)
    i16list ListBitwiseAnd(i16list lhs, i16 rhs)
    i32list ListBitwiseAnd(i32list lhs, i32 rhs)
    i64list ListBitwiseAnd(i64list lhs, i64 rhs)

Perform a bitwise AND operation element-wise on two lists and return a new list. Both lists must have equal length.

    u8list  ListBitwiseAnd(u8list lhs, u8list rhs)
    u16list ListBitwiseAnd(u16list lhs, u16list rhs)
    u32list ListBitwiseAnd(u32list lhs, u32list rhs)
    u64list ListBitwiseAnd(u64list lhs, u64list rhs)
    i8list  ListBitwiseAnd(i8list lhs, i8list rhs)
    i16list ListBitwiseAnd(i16list lhs, i16list rhs)
    i32list ListBitwiseAnd(i32list lhs, i32list rhs)
    i64list ListBitwiseAnd(i64list lhs, i64list rhs)

## ListBitwiseOr
Return a new list after performing a bitwise OR operation between all elements in the list and a given number.

    u8list  ListBitwiseOr(u8list lhs, u8 rhs)
    u16list ListBitwiseOr(u16list lhs, u16 rhs)
    u32list ListBitwiseOr(u32list lhs, u32 rhs)
    u64list ListBitwiseOr(u64list lhs, u64 rhs)
    i8list  ListBitwiseOr(i8list lhs, i8 rhs)
    i16list ListBitwiseOr(i16list lhs, i16 rhs)
    i32list ListBitwiseOr(i32list lhs, i32 rhs)
    i64list ListBitwiseOr(i64list lhs, i64 rhs)

Perform a bitwise OR operation element-wise on two lists and return a new list. Both lists must have equal length.

    u8list  ListBitwiseOr(u8list lhs, u8list rhs)
    u16list ListBitwiseOr(u16list lhs, u16list rhs)
    u32list ListBitwiseOr(u32list lhs, u32list rhs)
    u64list ListBitwiseOr(u64list lhs, u64list rhs)
    i8list  ListBitwiseOr(i8list lhs, i8list rhs)
    i16list ListBitwiseOr(i16list lhs, i16list rhs)
    i32list ListBitwiseOr(i32list lhs, i32list rhs)
    i64list ListBitwiseOr(i64list lhs, i64list rhs)

## ListBitwiseXor
Return a new list after performing a bitwise XOR operation between all elements in the list and a given number.

    u8list  ListBitwiseXor(u8list lhs, u8 rhs)
    u16list ListBitwiseXor(u16list lhs, u16 rhs)
    u32list ListBitwiseXor(u32list lhs, u32 rhs)
    u64list ListBitwiseXor(u64list lhs, u64 rhs)
    i8list  ListBitwiseXor(i8list lhs, i8 rhs)
    i16list ListBitwiseXor(i16list lhs, i16 rhs)
    i32list ListBitwiseXor(i32list lhs, i32 rhs)
    i64list ListBitwiseXor(i64list lhs, i64 rhs)

Perform a bitwise XOR operation element-wise on two lists and return a new list. Both lists must have equal length.

    u8list  ListBitwiseXor(u8list lhs, u8list rhs)
    u16list ListBitwiseXor(u16list lhs, u16list rhs)
    u32list ListBitwiseXor(u32list lhs, u32list rhs)
    u64list ListBitwiseXor(u64list lhs, u64list rhs)
    i8list  ListBitwiseXor(i8list lhs, i8list rhs)
    i16list ListBitwiseXor(i16list lhs, i16list rhs)
    i32list ListBitwiseXor(i32list lhs, i32list rhs)
    i64list ListBitwiseXor(i64list lhs, i64list rhs)

## ListExp
Return a new list after performing the exp operation on all elements in the list.

    f64list ListExp(u8list src)
    f64list ListExp(u16list src)
    f64list ListExp(u32list src)
    f64list ListExp(u64list src)
    f64list ListExp(i8list src)
    f64list ListExp(i16list src)
    f64list ListExp(i32list src)
    f64list ListExp(i64list src)
    f32list ListExp(f32list src)
    f64list ListExp(f64list src)

## ListLog
Return a new list after performing the log operation on all elements in the list.

    f64list ListLog(u8list src)
    f64list ListLog(u16list src)
    f64list ListLog(u32list src)
    f64list ListLog(u64list src)
    f64list ListLog(i8list src)
    f64list ListLog(i16list src)
    f64list ListLog(i32list src)
    f64list ListLog(i64list src)
    f32list ListLog(f32list src)
    f64list ListLog(f64list src)

## ListLog2
Return a new list after performing the log2 operation on all elements in the list.

    f64list ListLog2(u8list src)
    f64list ListLog2(u16list src)
    f64list ListLog2(u32list src)
    f64list ListLog2(u64list src)
    f64list ListLog2(i8list src)
    f64list ListLog2(i16list src)
    f64list ListLog2(i32list src)
    f64list ListLog2(i64list src)
    f32list ListLog2(f32list src)
    f64list ListLog2(f64list src)

## ListLog10
Return a new list after performing the log10 operation on all elements in the list.

    f64list ListLog10(u8list src)
    f64list ListLog10(u16list src)
    f64list ListLog10(u32list src)
    f64list ListLog10(u64list src)
    f64list ListLog10(i8list src)
    f64list ListLog10(i16list src)
    f64list ListLog10(i32list src)
    f64list ListLog10(i64list src)
    f32list ListLog10(f32list src)
    f64list ListLog10(f64list src)

## ListCeil
Return a new list after performing the ceil operation on all elements in the list.

    f32list ListCeil(f32list src)
    f64list ListCeil(f64list src)

## ListFloor
Return a new list after performing the floor operation on all elements in the list.

    f32list ListFloor(f32list src)
    f64list ListFloor(f64list src)

## ListRound
Return a new list after performing the round operation on all elements in the list.

    f32list ListRound(f32list src)
    f64list ListRound(f64list src)

## ListMin
Return a new list after performing the min operation between each element in the list and a given number.

    u8list  ListMin(u8list lhs, u8 rhs)
    u16list ListMin(u16list lhs, u16 rhs)
    u32list ListMin(u32list lhs, u32 rhs)
    u64list ListMin(u64list lhs, u64 rhs)
    i8list  ListMin(i8list lhs, i8 rhs)
    i16list ListMin(i16list lhs, i16 rhs)
    i32list ListMin(i32list lhs, i32 rhs)
    i64list ListMin(i64list lhs, i64 rhs)
    f32list ListMin(f32list lhs, f32 rhs)
    f64list ListMin(f64list lhs, f64 rhs)

## ListMax
Return a new list after performing the max operation between each element in the list and a given number.

    u8list  ListMax(u8list lhs, u8 rhs)
    u16list ListMax(u16list lhs, u16 rhs)
    u32list ListMax(u32list lhs, u32 rhs)
    u64list ListMax(u64list lhs, u64 rhs)
    i8list  ListMax(i8list lhs, i8 rhs)
    i16list ListMax(i16list lhs, i16 rhs)
    i32list ListMax(i32list lhs, i32 rhs)
    i64list ListMax(i64list lhs, i64 rhs)
    f32list ListMax(f32list lhs, f32 rhs)
    f64list ListMax(f64list lhs, f64 rhs)

## GenLargeBitmap
Return a bitmap representation after performing a greater-than comparison on all elements in the list.

    u8list GenLargeBitmap(u8list, u8)
    u8list GenLargeBitmap(u16list, u16)
    u8list GenLargeBitmap(u32list, u32)
    u8list GenLargeBitmap(u64list, u64)
    u8list GenLargeBitmap(i8list, i8)
    u8list GenLargeBitmap(i16list, i16)
    u8list GenLargeBitmap(i32list, i32)
    u8list GenLargeBitmap(i64list, i64)
    u8list GenLargeBitmap(f32list, f32)
    u8list GenLargeBitmap(f64list, f64)

Return a bitmap representing the results of greater-than comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenLargeBitmap(u8list, u8list)
    u8list GenLargeBitmap(u16list, u16list)
    u8list GenLargeBitmap(u32list, u32list)
    u8list GenLargeBitmap(u64list, u64list)
    u8list GenLargeBitmap(i8list, i8list)
    u8list GenLargeBitmap(i16list, i16list)
    u8list GenLargeBitmap(i32list, i32list)
    u8list GenLargeBitmap(i64list, i64list)
    u8list GenLargeBitmap(f32list, f32list)
    u8list GenLargeBitmap(f64list, f64list)

## GenLargeEqualBitmap
Return a bitmap representation after performing a greater-than or equal comparison on all elements in the list.

    u8list GenLargeEqualBitmap(u8list, u8)
    u8list GenLargeEqualBitmap(u16list, u16)
    u8list GenLargeEqualBitmap(u32list, u32)
    u8list GenLargeEqualBitmap(u64list, u64)
    u8list GenLargeEqualBitmap(i8list, i8)
    u8list GenLargeEqualBitmap(i16list, i16)
    u8list GenLargeEqualBitmap(i32list, i32)
    u8list GenLargeEqualBitmap(i64list, i64)
    u8list GenLargeEqualBitmap(f32list, f32)
    u8list GenLargeEqualBitmap(f64list, f64)

Return a bitmap representing the results of greater-than or equal comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenLargeEqualBitmap(u8list, u8list)
    u8list GenLargeEqualBitmap(u16list, u16list)
    u8list GenLargeEqualBitmap(u32list, u32list)
    u8list GenLargeEqualBitmap(u64list, u64list)
    u8list GenLargeEqualBitmap(i8list, i8list)
    u8list GenLargeEqualBitmap(i16list, i16list)
    u8list GenLargeEqualBitmap(i32list, i32list)
    u8list GenLargeEqualBitmap(i64list, i64list)
    u8list GenLargeEqualBitmap(f32list, f32list)
    u8list GenLargeEqualBitmap(f64list, f64list)

## GenEqualBitmap
Return a bitmap representation after performing a equal comparison on all elements in the list.

    u8list GenEqualBitmap(u8list, u8)
    u8list GenEqualBitmap(u16list, u16)
    u8list GenEqualBitmap(u32list, u32)
    u8list GenEqualBitmap(u64list, u64)
    u8list GenEqualBitmap(i8list, i8)
    u8list GenEqualBitmap(i16list, i16)
    u8list GenEqualBitmap(i32list, i32)
    u8list GenEqualBitmap(i64list, i64)
    u8list GenEqualBitmap(f32list, f32)
    u8list GenEqualBitmap(f64list, f64)

Return a bitmap representing the results of equal comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenEqualBitmap(u8list, u8list)
    u8list GenEqualBitmap(u16list, u16list)
    u8list GenEqualBitmap(u32list, u32list)
    u8list GenEqualBitmap(u64list, u64list)
    u8list GenEqualBitmap(i8list, i8list)
    u8list GenEqualBitmap(i16list, i16list)
    u8list GenEqualBitmap(i32list, i32list)
    u8list GenEqualBitmap(i64list, i64list)
    u8list GenEqualBitmap(f32list, f32list)
    u8list GenEqualBitmap(f64list, f64list)

## GenLessBitmap
Return a bitmap representation after performing a less-than comparison on all elements in the list.

    u8list GenLessBitmap(u8list, u8)
    u8list GenLessBitmap(u16list, u16)
    u8list GenLessBitmap(u32list, u32)
    u8list GenLessBitmap(u64list, u64)
    u8list GenLessBitmap(i8list, i8)
    u8list GenLessBitmap(i16list, i16)
    u8list GenLessBitmap(i32list, i32)
    u8list GenLessBitmap(i64list, i64)
    u8list GenLessBitmap(f32list, f32)
    u8list GenLessBitmap(f64list, f64)

Return a bitmap representing the results of less-than comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenLessBitmap(u8list, u8list)
    u8list GenLessBitmap(u16list, u16list)
    u8list GenLessBitmap(u32list, u32list)
    u8list GenLessBitmap(u64list, u64list)
    u8list GenLessBitmap(i8list, i8list)
    u8list GenLessBitmap(i16list, i16list)
    u8list GenLessBitmap(i32list, i32list)
    u8list GenLessBitmap(i64list, i64list)
    u8list GenLessBitmap(f32list, f32list)
    u8list GenLessBitmap(f64list, f64list)

## GenLessEqualBitmap
Return a bitmap representation after performing a less-than or equal comparison on all elements in the list.

    u8list GenLessEqualBitmap(u8list, u8)
    u8list GenLessEqualBitmap(u16list, u16)
    u8list GenLessEqualBitmap(u32list, u32)
    u8list GenLessEqualBitmap(u64list, u64)
    u8list GenLessEqualBitmap(i8list, i8)
    u8list GenLessEqualBitmap(i16list, i16)
    u8list GenLessEqualBitmap(i32list, i32)
    u8list GenLessEqualBitmap(i64list, i64)
    u8list GenLessEqualBitmap(f32list, f32)
    u8list GenLessEqualBitmap(f64list, f64)

Return a bitmap representing the results of less-than or equal comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenLessEqualBitmap(u8list, u8list)
    u8list GenLessEqualBitmap(u16list, u16list)
    u8list GenLessEqualBitmap(u32list, u32list)
    u8list GenLessEqualBitmap(u64list, u64list)
    u8list GenLessEqualBitmap(i8list, i8list)
    u8list GenLessEqualBitmap(i16list, i16list)
    u8list GenLessEqualBitmap(i32list, i32list)
    u8list GenLessEqualBitmap(i64list, i64list)
    u8list GenLessEqualBitmap(f32list, f32list)
    u8list GenLessEqualBitmap(f64list, f64list)

## GenNotEqualBitmap
Return a bitmap representation after performing a not equal comparison on all elements in the list.

    u8list GenNotEqualBitmap(u8list, u8)
    u8list GenNotEqualBitmap(u16list, u16)
    u8list GenNotEqualBitmap(u32list, u32)
    u8list GenNotEqualBitmap(u64list, u64)
    u8list GenNotEqualBitmap(i8list, i8)
    u8list GenNotEqualBitmap(i16list, i16)
    u8list GenNotEqualBitmap(i32list, i32)
    u8list GenNotEqualBitmap(i64list, i64)
    u8list GenNotEqualBitmap(f32list, f32)
    u8list GenNotEqualBitmap(f64list, f64)

Return a bitmap representing the results of not equal comparisons for each element in the two lists. Both lists must have equal length.

    u8list GenNotEqualBitmap(u8list, u8list)
    u8list GenNotEqualBitmap(u16list, u16list)
    u8list GenNotEqualBitmap(u32list, u32list)
    u8list GenNotEqualBitmap(u64list, u64list)
    u8list GenNotEqualBitmap(i8list, i8list)
    u8list GenNotEqualBitmap(i16list, i16list)
    u8list GenNotEqualBitmap(i32list, i32list)
    u8list GenNotEqualBitmap(i64list, i64list)
    u8list GenNotEqualBitmap(f32list, f32list)
    u8list GenNotEqualBitmap(f64list, f64list)

## FilterByBitmap
Return a new list by filtering out the corresponding elements based on the bitmap.

    u8list  FilterByBitmap(u8list src, u8list bitmap)
    u16list FilterByBitmap(u16list src, u8list bitmap)
    u32list FilterByBitmap(u32list src, u8list bitmap)
    u64list FilterByBitmap(u64list src, u8list bitmap)
    i8list  FilterByBitmap(i8list src, u8list bitmap)
    i16list FilterByBitmap(i16list src, u8list bitmap)
    i32list FilterByBitmap(i32list src, u8list bitmap)
    i64list FilterByBitmap(i64list src, u8list bitmap)
    f32list FilterByBitmap(f32list src, u8list bitmap)
    f64list FilterByBitmap(f64list src, u8list bitmap)

## CountBits
Count the number of 1s in the bitmap.

    u32 CountBits(u8list)

## IfLarge
Traverse the list to find all values that meet the condition of being greater than the specified value, modify them to the target value, and return a new list.

    u8list  IfLarge(u8list src, u8 cmp_value, u8 target_value)
    u16list IfLarge(u16list src, u16 cmp_value, u16 target_value)
    u32list IfLarge(u32list src, u32 cmp_value, u32 target_value)
    u64list IfLarge(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfLarge(i8list src, i8 cmp_value, i8 target_value)
    i16list IfLarge(i16list src, i16 cmp_value, i16 target_value)
    i32list IfLarge(i32list src, i32 cmp_value, i32 target_value)
    i64list IfLarge(i64list src, i64 cmp_value, i64 target_value)
    f32list IfLarge(f32list src, f32 cmp_value, f32 target_value)
    f64list IfLarge(f64list src, f64 cmp_value, f64 target_value)

## IfLargeEqual
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfLargeEqual(u8list src, u8 cmp_value, u8 target_value)
    u16list IfLargeEqual(u16list src, u16 cmp_value, u16 target_value)
    u32list IfLargeEqual(u32list src, u32 cmp_value, u32 target_value)
    u64list IfLargeEqual(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfLargeEqual(i8list src, i8 cmp_value, i8 target_value)
    i16list IfLargeEqual(i16list src, i16 cmp_value, i16 target_value)
    i32list IfLargeEqual(i32list src, i32 cmp_value, i32 target_value)
    i64list IfLargeEqual(i64list src, i64 cmp_value, i64 target_value)
    f32list IfLargeEqual(f32list src, f32 cmp_value, f32 target_value)
    f64list IfLargeEqual(f64list src, f64 cmp_value, f64 target_value)

## IfEqual
Traverse the list to find all values that meet the condition of being equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfEqual(u8list src, u8 cmp_value, u8 target_value)
    u16list IfEqual(u16list src, u16 cmp_value, u16 target_value)
    u32list IfEqual(u32list src, u32 cmp_value, u32 target_value)
    u64list IfEqual(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfEqual(i8list src, i8 cmp_value, i8 target_value)
    i16list IfEqual(i16list src, i16 cmp_value, i16 target_value)
    i32list IfEqual(i32list src, i32 cmp_value, i32 target_value)
    i64list IfEqual(i64list src, i64 cmp_value, i64 target_value)
    f32list IfEqual(f32list src, f32 cmp_value, f32 target_value)
    f64list IfEqual(f64list src, f64 cmp_value, f64 target_value)

## IfLess
Traverse the list to find all values that meet the condition of being less than the specified value, modify them to the target value, and return a new list.

    u8list  IfLess(u8list src, u8 cmp_value, u8 target_value)
    u16list IfLess(u16list src, u16 cmp_value, u16 target_value)
    u32list IfLess(u32list src, u32 cmp_value, u32 target_value)
    u64list IfLess(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfLess(i8list src, i8 cmp_value, i8 target_value)
    i16list IfLess(i16list src, i16 cmp_value, i16 target_value)
    i32list IfLess(i32list src, i32 cmp_value, i32 target_value)
    i64list IfLess(i64list src, i64 cmp_value, i64 target_value)
    f32list IfLess(f32list src, f32 cmp_value, f32 target_value)
    f64list IfLess(f64list src, f64 cmp_value, f64 target_value)

## IfLessEqual
Traverse the list to find all values that meet the condition of being less than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfLessEqual(u8list src, u8 cmp_value, u8 target_value)
    u16list IfLessEqual(u16list src, u16 cmp_value, u16 target_value)
    u32list IfLessEqual(u32list src, u32 cmp_value, u32 target_value)
    u64list IfLessEqual(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfLessEqual(i8list src, i8 cmp_value, i8 target_value)
    i16list IfLessEqual(i16list src, i16 cmp_value, i16 target_value)
    i32list IfLessEqual(i32list src, i32 cmp_value, i32 target_value)
    i64list IfLessEqual(i64list src, i64 cmp_value, i64 target_value)
    f32list IfLessEqual(f32list src, f32 cmp_value, f32 target_value)
    f64list IfLessEqual(f64list src, f64 cmp_value, f64 target_value)

## IfNotEqual
Traverse the list to find all values that meet the condition of being not equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfNotEqual(u8list src, u8 cmp_value, u8 target_value)
    u16list IfNotEqual(u16list src, u16 cmp_value, u16 target_value)
    u32list IfNotEqual(u32list src, u32 cmp_value, u32 target_value)
    u64list IfNotEqual(u64list src, u64 cmp_value, u64 target_value)
    i8list  IfNotEqual(i8list src, i8 cmp_value, i8 target_value)
    i16list IfNotEqual(i16list src, i16 cmp_value, i16 target_value)
    i32list IfNotEqual(i32list src, i32 cmp_value, i32 target_value)
    i64list IfNotEqual(i64list src, i64 cmp_value, i64 target_value)
    f32list IfNotEqual(f32list src, f32 cmp_value, f32 target_value)
    f64list IfNotEqual(f64list src, f64 cmp_value, f64 target_value)

## IfByBitmap
Return a new list based on the values from the bitmap.

    u8list  IfByBitmap(u8list bitmap, u8list lhs, u8 rhs)
    u16list IfByBitmap(u8list bitmap, u16list lhs, u16 rhs)
    u32list IfByBitmap(u8list bitmap, u32list lhs, u32 rhs)
    u64list IfByBitmap(u8list bitmap, u64list lhs, u64 rhs)
    i8list  IfByBitmap(u8list bitmap, i8list lhs, i8 rhs)
    i16list IfByBitmap(u8list bitmap, i16list lhs, i16 rhs)
    i32list IfByBitmap(u8list bitmap, i32list lhs, i32 rhs)
    i64list IfByBitmap(u8list bitmap, i64list lhs, i64 rhs)
    f32list IfByBitmap(u8list bitmap, f32list lhs, f32 rhs)
    f64list IfByBitmap(u8list bitmap, f64list lhs, f64 rhs)

    u8list  IfByBitmap(u8list bitmap, u8 lhs, u8list rhs)
    u16list IfByBitmap(u8list bitmap, u16 lhs, u16list rhs)
    u32list IfByBitmap(u8list bitmap, u32 lhs, u32list rhs)
    u64list IfByBitmap(u8list bitmap, u64 lhs, u64list rhs)
    i8list  IfByBitmap(u8list bitmap, i8 lhs, i8list rhs)
    i16list IfByBitmap(u8list bitmap, i16 lhs, i16list rhs)
    i32list IfByBitmap(u8list bitmap, i32 lhs, i32list rhs)
    i64list IfByBitmap(u8list bitmap, i64 lhs, i64list rhs)
    f32list IfByBitmap(u8list bitmap, f32 lhs, f32list rhs)
    f64list IfByBitmap(u8list bitmap, f64 lhs, f64list rhs)

    u8list  IfByBitmap(u8list bitmap, u8list lhs, u8list rhs)
    u16list IfByBitmap(u8list bitmap, u16list lhs, u16list rhs)
    u32list IfByBitmap(u8list bitmap, u32list lhs, u32list rhs)
    u64list IfByBitmap(u8list bitmap, u64list lhs, u64list rhs)
    i8list  IfByBitmap(u8list bitmap, i8list lhs, i8list rhs)
    i16list IfByBitmap(u8list bitmap, i16list lhs, i16list rhs)
    i32list IfByBitmap(u8list bitmap, i32list lhs, i32list rhs)
    i64list IfByBitmap(u8list bitmap, i64list lhs, i64list rhs)
    f32list IfByBitmap(u8list bitmap, f32list lhs, f32list rhs)
    f64list IfByBitmap(u8list bitmap, f64list lhs, f64list rhs)

## MurmurHash3X8632
MurmurHash3_x86_32 algorithm

    u32 MurmurHash3X8632(u8list)
    u32 MurmurHash3X8632(u16list)
    u32 MurmurHash3X8632(u32list)
    u32 MurmurHash3X8632(u64list)
    u32 MurmurHash3X8632(i8list)
    u32 MurmurHash3X8632(i16list)
    u32 MurmurHash3X8632(i32list)
    u32 MurmurHash3X8632(i64list)
    u32 MurmurHash3X8632(f32list)
    u32 MurmurHash3X8632(f64list)

## ListCast
List Type Conversion Functions. Cast a list from one element type to another.

Numeric/String list to numeric list conversion:

    u8list  CastU8List(i8list src)
    u8list  CastU8List(u16list src)
    u8list  CastU8List(i16list src)
    u8list  CastU8List(u32list src)
    u8list  CastU8List(i32list src)
    u8list  CastU8List(u64list src)
    u8list  CastU8List(i64list src)
    u8list  CastU8List(f32list src)
    u8list  CastU8List(f64list src)
    u8list  CastU8List(stringlist src)

    i8list  CastI8List(u8list src)
    i8list  CastI8List(u16list src)
    i8list  CastI8List(i16list src)
    i8list  CastI8List(u32list src)
    i8list  CastI8List(i32list src)
    i8list  CastI8List(u64list src)
    i8list  CastI8List(i64list src)
    i8list  CastI8List(f32list src)
    i8list  CastI8List(f64list src)
    i8list  CastI8List(stringlist src)

    u16list CastU16List(u8list src)
    u16list CastU16List(i8list src)
    u16list CastU16List(i16list src)
    u16list CastU16List(u32list src)
    u16list CastU16List(i32list src)
    u16list CastU16List(u64list src)
    u16list CastU16List(i64list src)
    u16list CastU16List(f32list src)
    u16list CastU16List(f64list src)
    u16list CastU16List(stringlist src)

    i16list CastI16List(u8list src)
    i16list CastI16List(i8list src)
    i16list CastI16List(u16list src)
    i16list CastI16List(u32list src)
    i16list CastI16List(i32list src)
    i16list CastI16List(u64list src)
    i16list CastI16List(i64list src)
    i16list CastI16List(f32list src)
    i16list CastI16List(f64list src)
    i16list CastI16List(stringlist src)

    u32list CastU32List(u8list src)
    u32list CastU32List(i8list src)
    u32list CastU32List(u16list src)
    u32list CastU32List(i16list src)
    u32list CastU32List(i32list src)
    u32list CastU32List(u64list src)
    u32list CastU32List(i64list src)
    u32list CastU32List(f32list src)
    u32list CastU32List(f64list src)
    u32list CastU32List(stringlist src)

    i32list CastI32List(u8list src)
    i32list CastI32List(i8list src)
    i32list CastI32List(u16list src)
    i32list CastI32List(i16list src)
    i32list CastI32List(u32list src)
    i32list CastI32List(u64list src)
    i32list CastI32List(i64list src)
    i32list CastI32List(f32list src)
    i32list CastI32List(f64list src)
    i32list CastI32List(stringlist src)

    u64list CastU64List(u8list src)
    u64list CastU64List(i8list src)
    u64list CastU64List(u16list src)
    u64list CastU64List(i16list src)
    u64list CastU64List(u32list src)
    u64list CastU64List(i32list src)
    u64list CastU64List(i64list src)
    u64list CastU64List(f32list src)
    u64list CastU64List(f64list src)
    u64list CastU64List(stringlist src)

    i64list CastI64List(u8list src)
    i64list CastI64List(i8list src)
    i64list CastI64List(u16list src)
    i64list CastI64List(i16list src)
    i64list CastI64List(u32list src)
    i64list CastI64List(i32list src)
    i64list CastI64List(u64list src)
    i64list CastI64List(f32list src)
    i64list CastI64List(f64list src)
    i64list CastI64List(stringlist src)

    f32list CastF32List(u8list src)
    f32list CastF32List(i8list src)
    f32list CastF32List(u16list src)
    f32list CastF32List(i16list src)
    f32list CastF32List(u32list src)
    f32list CastF32List(i32list src)
    f32list CastF32List(u64list src)
    f32list CastF32List(i64list src)
    f32list CastF32List(f64list src)

    f64list CastF64List(u8list src)
    f64list CastF64List(i8list src)
    f64list CastF64List(u16list src)
    f64list CastF64List(i16list src)
    f64list CastF64List(u32list src)
    f64list CastF64List(i32list src)
    f64list CastF64List(u64list src)
    f64list CastF64List(i64list src)
    f64list CastF64List(f32list src)

Note: CastF32List and CastF64List do not support stringlist input. String to integer list conversion uses std::from_chars internally. String to floating-point list conversion is not supported in C++17.

Numeric list to string list conversion:

    stringlist CastStringList(u8list src)
    stringlist CastStringList(i8list src)
    stringlist CastStringList(u16list src)
    stringlist CastStringList(i16list src)
    stringlist CastStringList(u32list src)
    stringlist CastStringList(i32list src)
    stringlist CastStringList(u64list src)
    stringlist CastStringList(i64list src)
    stringlist CastStringList(f32list src)
    stringlist CastStringList(f64list src)

## ListLookupIndex
Given two lists `a` and `b` of the same element type, for each element in `a` return the position of its first occurrence in `b`. When an element of `a` is not found in `b`, the result entry is set to `u32max` (`std::numeric_limits<uint32_t>::max()`) as a miss sentinel. Internally a hash table over `b` is built, so the per-element lookup is amortised O(1).

The output length always equals `a.len`.

    u32list ListLookupIndex(u8list  a, u8list  b)
    u32list ListLookupIndex(i8list  a, i8list  b)
    u32list ListLookupIndex(u16list a, u16list b)
    u32list ListLookupIndex(i16list a, i16list b)
    u32list ListLookupIndex(u32list a, u32list b)
    u32list ListLookupIndex(i32list a, i32list b)
    u32list ListLookupIndex(u64list a, u64list b)
    u32list ListLookupIndex(i64list a, i64list b)
    u32list ListLookupIndex(f32list a, f32list b)
    u32list ListLookupIndex(f64list a, f64list b)
    u32list ListLookupIndex(stringlist a, stringlist b)

## Find
Return the index of the first occurrence of `value` in the list. Returns `u32max` (`std::numeric_limits<uint32_t>::max()`) when the value is not found. This is the scalar counterpart of `ListLookupIndex` and composes naturally with `GetAt`: `GetAt(values, Find(keys, k))` performs a scalar join (miss falls through to `GetAt`'s out-of-bounds default of zero/empty).

    u32 Find(u8list     a, u8     value)
    u32 Find(i8list     a, i8     value)
    u32 Find(u16list    a, u16    value)
    u32 Find(i16list    a, i16    value)
    u32 Find(u32list    a, u32    value)
    u32 Find(i32list    a, i32    value)
    u32 Find(u64list    a, u64    value)
    u32 Find(i64list    a, i64    value)
    u32 Find(f32list    a, f32    value)
    u32 Find(f64list    a, f64    value)
    u32 Find(stringlist a, string value)

## FindSorted
Binary-search version of `Find`: O(log n) instead of O(n). The caller **must** guarantee `a` is sorted in ascending order (lexicographic order for `stringlist`); otherwise the result is unspecified. Returns `u32max` (same miss sentinel as `Find`) when `value` is absent, and the index of the first equal element when duplicates exist (same as `std::lower_bound`).

Use this over `Find` when the key list is large and known to be sorted (e.g. a pre-sorted dimension table).

    u32 FindSorted(u8list     a, u8     value)
    u32 FindSorted(i8list     a, i8     value)
    u32 FindSorted(u16list    a, u16    value)
    u32 FindSorted(i16list    a, i16    value)
    u32 FindSorted(u32list    a, u32    value)
    u32 FindSorted(i32list    a, i32    value)
    u32 FindSorted(u64list    a, u64    value)
    u32 FindSorted(i64list    a, i64    value)
    u32 FindSorted(f32list    a, f32    value)
    u32 FindSorted(f64list    a, f64    value)
    u32 FindSorted(stringlist a, string value)

## FindMiss
Zero-arg function that returns the lookup-miss sentinel (`u32max`, i.e. `std::numeric_limits<uint32_t>::max()`). Use this instead of spelling out `4294967295u32` when you need to compare against the miss value, e.g. `Find(keys, k) != FindMiss()`. Being a read-only pure function with no inputs, it is constant-folded at JIT time and has zero runtime cost.

    u32 FindMiss()


## ListCompactPositions
Takes a `u32list` produced by `ListLookupIndex` and returns the positions in `a` that hit (i.e. the indices in the raw list whose value is **not** the miss sentinel `u32max`). Useful for building the filtered a-side of a join.

Output length equals the number of hits; preserves the original order.

    u32list ListCompactPositions(u32list raw)

## ListCompactIndex
Takes a `u32list` produced by `ListLookupIndex` and returns the looked-up b-side indices themselves, skipping miss entries. Useful for directly gathering a b-side column aligned to the hit subset of a.

Output length equals the number of hits; preserves the original order.

    u32list ListCompactIndex(u32list raw)

## ListGather
Element-wise gather from a `values` list using a `u32list` of positions. For each `k`, if `idx[k] < values.len` the output is `values[idx[k]]`, otherwise it is the supplied `default_value`. This is the typical "realign a b-side column to the a-side shape" kernel of a join.

The output length always equals `idx.len`.

    u8list     ListGather(u8list     values, u32list idx, u8     default_value)
    i8list     ListGather(i8list     values, u32list idx, i8     default_value)
    u16list    ListGather(u16list    values, u32list idx, u16    default_value)
    i16list    ListGather(i16list    values, u32list idx, i16    default_value)
    u32list    ListGather(u32list    values, u32list idx, u32    default_value)
    i32list    ListGather(i32list    values, u32list idx, i32    default_value)
    u64list    ListGather(u64list    values, u32list idx, u64    default_value)
    i64list    ListGather(i64list    values, u32list idx, i64    default_value)
    f32list    ListGather(f32list    values, u32list idx, f32    default_value)
    f64list    ListGather(f64list    values, u32list idx, f64    default_value)
    stringlist ListGather(stringlist values, u32list idx, string default_value)

## Bucketize
Classify each element of `values` into a half-open bin defined by an
ascending `boundaries` list. Aligned with NumPy `digitize`, TensorFlow /
PyTorch `bucketize`, PostgreSQL `width_bucket(array)` and Apache Arrow's bin
kernel.

For `boundaries = [b0, b1, ..., b_{n-1}]` the bin layout is

    bucket 0       : (-inf, b0)
    bucket i (1..n-1) : [b_{i-1}, b_i)
    bucket n       : [b_{n-1}, +inf)

i.e. left-closed / right-open. Result has the same length as `values`.

* Out-of-range values land in bucket `0` or bucket `n` (no error).
* Empty `boundaries` collapses to a single `(-inf, +inf)` bucket — every
  value returns `0`.
* For floating-point lists, `NaN` is mapped to the top bucket `n`
  (matching NumPy / PyTorch).
* `boundaries` **must** be sorted ascending; like `FindSorted`, the result
  is unspecified otherwise (no runtime check).

Implementation is `std::lower_bound`, O(log n) per value.

    u32list Bucketize(u8list  values, u8list  boundaries)
    u32list Bucketize(i8list  values, i8list  boundaries)
    u32list Bucketize(u16list values, u16list boundaries)
    u32list Bucketize(i16list values, i16list boundaries)
    u32list Bucketize(u32list values, u32list boundaries)
    u32list Bucketize(i32list values, i32list boundaries)
    u32list Bucketize(u64list values, u64list boundaries)
    u32list Bucketize(i64list values, i64list boundaries)
    u32list Bucketize(f32list values, f32list boundaries)
    u32list Bucketize(f64list values, f64list boundaries)
