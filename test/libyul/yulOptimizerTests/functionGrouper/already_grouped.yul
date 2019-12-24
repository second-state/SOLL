// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    {
        let x := 2
    }
    function f() -> y { y := 8 }
}
// ====
// step: functionGrouper
// ----
// {
//     { let x := 2 }
//     function f() -> y
//     { y := 8 }
// }
