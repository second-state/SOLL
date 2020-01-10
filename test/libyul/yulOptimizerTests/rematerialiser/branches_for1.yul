// RUN: %soll -lang=Yul %s
// REQUIRES: pop
{
    let a := caller()
    pop(a)
    for {  } a { pop(a) } {
        pop(a)
    }
}
// ====
// step: rematerialiser
// ----
// {
//     let a := caller()
//     pop(caller())
//     for { } caller() { pop(caller()) }
//     { pop(caller()) }
// }
