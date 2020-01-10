// RUN: %soll -lang=Yul %s
// REQUIRES: calldatasize
{
    function f(a) -> x { x := add(a, a) }
    let y := f(calldatasize())
}
// ====
// step: expressionInliner
// ----
// {
//     function f(a) -> x
//     { x := add(a, a) }
//     let y := add(calldatasize(), calldatasize())
// }
