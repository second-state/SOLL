// RUN: %soll -lang=Yul %s
// REQUIRES: yulType
{
    let a:u256
    function f() {}
}
// ====
// step: mainFunction
// yul: true
// ----
// {
//     function main()
//     { let a:u256 }
//     function f()
//     { }
// }
