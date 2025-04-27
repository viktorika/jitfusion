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
    f32 log(f64)

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
    f64 log10(f64)
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

    string StringConcat(string lhs, string rhs, ptr exec_ctx)

## StringCmp
Compares two null-terminated byte strings lexicographically.The sign of the result is the sign of the difference between the values of the first pair of characters (both interpreted as unsigned char) that differ in the strings being compared.The behavior is undefined if lhs or rhs are not pointers to null-terminated byte strings.

    i32 StringCmp(string lhs, string rhs)

## StringLen
Return the length of string.

    u32 StringLen(string)

# List Type Functions

## ListConcat
Concatenating two lists returns a new list.

    u8list     ListConcat(u8list lhs, u8list rhs, ptr exec_ctx)
    u16list    ListConcat(u16list lhs, u16list rhs, ptr exec_ctx)
    u32list    ListConcat(u32list lhs, u32list rhs, ptr exec_ctx)
    u64list    ListConcat(u64list lhs, u64list rhs, ptr exec_ctx)
    i8list     ListConcat(i8list lhs, i8list rhs, ptr exec_ctx)
    i16list    ListConcat(i16list lhs, i16list rhs, ptr exec_ctx)
    i32list    ListConcat(i32list lhs, i32list rhs, ptr exec_ctx)
    i64list    ListConcat(i64list lhs, i64list rhs, ptr exec_ctx)
    f32list    ListConcat(f32list lhs, f32list rhs, ptr exec_ctx)
    f64list    ListConcat(f64list lhs, f64list rhs, ptr exec_ctx)
    stringlist ListConcat(stringlist lhs, stringlist rhs, ptr exec_ctx)

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

## SortAsc
Return the sorted list in ascending order.

    u8list  SortAsc(u8list unsorted, ptr exec_ctx)
    u16list SortAsc(u16list unsorted, ptr exec_ctx)
    u32list SortAsc(u32list unsorted, ptr exec_ctx)
    u64list SortAsc(u64list unsorted, ptr exec_ctx)
    i8list  SortAsc(i8list unsorted, ptr exec_ctx)
    i16list SortAsc(i16list unsorted, ptr exec_ctx)
    i32list SortAsc(i32list unsorted, ptr exec_ctx)
    i64list SortAsc(i64list unsorted, ptr exec_ctx)
    f32list SortAsc(f32list unsorted, ptr exec_ctx)
    f64list SortAsc(f64list unsorted, ptr exec_ctx)

## SortDesc
Return the sorted list in descending order.

    u8list  SortDesc(u8list unsorted, ptr exec_ctx)
    u16list SortDesc(u16list unsorted, ptr exec_ctx)
    u32list SortDesc(u32list unsorted, ptr exec_ctx)
    u64list SortDesc(u64list unsorted, ptr exec_ctx)
    i8list  SortDesc(i8list unsorted, ptr exec_ctx)
    i16list SortDesc(i16list unsorted, ptr exec_ctx)
    i32list SortDesc(i32list unsorted, ptr exec_ctx)
    i64list SortDesc(i64list unsorted, ptr exec_ctx)
    f32list SortDesc(f32list unsorted, ptr exec_ctx)
    f64list SortDesc(f64list unsorted, ptr exec_ctx)

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

    u8list  ListAdd(u8list src, u8 add, ptr exec_ctx)
    u16list ListAdd(u16list src, u16 add, ptr exec_ctx)
    u32list ListAdd(u32list src, u32 add, ptr exec_ctx)
    u64list ListAdd(u64list src, u64 add, ptr exec_ctx)
    i8list  ListAdd(i8list src, i8 add, ptr exec_ctx)
    i16list ListAdd(i16list src, i16 add, ptr exec_ctx)
    i32list ListAdd(i32list src, i32 add, ptr exec_ctx)
    i64list ListAdd(i64list src, i64 add, ptr exec_ctx)
    f32list ListAdd(f32list src, f32 add, ptr exec_ctx)
    f64list ListAdd(f64list src, f64 add, ptr exec_ctx)

