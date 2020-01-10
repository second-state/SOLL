// RUN: %soll -lang=Yul %s
// REQUIRES: yulType
{
    let a:u256
    function f() {}
}
// ====
// step: functionHoister
// yul: true
// ----
// {
//     let a:u256
//     function f()
//     { }
// }