## ListAddWithMinSize
Add two lists and return a new list, taking the smaller length.

    u8list  ListAddWithMinSize(u8list lhs, u8list rhs, ptr exec_ctx)
    u16list ListAddWithMinSize(u16list lhs, u16list rhs, ptr exec_ctx)
    u32list ListAddWithMinSize(u32list lhs, u32list rhs, ptr exec_ctx)
    u64list ListAddWithMinSize(u64list lhs, u64list rhs, ptr exec_ctx)
    i8list  ListAddWithMinSize(i8list lhs, i8list rhs, ptr exec_ctx)
    i16list ListAddWithMinSize(i16list lhs, i16list rhs, ptr exec_ctx)
    i32list ListAddWithMinSize(i32list lhs, i32list rhs, ptr exec_ctx)
    i64list ListAddWithMinSize(i64list lhs, i64list rhs, ptr exec_ctx)
    f32list ListAddWithMinSize(f32list lhs, f32list rhs, ptr exec_ctx)
    f64list ListAddWithMinSize(f64list lhs, f64list rhs, ptr exec_ctx)

## ListSub
Return a new list after subtracting a number to all elements in the list.

    u8list  ListSub(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListSub(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListSub(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListSub(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListSub(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListSub(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListSub(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListSub(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListSub(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListSub(f64list lhs, f64 rhs, ptr exec_ctx)

## ListSubWithMinSize
Subtract two lists and return a new list, taking the smaller length.

    u8list  ListSubWithMinSize(u8list lhs, u8list rhs, ptr exec_ctx)
    u16list ListSubWithMinSize(u16list lhs, u16list rhs, ptr exec_ctx)
    u32list ListSubWithMinSize(u32list lhs, u32list rhs, ptr exec_ctx)
    u64list ListSubWithMinSize(u64list lhs, u64list rhs, ptr exec_ctx)
    i8list  ListSubWithMinSize(i8list lhs, i8list rhs, ptr exec_ctx)
    i16list ListSubWithMinSize(i16list lhs, i16list rhs, ptr exec_ctx)
    i32list ListSubWithMinSize(i32list lhs, i32list rhs, ptr exec_ctx)
    i64list ListSubWithMinSize(i64list lhs, i64list rhs, ptr exec_ctx)
    f32list ListSubWithMinSize(f32list lhs, f32list rhs, ptr exec_ctx)
    f64list ListSubWithMinSize(f64list lhs, f64list rhs, ptr exec_ctx)

## ListMul
Return a new list after multiplying all elements in the list by a number.

    u8list  ListMul(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListMul(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListMul(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListMul(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListMul(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListMul(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListMul(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListMul(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListMul(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListMul(f64list lhs, f64 rhs, ptr exec_ctx)

## ListMulWithMinSize
Multiply two lists and return a new list, taking the smaller length.

    u8list  ListMulWithMinSize(u8list lhs, u8list rhs, ptr exec_ctx)
    u16list ListMulWithMinSize(u16list lhs, u16list rhs, ptr exec_ctx)
    u32list ListMulWithMinSize(u32list lhs, u32list rhs, ptr exec_ctx)
    u64list ListMulWithMinSize(u64list lhs, u64list rhs, ptr exec_ctx)
    i8list  ListMulWithMinSize(i8list lhs, i8list rhs, ptr exec_ctx)
    i16list ListMulWithMinSize(i16list lhs, i16list rhs, ptr exec_ctx)
    i32list ListMulWithMinSize(i32list lhs, i32list rhs, ptr exec_ctx)
    i64list ListMulWithMinSize(i64list lhs, i64list rhs, ptr exec_ctx)
    f32list ListMulWithMinSize(f32list lhs, f32list rhs, ptr exec_ctx)
    f64list ListMulWithMinSize(f64list lhs, f64list rhs, ptr exec_ctx)

# ListDiv
Return a new list after dividing all elements in the list by a number.

    u8list  ListDiv(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListDiv(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListDiv(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListDiv(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListDiv(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListDiv(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListDiv(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListDiv(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListDiv(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListDiv(f64list lhs, f64 rhs, ptr exec_ctx)

## ListDivWithMinSize
Divide two lists and return a new list, taking the smaller length.

    u8list  ListDivWithMinSize(u8list lhs, u8list rhs, ptr exec_ctx)
    u16list ListDivWithMinSize(u16list lhs, u16list rhs, ptr exec_ctx)
    u32list ListDivWithMinSize(u32list lhs, u32list rhs, ptr exec_ctx)
    u64list ListDivWithMinSize(u64list lhs, u64list rhs, ptr exec_ctx)
    i8list  ListDivWithMinSize(i8list lhs, i8list rhs, ptr exec_ctx)
    i16list ListDivWithMinSize(i16list lhs, i16list rhs, ptr exec_ctx)
    i32list ListDivWithMinSize(i32list lhs, i32list rhs, ptr exec_ctx)
    i64list ListDivWithMinSize(i64list lhs, i64list rhs, ptr exec_ctx)
    f32list ListDivWithMinSize(f32list lhs, f32list rhs, ptr exec_ctx)
    f64list ListDivWithMinSize(f64list lhs, f64list rhs, ptr exec_ctx)

## ListMod
Return a new list after applying the modulus operation to all elements in the list with a given number.

    u8list  ListMod(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListMod(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListMod(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListMod(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListMod(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListMod(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListMod(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListMod(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListMod(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListMod(f64list lhs, f64 rhs, ptr exec_ctx)

## ListModWithMinSize
Applying the modulus operation of two lists and return a new list, using the smaller length.

    u8list  ListModWithMinSize(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListModWithMinSize(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListModWithMinSize(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListModWithMinSize(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListModWithMinSize(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListModWithMinSize(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListModWithMinSize(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListModWithMinSize(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListModWithMinSize(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListModWithMinSize(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseAnd
Return a new list after performing a bitwise AND operation between all elements in the list and a given number.

    u8list  ListBitwiseAnd(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseAnd(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseAnd(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseAnd(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseAnd(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseAnd(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseAnd(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseAnd(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseAnd(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseAnd(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseAndWithMinSize
Perform a bitwise AND operation between two lists and return a new list, using the smaller length.

    u8list  ListBitwiseAndWithMinSize(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseAndWithMinSize(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseAndWithMinSize(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseAndWithMinSize(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseAndWithMinSize(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseAndWithMinSize(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseAndWithMinSize(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseAndWithMinSize(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseAndWithMinSize(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseAndWithMinSize(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseOr
Return a new list after performing a bitwise OR operation between all elements in the list and a given number.

    u8list  ListBitwiseOr(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseOr(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseOr(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseOr(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseOr(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseOr(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseOr(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseOr(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseOr(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseOr(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseOrWithMinSize
Perform a bitwise OR operation between two lists and return a new list, using the smaller length.

    u8list  ListBitwiseOrWithMinSize(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseOrWithMinSize(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseOrWithMinSize(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseOrWithMinSize(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseOrWithMinSize(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseOrWithMinSize(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseOrWithMinSize(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseOrWithMinSize(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseOrWithMinSize(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseOrWithMinSize(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseXor
Return a new list after performing a bitwise XOR operation between all elements in the list and a given number.

    u8list  ListBitwiseXor(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseXor(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseXor(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseXor(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseXor(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseXor(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseXor(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseXor(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseXor(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseXor(f64list lhs, f64 rhs, ptr exec_ctx)

## ListBitwiseXorWithMinSize
Perform a bitwise XOR operation between two lists and return a new list, using the smaller length.

    u8list  ListBitwiseXorWithMinSize(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListBitwiseXorWithMinSize(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListBitwiseXorWithMinSize(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListBitwiseXorWithMinSize(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListBitwiseXorWithMinSize(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListBitwiseXorWithMinSize(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListBitwiseXorWithMinSize(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListBitwiseXorWithMinSize(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListBitwiseXorWithMinSize(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListBitwiseXorWithMinSize(f64list lhs, f64 rhs, ptr exec_ctx)

## ListExp
Return a new list after performing the exp operation on all elements in the list.

    f64list ListExp(u8list src, ptr exec_ctx)
    f64list ListExp(u16list src, ptr exec_ctx)
    f64list ListExp(u32list src, ptr exec_ctx)
    f64list ListExp(u64list src, ptr exec_ctx)
    f64list ListExp(i8list src, ptr exec_ctx)
    f64list ListExp(i16list src, ptr exec_ctx)
    f64list ListExp(i32list src, ptr exec_ctx)
    f64list ListExp(i64list src, ptr exec_ctx)
    f32list ListExp(f32list src, ptr exec_ctx)
    f64list ListExp(f64list src, ptr exec_ctx)

## ListLog
Return a new list after performing the log operation on all elements in the list.

    f64list ListLog(u8list src, ptr exec_ctx)
    f64list ListLog(u16list src, ptr exec_ctx)
    f64list ListLog(u32list src, ptr exec_ctx)
    f64list ListLog(u64list src, ptr exec_ctx)
    f64list ListLog(i8list src, ptr exec_ctx)
    f64list ListLog(i16list src, ptr exec_ctx)
    f64list ListLog(i32list src, ptr exec_ctx)
    f64list ListLog(i64list src, ptr exec_ctx)
    f32list ListLog(f32list src, ptr exec_ctx)
    f64list ListLog(f64list src, ptr exec_ctx)

## ListLog2
Return a new list after performing the log2 operation on all elements in the list.

    f64list ListLog2(u8list src, ptr exec_ctx)
    f64list ListLog2(u16list src, ptr exec_ctx)
    f64list ListLog2(u32list src, ptr exec_ctx)
    f64list ListLog2(u64list src, ptr exec_ctx)
    f64list ListLog2(i8list src, ptr exec_ctx)
    f64list ListLog2(i16list src, ptr exec_ctx)
    f64list ListLog2(i32list src, ptr exec_ctx)
    f64list ListLog2(i64list src, ptr exec_ctx)
    f32list ListLog2(f32list src, ptr exec_ctx)
    f64list ListLog2(f64list src, ptr exec_ctx)

## ListLog10
Return a new list after performing the log10 operation on all elements in the list.

    f64list ListLog10(u8list src, ptr exec_ctx)
    f64list ListLog10(u16list src, ptr exec_ctx)
    f64list ListLog10(u32list src, ptr exec_ctx)
    f64list ListLog10(u64list src, ptr exec_ctx)
    f64list ListLog10(i8list src, ptr exec_ctx)
    f64list ListLog10(i16list src, ptr exec_ctx)
    f64list ListLog10(i32list src, ptr exec_ctx)
    f64list ListLog10(i64list src, ptr exec_ctx)
    f32list ListLog10(f32list src, ptr exec_ctx)
    f64list ListLog10(f64list src, ptr exec_ctx)

## ListCeil
Return a new list after performing the ceil operation on all elements in the list.

    f32list ListCeil(f32list src, ptr exec_ctx)
    f64list ListCeil(f64list src, ptr exec_ctx)

## ListFloor
Return a new list after performing the floor operation on all elements in the list.

    f32list ListFloor(f32list src, ptr exec_ctx)
    f64list ListFloor(f64list src, ptr exec_ctx)

## ListRound
Return a new list after performing the round operation on all elements in the list.

    f32list ListRound(f32list src, ptr exec_ctx)
    f64list ListRound(f64list src, ptr exec_ctx)

## ListMin
Return a new list after performing the min operation between each element in the list and a given number.

    u8list  ListMin(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListMin(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListMin(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListMin(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListMin(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListMin(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListMin(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListMin(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListMin(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListMin(f64list lhs, f64 rhs, ptr exec_ctx)

## ListMax
Return a new list after performing the max operation between each element in the list and a given number.

    u8list  ListMax(u8list lhs, u8 rhs, ptr exec_ctx)
    u16list ListMax(u16list lhs, u16 rhs, ptr exec_ctx)
    u32list ListMax(u32list lhs, u32 rhs, ptr exec_ctx)
    u64list ListMax(u64list lhs, u64 rhs, ptr exec_ctx)
    i8list  ListMax(i8list lhs, i8 rhs, ptr exec_ctx)
    i16list ListMax(i16list lhs, i16 rhs, ptr exec_ctx)
    i32list ListMax(i32list lhs, i32 rhs, ptr exec_ctx)
    i64list ListMax(i64list lhs, i64 rhs, ptr exec_ctx)
    f32list ListMax(f32list lhs, f32 rhs, ptr exec_ctx)
    f64list ListMax(f64list lhs, f64 rhs, ptr exec_ctx)

## GenLargeFilterBitmap
Return a bitmap representation after performing a greater-than comparison on all elements in the list.

    u8list GenLargeFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenLargeFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenLargeFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenLargeFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenLargeFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenLargeFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenLargeFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenLargeFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenLargeFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenLargeFilterBitmap(f64list, f64, ptr exec_ctx)

## GenLargeEqualFilterBitmap
Return a bitmap representation after performing a greater-than or equal comparison on all elements in the list.

    u8list GenLargeEqualFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenLargeEqualFilterBitmap(f64list, f64, ptr exec_ctx)

## GenEqualFilterBitmap
Return a bitmap representation after performing a equal comparison on all elements in the list.

    u8list GenEqualFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenEqualFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenEqualFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenEqualFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenEqualFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenEqualFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenEqualFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenEqualFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenEqualFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenEqualFilterBitmap(f64list, f64, ptr exec_ctx)

## GenLessFilterBitmap
Return a bitmap representation after performing a less-than comparison on all elements in the list.

    u8list GenLessFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenLessFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenLessFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenLessFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenLessFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenLessFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenLessFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenLessFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenLessFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenLessFilterBitmap(f64list, f64, ptr exec_ctx)

## GenLessEqualFilterBitmap
Return a bitmap representation after performing a less-than or equal comparison on all elements in the list.

    u8list GenLessEqualFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenLessEqualFilterBitmap(f64list, f64, ptr exec_ctx)

## GenNotEqualFilterBitmap
Return a bitmap representation after performing a not equal comparison on all elements in the list.

    u8list GenNotEqualFilterBitmap(u8list, u8, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(u16list, u16, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(u32list, u32, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(u64list, u64, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(i8list, i8, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(i16list, i16, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(i32list, i32, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(i64list, i64, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(f32list, f32, ptr exec_ctx)
    u8list GenNotEqualFilterBitmap(f64list, f64, ptr exec_ctx)

## FilterByBitmap
Return a new list by filtering out the corresponding elements based on the bitmap.

    u8list  FilterByBitmap(u8list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    u16list FilterByBitmap(u16list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    u32list FilterByBitmap(u32list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    u64list FilterByBitmap(u64list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    i8list  FilterByBitmap(i8list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    i16list FilterByBitmap(i16list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    i32list FilterByBitmap(i32list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    i64list FilterByBitmap(i64list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    f32list FilterByBitmap(f32list src, u8list bitmap, u32 bits_count, ptr exec_ctx)
    f64list FilterByBitmap(f64list src, u8list bitmap, u32 bits_count, ptr exec_ctx)

## CountBits
Count the number of 1s in the bitmap.

    u32 CountBits(u8list)

## IfLarge
Traverse the list to find all values that meet the condition of being greater than the specified value, modify them to the target value, and return a new list.

    u8list  IfLarge(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfLarge(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfLarge(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfLarge(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfLarge(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfLarge(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfLarge(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfLarge(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfLarge(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfLarge(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

## IfLargeEqual
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfLargeEqual(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfLargeEqual(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfLargeEqual(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfLargeEqual(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfLargeEqual(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfLargeEqual(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfLargeEqual(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfLargeEqual(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfLargeEqual(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfLargeEqual(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

## IfEqual
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfEqual(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfEqual(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfEqual(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfEqual(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfEqual(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfEqual(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfEqual(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfEqual(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfEqual(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfEqual(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

## IfLess
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfLess(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfLess(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfLess(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfLess(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfLess(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfLess(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfLess(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfLess(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfLess(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfLess(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

## IfLessEqual
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfLessEqual(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfLessEqual(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfLessEqual(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfLessEqual(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfLessEqual(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfLessEqual(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfLessEqual(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfLessEqual(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfLessEqual(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfLessEqual(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

## IfNotEqual
Traverse the list to find all values that meet the condition of being greater than or equal to the specified value, modify them to the target value, and return a new list.

    u8list  IfNotEqual(u8list src, u8 cmp_value, u8 target_value, ptr exec_ctx)
    u16list IfNotEqual(u16list src, u16 cmp_value, u16 target_value, ptr exec_ctx)
    u32list IfNotEqual(u32list src, u32 cmp_value, u32 target_value, ptr exec_ctx)
    u64list IfNotEqual(u64list src, u64 cmp_value, u64 target_value, ptr exec_ctx)
    i8list  IfNotEqual(i8list src, i8 cmp_value, i8 target_value, ptr exec_ctx)
    i16list IfNotEqual(i16list src, i16 cmp_value, i16 target_value, ptr exec_ctx)
    i32list IfNotEqual(i32list src, i32 cmp_value, i32 target_value, ptr exec_ctx)
    i64list IfNotEqual(i64list src, i64 cmp_value, i64 target_value, ptr exec_ctx)
    f32list IfNotEqual(f32list src, f32 cmp_value, f32 target_value, ptr exec_ctx)
    f64list IfNotEqual(f64list src, f64 cmp_value, f64 target_value, ptr exec_ctx)

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